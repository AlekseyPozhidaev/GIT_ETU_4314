#include <iostream>
#include <omp.h>

using namespace std;

int main() {
	const long long N = 100000000;
	const int chunk_size = 4314100;
	double pi = 0.0;

	int thread_counts[] = { 1, 2, 4, 8, 12, 16 };

	for (int t = 0; t < 6; t++) {
		int num_threads = thread_counts[t];
		double sum = 0.0;

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