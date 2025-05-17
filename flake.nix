ption = "Development environment for dwl (Dynamic Wayland compositor)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux"; # Change if you're on ARM/aarch64
      pkgs = import nixpkgs {
        inherit system;
      };
    in
    {
      devShells.default = pkgs.mkShell {
        packages = [
          pkgs.gcc
          pkgs.make
          pkgs.pkg-config
          pkgs.wayland
          pkgs.libinput
          pkgs.xkeyboard-config
          pkgs.xkbcommon
          pkgs.wayland-protocols
          pkgs.wlroots
          pkgs.libxcb
          pkgs."xcbutil-wm"
          pkgs.xorgserver_xwayland # runtime only, for testing with XWayland
        ];

        shellHook = ''
          echo "Ready to build dwl!"
          gcc --version | head -n1
          make --version | head -n1
        '';
      };
    };
}
