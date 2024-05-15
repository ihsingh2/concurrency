#ifndef __BARRIER_H
#define __BARRIER_H

#include <semaphore.h>
#include <unistd.h>

typedef struct __barrier {
    int parties;
    int phases;
    int sleep;
    int completed;
    int arrived;
    sem_t mutex;
    sem_t turnstile1;
    sem_t turnstile2;
} barrier_t;

int barrier_init(barrier_t* b, int parties, int phases, int sleep);
int barrier_destroy(barrier_t* b);
void await(barrier_t* b);
int get_epoch(barrier_t* b);

#endif
