$ErrorActionPreference = "Stop"

if (-not (Get-Command magick -ErrorAction SilentlyContinue)) {
    if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
        throw "ImageMagick est absent et winget est introuvable. Installez ImageMagick depuis https://imagemagick.org."
    }

    Write-Host "ImageMagick est absent : installation automatique en cours..."
    winget install --id ImageMagick.ImageMagick --exact --accept-package-agreements --accept-source-agreements
    if ($LASTEXITCODE -ne 0) {
        throw "L'installation automatique d'ImageMagick a echoue (code $LASTEXITCODE)."
    }
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "CMake est introuvable. Installez Visual Studio Build Tools avec le composant C++, puis CMake."
}

cmake -S . -B build
cmake --build build --config Release

if (Test-Path "build/Release/ec2.exe") {
    Copy-Item "build/Release/ec2.exe" "ec2.exe" -Force
} elseif (Test-Path "build/ec2.exe") {
    Copy-Item "build/ec2.exe" "ec2.exe" -Force
} else {
    throw "Compilation terminee, mais ec2.exe est introuvable."
}

Write-Host "Termine : $PWD/ec2.exe"
