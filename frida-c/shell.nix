with import <nixpkgs> {
  crossSystem = "i686-linux";
};

stdenv.mkDerivation {
  name = "frida-test";
  src = ./.;
  nativeBuildInputs = with pkgsBuildHost; [ meson ninja pkg-config glib python3 ];
  buildInputs = with pkgsHostTarget; [ glib ];
}
