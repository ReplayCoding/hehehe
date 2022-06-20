with import <nixpkgs> {};
mkShell {
  packages = [ pkgsi686Linux.gcc ];
}
