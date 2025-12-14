@echo off
echo ==========================================
echo        PLTM PRODUCTION CHECK
echo ==========================================

echo [1/2] Building Polylog Core...
cd core
call build_core.bat
if %errorlevel% neq 0 (
    echo [ERROR] Build Failed.
    cd ..
    exit /b 1
)
cd ..
echo [OK] Core Built.

echo.
echo [2/2] Verifying Python Bindings...
python -c "import sys; sys.path.append('bindings'); from pltm_core import PLTM_Engine; eng = PLTM_Engine(100, 2.0); res = eng.process([1.0]*10); print('Engine Output:', res[:5]); print('[OK] Engine Functional')"
if %errorlevel% neq 0 (
    echo [ERROR] Python Bindings Failed.
    exit /b 1
)

echo.
echo ==========================================
echo        PLTM SYSTEM OPERATIONAL
echo ==========================================
