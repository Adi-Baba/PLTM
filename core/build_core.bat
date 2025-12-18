@echo off
echo Building Polylog Core (with KissFFT)...
cd /d "%~dp0"
gcc -shared -o polylog.dll polylog.c kiss_fft/kiss_fft.c -I"./kiss_fft" -O3 -lm -static
if %errorlevel% neq 0 (
    echo Build Failed!
    exit /b %errorlevel%
)
echo Build Success: polylog.dll created.
dir polylog.dll
