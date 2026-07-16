#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define SHM_NAME "/demo_shm"
#define SEM_NAME "/demo_sem"

struct shared_data {
    int value;
    char message[128];
};

#endif
