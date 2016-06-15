#!/bin/sh

SYS_NAME="$(uname -o)";
SYS_NAME="$(basename $SYS_NAME)";

BUILD_DIR="build_$SYS_NAME";

cd "$(dirname $0)";
mkdir -p "$BUILD_DIR";
cd "$BUILD_DIR";

cmake .. -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON "$@";
