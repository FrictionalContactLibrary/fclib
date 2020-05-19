{ pkgs ? import <nixpkgs> {},
  gcc ? pkgs.gcc ,
  hdf5 ? pkgs.hdf5,
  header_only ? true
  }:
  
with pkgs;

let  version = "3.1.0" ; in
 stdenv.mkDerivation rec {

 name = "fclib-${version}";
 buildInputs = [ cmake pkgconfig gcc hdf5];
 cmakeFlags = [ " " ]
    ++ stdenv.lib.optional (header_only != true) [ "-DFCLIB_HEADER_ONLY=OFF" ];


hardeningDisable = [ "format" ];
 src = ./.;
    }