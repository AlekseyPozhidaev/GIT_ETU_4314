@echo off
chcp 65001 >nul

echo =====================================================
echo     Lab 4.1 - Readers-Writers Problem
echo     Student: 431417  Pages: 16
echo =====================================================
echo.

set NUM_WRITERS=6
set NUM_READERS=8
set DURATION=45

echo [1/4] Cleaning old files...
del /Q reader_*.csv writer_*.csv buffer.bin 2>nul

echo [2/4] Starting %NUM_WRITERS% Writers...
for /L %%i in (1,1,%NUM_WRITERS%) do (
    echo   Launching writer %%i
    start "" writer.exe
    timeout /t 1 >nul
)

echo.
timeout /t 2 >nul

echo [3/4] Starting %NUM_READERS% Readers...
for /L %%i in (1,1,%NUM_READERS%) do (
    echo   Launching reader %%i
    start "" reader.exe
    timeout /t 1 >nul
)

echo.
echo [4/4] Experiment running for %DURATION% seconds...
timeout /t %DURATION% >nul

echo.
echo Terminating processes...
taskkill /F /IM writer.exe >nul 2>&1
taskkill /F /IM reader.exe >nul 2>&1

echo.
echo Experiment completed!
echo Log files: 
dir /b reader_*.csv writer_*.csv 2>nul | find /c /v ""
echo.
pause