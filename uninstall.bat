@echo off
setlocal enabledelayedexpansion

echo ========================================
echo    NexusCLI Uninstaller
echo ========================================
echo.

:: Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting administrator privileges...
    powershell -Command "Start-Process cmd -ArgumentList '/c \"%~f0\"' -Verb RunAs"
    exit /b
)

set "INSTALL_DIR=%ProgramFiles%\NexusCLI"

if not exist "%INSTALL_DIR%" (
    echo NexusCLI is not installed or already removed.
    pause
    exit /b
)

echo Removing NexusCLI from PATH...
set "NEW_PATH=%PATH:;%INSTALL_DIR%=%"
setx PATH "%NEW_PATH%" /M >nul 2>&1

echo Deleting installation directory...
rmdir /s /q "%INSTALL_DIR%"

if exist "%INSTALL_DIR%" (
    echo ERROR: Failed to remove installation directory
    echo You may need to manually delete: %INSTALL_DIR%
) else (
    echo Successfully uninstalled NexusCLI
)

echo.
pause