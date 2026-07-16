#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <semaphore.h>

#include "common.h"

int main(void)
{
    int fd;

    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(fd, sizeof(struct shared_data));

    struct shared_data *data =
        mmap(NULL,
             sizeof(struct shared_data),
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fd,
             0);

    sem_t *sem =
        sem_open(SEM_NAME,
                 O_CREAT,
                 0666,
                 0);

    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    data->value = 1234;
    strcpy(data->message, "Hello from producer!");

    printf("Producer: write done.\n");

    sem_post(sem);

    munmap(data, sizeof(*data));
    close(fd);
    sem_close(sem);

    return 0;
}
