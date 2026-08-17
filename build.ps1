$ErrorActionPreference = "Stop"

function Find-ImageMagick {
    $command = Get-Command magick.exe -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    $roots = @($env:ProgramFiles, ${env:ProgramW6432}) |
        Where-Object { $_ } |
        Select-Object -Unique

    foreach ($root in $roots) {
        $executable = Get-ChildItem -LiteralPath $root -Directory -Filter "ImageMagick*" -ErrorAction SilentlyContinue |
            ForEach-Object { Join-Path $_.FullName "magick.exe" } |
            Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
            Select-Object -First 1

        if ($executable) {
            return $executable
        }
    }

    return $null
}

$magickExecutable = Find-ImageMagick
if (-not $magickExecutable) {
    if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
        throw "ImageMagick est absent et winget est introuvable. Installez ImageMagick depuis https://imagemagick.org."
    }

    Write-Host "ImageMagick est absent : installation automatique en cours..."
    winget install --id ImageMagick.ImageMagick --exact --accept-package-agreements --accept-source-agreements
    $wingetExitCode = $LASTEXITCODE
    $magickExecutable = Find-ImageMagick
    if (-not $magickExecutable) {
        throw "L'installation automatique d'ImageMagick a echoue (code $wingetExitCode)."
    }

    if ($wingetExitCode -ne 0) {
        Write-Host "ImageMagick etait deja installe ; utilisation de l'installation existante."
    }
}

Write-Host "ImageMagick detecte : $magickExecutable"

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
