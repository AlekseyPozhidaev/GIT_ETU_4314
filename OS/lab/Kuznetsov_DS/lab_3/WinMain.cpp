#include <cstdio>
#include <iostream>
#include <vector>
#include <windows.h>

const long long N = 100000000;
const long long BLOCK_SIZE = 4314100;

CRITICAL_SECTION cs;

long long currentIndex = 0;
double globalSum = 0.0;
bool done = false;

struct ThreadData {
	int id;
	HANDLE handle;
};

// Функция потока
DWORD WINAPI ThreadFunc(LPVOID param) {
	ThreadData* data = (ThreadData*)param;

	while (true) {
		// Ждём, пока нас "разбудят"
		SuspendThread(GetCurrentThread());

		if (done) break;

		long long start, end;

		// Берём блок
		EnterCriticalSection(&cs);
		if (currentIndex >= N) {
			LeaveCriticalSection(&cs);
			continue;
		}

		start = currentIndex;
		end = (start + BLOCK_SIZE > N) ? N : start + BLOCK_SIZE;
		currentIndex = end;
		LeaveCriticalSection(&cs);

		double localSum = 0.0;

		// Вычисление
		for (long long i = start; i < end; ++i) {
			double term = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
			localSum += term;
		}

		// Добавляем в общий результат
		EnterCriticalSection(&cs);
		globalSum += localSum;
		LeaveCriticalSection(&cs);
	}

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: pi_calc.exe <num_threads>\n");
		return 1;
	}

	int numThreads = atoi(argv[1]);
	if (numThreads <= 0) {
		printf("Invalid thread count\n");
		return 1;
	}

	if (!InitializeCriticalSectionAndSpinCount(&cs, 4000)) {
		printf("Failed to initialize critical section\n");
		return 1;
	}

	std::vector<ThreadData> threads(numThreads);

	// Таймер
	LARGE_INTEGER t1, t2, freq;

	if (!QueryPerformanceFrequency(&freq)) {
		printf("High-resolution timer not supported\n");
		return 1;
	}

	QueryPerformanceCounter(&t1);

	// Создание потоков (в suspended)
	for (int i = 0; i < numThreads; ++i) {
		threads[i].id = i;

		threads[i].handle = CreateThread(
			NULL,
			0,
			ThreadFunc,
			&threads[i],
			CREATE_SUSPENDED,
			NULL
		);

		if (!threads[i].handle) {
			printf("Failed to create thread %d\n", i);
			return 1;
		}
	}

	// Основной цикл распределения
	while (true) {
		EnterCriticalSection(&cs);
		bool finished = (currentIndex >= N);
		LeaveCriticalSection(&cs);

		if (finished) break;

		// Будим все потоки
		for (int i = 0; i < numThreads; ++i) {
			ResumeThread(threads[i].handle);
		}

		Sleep(1); // чтобы не грузить CPU
	}

	// Завершаем потоки
	done = true;

	for (int i = 0; i < numThreads; ++i) {
		ResumeThread(threads[i].handle);
	}

	// Ждём завершения
	std::vector<HANDLE> handles(numThreads);
	for (int i = 0; i < numThreads; ++i) {
		handles[i] = threads[i].handle;
	}

	WaitForMultipleObjects(numThreads, handles.data(), TRUE, INFINITE);

	// Останавливаем таймер
	QueryPerformanceCounter(&t2);

	double time = (double)(t2.QuadPart - t1.QuadPart) / freq.QuadPart;
	double pi = 4.0 * globalSum;

	printf("threads=%d pi=%.10f time=%.6f\n", numThreads, pi, time);

	// Очистка
	for (int i = 0; i < numThreads; ++i) {
		CloseHandle(threads[i].handle);
	}

	DeleteCriticalSection(&cs);

	return 0;
}