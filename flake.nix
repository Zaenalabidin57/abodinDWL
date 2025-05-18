{
  description = "Development environment for dwl (Dynamic Wayland compositor)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux"; # Change if you're on ARM/aarch64
      pkgs = import nixpkgs {
        inherit system;
      };
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = [
          pkgs.gcc
          pkgs.gnumake
          pkgs.pkg-config
          pkgs.wayland
          pkgs.libinput
          pkgs.xkeyboard_config
          pkgs.libxkbcommon
          pkgs.wayland-protocols
          pkgs.libinput
          pkgs.pixman
          pkgs.wayland-scanner
          pkgs.wayland-protocols
          pkgs.fcft
          pkgs.dbus
          pkgs.wlroots
          pkgs.xorg.libxcb
          pkgs.libdrm
          pkgs.xorg.xcbutilwm
          pkgs.xwayland # runtime only, for testing with XWayland
        ];

        shellHook = ''
          echo "Ready to build dwl!"
          gcc --version | head -n1
          make --version | head -n1
        '';
      };
      userConfigurations."shigure".home.packages = [
        pkgs.dwl
        pkgs.neovim
        pkgs.libinput
        pkgs.libxkbcommon
        pkgs.wayland
        pkgs.wayland-protocols
        pkgs.xorg.xcbutilwm
        pkgs.xorg.libxcb
        pkgs.dbus
      ];
    };
}
