#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>

#define FILENAME "shared_lab_file.dat"
#define FILESIZE 4096

typedef struct {
    sem_t write_sem;    // клиент сигнализирует, что прочитал
    sem_t read_sem;     // сервер сигнализирует, что записал
    char  data[FILESIZE - 32];  // оставляем место для семафоров
} SharedData;

int main() {
    int fd;
    SharedData *ptr = NULL;
    int choice;

    while (1) {
        printf("\n=== Сервер (Linux) ===\n");
        printf("1. Выполнить проецирование (создать файл и mmap)\n");
        printf("2. Записать данные\n");
        printf("0. Завершить работу (munmap + unlink)\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        getchar(); // очистка буфера

        if (choice == 1) {
            // Создание файла
            fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd == -1) { perror("open"); continue; }

            if (ftruncate(fd, sizeof(SharedData)) == -1) { perror("ftruncate"); close(fd); continue; }

            // Проецирование
            ptr = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd); // fd больше не нужен

            if (ptr == MAP_FAILED) { perror("mmap"); ptr = NULL; continue; }

            // Инициализация семафоров (один раз)
            sem_init(&ptr->write_sem, 1, 0);  // клиент ещё не прочитал
            sem_init(&ptr->read_sem,  1, 0);  // сервер ещё не записал

            printf("Проецирование выполнено успешно! Файл: %s\n", FILENAME);
        }
        else if (choice == 2) {
            if (!ptr) { printf("Сначала выполните проецирование!\n"); continue; }

            printf("Введите данные для записи: ");
            fgets(ptr->data, sizeof(ptr->data), stdin);
            ptr->data[strcspn(ptr->data, "\n")] = 0;

            // Сигнал клиенту, что данные готовы
            sem_post(&ptr->read_sem);

            printf("Данные записаны. Ожидаем чтения клиентом...\n");

            // Ждём, пока клиент прочитает
            sem_wait(&ptr->write_sem);
            printf("Клиент прочитал данные.\n");
        }
        else if (choice == 0) {
            if (ptr) {
                sem_destroy(&ptr->write_sem);
                sem_destroy(&ptr->read_sem);
                munmap(ptr, sizeof(SharedData));
                ptr = NULL;
            }
            unlink(FILENAME);
            printf("Работа завершена. Файл удалён.\n");
            break;
        }
    }
    return 0;
}