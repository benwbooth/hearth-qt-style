#!/usr/bin/env bash
# Screenshot a Flatpak app headless, as it looks on the desktop with the
# Hearth extensions (flatpak/build.sh): Xvfb at 4/3 scale, a private D-Bus
# with no service activation carrying hearth-portal and the real
# xdg-desktop-portal frontend (so the app's KDE platform theme reads
# kdeglobals through the Settings portal), and a throwaway HOME so the
# app's real data (~/.var/app) is untouched. Never the live display or bus.
#
#   test/flatpak-shot.sh out.png APP [args...]
# WAIT=seconds before the shot (default 8); XDOTOOL=path/to/xdotool lists the
# windows into the log folder; with it, CLICK="x y" clicks there first.
# FLATPAK_ARGS: more `flatpak run` options (e.g. --command=... --filesystem=...).
set -euo pipefail
shot=$(realpath -m "$1"); shift
app=$1; shift
wait=${WAIT:-8}
here=$(cd "$(dirname "$0")/.." && pwd)
t=$(mktemp -d "${TMPDIR:-/tmp}/hearth-flatpak-shot.XXXXXX")
mkdir -p "$t/home" "$t/run" "$t/cfg"
chmod 700 "$t/run"
cp ~/.config/kdeglobals "$t/cfg/" 2>/dev/null || true
xdp=$(dirname "$(readlink -f /run/current-system/sw/share/xdg-desktop-portal/portals/hearth.portal)")
xdp_bin=$(ls /nix/store/*-xdg-desktop-portal-*/libexec/xdg-desktop-portal 2>/dev/null | head -1)
cleanup() {
    [[ -n ${xvfb:-} ]] && kill "$xvfb" 2>/dev/null
    [[ -s $t/run/bus.pid ]] && kill "$(cat "$t/run/bus.pid")" 2>/dev/null
    true
}
trap cleanup EXIT

cat > "$t/bus.conf" <<EOF
<!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-Bus Bus Configuration 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
<busconfig><type>session</type><listen>unix:dir=$t/run</listen>
<policy context="default"><allow send_destination="*" eavesdrop="true"/><allow eavesdrop="true"/><allow own="*"/></policy></busconfig>
EOF

for n in $(seq 90 140); do [[ -e /tmp/.X11-unix/X$n || -e /tmp/.X$n-lock ]] || break; done
Xvfb ":$n" -screen 0 1400x900x24 -nolisten tcp >/dev/null 2>&1 &
xvfb=$!

env -i PATH="$PATH" HOME="$t/home" USER="$USER" LANG="${LANG:-C.UTF-8}" \
    XDG_RUNTIME_DIR="$t/run" XDG_CONFIG_HOME="$t/cfg" XDG_DATA_HOME="$t/home/.local/share" \
    XDG_DATA_DIRS="/run/current-system/sw/share:$HOME/.local/share/flatpak/exports/share:/var/lib/flatpak/exports/share" \
    XDG_CONFIG_DIRS=/etc/xdg XDG_CURRENT_DESKTOP=arbor:KDE \
    FLATPAK_USER_DIR="$HOME/.local/share/flatpak" GSETTINGS_BACKEND=memory \
    DISPLAY=":$n" XDOTOOL="${XDOTOOL:-}" CLICK="${CLICK:-}" FLATPAK_ARGS="${FLATPAK_ARGS:-}" SHOT="$shot" WAIT="$wait" T="$t" XDP="$xdp_bin" FLATPAK_PORTAL="$(dirname "$(dirname "$(readlink -f "$(command -v flatpak)")")")/libexec/flatpak-portal" \
    bash -c '
set -e
for _ in $(seq 50); do [[ -e /tmp/.X11-unix/X${DISPLAY#:} ]] && break; sleep 0.1; done
dbus-daemon --config-file="$T/bus.conf" --fork --print-address=3 --print-pid=4 3>"$T/run/bus.addr" 4>"$T/run/bus.pid"
export DBUS_SESSION_BUS_ADDRESS=$(head -1 "$T/run/bus.addr")
hearth-portal serve >"$T/portal.log" 2>&1 &
sleep 1
"$XDP" >"$T/xdp.log" 2>&1 &
# (Apps that start sandboxed helpers, QtWebEngine ones, need the Flatpak portal.)
"$FLATPAK_PORTAL" >"$T/flatpak-portal.log" 2>&1 &
sleep 1.5
# (--die-with-parent: the sandbox ends with this launcher, killed below; a
# sandbox left running after the X server went away lingered.)
flatpak run --die-with-parent --env=QT_SCALE_FACTOR=1.3333333 --env=QT_QPA_PLATFORM=xcb --nosocket=wayland $FLATPAK_ARGS "$@" >"$T/app.log" 2>&1 &
app_pid=$!
sleep "$WAIT"
# CLICK="x y" (screen px): click there first (to open a menu), then wait 1.5 s.
[[ -n ${CLICK:-} && -n ${XDOTOOL:-} ]] && { $XDOTOOL mousemove $CLICK click 1; sleep 1.5; }
import -window root "$SHOT"
[[ -n ${XDOTOOL:-} ]] && { for w in $($XDOTOOL search --onlyvisible --name "" 2>/dev/null); do echo "win $w $($XDOTOOL getwindowname $w) $($XDOTOOL getwindowgeometry $w | tr "\n" " ")"; done > "$T/windows.txt"; }
kill $app_pid 2>/dev/null || true
' _ "$app" "$@"
echo "$shot  (logs: $t/{app,xdp,portal}.log)"
