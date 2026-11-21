$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root "build"

Write-Host "Configuration CMake..."
cmake -S $root -B $build

Write-Host "Compilation..."
cmake --build $build --config Debug

Write-Host "Vérification des tests..."
ctest --test-dir $build -C Debug --output-on-failure
