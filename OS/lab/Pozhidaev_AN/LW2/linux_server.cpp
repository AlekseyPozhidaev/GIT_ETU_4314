#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define FILENAME "shared.dat"
#define FILESIZE 4096
#define FIFO_ALIVE "/tmp/fifo_alive"
#define FIFO_DATA  "/tmp/fifo_data"
#define FIFO_ACK   "/tmp/fifo_ack"

typedef struct {
    int version;          // incremented on each new message
    char data[FILESIZE - sizeof(int)];
} SharedData;

int main() {
    // 1. Create FIFOs (ignore if they already exist)
    mkfifo(FIFO_ALIVE, 0666);
    mkfifo(FIFO_DATA, 0666);
    mkfifo(FIFO_ACK,  0666);

    // 2. Open FIFO_ALIVE for writing (shows client that server is alive)
    int fd_alive = open(FIFO_ALIVE, O_RDWR | O_NONBLOCK);
    if (fd_alive == -1) { perror("open alive"); exit(1); }

    int fd_data;

    // 5. Create file and map it into memory
    int fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) { perror("open file"); exit(1); }
    ftruncate(fd, FILESIZE);
    SharedData *ptr = (SharedData*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }
    ptr->version = 0;
    ptr->data[0] = '\0';

    printf("Server started. Client may connect at any time.\n");

    int choice;
    while (1) {
        printf("\n=== SERVER ===\n");
        printf("1. Send message\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Enter message: ");
            fgets(ptr->data, FILESIZE - sizeof(int), stdin);
            ptr->data[strcspn(ptr->data, "\n")] = 0;
            ptr->version++;   // increment version – client will notice

            // 3. Open FIFO_DATA for writing (non‑blocking to avoid hang)
            fd_data = open(FIFO_DATA, O_RDWR | O_NONBLOCK);
            if (fd_data == -1) { perror("open data"); exit(1); }

            // Try to notify client (if FIFO buffer is full, skip)
            char c = 1;
            if (write(fd_data, &c, 1) == -1 && errno != EAGAIN)
                perror("write (notify)");
            printf("Message saved.\n");
        }
        else if (choice == 0) {
            break;
        }
    }

    // 6. Termination: wait until client reads at least one message or exits
    printf("Waiting for client to exit...\n");
    
    // 4. Open FIFO_ACK for reading (will wait for acknowledgement on exit)
    int fd_ack = open(FIFO_ACK, O_RDONLY);
    if (fd_ack == -1) { perror("open ack"); exit(1); }

    char dummy;
    read(fd_ack, &dummy, 1);   // blocks until client sends a byte
    printf("Client acknowledged (terminated).\n");

    // 7. Cleanup
    close(fd_data);
    close(fd_ack);
    close(fd_alive);
    munmap(ptr, FILESIZE);
    unlink(FILENAME);
    unlink(FIFO_ALIVE);
    unlink(FIFO_DATA);
    unlink(FIFO_ACK);
    printf("Server finished.\n");
    return 0;
}