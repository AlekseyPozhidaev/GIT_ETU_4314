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
    sem_t write_sem;
    sem_t read_sem;
    char  data[FILESIZE - 32];
} SharedData;

int main() {
    int fd;
    SharedData *ptr = NULL;
    int choice;

    while (1) {
        printf("\n=== Клиент (Linux) ===\n");
        printf("1. Выполнить проецирование (открыть файл и mmap)\n");
        printf("2. Прочитать данные\n");
        printf("0. Завершить работу\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            fd = open(FILENAME, O_RDWR);
            if (fd == -1) { perror("open"); continue; }

            ptr = (SharedData *) mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);

            if (ptr == MAP_FAILED) { perror("mmap"); ptr = NULL; continue; }

            printf("Проецирование открыто успешно!\n");
        }
        else if (choice == 2) {
            if (!ptr) { printf("Сначала выполните проецирование!\n"); continue; }

            printf("Ожидаем данных от сервера...\n");
            sem_wait(&ptr->read_sem);   // ждём, пока сервер запишет

            printf("Клиент получил: %s\n", ptr->data);

            // Сигнал серверу, что прочитали
            sem_post(&ptr->write_sem);
        }
        else if (choice == 0) {
            if (ptr) {
                munmap(ptr, sizeof(SharedData));
                ptr = NULL;
            }
            printf("Работа клиента завершена.\n");
            break;
        }
    }
    return 0;
}