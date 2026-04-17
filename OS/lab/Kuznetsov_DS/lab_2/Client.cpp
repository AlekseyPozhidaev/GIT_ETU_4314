#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "shared_memory.dat"
#define PIPE_NAME "sync_pipe"
#define FILESIZE 4096

int main() {
	int fd = -1;
	int pipe_fd = -1;
	char* ptr = NULL;
	int choice;

	while (1) {
		printf("\n=== МЕНЮ КЛИЕНТА ===\n");
		printf("1. Выполнить проецирование\n");
		printf("2. Прочитать данные\n");
		printf("3. Завершить работу\n");
		printf("Введите выбор: ");

		scanf("%d", &choice);

		switch (choice) {
			case 1:
				if (fd != -1) {
					printf("Уже выполнено!\n");
					break;
				}

				fd = open(FILENAME, O_RDWR);
				if (fd == -1) {
					perror("open файл");
					break;
				}

				ptr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if (ptr == MAP_FAILED) {
					perror("mmap");
					exit(1);
				}

				pipe_fd = open(PIPE_NAME, O_RDONLY);
				if (pipe_fd == -1) {
					perror("open pipe");
					exit(1);
				}

				printf("Клиент готов.\n");
				break;

			case 2:
				if (ptr == NULL) {
					printf("Сначала проецирование!\n");
					break;
				}

				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(pipe_fd, &readfds);

				printf("Ожидание данных...\n");

				select(pipe_fd + 1, &readfds, NULL, NULL, NULL);

				char signal;
				read(pipe_fd, &signal, 1);

				printf("Получено: %s\n", ptr);
				break;

			case 3:
				if (ptr) munmap(ptr, FILESIZE);
				if (fd != -1) close(fd);
				if (pipe_fd != -1) close(pipe_fd);

				printf("Клиент завершён.\n");
				exit(0);
		}
	}
}