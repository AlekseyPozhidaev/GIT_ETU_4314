#!/bin/bash

PROG="./lw3e1.exe"
OUTPUT="results1.csv"

# Проверяем, существует ли программа
if [ ! -x "$PROG" ]; then
    echo "Ошибка: программа $PROG не найдена или не исполняема."
    echo "Сначала скомпилируйте: gcc -fopenmp -O2 -o pi_openmp pi_openmp.c"
    exit 1
fi

# Список чисел потоков для теста
THREADS_LIST=(1 2 4 8 12 16)

# Заголовок CSV
echo "threads,time_seconds,pi_value" > $OUTPUT

# Прогон для каждого числа потоков
for threads in "${THREADS_LIST[@]}"; do
    echo "Запуск с $threads поток(ов)..."
    OUTPUT_LINE=$($PROG $threads)
    # Пример OUTPUT_LINE: "pi=3.1415926436 time=0.059967"
    # Извлекаем время и значение pi
    pi=$(echo $OUTPUT_LINE | sed -n 's/.*pi=\([0-9.e+-]*\).*/\1/p')
    time=$(echo $OUTPUT_LINE | sed -n 's/.*time=\([0-9.e+-]*\).*/\1/p')
    echo "$threads,$time,$pi" >> $OUTPUT
    echo "  $OUTPUT_LINE"
done

echo "Готово. Результаты сохранены в $OUTPUT"