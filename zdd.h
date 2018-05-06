/* zdd.h */

#ifndef __TANAKALAB_ZDD_H__
#define __TANAKALAB_ZDD_H__

#ifndef __TANAKALAB_BOOL_H__
#include "bool.h"
#endif

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

typedef enum { occupied, empty, deleted } oed;

typedef struct {
  unsigned _var;     // variable number (constant node has the value of the size of a table + 1)
  unsigned _left;    // 0 edge
  unsigned _right;   // 1 edge
  unsigned _ref_cnt; // reference counter
  oed state;
} zdd_element;

uint32_t NUM_OF_NODES;
zdd_element* zdd_table = NULL; 
// constant 0 is stored in zdd_table[0] and constant 1 is stored in zdd_table[1]

bool member(unsigned, unsigned, unsigned);
void delete(unsigned, unsigned, unsigned);
bool insert(unsigned, unsigned, unsigned);
bool eq_zdd_table_entry(unsigned, unsigned, unsigned, unsigned);
uint32_t hf(unsigned, unsigned, unsigned);

bool eq_zdd_table_entry(unsigned k, unsigned var, unsigned left, unsigned right) {
  if (zdd_table[k]._var != var) { return false; }
  if (zdd_table[k]._left != left) { return false; }
  if (zdd_table[k]._right != right) { return false; }
  return true;
}

bool insert(unsigned var, unsigned left, unsigned right) {
  int i, k, found = -1;
  oed cstate;

  k = i = hf(var, left, right);
  do {
    cstate = zdd_table[k].state;
    if (empty == cstate || deleted == cstate) { if (found < 0) { found = k; } } /* there is an empty cell */
    else { if (eq_zdd_table_entry(k, var, left, right)) { return true; } } /* input node already exists */
    k = (k+1) % NUM_OF_NODES;
  } while (empty != cstate && k != i);

  if (found < 0) {
    printf("Dictionary is full.\n");
    return false;
  }

  zdd_table[found].state = occupied;
  zdd_table[found]._var = var;
  zdd_table[found]._left = left;
  zdd_table[found]._right = right;
  
  return true;
}

void delete(unsigned var, unsigned left, unsigned right) {
  int i, k;
  oed cstate;

  k = i = hf(var, left, right);
  do {
    cstate = zdd_table[k].state;
    if (occupied == cstate) {
      if (eq_zdd_table_entry(k, var, left, right)) { zdd_table[k].state = deleted; return; } /* find the input node and delete it */
    }
    k = (k+1) % NUM_OF_NODES;
  } while (empty != cstate && k != i);
  return; /* there is no input node */
}

bool member(unsigned var, unsigned left, unsigned right) {
  int i, k;
  oed cstate;

  k = i = hf(var, left, right);
  do {
    cstate = zdd_table[k].state;
    if (occupied == cstate) {
      if (eq_zdd_table_entry(k, var, left, right)) { return true; } /* find the input node */
    }
    k = (k+1) % NUM_OF_NODES;
  } while (empty != cstate && k != i);
  return false; /* there is no input node */
}

uint32_t hf(unsigned var, unsigned l, unsigned r) { return (var+l+r) % NUM_OF_NODES; }

/************************************** Operation Cache Table **************************************/

typedef enum { _union, _intersection, _setminus } op;

typedef struct {
  op _op;
  unsigned _F;
  unsigned _G;
  unsigned _H;
  oed state;
} op_cache_element;

uint32_t OP_CACHE_TABLE_SIZE;
op_cache_element* op_cache_table = NULL; 

unsigned member_op(op, unsigned, unsigned);
void delete_op(op, unsigned, unsigned);
void insert_op(op, unsigned, unsigned, unsigned);
bool eq_op_table_entry(unsigned, op, unsigned, unsigned);
uint32_t hf_op(op, unsigned, unsigned);

bool eq_op_table_entry(unsigned k, op o, unsigned f, unsigned g) {
  if (op_cache_table[k]._op != o) { return false; }
  if (op_cache_table[k]._F != f) { return false; }
  if (op_cache_table[k]._G != g) { return false; }
  return true;
}

unsigned member_op(op o, unsigned f, unsigned g) {
  int i, k;
  oed cstate;

  k = i = hf_op(o, f, g);
  do {
    cstate = op_cache_table[k].state;
    if (occupied == cstate) {
      if (eq_op_table_entry(k, o, f, g)) { return true; } /* find the input node */
    }
    k = (k+1) % OP_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);
  return false; /* there is no input node */
}

void delete_op(op o, unsigned f, unsigned g) {
  int i, k;
  oed cstate;

  k = i = hf_op(o, f, g);
  do {
    cstate = op_cache_table[k].state;
    if (occupied == cstate) {
      if (eq_op_table_entry(k, o, f, g)) { op_cache_table[k].state = deleted; return; } /* find the input node and delete it */
    }
    k = (k+1) % OP_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);
  return; /* there is no input node */
}

void insert_op(op o, unsigned f, unsigned g, unsigned h) {
  int i, k, found = -1;
  oed cstate;

  k = i = hf_op(o, f, g);
  do {
    cstate = op_cache_table[k].state;
    if (empty == cstate || deleted == cstate) { if (found < 0) { found = k; } } /* there is an empty cell */
    else { if (eq_op_table_entry(k, o, f, g)) { return; } } /* input node already exists */
    k = (k+1) % OP_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);

  if (-1 == found) { found = i; } /* update the tables by new entry */

  op_cache_table[found].state = occupied;
  op_cache_table[found]._op = o;
  op_cache_table[found]._F = f;
  op_cache_table[found]._G = g;
  op_cache_table[found]._H = h;
}


uint32_t hf_op(op o, unsigned f, unsigned g) {
  unsigned uo = 0;
  if (_union == o) { o = 1; }
  if (_intersection == o) { o = 2; }
  if (_setminus == o) { o = 3; }
  return (uo + f + g) % OP_CACHE_TABLE_SIZE;
}

/******************************************* ZDD manipulating routine *******************************************/

typedef struct {
  list_unsigned* ref0; 
} zdd_manager;


void zdd_init(unsigned); /* initialize the table size */


#endif
