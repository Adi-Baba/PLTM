@echo off
setlocal
echo ==========================================
echo       PLTM RELEASE PACKAGER (v1.0)
echo ==========================================

set "DIST_DIR=PLTM_SDK_v1"

echo [1/4] Building Core Engine...
cd core
call build_core.bat
cd ..
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [2/4] Creating Distribution Directory: %DIST_DIR%...
if exist "%DIST_DIR%" rd /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"
mkdir "%DIST_DIR%\bin"
mkdir "%DIST_DIR%\python"
mkdir "%DIST_DIR%\examples"

echo [3/4] Copying Assets...
REM Core Binaries
copy "core\polylog.dll" "%DIST_DIR%\bin\" >nul
copy "core\polylog.h" "%DIST_DIR%\bin\" >nul

REM Python Bindings (Bundle DLL for pip install)
mkdir "%DIST_DIR%\pltm"
copy "bindings\pltm_core.py" "%DIST_DIR%\pltm\__init__.py" >nul
copy "core\polylog.dll" "%DIST_DIR%\pltm\" >nul
copy "setup.py" "%DIST_DIR%\" >nul

REM Examples/Docs
copy "README.md" "%DIST_DIR%\" >nul
xcopy "bindings\needle_test.py" "%DIST_DIR%\examples\"* >nul

echo [4/4] Finalizing...
echo.
echo ==========================================
echo        PACKAGE READY: %DIST_DIR%
echo ==========================================
echo This folder contains the compiled DLL and Python wrappers.
echo Ready for deployment to production servers.
pause
