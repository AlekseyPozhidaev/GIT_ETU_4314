#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "shared_memory.dat"
#define FILESIZE 4096

int main() {
	int fd = -1;
	char* base_ptr = NULL;
	sem_t* ready_sem = NULL;
	sem_t* ack_sem = NULL;
	char* data_area = NULL;
	int choice;

	while (1) {
		printf("\n=== МЕНЮ КЛИЕНТА ===\n");
		printf("1. Выполнить проецирование\n");
		printf("2. Прочитать данные\n");
		printf("3. Завершить работу\n");
		printf("Введите выбор: ");

		if (scanf("%d", &choice) != 1) {
			while (getchar() != '\n');
			continue;
		}

		switch (choice) {
			case 1: {
					if (fd != -1) {
						printf("Проецирование уже выполнено!\n");
						break;
					}

					fd = open(FILENAME, O_RDWR);
					if (fd == -1) {
						perror("Ошибка открытия файла");
						break;
					}

					base_ptr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
					if (base_ptr == MAP_FAILED) {
						perror("mmap");
						close(fd);
						fd = -1;
						base_ptr = NULL;
						break;
					}

					ready_sem = (sem_t*)base_ptr;
					ack_sem = (sem_t*)(base_ptr + sizeof(sem_t));
					data_area = base_ptr + 2 * sizeof(sem_t);

					printf("Файл успешно проецирован в память (клиент).\n");
					break;
				}

			case 2: {
					if (base_ptr == NULL) {
						printf("Сначала выполните проецирование (пункт 1)!\n");
						break;
					}

					printf("Ожидаем данные от сервера...\n");

					sem_wait(ready_sem);

					printf("Client received: %s\n", data_area);

					sem_post(ack_sem);
					break;
				}

			case 3: {
					if (base_ptr != NULL) {
						munmap(base_ptr, FILESIZE);
					}
					if (fd != -1) {
						close(fd);
					}
					printf("Проецирование отменено. Работа клиента завершена.\n");
					exit(0);
				}

			default:
				printf("Неверный выбор! Попробуйте снова.\n");
		}
	}
	return 0;
}