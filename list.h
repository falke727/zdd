/* list.h */

#ifndef __TANAKALAB_LIST_H__
#define __TANAKALAB_LIST_H__

#ifndef __TANAKALAB_BOOL_H__
#include "bool.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************* list unsigned *************************/
struct LIST_UNSIGNED_CELL {
  unsigned key;
  struct LIST_UNSIGNED_CELL* prev;
  struct LIST_UNSIGNED_CELL* next;
};
typedef struct LIST_UNSIGNED_CELL list_unsigned_cell;

struct LIST_UNSIGNED {
  list_unsigned_cell* head;
  list_unsigned_cell* last;
  unsigned size;
};
typedef struct LIST_UNSIGNED list_unsigned;

bool list_unsigned_member(list_unsigned*, unsigned);
void list_unsigned_remove_head(list_unsigned*);
bool list_unsigned_is_empty(list_unsigned*);
list_unsigned_cell* list_unsigned_search(list_unsigned*, unsigned);
void list_unsigned_add_rear(list_unsigned*, unsigned);
void list_unsigned_add_rear_sub(list_unsigned*, list_unsigned_cell*);
void list_unsigned_add_front(list_unsigned*, unsigned);
void list_unsigned_add_front_sub(list_unsigned*, list_unsigned_cell*);
void list_unsigned_delete(list_unsigned*, unsigned);
void list_unsigned_delete_sub(list_unsigned*, list_unsigned_cell*);
list_unsigned* list_unsigneds_concat(list_unsigned*, list_unsigned*);
void list_unsigned_clear(list_unsigned*);
void list_unsigned_print(list_unsigned*);

bool list_unsigned_member(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x;
  for (x = L->head; NULL != x; x = x->next) {
    if (x->key == u) { return true; }
  }
  return false;
}

void list_unsigned_remove_head(list_unsigned* L) { list_unsigned_delete_sub(L, L->head); }
bool list_unsigned_is_empty(list_unsigned* L) { return (L->size == 0); }

list_unsigned_cell* list_unsigned_search(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x = L->head;
  while (NULL != x && x->key != u) { x = x->next; }
  return x;
}

void list_unsigned_add_front(list_unsigned* L, unsigned u) {
  list_unsigned_cell* new = (list_unsigned_cell*)malloc(sizeof(list_unsigned_cell));
  L->size = L->size + 1;
  new->key = u;
  list_unsigned_add_front_sub(L,new);
}

void list_unsigned_add_front_sub(list_unsigned* L, list_unsigned_cell* x) {
  x->next = L->head;
  if (NULL != L->head) { L->head->prev = x; }
  else { L->last = x; }
  L->head = x;
  x->prev = NULL;
}

void list_unsigned_add_rear(list_unsigned* L, unsigned u) {
  list_unsigned_cell* new = (list_unsigned_cell*)malloc(sizeof(list_unsigned_cell));
  L->size = L->size + 1;
  new->key = u;
  list_unsigned_add_rear_sub(L,new);
}

void list_unsigned_add_rear_sub(list_unsigned* L, list_unsigned_cell* x) {
  x->prev = L->last;
  /* L is empty */
  if (NULL != L->last) { L->last->next = x; }
  else { L->head = x; }
  L->last = x;
  x->next = NULL;
}

void list_unsigned_delete(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x = list_unsigned_search(L, u);
  if (NULL != x) { list_unsigned_delete_sub(L, x); }
}

void list_unsigned_delete_sub(list_unsigned* L, list_unsigned_cell* x) {
  L->size = L->size-1;
  if (NULL != x->prev) { x->prev->next = x->next; }
  else { L->head = x->next; }
  if (NULL != x->next) { x->next->prev = x->prev; }
  else { L->last = x->prev; }
  free(x);
}

/* L1 := L1 ++ L2 */
list_unsigned* list_unsigneds_concat(list_unsigned* L1, list_unsigned* L2) {
  if (NULL == L1) { return L2; }
  L1->last->next = L2->head;
  if (NULL != L2) { L2->head->prev = L1->last; }
  L1->size = L1->size + L2->size;
  return L1;
}

void list_unsigned_clear(list_unsigned* L) {
  if (NULL == L) { return; }
  list_unsigned_cell* p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q);
  }
}

void list_unsigned_print(list_unsigned* L) {
  list_unsigned_cell* p = L->head;
  if (NULL != p) { 
    printf("%u", p->key); 
    for (p = p->next; NULL != p; p = p->next) { printf(", %u", p->key); } 
  }
}

#endif
