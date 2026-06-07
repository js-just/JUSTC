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
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

Set-Location $ProjectRoot

if (-not (Test-Path "vcpkg")) {
    git clone https://github.com/microsoft/vcpkg.git
    & .\vcpkg\bootstrap-vcpkg.bat
}

Write-Host "Downloading and installing dependencies..."
& .\vcpkg\vcpkg install boost-multiprecision:x64-windows-static
& .\vcpkg\vcpkg install cpr:x64-windows-static
& .\vcpkg\vcpkg install libidn2:x64-windows-static
& .\vcpkg\vcpkg install icu:x64-windows-static

$LuaVersion = "5.4.7"
$LuaUrl = "https://www.lua.org/ftp/lua-$LuaVersion.tar.gz"
$LuaDir = "third-party/lua"

New-Item -ItemType Directory -Force -Path "$LuaDir\include" | Out-Null
New-Item -ItemType Directory -Force -Path "$LuaDir\source" | Out-Null

Write-Host "Downloading Lua $LuaVersion..."
if (-not (Test-Path "$LuaDir\include\lua.h")) {
    Invoke-WebRequest -Uri $LuaUrl -OutFile "$env:TEMP\lua.tar.gz"

    tar -xzf "$env:TEMP\lua.tar.gz" -C "$env:TEMP/"

    Write-Host "Installing Lua $LuaVersion..."

    Copy-Item "$env:TEMP\lua-$LuaVersion\src\*.h" -Destination "$LuaDir\include\"
    Copy-Item "$env:TEMP\lua-$LuaVersion\src\*.c" -Destination "$LuaDir\source\"
    Remove-Item "$LuaDir\source\lua.c", "$LuaDir\source\luac.c" -ErrorAction SilentlyContinue

    Remove-Item "$env:TEMP\lua.tar.gz"
    Remove-Item -Recurse "$env:TEMP\lua-$LuaVersion"

}
Write-Host "Installed Lua $LuaVersion."

New-Item -ItemType Directory -Force -Path "build" | Out-Null
Set-Location "build"

cmake .. -G "Visual Studio 17 2022" `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DCMAKE_TOOLCHAIN_FILE="$ProjectRoot/vcpkg/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows-static `
    -DBoost_USE_STATIC_LIBS=ON

cmake --build . --config $BuildType --target ALL_BUILD -j 20
