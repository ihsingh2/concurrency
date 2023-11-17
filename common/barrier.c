#include "barrier.h"
#include <unistd.h>

int barrier_init(barrier_t* b, int parties, int phases, int sleep)
{
  b->parties = parties;
  b->phases = phases;
  b->sleep = sleep;
  b->completed = 0;
  b->arrived = 0;
  sem_init(&(b->mutex), 0, 1);
  sem_init(&(b->turnstile1), 0, 0);
  sem_init(&(b->turnstile2), 0, 1);
  return 0;
}

int barrier_destroy(barrier_t* b)
{
  sem_destroy(&(b->mutex));
  sem_destroy(&(b->turnstile1));
  sem_destroy(&(b->turnstile2));
  return 0;
}

void await(barrier_t* b)
{
  sem_t *lock, *t1, *t2;
  lock = &(b->mutex);
  if (b->completed % 2 == 0) {
    t1 = &(b->turnstile1);
    t2 = &(b->turnstile2);
  } else {
    t1 = &(b->turnstile2);
    t2 = &(b->turnstile1);
  }

  // Adapted from Downey's book on Semaphores
  // https://greenteapress.com/wp/semaphores/

  sem_wait(lock);
  b->arrived++;
  if (b->arrived == b->parties)
  {
    b->arrived = 0;
    b->completed++;
    if (b->sleep && (b->completed % b->phases == 0))
      sleep(1);
    sem_wait(t2);
    sem_post(t1);
  }
  sem_post(lock);
  sem_wait(t1);
  sem_post(t1);
}

int get_epoch(barrier_t* b)
{
  return (b->completed / b->phases);
}
