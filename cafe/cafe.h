#ifndef __CAFE_H
#define __CAFE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "../common/barrier.h"
#include "../common/colors.h"

// barista
#define CHECK 0
#define IDLE 1
#define SERVING 2

// customer
#define INVALID 3
#define ARRIVED 4
#define WAITING 5
#define SERVED 6
#define LEFT 7

typedef struct __barista {
  int id;
  int st;
  int csid;
  int ctime;
} barista_t;

typedef struct __coffee {
  char name[64];
  int prep;
} coffee_t;

typedef struct __customer {
  int id;
  char order[64];
  int arr;
  int tol;
  int st;
  int wt;
} customer_t;

// main.c
int main(void);
void* barista(void* arg);
void* customer(void* arg);

#endif
