{
  description = "Development environment for dwl (Dynamic Wayland compositor)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      # Define the system once for easy reuse.
      system = "x86_64-linux";
      # Use legacyPackages for consistency.
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        # Use buildInputs to make headers and libraries available to the compiler.
        buildInputs = [
          # --- Build Tools ---
          pkgs.gcc
          pkgs.gnumake
          pkgs.pkg-config

          # --- dwl Dependencies (as found in the Makefile) ---
          pkgs.dbus
          pkgs.fcft
          pkgs.libdrm
          pkgs.libinput
          pkgs.libxkbcommon
          pkgs.pixman
          pkgs.wayland
          pkgs.wayland-protocols

          # --- wlroots: Match the build to 0.19 ---
          # Upgrade devShell to wlroots 0.19 to match Makefile.
          pkgs.wlroots_0_19

          # --- XCB Libraries ---
          # These provide the missing xcb-*.pc files.
          pkgs.xorg.libxcb
          pkgs.xorg.xcbutil
          pkgs.xorg.xcbutilcursor
          pkgs.xorg.xcbutilkeysyms
          pkgs.xorg.xcbutilwm

          # For XWayland support (runtime dependency for testing)
          pkgs.xwayland
        ];

        shellHook = ''
          echo "Ready to build dwl!"
          echo "Using $(gcc --version | head -n1)"
          fish
        '';
      };
    };
}
