#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h> // Для _aligned_malloc

#define N 100000000
#define STUDENT_TICKET 431417
#define BLOCK_SIZE (10LL * STUDENT_TICKET)  // 4 314 170

// Выравнивание структуры на 64 байта предотвращает False Sharing (сброс кэша ядер)
__declspec(align(64)) typedef struct {
    int id;
    HANDLE event_done;
    long long start;
    long long end;
    double local_sum;
    volatile int stop;
    volatile int is_suspending;
} ThreadData;

DWORD WINAPI WorkerThread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    while (1) {
        if (data->stop) break;

        double sum = 0.0;
    
        // Математика, идентичная OpenMP программе
        for (long long k = data->start; k < data->end; ++k) {
            // Убираем if, заменяя его на формулу знака: (1.0 - 2.0 * (k & 1))
            // Это превращает условие в простую арифметику
            double term = (1.0 - 2.0 * (k % 2)) / (2.0 * k + 1.0);
            sum += term;
        }
    
        data->local_sum = sum;
        SetEvent(data->event_done);
        data->is_suspending = 1;
        SuspendThread(GetCurrentThread());
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads < 1) return 1;

    HANDLE* threads = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
    HANDLE* events = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
    // Выделяем выровненную память
    ThreadData* thread_data = (ThreadData*)_aligned_malloc(num_threads * sizeof(ThreadData), 64);

    for (int i = 0; i < num_threads; ++i) {
        events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        thread_data[i].id = i;
        thread_data[i].event_done = events[i];
        thread_data[i].stop = 0;
        thread_data[i].is_suspending = 0;
        thread_data[i].local_sum = 0.0;

        threads[i] = CreateThread(NULL, 0, WorkerThread, &thread_data[i], CREATE_SUSPENDED, NULL);
    }

    long long current_block = 0;
    double total_sum = 0.0;
    int active_threads = 0;
    int started_threads = 0;

    LARGE_INTEGER freq, start_time, end_time;
    QueryPerformanceFrequency(&freq);
    
    // ==========================================
    // НАЧАЛО ЧЕСТНОГО ЗАМЕРА ВРЕМЕНИ
    QueryPerformanceCounter(&start_time);
    // ==========================================

    // Первичная раздача блоков
    for (int i = 0; i < num_threads && current_block < N; ++i) {
        thread_data[i].start = current_block;
        long long end = current_block + BLOCK_SIZE;
        thread_data[i].end = (end > N) ? N : end;
        current_block += BLOCK_SIZE;
        
        started_threads++;
        active_threads++;
        ResumeThread(threads[i]);
    }

    // Динамический планировщик
    while (active_threads > 0) {
        // Главный поток "спит" здесь, не тратя процессор
        DWORD dwWait = WaitForMultipleObjects(num_threads, events, FALSE, INFINITE);
        int idx = dwWait - WAIT_OBJECT_0;

        // Забираем данные без критических секций
        total_sum += thread_data[idx].local_sum;
        active_threads--;

        if (current_block < N) {
            thread_data[idx].start = current_block;
            long long end = current_block + BLOCK_SIZE;
            thread_data[idx].end = (end > N) ? N : end;
            current_block += BLOCK_SIZE;

            // БЕЗОПАСНАЯ СИНХРОНИЗАЦИЯ:
            // 1. Ждем, пока рабочий поток выставит флаг (означает, что он дошел до SuspendThread)
            while (thread_data[idx].is_suspending == 0) {
                YieldProcessor(); 
            }
            thread_data[idx].is_suspending = 0; // Сброс для следующего раза

            // 2. Гарантируем, что ОС действительно успела заморозить поток.
            // ResumeThread возвращает количество приостановок. 0 означает, что поток еще работал.
            while (ResumeThread(threads[idx]) == 0) {
                YieldProcessor();
            }
            
            active_threads++;
        }
    }

    // ==========================================
    // КОНЕЦ ЧЕСТНОГО ЗАМЕРА ВРЕМЕНИ
    QueryPerformanceCounter(&end_time);
    // ==========================================

    // Вычисление итогового Пи (по формуле)
    double pi = 4.0 * total_sum;
    
    double elapsed = (double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart;
    printf("pi=%.10f time=%.6f\n", pi, elapsed);

    // Аккуратное и безопасное завершение всех созданных потоков
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].stop = 1;
        if (i < started_threads) {
            // Если поток работал, ждем, пока он заснет после своего последнего блока
            while (thread_data[i].is_suspending == 0) { YieldProcessor(); }
            while (ResumeThread(threads[i]) == 0) { YieldProcessor(); }
        } else {
            // Если поток вообще ни разу не получил блок (при N < BLOCK_SIZE * num_threads)
            ResumeThread(threads[i]);
        }
    }

    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);

    // Очистка памяти
    for (int i = 0; i < num_threads; ++i) {
        CloseHandle(events[i]);
        CloseHandle(threads[i]);
    }
    free(threads);
    free(events);
    _aligned_free(thread_data);

    return 0;
}