{ pkgs ? import <nixpkgs> {} }:
with pkgs;

{
   fclib-header-only = callPackage ./fclib.nix {};
   fclib-gcc6 = callPackage ./fclib.nix { header_only = false; gcc = gcc6;};
   fclib = callPackage ./fclib.nix { header_only = false;};
}
