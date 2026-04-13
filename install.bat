@echo off
REM ============================================================
REM  Text Expansion Keyboard — Quick Installer
REM  Run this script as Administrator to register the keyboard.
REM ============================================================

setlocal

REM --- Check for admin privileges ---
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo ERROR: This script must be run as Administrator.
    echo Right-click the script and select "Run as administrator".
    echo.
    pause
    exit /b 1
)

REM --- Detect architecture ---
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set "ARCH=x64"
) else (
    set "ARCH=x86"
)

set "DLL_PATH=%~dp0%ARCH%\TextExpansionKeyboard.dll"

if not exist "%DLL_PATH%" (
    echo.
    echo ERROR: DLL not found at %DLL_PATH%
    echo Make sure the %ARCH% folder contains TextExpansionKeyboard.dll.
    echo.
    pause
    exit /b 1
)

echo.
echo Registering Text Expansion Keyboard (%ARCH%) ...
echo.

regsvr32 /s "%DLL_PATH%"
if %errorlevel% neq 0 (
    echo ERROR: Registration failed. See above for details.
    pause
    exit /b 1
)

echo SUCCESS: Text Expansion Keyboard has been registered.
echo.
echo You can now select it in:
echo   Settings -^> Time ^& Language -^> Language ^& Region -^> Keyboard
echo.
echo To unregister later, run:  regsvr32 /u "%DLL_PATH%"
echo.
pause
