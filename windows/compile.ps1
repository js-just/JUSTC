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

Write-Host "Compiling JUSTC for Windows..." -ForegroundColor Green

$CurrentDir = Get-Location
Write-Host "Current directory: $CurrentDir" -ForegroundColor Yellow

$BuildDir = Join-Path $CurrentDir "build"
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Force -Path $BuildDir
}

Set-Location $BuildDir

if ($Options -eq "") {
    $InstallPrefix = Join-Path $CurrentDir "install_root"
    $Options = "-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`"$InstallPrefix`""
}

Write-Host "Configuring with CMake..." -ForegroundColor Yellow
Write-Host "Options: $Options" -ForegroundColor Cyan

cmake .. $Options

if ($LASTEXITCODE -ne 0) {
    Write-Host "::error::CMake configuration failed" -ForegroundColor Red
    exit 1
}

Write-Host "Building JUSTC..." -ForegroundColor Yellow
$Processors = [Environment]::ProcessorCount
cmake --build . --config Release --parallel $Processors

if ($LASTEXITCODE -ne 0) {
    Write-Host "::error::Build failed" -ForegroundColor Red
    exit 1
}

Write-Host "Installing JUSTC..." -ForegroundColor Yellow
cmake --install . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "::error::Installation failed" -ForegroundColor Red
    exit 1
}

Set-Location $CurrentDir
$ExecutablePath = Join-Path $CurrentDir "install_root\bin\justc.exe"
if (Test-Path $ExecutablePath) {
    Write-Host "Done!" -ForegroundColor Green
    Write-Host "Executable location: $ExecutablePath" -ForegroundColor Cyan

    Write-Host "Testing executable..." -ForegroundColor Yellow
    & $ExecutablePath --help
} else {
    Write-Host "::error::CMake error." -ForegroundColor Red
    Write-Host "Available files in bin directory:" -ForegroundColor Yellow
    $BinDir = Join-Path $CurrentDir "install_root\bin"
    if (Test-Path $BinDir) {
        Get-ChildItem $BinDir
    }
    exit 1
}
