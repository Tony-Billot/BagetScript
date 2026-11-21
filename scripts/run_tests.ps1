$ErrorActionPreference = "Stop"

$OutputEncoding = [System.Text.UTF8Encoding]::new($false)
[Console]::InputEncoding = [System.Text.UTF8Encoding]::new($false)
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
cmd.exe /d /c "chcp 65001>nul"

$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root "build"

Write-Host "Configuration CMake..."
cmake -S $root -B $build

Write-Host "Compilation..."
cmake --build $build --config Debug

Write-Host "Vérification des tests..."
ctest --test-dir $build -C Debug --output-on-failure
