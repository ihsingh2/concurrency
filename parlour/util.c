#include "parlour.h"

int numtokens(char* src, int len, char* delim)
{
  int num = 0;
  char* temp = (char*) calloc(1, len * sizeof(char));
  strncpy(temp, src, len);

  char* token = strtok(temp, delim);
  while (token != NULL) {
    num++;
    token = strtok(NULL, delim);
  }

  free(temp);
  return num;
}

void list_init(list_t* L)
{
  L->head = (node_t*) calloc(1, sizeof(node_t));
  L->head->prev = L->head;
  L->head->next = L->head;
}

void list_insert(list_t* L, customer_t* cs)
{
  node_t* x = (node_t*) malloc(sizeof(node_t));
  x->cs = cs;
  L->head->prev->next = x;
  x->prev = L->head->prev;
  L->head->prev = x;
  x->next = L->head;
}

void list_delete(list_t* L, node_t* x)
{
  if (L->head == x)
    return;
  x->prev->next = x->next;
  x->next->prev = x->prev;
  free(x);
}

void list_free(list_t* L)
{
  node_t *prev, *next;
  prev = L->head->next;
  while (prev != L->head)
  {
    next = prev->next;
    free(prev);
    prev = next;
  }
  free(prev);
}

void list_free_rec(list_t* L)
{
  node_t *prev, *next;
  prev = L->head->next;
  while (prev != L->head)
  {
    next = prev->next;
    for (order_t* optr = prev->cs->ord; optr < prev->cs->ord + prev->cs->nord; optr++)
      free(optr->top);
    free(prev->cs->ord);
    free(prev->cs);
    free(prev);
    prev = next;
  }
  free(prev);
}
