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

param(
    [string]$Options = ""
)

Write-Host "Starting Windows compilation..." -ForegroundColor Green

if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Chocolatey..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
}

Write-Host "Installing required packages..." -ForegroundColor Yellow
choco install cmake curl -y --no-progress

New-Item -ItemType Directory -Force -Path "build"
Set-Location "build"

if ($Options -eq "") {
    $Options = "-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=..\install_root"
}

Write-Host "Configuring with CMake..." -ForegroundColor Yellow
cmake .. $Options

Write-Host "Building JUSTC..." -ForegroundColor Yellow
cmake --build . --config Release --parallel $env:NUMBER_OF_PROCESSORS

Write-Host "Installing..." -ForegroundColor Yellow
cmake --install . --config Release

Set-Location ".."
if (Test-Path "install_root\bin\justc.exe") {
    Write-Host "Installed JUSTC!" -ForegroundColor Green
    Write-Host "Executable location: $(Resolve-Path 'install_root\bin\justc.exe')" -ForegroundColor Cyan
} else {
    Write-Host "::error::CMake error." -ForegroundColor Red
    exit 1
}
