#!/bin/bash
set -e

# MIT License
#
# Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

echo "Installing system dependencies..."
brew install libidn2 pkg-config boost icu4c

if [ ! -f /opt/homebrew/lib/libidn2.dylib ] && [ -f /opt/homebrew/lib/libidn2.0.dylib ]; then
    ln -sf /opt/homebrew/lib/libidn2.0.dylib /opt/homebrew/lib/libidn2.dylib
fi

LUA_VERSION="5.4.7"
LUA_URL="https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz"
LUA_DIR="third-party/lua"

mkdir -p ${LUA_DIR}/include
mkdir -p ${LUA_DIR}/source

if [ ! -f "${LUA_DIR}/include/lua.h" ]; then
    echo "Downloading Lua ${LUA_VERSION}..."
    curl -L ${LUA_URL} -o /tmp/lua.tar.gz
    tar -xzf /tmp/lua.tar.gz -C /tmp/

    echo "Installing Lua ${LUA_VERSION}..."
    cp /tmp/lua-${LUA_VERSION}/src/*.h ${LUA_DIR}/include/
    cp /tmp/lua-${LUA_VERSION}/src/*.c ${LUA_DIR}/source/
    rm -f ${LUA_DIR}/source/lua.c ${LUA_DIR}/source/luac.c

    rm -f /tmp/lua.tar.gz
    rm -rf /tmp/lua-${LUA_VERSION}

    echo "Installed Lua ${LUA_VERSION}."
fi

mkdir -p build
cd build

export LDFLAGS="-L/opt/homebrew/lib"
export CPPFLAGS="-I/opt/homebrew/include"
export PKG_CONFIG_PATH="$(brew --prefix icu4c)/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"

cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-L/opt/homebrew/lib" \
    -DCMAKE_SHARED_LINKER_FLAGS="-L/opt/homebrew/lib" \
    -DCMAKE_PREFIX_PATH="$(brew --prefix icu4c)"

rm -f _deps/quickjs-src/version 2>/dev/null || true

make -j$(sysctl -n hw.ncpu)
