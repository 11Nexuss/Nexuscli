@echo off
setlocal enabledelayedexpansion

:: Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting administrator privileges...
    powershell -Command "Start-Process cmd -ArgumentList '/c %0' -Verb RunAs"
    exit /b
)

echo ========================================
echo    NexusCLI Installer
echo ========================================
echo.

set "INSTALL_DIR=%ProgramFiles%\NexusCLI"
set "DOWNLOAD_URL=https://github.com/11Nexuss/Nexuscli/releases/latest/download/nexus.exe"
set "BACKUP_URL=https://github.com/11Nexuss/Nexuscli/releases/download/v1.0.0/nexus.exe"

:: Create installation directory
if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
    if errorlevel 1 (
        echo ERROR: Failed to create installation directory
        pause
        exit /b 1
    )
)

echo [1/5] Installing to: %INSTALL_DIR%

:: Download NexusCLI
echo [2/5] Downloading NexusCLI...
powershell -Command "& {
    try {
        Invoke-WebRequest -Uri '%DOWNLOAD_URL%' -OutFile '%INSTALL_DIR%\nexus.exe' -UseBasicParsing
        if (-not (Test-Path '%INSTALL_DIR%\nexus.exe')) {
            throw 'Primary download failed'
        }
    }
    catch {
        Write-Host 'Primary download failed, trying backup URL...'
        try {
            Invoke-WebRequest -Uri '%BACKUP_URL%' -OutFile '%INSTALL_DIR%\nexus.exe' -UseBasicParsing
        }
        catch {
            Write-Host 'ERROR: Download failed - $($_.Exception.Message)'
            exit 1
        }
    }
}"

if not exist "%INSTALL_DIR%\nexus.exe" (
    echo ERROR: Download failed
    pause
    exit /b 1
)

:: Create version file
echo [3/5] Creating version file...
echo 1.0.0 > "%INSTALL_DIR%\version.txt"

:: Add to PATH
echo [4/5] Adding to system PATH...
setx PATH "%PATH%;%INSTALL_DIR%" /M >nul 2>&1

:: Also update current session PATH temporarily
set "PATH=%PATH%;%INSTALL_DIR%"

:: Create uninstaller
echo [5/5] Creating uninstaller...
echo @echo off > "%INSTALL_DIR%\uninstall.bat"
echo setlocal enabledelayedexpansion >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo echo ======================================== >> "%INSTALL_DIR%\uninstall.bat"
echo echo    NexusCLI Uninstaller >> "%INSTALL_DIR%\uninstall.bat"
echo echo ======================================== >> "%INSTALL_DIR%\uninstall.bat"
echo echo. >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo :: Check if running as administrator >> "%INSTALL_DIR%\uninstall.bat"
echo net session ^>nul 2^>^&1 >> "%INSTALL_DIR%\uninstall.bat"
echo if %%errorLevel%% neq 0 ( >> "%INSTALL_DIR%\uninstall.bat"
echo     echo Requesting administrator privileges... >> "%INSTALL_DIR%\uninstall.bat"
echo     powershell -Command "Start-Process cmd -ArgumentList '/c \"\"%%~f0\"\"' -Verb RunAs" >> "%INSTALL_DIR%\uninstall.bat"
echo     exit /b >> "%INSTALL_DIR%\uninstall.bat"
echo ) >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo set "INSTALL_DIR=%INSTALL_DIR%" >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo echo Removing NexusCLI from PATH... >> "%INSTALL_DIR%\uninstall.bat"
echo set "NEW_PATH=%%PATH:;%INSTALL_DIR%=%%" >> "%INSTALL_DIR%\uninstall.bat"
echo setx PATH "%%NEW_PATH%%" /M ^>nul 2^>^&1 >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo echo Deleting installation directory... >> "%INSTALL_DIR%\uninstall.bat"
echo rmdir /s /q "%%INSTALL_DIR%%" >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo if exist "%%INSTALL_DIR%%" ( >> "%INSTALL_DIR%\uninstall.bat"
echo     echo ERROR: Failed to remove installation directory >> "%INSTALL_DIR%\uninstall.bat"
echo     echo You may need to manually delete: %%INSTALL_DIR%% >> "%INSTALL_DIR%\uninstall.bat"
echo ) else ( >> "%INSTALL_DIR%\uninstall.bat"
echo     echo Successfully uninstalled NexusCLI >> "%INSTALL_DIR%\uninstall.bat"
echo ) >> "%INSTALL_DIR%\uninstall.bat"
echo. >> "%INSTALL_DIR%\uninstall.bat"
echo echo. >> "%INSTALL_DIR%\uninstall.bat"
echo pause >> "%INSTALL_DIR%\uninstall.bat"

:: Create Start Menu shortcut
echo Creating Start Menu shortcut...
powershell -Command "& {
    $WshShell = New-Object -comObject WScript.Shell
    $Shortcut = $WshShell.CreateShortcut('$env:ProgramData\Microsoft\Windows\Start Menu\Programs\NexusCLI.lnk')
    $Shortcut.TargetPath = '%INSTALL_DIR%\nexus.exe'
    $Shortcut.WorkingDirectory = '%INSTALL_DIR%'
    $Shortcut.Save()
}"

:: Test installation
echo.
echo Testing installation...
"%INSTALL_DIR%\nexus.exe" version >nul 2>&1
if errorlevel 1 (
    echo WARNING: Installation test failed
) else (
    echo Installation test successful!
)

echo.
echo ========================================
echo    Installation Complete!
echo ========================================
echo.
echo NexusCLI has been installed to:
echo   %INSTALL_DIR%
echo.
echo You can now use 'nexus' command from anywhere!
echo.
echo Available commands:
echo   nexus new ^<project_name^> [type]
echo   nexus update
echo   nexus version
echo   nexus list
echo   nexus help
echo.
echo Uninstaller location:
echo   %INSTALL_DIR%\uninstall.bat
echo.
echo Note: You may need to restart your command prompt
echo       for the PATH changes to take effect.
echo.
pause