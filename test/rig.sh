#!/usr/bin/env bash
# Headless test rig: run a Qt app under Xvfb with the Hearth style, hearth's
# palette (written by hearth-color-scheme into private config dirs), at 4/3
# scale, without touching the live display, session bus or dconf.
#
#   test/rig.sh [--mode dark|light] [--plugins DIR] [--shot out.png] -- cmd args...
#
# --plugins: an installed package prefix (e.g. ./result; default: build6 and
# build5 here). --shot: after the app
# has drawn, grab the whole screen with ImageMagick's `import` and stop it.
set -euo pipefail
here=$(cd "$(dirname "$0")/.." && pwd)
mode=dark plugins="" shot="" wait=4 size=1400x900
while [[ $# -gt 0 ]]; do
    case $1 in
        --mode) mode=$2; shift 2 ;;
        --plugins) plugins=$(readlink -f "$2"); shift 2 ;;
        --shot) shot=$2; shift 2 ;;
        --wait) wait=$2; shift 2 ;;
        --size) size=$2; shift 2 ;;
        --) shift; break ;;
        *) break ;;
    esac
done

rig=$(mktemp -d "${TMPDIR:-/tmp}/hearth-style-rig.XXXXXX")
trap 'kill ${xvfb:-} 2>/dev/null || true; rm -rf "$rig"' EXIT
mkdir -p "$rig"/{config,data,cache,state,runtime,plugins6/styles,plugins6/kf6/kirigami/platform,plugins5/styles}
chmod 700 "$rig/runtime"
export XDG_CONFIG_HOME=$rig/config XDG_DATA_HOME=$rig/data XDG_CACHE_HOME=$rig/cache
export XDG_STATE_HOME=$rig/state XDG_RUNTIME_DIR=$rig/runtime
export GSETTINGS_BACKEND=memory
export DBUS_SESSION_BUS_ADDRESS=unix:path=$rig/runtime/no-bus
export NO_AT_BRIDGE=1 QT_ACCESSIBILITY=0
unset WAYLAND_DISPLAY ARBOR_SOCKET NIRI_SOCKET QT_STYLE_OVERRIDE

# The plugins under test first on the plugin path.
if [[ -n $plugins ]]; then
    # A package prefix (nix build's result): both Qt versions' plugins.
    ln -s "$plugins"/lib/qt-6/plugins/styles/* "$rig/plugins6/styles/"
    ln -s "$plugins"/lib/qt-5*/plugins/styles/* "$rig/plugins5/styles/"
    ln -s "$plugins"/lib/qt-6/plugins/kf6/kirigami/platform/* "$rig/plugins6/kf6/kirigami/platform/" 2>/dev/null || true
    qml=$plugins/lib/qt-6/qml
else
    qml=$here/qml
    [[ -e $here/build6/org.hearth.desktop.so ]] && ln -s "$here/build6/org.hearth.desktop.so" "$rig/plugins6/kf6/kirigami/platform/"
    [[ -e $here/build6/libhearth6.so ]] && ln -s "$here/build6/libhearth6.so" "$rig/plugins6/styles/"
    [[ -e $here/build5/libhearth5.so ]] && ln -s "$here/build5/libhearth5.so" "$rig/plugins5/styles/"
fi
export QT_PLUGIN_PATH=$rig/plugins6:$rig/plugins5:${RIG_EXTRA_PLUGINS:+$RIG_EXTRA_PLUGINS:}/run/current-system/sw/lib/qt-6/plugins:/run/current-system/sw/lib/qt-5.15.19/plugins
export QT_QPA_PLATFORMTHEME=qt5ct QT_QPA_PLATFORM=xcb
# The org.hearth.desktop Qt Quick Controls style (as the session sets it).
export QML_IMPORT_PATH=$qml${QML_IMPORT_PATH:+:$QML_IMPORT_PATH} QT_QUICK_CONTROLS_STYLE=${RIG_QQC_STYLE-org.hearth.desktop}
[[ -z $QT_QUICK_CONTROLS_STYLE ]] && unset QT_QUICK_CONTROLS_STYLE
export QT_SCALE_FACTOR=1.3333333 QT_AUTO_SCREEN_SCALE_FACTOR=0 QT_ENABLE_HIGHDPI_SCALING=0

# hearth's palette, accent, fonts and style into the private config.
hcs=${HEARTH_COLOR_SCHEME:-$HOME/src/hearth/helpers/hearth-color-scheme}
mkdir -p "$rig/home"; HOME=$rig/home python3 "$hcs" "$mode" >/dev/null 2>&1 || true
for ct in qt5ct qt6ct; do
    [[ -f $XDG_CONFIG_HOME/$ct/$ct.conf ]] && sed -i 's/^style=.*/style=Hearth/' "$XDG_CONFIG_HOME/$ct/$ct.conf"
done

for n in $(seq 90 140); do [[ -e /tmp/.X11-unix/X$n || -e /tmp/.X$n-lock ]] || break; done
Xvfb ":$n" -screen 0 "${size}x24" -nolisten tcp >/dev/null 2>&1 &
xvfb=$!
export DISPLAY=:$n
for _ in $(seq 50); do [[ -e /tmp/.X11-unix/X$n ]] && break; sleep 0.1; done

if [[ -z $shot ]]; then
    "$@"
    exit
fi
"$@" &
app=$!
sleep "$wait"
import -window root "$shot"
kill $app 2>/dev/null || true
wait $app 2>/dev/null || true
