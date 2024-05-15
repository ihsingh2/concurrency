#include "cafe.h"

// global variables
int b, k, n;
int wasted;
int terminate;
barista_t* B;
coffee_t* CF;
customer_t* CS;
#ifdef PRIORITY
    pthread_cond_t cond;
#endif
pthread_mutex_t mutex;
barrier_t barrier;
#define timer get_epoch(&barrier)

int main(void)
{
    // 1: input
    scanf("%d %d %d", &b, &k, &n);

    int i = 0;
    B = (barista_t*) calloc(b, sizeof(barista_t));
    for (barista_t* ptr = B; ptr < B + b; ptr++) {
        ptr->id = ++i;
        ptr->st = IDLE;
    }

    CF = (coffee_t*) calloc(k, sizeof(coffee_t));
    for (coffee_t* ptr = CF; ptr < CF + k; ptr++)
        scanf("%s %d", ptr->name, &ptr->prep);

    CS = (customer_t*) calloc(n, sizeof(customer_t));
    for (customer_t* ptr = CS; ptr < CS + n; ptr++) {
        ptr->st = INVALID;
        ptr->wt = -1;
        scanf("%d %s %d %d", &ptr->id, ptr->order, &ptr->arr, &ptr->tol);
    }

    // 2: simulate
    wasted = 0;
    terminate = 0;
#ifdef PRIORITY
    pthread_cond_init(&cond, NULL);
#endif
    pthread_mutex_init(&mutex, NULL);
#ifdef SIM
    barrier_init(&barrier, b + n, 3, 1);
#else
    barrier_init(&barrier, b + n, 3, 0);
#endif

    pthread_t bthreads[b], csthreads[n];
    for (i = 0; i < b; i++)
        pthread_create(&bthreads[i], 0, barista, B + i);
    for (i = 0; i < n; i++)
        pthread_create(&csthreads[i], 0, customer, CS + i);

    // 3: terminate
    for (i = 0; i < b; i++)
        pthread_join(bthreads[i], 0);
    for (i = 0; i < n; i++)
        pthread_join(csthreads[i], 0);
    printf("\n%d coffee wasted\n", wasted);

    i = 0;
    for (customer_t* ptr = CS; ptr < CS + n; ptr++) {
        if (ptr->wt != -1)
            i += ptr->wt;
        else
            i += ptr->tol;
    }
    printf("Total waiting time: %d second(s)\n", i);
    printf("Average waiting time: %.2f second(s)\n", (float) i / n);

    free(B);
    free(CF);
    free(CS);
#ifdef PRIORITY
    pthread_cond_destroy(&cond);
#endif
    pthread_mutex_destroy(&mutex);
    barrier_destroy(&barrier);

    return 0;
}

void* barista(void* arg)
{
    barista_t *br = (barista_t*) arg;
    while (1)
    {
        // 1: barista phase
        if (terminate)
            return NULL;

        switch (br->st)
        {
            case SERVING:
                if (timer >= br->ctime) {
                    for (customer_t* csptr = CS; csptr < CS + n; csptr++)
                        if (br->csid == csptr->id) {
                            br->st = IDLE;
                            if (csptr->st == WAITING) {
                                csptr->st = SERVED;
                                printf(BLUE "Barista %d completes the order of customer %d at %d second(s)" RESET "\n", br->id, br->csid, timer);
                            }
                            else if (csptr->st == LEFT) {
                                pthread_mutex_lock(&mutex);
                                wasted++;
                                pthread_mutex_unlock(&mutex);
                                printf(MAGENTA "Barista %d unsuccessfully completes the order of customer %d at %d second(s)" RESET "\n", br->id, br->csid, timer);
                            }
                            break;
                        }
                }
                break;

            case CHECK:
                pthread_mutex_lock(&mutex);
#ifdef PRIORITY
                for (barista_t *brptr = B; brptr < B + br->id - 1; brptr++)
                    while (brptr->st == CHECK)
                        pthread_cond_wait(&cond, &mutex);
#endif

                for (customer_t* csptr = CS; csptr < CS + n; csptr++)
                    if (csptr->st == ARRIVED) {
                        csptr->st = WAITING;
                        csptr->wt = timer - csptr->arr;
                        br->st = SERVING;
                        br->csid = csptr->id;
                        printf(CYAN "Barista %d begins preparing the order of customer %d at %d second(s)" RESET "\n", br->id, br->csid, timer);

                        pthread_mutex_unlock(&mutex);
#ifdef PRIORITY
                        pthread_cond_broadcast(&cond);
#endif

                        br->ctime = timer + 1;  // if coffee not found, assume ctime to be +1 to avoid infinite wait
                        for (coffee_t* cfptr = CF; cfptr < CF + k; cfptr++) {
                            if (strcmp(csptr->order, cfptr->name) == 0) {
                                br->ctime = timer + cfptr->prep;
                                break;
                            }
                        }
                        break;
                    }

                if (br->st == CHECK) {
                    br->st = IDLE;
                    pthread_mutex_unlock(&mutex);
#ifdef PRIORITY
                    pthread_cond_broadcast(&cond);
#endif
                }

            default:
        }
        await(&barrier);

        // 2: customer phase
        if (br->st == IDLE)
            br->st = CHECK;
        await(&barrier);
        
        // 3: termination check phase
        if (br->id == 1) {
            int flag1 = 1, flag2 = 1;
            for (customer_t* csptr = CS; csptr < CS + n && flag1; csptr++)
                if (csptr->st == INVALID || csptr->st == ARRIVED || csptr->st == WAITING)
                    flag1 = 0;
            for (barista_t* brptr = B; brptr < B + b && flag1 && flag2; brptr++)
                if (brptr->st == SERVING)
                    flag2 = 0;
            if (flag1 && flag2)
                terminate = 1;
        }
        await(&barrier);
    }
}

void* customer(void* arg)
{
    customer_t* cs = (customer_t*) arg;
    while (1)
    {
        // 1: barista phase
        if (terminate)
            return NULL;
        await(&barrier);

        // 2: customer phase
        switch (cs->st)
        {
            case INVALID:
                if (timer >= cs->arr) {
                    cs->st = ARRIVED;
                    printf("Customer %d arrives at %d second(s)\n", cs->id, timer);
                    printf(YELLOW "Customer %d orders %s" RESET "\n", cs->id, cs->order);
                }
                break;

            case ARRIVED:
            case WAITING:
                if (timer > cs->arr + cs->tol) {
                    cs->st = LEFT;
                    printf(RED "Customer %d leaves without their order at %d second(s)" RESET "\n", cs->id, timer);
                }
                break;

            case SERVED:
                cs->st = LEFT;
                printf(GREEN "Customer %d leaves with their order at %d second(s)" RESET "\n", cs->id, timer);
                break;

            case LEFT:
            default:
        }
        await(&barrier);

        // 3: termination check phase
        await(&barrier);
    }
}
