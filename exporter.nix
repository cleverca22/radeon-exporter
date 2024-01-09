{ stdenv, libxcb, libdrm, linuxHeaders, pkgconfig }:

stdenv.mkDerivation {
  name = "radeon-exporter";
  src = ./.;
  preBuild = ''
    rm *.o
  '';
  buildInputs = [ libxcb libdrm linuxHeaders pkgconfig ];
  installPhase = ''
    mkdir -pv $out/bin
    cp radeon-exporter $out/bin/
  '';
}
