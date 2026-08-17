$ErrorActionPreference = "Stop"
Set-Location -LiteralPath $PSScriptRoot

function Assert-Winget {
    if (-not (Get-Command winget.exe -ErrorAction SilentlyContinue)) {
        throw "winget est necessaire pour installer automatiquement les composants manquants."
    }
}

function Find-CMake {
    $command = Get-Command cmake.exe -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }

    $candidate = Join-Path $env:ProgramFiles "CMake\bin\cmake.exe"
    if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
    return $null
}

function Find-CppBuildTools {
    $roots = @(${env:ProgramFiles(x86)}, $env:ProgramFiles) |
        Where-Object { $_ } |
        Select-Object -Unique

    foreach ($root in $roots) {
        $pattern = Join-Path $root "Microsoft Visual Studio\2022\*\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
        $compiler = Resolve-Path -Path $pattern -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($compiler) { return $compiler.Path }
    }
    return $null
}

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

$cmakeExecutable = Find-CMake
if (-not $cmakeExecutable) {
    Assert-Winget
    Write-Host "CMake est absent : installation automatique en cours..."
    winget install --id Kitware.CMake --exact --accept-package-agreements --accept-source-agreements
    $cmakeExitCode = $LASTEXITCODE
    $cmakeExecutable = Find-CMake
    if (-not $cmakeExecutable) {
        throw "L'installation automatique de CMake a echoue (code $cmakeExitCode)."
    }
}
Write-Host "CMake detecte : $cmakeExecutable"

$compilerExecutable = Find-CppBuildTools
if (-not $compilerExecutable) {
    Assert-Winget
    Write-Host "Visual Studio Build Tools C++ est absent : installation automatique en cours..."
    Write-Host "Cette operation peut prendre plusieurs minutes et demander une autorisation Windows."
    winget install --id Microsoft.VisualStudio.2022.BuildTools --exact `
        --accept-package-agreements --accept-source-agreements `
        --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
    $buildToolsExitCode = $LASTEXITCODE
    $compilerExecutable = Find-CppBuildTools
    if (-not $compilerExecutable) {
        throw "L'installation des outils C++ a echoue ou doit etre finalisee (code $buildToolsExitCode)."
    }
}
Write-Host "Compilateur C++ detecte : $compilerExecutable"

& $cmakeExecutable -S . -B build
if ($LASTEXITCODE -ne 0) { throw "La configuration CMake a echoue (code $LASTEXITCODE)." }

& $cmakeExecutable --build build --config Release
if ($LASTEXITCODE -ne 0) { throw "La compilation a echoue (code $LASTEXITCODE)." }

if (Test-Path "build/Release/ec2.exe") {
    Copy-Item "build/Release/ec2.exe" "ec2.exe" -Force
} elseif (Test-Path "build/ec2.exe") {
    Copy-Item "build/ec2.exe" "ec2.exe" -Force
} else {
    throw "Compilation terminee, mais ec2.exe est introuvable."
}

Write-Host "Termine : $PWD/ec2.exe"
