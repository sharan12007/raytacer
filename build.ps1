# ==========================================
# Ray Tracer - GLFW Window Build Script
# ==========================================

$ErrorActionPreference = "Stop"

Write-Host "`n[1/2] Configuring CMake..." -ForegroundColor Cyan

cmake -S . -B build

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n[2/2] Building project..." -ForegroundColor Cyan

cmake --build build --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`nStarting Ray Tracer Window..." -ForegroundColor Green

& ".\build\Release\renderer.exe"
