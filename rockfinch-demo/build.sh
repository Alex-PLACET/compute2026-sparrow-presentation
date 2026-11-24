#!/bin/bash
set -euxo pipefail

# cmake is a shell function calling emcmake (emscripten compiler activation), so it can only configure.
# FindPython needs the native build interpreter; headers, library and site-packages come from the wasm host prefix
cmake -S rockfinch-demo -B build -G Ninja ${CMAKE_ARGS} \
    -DCMAKE_BUILD_TYPE=Release \
    -DPython_EXECUTABLE="$BUILD_PREFIX/bin/python" \
    -DPython_LIBRARY="$PREFIX/lib/libpython${PY_VER}.a" \
    -DROCKFINCH_DEMO_INSTALL_DIR="$PREFIX/lib/python${PY_VER}/site-packages" \
    -DSPARROW_ROCKFINCH_SOURCE_DIR="$SRC_DIR/sparrow-rockfinch"
ninja -C build install
