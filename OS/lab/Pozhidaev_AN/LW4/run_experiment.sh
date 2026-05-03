#!/bin/bash

# --- Настройки эксперимента ---
NUM_READERS=5
NUM_WRITERS=5
DURATION=20  # Продолжительность в секундах
BUFFER_FILE="buffer.bin"

echo "=== Запуск эксперимента (Билет 431417) ==="

# 1. Очистка старых данных
echo "[1/4] Очистка старых логов и буфера..."
rm -f reader_*.csv writer_*.csv $BUFFER_FILE

# 2. Запуск писателей
echo "[2/4] Запуск $NUM_WRITERS писателей..."
for ((i=1; i<=NUM_WRITERS; i++))
do
    ./writer.exe & 
    sleep 0.01
done

sleep 1
# 3. Запуск читателей
echo "[3/4] Запуск $NUM_READERS читателей..."
for ((i=1; i<=NUM_READERS; i++))
do
    ./reader.exe &
    sleep 0.01
done

sleep 0.1

echo "Эксперимент запущен. Сбор данных в течение $DURATION сек..."
sleep $DURATION

# 4. Завершение процессов
echo "[4/4] Завершение процессов..."
# Используем taskkill для Windows или kill для Unix-подобных сред
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    taskkill //F //IM reader.exe > /dev/null 2>&1
    taskkill //F //IM writer.exe > /dev/null 2>&1
else
    pkill reader.exe
    pkill writer.exe
fi

echo "=== Эксперимент окончен ==="
echo "Создано логов:"
ls -1 reader_*.csv writer_*.csv | wc -l

read -p "Нажмите Enter, чтобы закрыть окно..."