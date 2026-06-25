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

OPTIONS="${1:-""}"

brew --version

brew install libidn2 pkg-config boost icu4c

if [ ! -f /opt/homebrew/lib/libidn2.dylib ] && [ -f /opt/homebrew/lib/libidn2.0.dylib ]; then
    ln -sf /opt/homebrew/lib/libidn2.0.dylib /opt/homebrew/lib/libidn2.dylib
fi

export LDFLAGS="-L/opt/homebrew/lib"
export CPPFLAGS="-I/opt/homebrew/include"
export PKG_CONFIG_PATH="$(brew --prefix icu4c)/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"

mkdir -p build
cd build
cmake .. $OPTIONS \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-L/opt/homebrew/lib" \
    -DCMAKE_SHARED_LINKER_FLAGS="-L/opt/homebrew/lib" \
    -DCMAKE_PREFIX_PATH="$(brew --prefix icu4c)"

rm -f build/_deps/quickjs-src/version 2>/dev/null || true

make -j$(sysctl -n hw.ncpu)
sudo make install

hash -r
if [[ "$OPTIONS" == "" ]] && ! command -v justc &> /dev/null; then
    sudo ln -sf /usr/local/bin/justc /usr/bin/justc
    hash -r
fi

if [[ "$OPTIONS" == "" ]] && ! command -v justc &> /dev/null; then
    echo -e "::error::CMake error." && exit 1
fi

echo "Built files:"
find . -name "*.dylib"
find . -name "justc"
