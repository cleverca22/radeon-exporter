let
  pkgs = import <nixpkgs> {};
 in pkgs.stdenv.mkDerivation {
   name = "radeon-exporter";
   buildInputs = [ pkgs.libdrm pkgs.linuxHeaders pkgs.pkgconfig pkgs.xorg.libxcb ];
 }
