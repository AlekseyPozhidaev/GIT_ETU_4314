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
	sem_t* ready_sem = NULL;      // сервер сигнализирует «данные готовы»
	sem_t* ack_sem = NULL;      // клиент сигнализирует «прочитал»
	char* data_area = NULL;
	int choice;

	while (1) {
		printf("\n=== МЕНЮ СЕРВЕРА ===\n");
		printf("1. Выполнить проецирование\n");
		printf("2. Записать данные\n");
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

					fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
					if (fd == -1) {
						perror("Ошибка create/open");
						break;
					}

					if (ftruncate(fd, FILESIZE) == -1) {
						perror("ftruncate");
						close(fd);
						fd = -1;
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

					// Размещаем семафоры в начале отображённой памяти (без структуры)
					ready_sem = (sem_t*)base_ptr;
					ack_sem = (sem_t*)(base_ptr + sizeof(sem_t));
					data_area = base_ptr + 2 * sizeof(sem_t);

					// Инициализация один раз
					sem_init(ready_sem, 1, 0);
					sem_init(ack_sem, 1, 0);

					printf("Файл успешно проецирован в память (сервер).\n");
					break;
				}

			case 2: {
					if (base_ptr == NULL) {
						printf("Сначала выполните проецирование (пункт 1)!\n");
						break;
					}

					// Жёстко заданные данные (как в исходной версии)
					sprintf(data_area, "Hello, shared memory!");

					// Сигнал клиенту
					sem_post(ready_sem);

					printf("Данные записаны в общую память.\n");
					printf("Ожидаем подтверждения от клиента...\n");

					// Ждём, пока клиент прочитает
					sem_wait(ack_sem);
					printf("Клиент успешно прочитал данные.\n");
					break;
				}

			case 3: {
					if (base_ptr != NULL) {
						sem_destroy(ready_sem);
						sem_destroy(ack_sem);
						munmap(base_ptr, FILESIZE);
					}
					if (fd != -1) {
						close(fd);
					}
					unlink(FILENAME);

					printf("Проецирование отменено, файл удалён. Работа сервера завершена.\n");
					exit(0);
				}

			default:
				printf("Неверный выбор! Попробуйте снова.\n");
		}
	}
	return 0;
}