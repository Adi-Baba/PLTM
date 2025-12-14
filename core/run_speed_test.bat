@echo off
cd /d "%~dp0"
echo Compiling Benchmark (with KissFFT)...

:: Needs to include the kiss_fft directory
gcc benchmark.c polylog.c kiss_fft/kiss_fft.c -I"./kiss_fft" -o benchmark.exe -O3 -lm

if %errorlevel% neq 0 (
    echo Compilation Failed!
    exit /b %errorlevel%
)

echo.
echo Running Benchmark...
benchmark.exe
echo.
pause
