#define _GNU_SOURCE
#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

struct aio_operation {
	struct aiocb aio;
	char* buffer;
	int write_operation;
	void* next_operation;
};

pthread_mutex_t offset_mutex = PTHREAD_MUTEX_INITIALIZER;

off_t file_size;
off_t next_read_offset = 0;
int read_fd, write_fd;
size_t chunk_size;

void aio_completion_handler(sigval_t sigval) {
	struct aio_operation* aio_op = (struct aio_operation*)sigval.sival_ptr;

	int err = aio_error(&aio_op->aio);
	if (err != 0) {
		if (err != EINPROGRESS) {
			fprintf(stderr, "AIO error: %d\n", err);
		}
		return;
	}

	ssize_t ret = aio_return(&aio_op->aio);
	if (ret != (ssize_t)aio_op->aio.aio_nbytes) {
		fprintf(stderr, "Partial I/O: %" PRId64 " against %zu\n", (int64_t)ret, aio_op->aio.aio_nbytes);
		return;
	}

	if (aio_op->write_operation) {
		pthread_mutex_lock(&offset_mutex);
		if (next_read_offset >= file_size) {
			pthread_mutex_unlock(&offset_mutex);
			return;
		}
		off_t offset = next_read_offset;
		size_t len = (size_t)min((off_t)chunk_size, file_size - offset);
		next_read_offset += (off_t)len;
		pthread_mutex_unlock(&offset_mutex);

		aio_op->aio.aio_fildes = read_fd;
		aio_op->aio.aio_offset = offset;
		aio_op->aio.aio_nbytes = len;
		aio_op->aio.aio_buf = aio_op->buffer;
		aio_op->write_operation = 0;

		if (aio_read(&aio_op->aio) == -1) {
			perror("aio_read (next chunk)");
		}
	}
	else {
		aio_op->aio.aio_fildes = write_fd;
		aio_op->write_operation = 1;

		if (aio_write(&aio_op->aio) == -1) {
			perror("aio_write");
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <origin_file> <destination_file> <n>\n", argv[0]);
		exit(1);
	}

	char* read_filename = argv[1];
	char* write_filename = argv[2];
	int n = atoi(argv[3]);
	if (n <= 0) n = 1;

	read_fd = open(read_filename, O_RDONLY | O_NONBLOCK);
	if (read_fd < 0) {
		perror("open read");
		exit(1);
	}

	write_fd = open(write_filename, O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);
	if (write_fd < 0) {
		perror("open write (create/open)");
		close(read_fd);
		exit(1);
	}

	struct stat st;
	if (fstat(read_fd, &st) == -1) {
		perror("fstat");
		close(read_fd);
		close(write_fd);
		exit(1);
	}
	file_size = st.st_size;
	if (file_size == 0) {
		printf("Origin file is empty. Write is complete (0 sec).\n");
		close(read_fd);
		close(write_fd);
		return 0;
	}

	struct statvfs vfs;
	chunk_size = 4096;
	if (statvfs(read_filename, &vfs) == 0) {
		chunk_size = vfs.f_bsize;
	}

	// Выделяем структуры и буферы (с кастами, чтобы Linux/g++ не ругался)
	struct aio_operation** ops = (struct aio_operation**)malloc(n * sizeof(struct aio_operation*));
	if (!ops) {
		perror("malloc ops");
		exit(1);
	}

	int i;
	for (i = 0; i < n; i++) {
		ops[i] = (struct aio_operation*)malloc(sizeof(struct aio_operation));
		if (!ops[i]) {
			perror("malloc aio_op");
			exit(1);
		}
		ops[i]->buffer = (char*)malloc(chunk_size);
		if (!ops[i]->buffer) {
			perror("malloc buffer");
			exit(1);
		}
		ops[i]->next_operation = NULL;
		memset(&ops[i]->aio, 0, sizeof(struct aiocb));

		ops[i]->aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
		ops[i]->aio.aio_sigevent.sigev_notify_function = aio_completion_handler;
		ops[i]->aio.aio_sigevent.sigev_value.sival_ptr = ops[i];
		ops[i]->aio.aio_buf = ops[i]->buffer;
		ops[i]->aio.aio_reqprio = 0;
	}

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (i = 0; i < n; i++) {
		pthread_mutex_lock(&offset_mutex);
		if (next_read_offset >= file_size) {
			pthread_mutex_unlock(&offset_mutex);
			break;
		}
		off_t offset = next_read_offset;
		size_t len = (size_t)min((off_t)chunk_size, file_size - offset);
		next_read_offset += (off_t)len;
		pthread_mutex_unlock(&offset_mutex);

		struct aio_operation* op = ops[i];
		op->aio.aio_fildes = read_fd;
		op->aio.aio_offset = offset;
		op->aio.aio_nbytes = len;
		op->write_operation = 0;

		if (aio_read(&op->aio) == -1) {
			perror("aio_read initial");
		}
	}

	int done = 0;
	while (!done) {
		const struct aiocb* list[100];
		int cnt = 0;
		for (i = 0; i < n; i++) {
			if (aio_error(&ops[i]->aio) == EINPROGRESS) {
				list[cnt++] = &ops[i]->aio;
			}
		}
		if (cnt == 0) {
			done = 1;
		}
		else {
			if (aio_suspend(list, cnt, NULL) == -1 && errno != EINTR) {
				perror("aio_suspend");
			}
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &end);
	double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("Copy complete for %.6f seconds\n", time_taken);

	close(read_fd);
	close(write_fd);

	for (i = 0; i < n; i++) {
		free(ops[i]->buffer);
		free(ops[i]);
	}
	free(ops);

	return 0;
}