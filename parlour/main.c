#include "parlour.h"

// global variables
int n, k, f, t, c;
int waiting;
int terminate;
machine_t* M;
flavour_t* F;
topping_t* T;
list_t* C;
#ifdef PRIORITY
    pthread_cond_t cond;
#endif
pthread_mutex_t mutex;
barrier_t barrier;
#define timer get_epoch(&barrier)

int main(void)
{
    // 1: input
    scanf("%d %d %d %d", &n, &k, &f, &t);

    int i = 0;
    M = (machine_t*) calloc(n, sizeof(machine_t));
    for (machine_t* ptr = M; ptr < M + n; ptr++) {
        ptr->id = ++i;
        ptr->st = INVALID;
        scanf("%d %d", &ptr->start, &ptr->stop);
    }

    F = (flavour_t*) calloc(k, sizeof(flavour_t));
    for (flavour_t* ptr = F; ptr < F + f; ptr++)
        scanf("%s %d", ptr->name, &ptr->prep);
    
    T = (topping_t*) calloc(t, sizeof(topping_t));
    for (topping_t* ptr = T; ptr < T + t; ptr++)
        scanf("%s %d\n", ptr->name, &ptr->quant);

    c = 0;
    C = (list_t*) calloc(1, sizeof(list_t));
    list_init(C);

    while (1)
    {
        char buf[4096];
        memset(buf, 0, 4096);
        if (fgets(buf, 4096, stdin) != NULL) {
            customer_t* cs = (customer_t*) calloc(1, sizeof(customer_t));
            sscanf(buf, "%d %d %d", &(cs->id), &(cs->arr), &(cs->nord));
            cs->ord = (order_t*) calloc(cs->nord, sizeof(order_t));
            cs->st = INVALID;
            cs->nrem = cs->nord;
            cs->nwt = cs->nord;
            c++;

            for (order_t* optr = cs->ord; optr < cs->ord + cs->nord; optr++) {
                fgets(buf, 4096, stdin);
                optr->ntop = numtokens(buf, 4096, " \t\n") - 1;
                optr->top = (char*) calloc(optr->ntop, 64 * sizeof(char));
                optr->st = ARRIVED;
                char* token = strtok(buf, " \t\n");
                strcpy(optr->flav, token);
                for (char* tptr = optr->top; tptr < optr->top + (optr->ntop * 64); (tptr = tptr + 64)) {
                    token = strtok(NULL, " \t\n");
                    strcpy(tptr, token);
                }
            }

            list_insert(C, cs);
        }
        else
            break;
    }

    // 2: simulate
    waiting = 0;
    terminate = 0;
#ifdef PRIORITY
    pthread_cond_init(&cond, NULL);
#endif
    pthread_mutex_init(&mutex, NULL);
#ifdef SIM
    barrier_init(&barrier, n + c, 4, 1);
#else
    barrier_init(&barrier, n + c, 4, 0);
#endif

    pthread_t mthreads[n], csthreads[c];
    for (i = 0; i < n; i++)
        pthread_create(&mthreads[i], 0, machine, M + i);
    node_t *nptr = C->head->next;
    for (i = 0; i < c; i++, (nptr = nptr->next))
        pthread_create(&csthreads[i], 0, customer, nptr->cs);

    // 3: terminate
    for (i = 0; i < n; i++)
        pthread_join(mthreads[i], 0);
    for (i = 0; i < c; i++)
        pthread_join(csthreads[i], 0);
    printf("Parlour Closed\n");

    free(M);
    free(F);
    free(T);
    list_free_rec(C);
#ifdef PRIORITY
    pthread_cond_destroy(&cond);
#endif
    pthread_mutex_destroy(&mutex);
    barrier_destroy(&barrier);

    return 0;
}

void* machine(void *arg)
{
    machine_t* mc = (machine_t*) arg;
    while (1)
    {
        // 1: machine phase
        switch (mc->st)
        {
            case INVALID:
                if (timer < mc->start)
                    break;
                mc->st = CHECK;
                printf(ORANGE "Machine %d has started working at %d second(s)" RESET "\n", mc->id, timer);

            case CHECK:
                if (timer >= mc->stop) {
                    mc->st = LEFT;
#ifdef PRIORITY
                    pthread_cond_broadcast(&cond);
#endif
                    printf(ORANGE "Machine %d has stopped working at %d second(s)" RESET "\n", mc->id, timer);
                    break;
                }
                pthread_mutex_lock(&mutex);
#ifdef PRIORITY
                for (machine_t* mcptr = M; mcptr < M + mc->id - 1; mcptr++)
                    while ((mcptr->st == INVALID && mcptr->start == timer) || mcptr->st == CHECK)
                        pthread_cond_wait(&cond, &mutex);
#endif

                node_t *csptr = C->head->next;
                while (csptr != C->head) {
                    switch (csptr->cs->st)
                    {
                        case ARRIVED:
                            for (order_t *optr = csptr->cs->ord; optr < csptr->cs->ord + csptr->cs->nord; optr++)
                                for (char *tptr = csptr->cs->ord->top; tptr < csptr->cs->ord->top + csptr->cs->nord; (tptr = tptr + 64))
                                    for (topping_t *tptr2 = T; tptr2 < T + t; tptr2++)
                                        if (strcmp(tptr, tptr2->name) == 0 && tptr2->quant == 0) {
                                            csptr->cs->st = SHORTAGE;
                                            goto nextcs;
                                        }
                            csptr->cs->st = WAITING;
                            for (order_t *optr = csptr->cs->ord; optr < csptr->cs->ord + csptr->cs->nord; optr++)
                                for (char *tptr = csptr->cs->ord->top; tptr < csptr->cs->ord->top + csptr->cs->nord; (tptr = tptr + 64))
                                    for (topping_t *tptr2 = T; tptr2 < T + t; tptr2++)
                                        if (strcmp(tptr, tptr2->name) == 0)
                                            tptr2->quant--;

                        case WAITING:
                            if (csptr->cs->nrem != 0) {
                                int i = 1;
                                for (order_t *optr = csptr->cs->ord; optr < csptr->cs->ord + csptr->cs->nord; optr++, i++)
                                    if (optr->st == ARRIVED) {
                                        for (flavour_t *fptr = F; fptr < F + f; fptr++)
                                            if (strcmp(optr->flav, fptr->name) == 0 && fptr->prep < mc->stop - timer) {
                                                optr->st = WAITING;
                                                csptr->cs->nrem--;
                                                mc->st = SERVING;
                                                pthread_mutex_unlock(&mutex);
#ifdef PRIORITY
                                                pthread_cond_broadcast(&cond);
#endif
                                                mc->csid = csptr->cs->id;
                                                mc->ordid = i;
                                                mc->ctime = timer + fptr->prep;
                                                printf(CYAN "Machine %d starts preparing ice cream %d of customer %d at %d seconds(s)" RESET "\n", mc->id, i, csptr->cs->id, timer);
                                                goto end;
                                            }
                                    }
                            }
                            break;

                        default:
                    }
nextcs:
                    csptr = csptr->next;
                }
                mc->st = IDLE;
                pthread_mutex_unlock(&mutex);
#ifdef PRIORITY
                pthread_cond_broadcast(&cond);
#endif
                break;

            case SERVING:
                if (timer >= mc->ctime) {
                    mc->st = IDLE;
                    node_t *csptr = C->head->next;
                    while (csptr->cs->id != mc->csid)
                        csptr = csptr->next;
                    int i = 1;
                    order_t *optr = csptr->cs->ord;
                    for (;i != mc->ordid; optr++, i++);
                    optr->st = SERVED;
                    pthread_mutex_lock(&mutex);
                    csptr->cs->nwt--;
                    if (csptr->cs->nwt == 0)
                        csptr->cs->st = SERVED;
                    pthread_mutex_unlock(&mutex);
                    printf(BLUE "Machine %d completes preparing ice cream %d of customer %d at %d seconds(s)" RESET "\n", mc->id, i, csptr->cs->id, timer);
                }
                break;

            default:
        }
end:
        await(&barrier);

        // 2: customer phase
        if (mc->st == IDLE)
            mc->st = CHECK;
        await(&barrier);

        // 3: termination phase
        if (terminate)
            return NULL;
        await(&barrier);

        // 4: termination check phase
        if (mc->id == 1) {
            int flag1 = 1, flag2 = 1, flag3 = 1;
            for (topping_t* tptr = T; tptr < T + t; tptr++)
                if (tptr->quant != 0)
                    flag1 = 0;
            for (machine_t* mptr = M; mptr < M + n; mptr++)
                if (timer < mptr->stop)
                    flag2 = 0;
            node_t *csptr = C->head->next;
            while (csptr != C->head) {
                if (csptr->cs->st == WAITING) {
                    flag3 = 0;
                    break;
                }
                csptr = csptr->next;
            }
            if ((flag1 && flag3) || (flag2))
                terminate = 1;
        }
        await(&barrier);
    }
}

void* customer(void *arg)
{
    customer_t* cs = (customer_t*) arg;
    while (1)
    {
        // 1: machine phase
        await(&barrier);

        // 2: customer phase
        switch (cs->st)
        {
            case INVALID:
                if (timer >= cs->arr) {
                    pthread_mutex_lock(&mutex);
                    if (waiting < k) {
                        waiting++;
                        pthread_mutex_unlock(&mutex);
                        cs->st = ARRIVED;
                        printf("Customer %d enters at %d seconds\n", cs->id, timer);
                        printf(YELLOW "Customer %d orders %d ice cream(s)" RESET "\n", cs->id, cs->nord);
                        int i = 1;
                        for (order_t* optr = cs->ord; optr < cs->ord + cs->nord; optr++, i++) {
                            printf(YELLOW "Ice cream %d: %s " RESET, i, optr->flav);
                            for (char* tptr = optr->top; tptr < optr->top + (optr->ntop * 64 * sizeof(char)); tptr += 64)
                                printf(YELLOW "%s " RESET, tptr);
                            printf("\n");
                        }
                    }
                    else {
                        pthread_mutex_unlock(&mutex);
                        cs->st = LEFT;
                        printf(RED "Customer %d failed to enter and left at %d second(s)" RESET "\n", cs->id, timer);
                    }
                }
                break;

            case SERVED:
                cs->st = LEFT;
                pthread_mutex_lock(&mutex);
                waiting--;
                pthread_mutex_unlock(&mutex);
                printf(GREEN "Customer %d has collected their order(s) and left at %d second(s)" RESET "\n", cs->id, timer);
                break;
            
            case SHORTAGE:
                cs->st = LEFT;
                pthread_mutex_lock(&mutex);
                waiting--;
                pthread_mutex_unlock(&mutex);
                printf(RED "Customer %d left at %d second(s) with an unfulfilled order" RESET "\n", cs->id, timer);
                break;

            case CLOSED:
                cs->st = LEFT;
                pthread_mutex_lock(&mutex);
                waiting--;
                pthread_mutex_unlock(&mutex);
                printf(RED "Customer %d was not serviced due to unavailability of machines" RESET "\n", cs->id);
                break;

            default:
        }
        await(&barrier);

        // 3: termination phase
        if (terminate) {
            if (cs->st == WAITING)
                printf(RED "Customer %d was not serviced due to unavailability of machines" RESET "\n", cs->id);
            return NULL;
        }
        await(&barrier);

        // 4: termination check phase
        await(&barrier);
    }
}
