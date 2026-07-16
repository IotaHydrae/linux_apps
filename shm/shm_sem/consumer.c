#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <semaphore.h>

#include "common.h"

int main(void)
{
    int fd;

    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        exit(1);
    }

    struct shared_data *data =
        mmap(NULL,
             sizeof(struct shared_data),
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fd,
             0);

    sem_t *sem =
        sem_open(SEM_NAME,
                 0);

    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Consumer: waiting...\n");

    sem_wait(sem);

    printf("value   = %d\n", data->value);
    printf("message = %s\n", data->message);

    munmap(data, sizeof(*data));
    close(fd);
    sem_close(sem);

    return 0;
}
