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

param(
    [string]$Options = ""
)

Write-Host "PowerShell version: $($PSVersionTable.PSVersion)"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "Installing CMake..."
    choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
}

if (-not (Get-Command ninja -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Ninja..."
    choco install ninja -y
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Git..."
    choco install git -y
}

$env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")

if (-not (Test-Path "vcpkg")) {
    Write-Host "Cloning vcpkg..."
    git clone --depth=1 https://github.com/microsoft/vcpkg.git
}
& .\vcpkg\bootstrap-vcpkg.bat

Write-Host "Installing dependencies..."
& .\vcpkg\vcpkg install cpr:x64-windows-static --recurse
& .\vcpkg\vcpkg install libidn2:x64-windows-static --recurse
& .\vcpkg\vcpkg install icu:x64-windows-static --recurse
& .\vcpkg\vcpkg install boost-multiprecision:x64-windows-static --recurse

if (Test-Path "build") {
    Remove-Item -Recurse -Force build
}
if (Test-Path "CMakeCache.txt") {
    Remove-Item CMakeCache.txt
}
if (Test-Path "CMakeFiles") {
    Remove-Item -Recurse -Force CMakeFiles
}
if (Test-Path "_deps") {
    Remove-Item -Recurse -Force _deps
}

Write-Host "Configuring CMake with Ninja + MSVC..."
cmake -B build -G "Ninja" `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_TOOLCHAIN_FILE="$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows-static `
    -DBoost_USE_STATIC_LIBS=ON `
    -DCMAKE_CXX_COMPILER=cl.exe `
    -DCMAKE_C_COMPILER=cl.exe `
    -DCMAKE_PREFIX_PATH="$PWD\vcpkg\installed\x64-windows-static" `
    -DCMAKE_LIBRARY_PATH="$PWD\vcpkg\installed\x64-windows-static\lib" `
    -DCMAKE_INCLUDE_PATH="$PWD\vcpkg\installed\x64-windows-static\include" `
    $Options

$paths = @(
    "build/_deps/quickjs-src/version",
    "_deps/quickjs-src/version",
    "build/_deps/quickjscmake-build/__/quickjs-src/version"
)
foreach ($p in $paths) {
    if (Test-Path $p) {
        Remove-Item $p -Force
        Write-Host "Removed: $p"
    }
}

Write-Host "Building..."
$cores = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
$jobs = [Math]::Min($cores + 1, 8)
Write-Host "Building with -j $jobs"
cmake --build build --config Release -j $jobs

Write-Host "Built files:"
Get-ChildItem -Recurse -Filter "*.exe" | ForEach-Object { Write-Host $_.FullName }
Get-ChildItem -Recurse -Filter "*.dll" | ForEach-Object { Write-Host $_.FullName }
