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
set "DOWNLOAD_URL=https://github.com/11Nexuss/Nexuscli/releases/download/Release/Nexus.exe"

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

:: Method 1: Try with bitsadmin (built into Windows)
echo Trying bitsadmin download...
bitsadmin /transfer "NexusDownload" /download /priority normal "%DOWNLOAD_URL%" "%INSTALL_DIR%\nexus.exe"

:: Method 2: If bitsadmin fails, try certutil
if not exist "%INSTALL_DIR%\nexus.exe" (
    echo Bitsadmin failed, trying certutil...
    certutil -urlcache -split -f "%DOWNLOAD_URL%" "%INSTALL_DIR%\nexus.exe"
)

:: Method 3: If both fail, try PowerShell
if not exist "%INSTALL_DIR%\nexus.exe" (
    echo Certutil failed, trying PowerShell...
    powershell -Command "(New-Object Net.WebClient).DownloadFile('%DOWNLOAD_URL%', '%INSTALL_DIR%\nexus.exe')"
)

if not exist "%INSTALL_DIR%\nexus.exe" (
    echo ERROR: All download methods failed
    echo Please check your internet connection and try again
    echo.
    echo You can also manually download from:
    echo %DOWNLOAD_URL%
    pause
    exit /b 1
)

:: Create version file
echo [3/5] Creating version file...
echo 1.1.0 > "%INSTALL_DIR%\version.txt"

:: Add to PATH
echo [4/5] Adding to system PATH...

:: Get current system PATH
for /f "skip=2 tokens=1,2*" %%a in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path 2^>nul') do (
    if /i "%%a"=="Path" (
        set "current_path=%%c"
    )
)

:: Check if already in PATH
echo %current_path% | find /i "%INSTALL_DIR%" > nul
if errorlevel 1 (
    :: Not in PATH, so add it
    set "new_path=%current_path%;%INSTALL_DIR%"
    reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "!new_path!" /f > nul
)

:: Also update current session PATH temporarily
set "PATH=%PATH%;%INSTALL_DIR%"

:: Create uninstaller
echo [5/5] Creating uninstaller...
(
echo @echo off
echo setlocal enabledelayedexpansion
echo.
echo echo ========================================
echo echo    NexusCLI Uninstaller
echo echo ========================================
echo echo.
echo.
echo :: Check if running as administrator
echo net session ^>nul 2^>^&1
echo if %%errorLevel%% neq 0 (
echo     echo Requesting administrator privileges...
echo     powershell -Command "Start-Process cmd -ArgumentList '/c \"\"%%~f0\"\"' -Verb RunAs"
echo     exit /b
echo )
echo.
echo set "INSTALL_DIR=%INSTALL_DIR%"
echo.
echo echo Removing NexusCLI from system PATH...
echo for /f "skip=2 tokens=1,2*" %%a in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path 2^>nul'^) do (
echo     if /i "%%a"=="Path" set "current_path=%%c"
echo )
echo.
echo set "new_path=%%current_path:;%INSTALL_DIR%=%%"
echo set "new_path=%%new_path:;%INSTALL_DIR%=%%"
echo reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%%new_path%%" /f ^> nul
echo.
echo echo Deleting installation directory...
echo timeout /t 2 /nobreak ^> nul
echo rmdir /s /q "%%INSTALL_DIR%%" 2^>nul
echo.
echo if exist "%%INSTALL_DIR%%" (
echo     echo ERROR: Failed to remove installation directory
echo     echo You may need to manually delete: %%INSTALL_DIR%%
echo ) else (
echo     echo Successfully uninstalled NexusCLI
echo )
echo.
echo echo.
echo pause
) > "%INSTALL_DIR%\uninstall.bat"

:: Create Start Menu shortcut
echo Creating Start Menu shortcut...
powershell -Command "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('$env:ProgramData\Microsoft\Windows\Start Menu\Programs\NexusCLI.lnk'); $Shortcut.TargetPath = '%INSTALL_DIR%\nexus.exe'; $Shortcut.WorkingDirectory = '%INSTALL_DIR%'; $Shortcut.Save()" >nul 2>&1

:: Test installation
echo.
echo Testing installation...
"%INSTALL_DIR%\nexus.exe" version >nul 2>&1
if errorlevel 1 (
    echo WARNING: Installation test failed - but installation completed
    echo You may need to restart your command prompt and try 'nexus version'
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