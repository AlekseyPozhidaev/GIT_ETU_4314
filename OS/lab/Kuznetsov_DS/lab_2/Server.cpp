#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
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
	char buffer[1024];

	while (1) {
		printf("\n=== МЕНЮ СЕРВЕРА ===\n");
		printf("1. Выполнить проецирование\n");
		printf("2. Записать данные\n");
		printf("3. Завершить работу\n");
		printf("Введите выбор: ");

		scanf("%d", &choice);
		getchar();

		switch (choice) {
			case 1:
				if (fd != -1) {
					printf("Уже выполнено!\n");
					break;
				}

				// создаём pipe (FIFO)
				mkfifo(PIPE_NAME, 0666);

				fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
				if (fd == -1) {
					perror("open");
					break;
				}

				ftruncate(fd, FILESIZE);

				ptr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if (ptr == MAP_FAILED) {
					perror("mmap");
					exit(1);
				}

				pipe_fd = open(PIPE_NAME, O_WRONLY);
				if (pipe_fd == -1) {
					perror("pipe open");
					exit(1);
				}

				printf("Сервер готов.\n");
				break;

			case 2:
				if (ptr == NULL) {
					printf("Сначала проецирование!\n");
					break;
				}

				printf("Введите строку: ");
				fgets(buffer, sizeof(buffer), stdin);
				buffer[strcspn(buffer, "\n")] = '\0';

				memset(ptr, 0, FILESIZE);
				snprintf(ptr, FILESIZE, "%s", buffer);

				write(pipe_fd, "1", 1);

				printf("Данные записаны и сигнал отправлен.\n");
				break;

			case 3:
				if (ptr) munmap(ptr, FILESIZE);
				if (fd != -1) close(fd);
				if (pipe_fd != -1) close(pipe_fd);

				unlink(FILENAME);
				unlink(PIPE_NAME);

				printf("Сервер завершён.\n");
				exit(0);
		}
	}
}