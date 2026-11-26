{
  description = "Hearth: a Qt 5 / Qt 6 widget style that makes Qt and KDE apps look like the hearth desktop";

  # The same nixpkgs revision as /etc/nixos, so the plugins are built against
  # the Qt the desktop's apps use (Qt only loads plugins from its own build).
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/ab5c18fdedfc8f9bbc9d773f10a540790e2787a7";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAll = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
      src = nixpkgs.lib.cleanSourceWith {
        src = ./.;
        filter = path: type: !(builtins.elem (baseNameOf path) [ "build" "build5" "build6" "result" "shots" ]);
      };
      styleFor = pkgs: qtbase: major: extra: pkgs.stdenv.mkDerivation {
        pname = "hearth-qt-style-qt${toString major}";
        version = "0.1.0";
        inherit src;
        nativeBuildInputs = [ pkgs.cmake ];
        buildInputs = [ qtbase ] ++ extra;
        # A plugin, not an app: nothing to wrap.
        dontWrapQtApps = true;
        cmakeFlags = [
          "-DQT_MAJOR_VERSION=${toString major}"
          "-DBUILD_GALLERY=OFF"
          "-DPLUGIN_INSTALL_DIR=${qtbase.qtPluginPrefix}/styles"
          "-DQML_INSTALL_DIR=${qtbase.qtQmlPrefix}"
        ];
        meta = {
          description = "Hearth widget style for Qt ${toString major}";
          license = nixpkgs.lib.licenses.mit;
          platforms = nixpkgs.lib.platforms.linux;
        };
      };
    in
    {
      packages = forAll (pkgs: rec {
        # (Qt 6 also: the org.hearth.desktop Qt Quick Controls style and its
        # Kirigami platform plugin.)
        qt6 = styleFor pkgs pkgs.qt6.qtbase 6 [ pkgs.qt6.qtdeclarative pkgs.kdePackages.kirigami.unwrapped ];
        qt5 = styleFor pkgs pkgs.libsForQt5.qtbase 5 [ ];
        # Both, for a system profile: lib/qt-6/plugins/styles and
        # lib/qt-5.x/plugins/styles land on the session's QT_PLUGIN_PATH.
        default = pkgs.symlinkJoin {
          name = "hearth-qt-style";
          paths = [ qt6 qt5 ];
        };
      });

      devShells = forAll (pkgs: {
        # (One Qt per shell: Qt's setup hooks refuse a mix.)
        default = pkgs.mkShell { packages = [ pkgs.cmake pkgs.ninja pkgs.qt6.qtbase pkgs.qt6.qtdeclarative pkgs.kdePackages.kirigami.unwrapped ]; };
        qt5 = pkgs.mkShell { packages = [ pkgs.cmake pkgs.ninja pkgs.libsForQt5.qtbase ]; };
      });
    };
}
