# MIT License
#
# Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>
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

#!/bin/bash
set -e
OPTIONS="${1:-""}"

echo "Compiling JUSTC for macOS..."

if command -v clang++ &> /dev/null; then
    clang++ --version
elif command -v g++ &> /dev/null; then
    g++ --version
else
    echo "::error::No C++ compiler found!"
    exit 1
fi

echo "Installing dependencies..."
brew install cmake pkg-config curl

echo "Creating build directory..."
mkdir -p build
cd build

echo "Configuring CMake..."
cmake .. $OPTIONS

if [ $? -ne 0 ]; then
    echo "::error::CMake configuration failed"
    exit 1
fi

echo "Building JUSTC..."
CPU_COUNT=$(sysctl -n hw.ncpu)
make -j$CPU_COUNT

if [ $? -ne 0 ]; then
    echo "::error::Build failed"
    exit 1
fi

echo "Installing JUSTC..."
sudo make install

if [ $? -ne 0 ]; then
    echo "::error::Installation failed"
    exit 1
fi

cd ..
hash -r
if command -v justc &> /dev/null; then
    echo "Done!"
    justc --help
else
    echo "::error::CMake error."
    exit 1
fi
