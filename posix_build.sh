#!/bin/sh

## Example build script for Linux.  Change the paths to appropriate
## locations.  Or just use cmake-gui.

SRC=$(cd $(dirname "$0"); pwd)
BUILD="${SRC}/build"
mkdir -p "$BUILD" ; cd "$BUILD"
nice cmake "$SRC" && nice make && echo "SUCCESS!";

