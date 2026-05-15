@echo off
echo Starting Readers and Writers experiment...

:: Запуск 10 процессов-читателей в фоновом режиме
for /L %%i in (1,1,10) do (
    start Reader.exe
)

:: Запуск 6 процессов-писателей в фоновом режиме
for /L %%i in (1,1,10) do (
    start Writer.exe
)

echo All processes started. Please wait for them to finish loop iterations...