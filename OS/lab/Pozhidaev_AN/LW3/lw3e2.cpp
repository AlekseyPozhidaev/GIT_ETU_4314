/**
 * Программа вычисления числа π с использованием OpenMP.
 * Компиляция: gcc -fopenmp -O2 -o pi_openmp pi_openmp.c
 * Запуск:     ./pi_openmp <число_потоков>
 * 
 * Пример вывода: pi=3.1415926436 time=0.059967
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ITERATIONS  100000000
#define STUDENT_ID  431417
#define CHUNK_SIZE  (10 * STUDENT_ID)   // 4 314 170

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads < 1) {
        fprintf(stderr, "Number of threads must be >= 1\n");
        return 1;
    }

    omp_set_num_threads(num_threads);

    double sum = 0.0;
    double start = omp_get_wtime();

    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) reduction(+:sum)
    for (int k = 0; k < ITERATIONS; ++k) {
        double term = (k % 2 == 0) ? 1.0 : -1.0;
        term /= (2.0 * k + 1.0);
        sum += term;
    }

    double end = omp_get_wtime();
    double pi = 4.0 * sum;

    // Выводим в формате, который просил пользователь
    printf("pi=%.10f time=%.6f\n", pi, end - start);
    return 0;
}