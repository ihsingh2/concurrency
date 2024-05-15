#ifndef __PARLOUR_H
#define __PARLOUR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "barrier.h"
#include "colors.h"

// machine 
#define CHECK 1
#define IDLE 2
#define SERVING 3

// customer
#define ARRIVED 4
#define WAITING 5
#define SERVED 6
#define SHORTAGE 7
#define CLOSED 8

// common
#define INVALID 0
#define LEFT 9

typedef struct __machine {
    int id;
    int start;
    int stop;
    int st;
    int csid;
    int ordid;
    int ctime;
} machine_t;

typedef struct __flavour {
    char name[64];
    int prep;
} flavour_t;

typedef struct __topping {
    char name[64];
    int quant;
} topping_t;

typedef struct __order {
    char flav[64];
    int ntop;
    char* top;
    int st;
} order_t;

typedef struct __customer {
    int id;
    int arr;
    int nord;
    order_t* ord;
    int st;
    int nrem;
    int nwt;
} customer_t;

typedef struct __node {
    customer_t* cs;
    struct __node* prev;
    struct __node* next;
} node_t;

typedef struct __list {
    node_t* head;
} list_t;

// main.c
int main(void);
void* machine(void *arg);
void* customer(void *arg);

// util.c
int numtokens(char* src, int len, char* delim);
void list_init(list_t* L);
void list_insert(list_t* L, customer_t* cs);
void list_delete(list_t* L, node_t* x);
void list_free(list_t* L);
void list_free_rec(list_t* L);

#endif
