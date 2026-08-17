@echo off
setlocal

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1"
set "EC2_BUILD_EXIT=%ERRORLEVEL%"

if not "%EC2_BUILD_EXIT%"=="0" (
    echo.
    echo La compilation a echoue avec le code %EC2_BUILD_EXIT%.
)

exit /b %EC2_BUILD_EXIT%
