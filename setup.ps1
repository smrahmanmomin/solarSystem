# ============================================================
# Solar System Project - Setup Script
# Downloads GLAD, stb_image, and planet textures
# ============================================================

Write-Host "=== Solar System Project Setup ===" -ForegroundColor Cyan

# ── Create directories ──────────────────────────────────────
$dirs = @(
    "vendor/glad/include/glad",
    "vendor/glad/include/KHR",
    "vendor/glad/src",
    "vendor",
    "textures",
    "build"
)
foreach ($d in $dirs) {
    if (-not (Test-Path $d)) {
        New-Item -ItemType Directory -Path $d -Force | Out-Null
        Write-Host "  Created: $d" -ForegroundColor Gray
    }
}

# ── Download stb_image.h ────────────────────────────────────
$stbUrl = "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
$stbDst = "vendor/stb_image.h"
if (-not (Test-Path $stbDst)) {
    Write-Host "Downloading stb_image.h..." -ForegroundColor Yellow
    Invoke-WebRequest -Uri $stbUrl -OutFile $stbDst
    Write-Host "  Done." -ForegroundColor Green
} else {
    Write-Host "stb_image.h already exists." -ForegroundColor Green
}

# ── Generate GLAD using Python ──────────────────────────────
$gladH = "vendor/glad/include/glad/glad.h"
if (-not (Test-Path $gladH)) {
    Write-Host "Generating GLAD (OpenGL 3.3 Core)..." -ForegroundColor Yellow
    
    # Try using pip glad package
    $pythonCmd = $null
    foreach ($cmd in @("python", "python3", "py")) {
        try {
            $ver = & $cmd --version 2>&1
            if ($ver -match "Python") {
                $pythonCmd = $cmd
                break
            }
        } catch {}
    }
    
    if ($pythonCmd) {
        Write-Host "  Using Python: $pythonCmd" -ForegroundColor Gray
        & $pythonCmd -m pip install glad --quiet 2>$null
        & $pythonCmd -m glad --generator c --out-path vendor/glad --api "gl=3.3" --profile core --extensions ""
        
        if (Test-Path $gladH) {
            Write-Host "  GLAD generated successfully." -ForegroundColor Green
        } else {
            Write-Host "  GLAD generation failed. Please download manually from https://glad.dav1d.de/" -ForegroundColor Red
            Write-Host "  Settings: Language=C, API=gl3.3, Profile=Core, Generate a loader=yes" -ForegroundColor Red
            Write-Host "  Extract to vendor/glad/" -ForegroundColor Red
        }
    } else {
        Write-Host "  Python not found. Please download GLAD manually from https://glad.dav1d.de/" -ForegroundColor Red
        Write-Host "  Settings: Language=C/C++, API=gl:3.3, Profile=Core" -ForegroundColor Red
        Write-Host "  Place files in vendor/glad/ (include/ and src/ subdirs)" -ForegroundColor Red
    }
} else {
    Write-Host "GLAD already exists." -ForegroundColor Green
}

# ── Download Planet Textures ────────────────────────────────
# Using Solar System Scope free textures (CC BY 4.0)
$textureBase = "https://www.solarsystemscope.com/textures/download"
$textures = @{
    "sun.jpg"     = "2k_sun.jpg"
    "mercury.jpg" = "2k_mercury.jpg"
    "venus.jpg"   = "2k_venus_surface.jpg"
    "earth.jpg"   = "2k_earth_daymap.jpg"
    "mars.jpg"    = "2k_mars.jpg"
    "jupiter.jpg" = "2k_jupiter.jpg"
    "saturn.jpg"  = "2k_saturn.jpg"
    "uranus.jpg"  = "2k_uranus.jpg"
    "neptune.jpg" = "2k_neptune.jpg"
    "stars.jpg"   = "2k_stars_milky_way.jpg"
}

Write-Host "Downloading planet textures..." -ForegroundColor Yellow
foreach ($entry in $textures.GetEnumerator()) {
    $dst = "textures/$($entry.Key)"
    if (-not (Test-Path $dst)) {
        $url = "$textureBase/$($entry.Value)"
        Write-Host "  Downloading $($entry.Key)..." -ForegroundColor Gray
        try {
            Invoke-WebRequest -Uri $url -OutFile $dst -TimeoutSec 30
            Write-Host "    OK" -ForegroundColor Green
        } catch {
            Write-Host "    Failed: $($_.Exception.Message)" -ForegroundColor Red
            Write-Host "    Please download manually from https://www.solarsystemscope.com/textures/" -ForegroundColor Yellow
        }
    } else {
        Write-Host "  $($entry.Key) already exists." -ForegroundColor Green
    }
}

# ── Saturn ring texture placeholder ─────────────────────────
$ringDst = "textures/saturn_ring.jpg"
if (-not (Test-Path $ringDst)) {
    $ringUrl = "$textureBase/2k_saturn_ring_alpha.png"
    Write-Host "  Downloading saturn_ring..." -ForegroundColor Gray
    try {
        Invoke-WebRequest -Uri $ringUrl -OutFile "textures/saturn_ring.png" -TimeoutSec 30
        Write-Host "    OK" -ForegroundColor Green
    } catch {
        Write-Host "    Saturn ring texture download failed (optional)." -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "=== Setup Complete ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "Build instructions:" -ForegroundColor White
Write-Host "  cd build" -ForegroundColor Gray
Write-Host "  cmake .." -ForegroundColor Gray
Write-Host "  cmake --build . --config Release" -ForegroundColor Gray
Write-Host ""
Write-Host "If GLAD download failed, visit: https://glad.dav1d.de/" -ForegroundColor Yellow
Write-Host "  Language: C/C++, API: gl 3.3, Profile: Core" -ForegroundColor Yellow
Write-Host "  Extract into vendor/glad/" -ForegroundColor Yellow
