#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>

#define FILENAME "shared.dat"
#define FILESIZE 4096
#define FIFO_ALIVE "/tmp/fifo_alive"
#define FIFO_DATA  "/tmp/fifo_data"
#define FIFO_ACK   "/tmp/fifo_ack"

typedef struct {
    int version;
    char data[FILESIZE - sizeof(int)];
} SharedData;

int main() {
    // 1. Create FIFOs (if server hasn't created them yet)
    mkfifo(FIFO_ALIVE, 0666);
    mkfifo(FIFO_DATA, 0666);
    mkfifo(FIFO_ACK,  0666);

    // 2. Wait for server to start via FIFO_ALIVE
    printf("Waiting for server to start...\n");
    int fd_alive = open(FIFO_ALIVE, O_RDONLY);
    if (fd_alive == -1) { perror("open alive"); exit(1); }
    close(fd_alive);   // server is alive, this FIFO no longer needed
    printf("Server started.\n");

    int fd_ack;

    // 5. Open file and map it into memory
    sleep(2);
    int fd = open(FILENAME, O_RDWR);
    if (fd == -1) { perror("open file"); exit(1); }
    SharedData *ptr = (SharedData*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }

    printf("Connection established.\n");

    printf("Waiting fore data...\n");

    // 3. Open FIFO_DATA for reading (for signals from server)
    int fd_data = open(FIFO_DATA, O_RDONLY);
    if (fd_data == -1) { perror("open data"); exit(1); }

    printf("Ready to read messages.\n");

    int last_version = -1;
    int choice;
    while (1) {
        printf("\n=== CLIENT ===\n");
        printf("1. Read message\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            // Check if a new message has arrived without a signal
            if (ptr->version != last_version) {
                last_version = ptr->version;
                printf("Message: %s\n", ptr->data);
                continue;
            }



            if (ptr->version != last_version) {
                last_version = ptr->version;
                printf("Message: %s\n", ptr->data);
            } else {
                printf("No new messages (timeout).\n");
            }
        }
        else if (choice == 0) {
            break;
        }
    }

    fd_ack = open(FIFO_ACK, O_WRONLY);
    if (fd_ack == -1) { perror("open ack"); exit(1); }
    char c = 1;
    write(fd_ack, &c, 1);
    // 6. Termination: send a byte to server so it can exit
    close(fd_ack);
    close(fd_data);
    munmap(ptr, FILESIZE);
    printf("Client finished.\n");
    return 0;
}