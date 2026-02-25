#!/bin/bash

EXE="./linux"          
SRC_FILE="testfile.bat"
DST_FILE="cptestfile.bat"
RESULTS_BLOCKS="results_blocks.txt"
RESULTS_OPS="results_ops.txt"
NUM_TRIES=3 # Количество замеров для каждого параметра

# 1. Создаем тестовый файл (500 МБ), если его нет
if [ ! -f $SRC_FILE ]; then
    echo "Создание тестового файла..."
    dd if=/dev/urandom of=$SRC_FILE bs=1M count=500 status=progress
fi

echo "Начало эксперимента (замеров на точку: $NUM_TRIES)..."
echo "Размер_блока_КБ Среднее_время_сек" > $RESULTS_BLOCKS

# --- Эксперимент 1: Зависимость от размера блока ---
FIXED_OPS=8
echo "--- Тестирование размера блоков (при ops=$FIXED_OPS) ---"

for block in 4 16 64 256 1024 4096 8192 16384 32768 65536; do
    echo -n "Блок ${block}KB: "
    SUM_TIME=0
    
    for (( i=1; i<=$NUM_TRIES; i++ )); do
        # Сброс системного кэша (желательно, если есть sudo)
        # sudo sync; echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
        
        # Выполнение замера
        TIME=$($EXE $SRC_FILE $DST_FILE $block $FIXED_OPS | grep -oP 'Time: \K[0-9.]+')
        
        # Проверка целостности при первом проходе для этого блока
        if [ $i -eq 1 ]; then
            if ! cmp -s "$SRC_FILE" "$DST_FILE"; then
                echo "ОШИБКА: Файлы различаются!"
                exit 1
            fi
        fi
        
        echo -n "$TIME.. "
        SUM_TIME=$(echo "$SUM_TIME + $TIME" | bc -l)
    done
    
    AVG_TIME=$(echo "$SUM_TIME / $NUM_TRIES" | bc -l)
    printf "Среднее: %.4f\n" $AVG_TIME
    echo "$block $AVG_TIME" >> $RESULTS_BLOCKS
done

# --- Эксперимент 2: Зависимость от числа операций ---
FIXED_BLOCK=1024
echo ""
echo "--- Тестирование числа операций (при block=${FIXED_BLOCK}KB) ---"
echo "Число_операций Среднее_время_сек" > $RESULTS_OPS

for ops in 1 2 4 8 12 16 24 32; do
    echo -n "Операций ${ops}: "
    SUM_TIME=0
    
    for (( i=1; i<=$NUM_TRIES; i++ )); do
        # sudo sync; echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
        
        TIME=$($EXE $SRC_FILE $DST_FILE $FIXED_BLOCK $ops | grep -oP 'Time: \K[0-9.]+')
        
        if [ $i -eq 1 ]; then
            if ! cmp -s "$SRC_FILE" "$DST_FILE"; then
                echo "ОШИБКА: Файлы различаются!"
                exit 1
            fi
        fi
        
        echo -n "$TIME.. "
        SUM_TIME=$(echo "$SUM_TIME + $TIME" | bc -l)
    done
    
    AVG_TIME=$(echo "$SUM_TIME / $NUM_TRIES" | bc -l)
    printf "Среднее: %.4f\n" $AVG_TIME
    echo "$ops $AVG_TIME" >> $RESULTS_OPS
done

echo ""
echo "Эксперимент завершен. Данные сохранены в $RESULTS_BLOCKS и $RESULTS_OPS"