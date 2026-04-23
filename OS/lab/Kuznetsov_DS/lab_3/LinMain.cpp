#include <cstdlib>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char* argv[]) {
	const long long N = 100000000;
	const int chunk_size = 4314100;

	// Проверка аргументов командной строки
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <num_threads1> [num_threads2] [num_threads3] ..." << endl;
		cerr << "Example: " << argv[0] << " 1 2 4 8 12 16" << endl;
		return 1;
	}

	// Перебираем все аргументы (первый аргумент - имя программы, поэтому начинаем с 1)
	for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
		int num_threads = atoi(argv[arg_idx]);

		if (num_threads <= 0) {
			cerr << "Error: Invalid number of threads: " << argv[arg_idx] << endl;
			continue;
		}

		double sum = 0.0;
		double pi = 0.0;

		omp_set_num_threads(num_threads);

		double start = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, chunk_size) reduction(+:sum)
		for (long long i = 0; i < N; i++) {
			double x = (i + 0.5) / N;
			sum += 4.0 / (1.0 + x * x);
		}

		pi = sum / N;

		double end = omp_get_wtime();

		cout << "Threads: " << num_threads
			<< " | PI = " << pi
			<< " | Time = " << (end - start) << " sec"
			<< endl;
	}

	return 0;
}