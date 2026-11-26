#!/usr/bin/env bash
# Build and install the Hearth style as org.kde.KStyle.Hearth for Flatpak's
# KDE runtimes, then wire the installed apps to it. One command; run it
# again whenever a KDE runtime updates (an extension is built per branch).
#
#   flatpak/build.sh                 every org.kde.Platform branch an installed app uses
#   flatpak/build.sh 6.11 5.15-25.08 just these branches
#
# Needs org.flatpak.Builder; the matching org.kde.Sdk branches are installed
# for the user as needed (from flathub).
#
# What it sets up (user installation and user overrides):
# - org.kde.KStyle.Hearth//<branch>: styles/hearth{5,6}.so (the widget style,
#   merged into the runtime's plugin path), and on Qt 6 branches qml/ (the
#   org.hearth.desktop Qt Quick Controls style) and kf6/ (its Kirigami
#   platform plugin).
# - Global overrides: QT_QPA_PLATFORMTHEME=kde, so Qt in the sandbox uses
#   the runtime's KDE platform theme (the session's qt5ct isn't in any
#   runtime); it reads kdeglobals through the Settings portal (hearth-portal
#   serves it): hearth's palette, Inter, icons and widgetStyle=Hearth.
# - Per app on a Qt 6 branch that has the extension: QT_QUICK_CONTROLS_STYLE=
#   org.hearth.desktop, with the extension's qml/ and plugin folder appended
#   to that app's own QML2_IMPORT_PATH and QT_PLUGIN_PATH. (Not global: a
#   Flatpak whose runtime lacks the extension couldn't load the style at all.)
set -euo pipefail
here=$(cd "$(dirname "$0")/.." && pwd)
out=${HEARTH_FLATPAK_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/hearth-qt-style-flatpak}
mkdir -p "$out"
ext=/usr/share/runtime/lib/plugins/Hearth

if [[ $# -gt 0 ]]; then
    branches=("$@")
else
    mapfile -t branches < <(flatpak list --app --columns=runtime | awk -F/ '$1 == "org.kde.Platform" {print $3}' | sort -u)
fi
[[ ${#branches[@]} -gt 0 ]] || { echo "no apps on org.kde.Platform runtimes" >&2; exit 0; }

for b in "${branches[@]}"; do
    qt=6; [[ $b == 5.* ]] && qt=5
    echo "== org.kde.KStyle.Hearth//$b (Qt $qt)"
    flatpak info --user org.kde.Sdk//"$b" >/dev/null 2>&1 || flatpak info --system org.kde.Sdk//"$b" >/dev/null 2>&1 \
        || flatpak install --user -y --noninteractive flathub org.kde.Sdk//"$b"
    sed -e "s|@BRANCH@|$b|g" -e "s|@QT@|$qt|g" -e "s|@SRC@|$here|g" \
        "$here/flatpak/org.kde.KStyle.Hearth.yml.in" > "$out/org.kde.KStyle.Hearth-$b.yml"
    flatpak run --filesystem="$here":ro --filesystem="$out" org.flatpak.Builder \
        --user --force-clean --disable-rofiles-fuse --state-dir="$out/state" --repo="$out/repo" \
        "$out/build-$b" "$out/org.kde.KStyle.Hearth-$b.yml"
done

flatpak remote-add --user --if-not-exists --no-gpg-verify hearth-qt-style "$out/repo"
for b in "${branches[@]}"; do
    flatpak install --user -y --noninteractive --reinstall hearth-qt-style org.kde.KStyle.Hearth//"$b"
done

# --- wiring -------------------------------------------------------------------
flatpak override --user --env=QT_QPA_PLATFORMTHEME=kde

# An app's effective value of an environment variable: its own metadata's,
# else its runtime's.
envof() {
    local app=$1 var=$2 v runtime
    v=$(flatpak info -m "$app" | envkey "$var")
    if [[ -z $v ]]; then
        runtime=$(flatpak info -m "$app" | awk -F= '/^runtime=/{print $2; exit}')
        v=$(flatpak info -m "runtime/$runtime" | envkey "$var")
    fi
    printf '%s' "$v"
}
# KEY's value in a metadata file's [Environment] group ("KEY=v" or "KEY = v").
envkey() {
    awk -v k="$1" '/^\[/ {s = ($0 == "[Environment]"); next}
        s { key = $0; sub(/[ ]*=.*/, "", key); if (key == k) { v = $0; sub(/^[^=]*=[ ]*/, "", v); print v; exit } }'
}

while read -r app runtime; do
    [[ $runtime == org.kde.Platform/* ]] || continue
    [[ $app == *.BaseApp ]] && continue
    b=${runtime##*/}
    [[ $b == 5.* ]] && continue
    flatpak info org.kde.KStyle.Hearth//"$b" >/dev/null 2>&1 || continue
    qml=$(envof "$app" QML2_IMPORT_PATH); plugins=$(envof "$app" QT_PLUGIN_PATH)
    qml=${qml//:$ext\/qml/}; plugins=${plugins//:$ext/}
    # (Never write the overrides without the app's own paths: it would lose
    # its plugins.)
    [[ -n $qml && -n $plugins ]] || { echo "-- $app: no QML/plugin path found, skipped" >&2; continue; }
    echo "-- $app: org.hearth.desktop"
    flatpak override --user "$app" \
        --env=QT_QUICK_CONTROLS_STYLE=org.hearth.desktop \
        --env=QML2_IMPORT_PATH="${qml:+$qml:}$ext/qml" \
        --env=QT_PLUGIN_PATH="${plugins:+$plugins:}$ext"
done < <(flatpak list --app --columns=application,runtime | sort -u)
echo "done"
