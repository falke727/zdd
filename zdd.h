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

int member(unsigned, unsigned, unsigned);
void delete(unsigned, unsigned, unsigned);
int insert(unsigned, unsigned, unsigned);
bool eq_zdd_table_entry(unsigned, unsigned, unsigned, unsigned);
uint32_t hf(unsigned, unsigned, unsigned);

bool eq_zdd_table_entry(unsigned k, unsigned var, unsigned left, unsigned right) {
  if (zdd_table[k]._var != var) { return false; }
  if (zdd_table[k]._left != left) { return false; }
  if (zdd_table[k]._right != right) { return false; }
  return true;
}

int insert(unsigned var, unsigned left, unsigned right) {
  int i, k, found = -1;
  oed cstate;

  k = i = hf(var, left, right);
  do {
    cstate = zdd_table[k].state;
    if (empty == cstate || deleted == cstate) { if (found < 0) { found = k; } } /* there is an empty cell */
    else { if (eq_zdd_table_entry(k, var, left, right)) { return -1; } } /* input node already exists */
    k = (k+1) % NUM_OF_NODES;
  } while (empty != cstate && k != i);

  if (found < 0) {
    printf("Dictionary is full.\n");
    return -2;
  }

  zdd_table[found].state = occupied;
  zdd_table[found]._var = var;
  zdd_table[found]._left = left;
  zdd_table[found]._right = right;
  
  return found;
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

int member(unsigned var, unsigned left, unsigned right) {
  int i, k;
  oed cstate;

  k = i = hf(var, left, right);
  do {
    cstate = zdd_table[k].state;
    if (occupied == cstate) {
      if (eq_zdd_table_entry(k, var, left, right)) { return k; } /* find the input node */
    }
    k = (k+1) % NUM_OF_NODES;
  } while (empty != cstate && k != i);
  return -2; /* there is no input node */
}

uint32_t hf(unsigned var, unsigned l, unsigned r) { return (var+l+r) % NUM_OF_NODES; }

void inc_refcounter(unsigned P) { ++zdd_table[P]._ref_cnt; }

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

int member_op(op, unsigned, unsigned);
void delete_op(op, unsigned, unsigned);
void insert_op(op, int, int, int);
bool eq_op_table_entry(unsigned, op, unsigned, unsigned);
uint32_t hf_op(op, unsigned, unsigned);

bool eq_op_table_entry(unsigned k, op o, unsigned f, unsigned g) {
  if (op_cache_table[k]._op != o) { return false; }
  if (op_cache_table[k]._F != f) { return false; }
  if (op_cache_table[k]._G != g) { return false; }
  return true;
}

int member_op(op o, unsigned f, unsigned g) {
  int i, k;
  oed cstate;

  k = i = hf_op(o, f, g);
  do {
    cstate = op_cache_table[k].state;
    if (occupied == cstate) {
      if (eq_op_table_entry(k, o, f, g)) { return op_cache_table[k]._H; } /* find the input node */
    }
    k = (k+1) % OP_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);
  return -2; /* there is no input node */
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

void insert_op(op o, int f, int g, int h) {
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

/************************************** Count Cache Table **************************************/

typedef struct {
  unsigned _P;
  unsigned sum;
  oed state;
} count_cache_element;

uint32_t COUNT_CACHE_TABLE_SIZE;
count_cache_element* count_cache_table = NULL; 

int member_count(const unsigned);
void delete_count(const unsigned);
void insert_count(const int, const int);
uint32_t hf_count(const unsigned);

int member_count(const unsigned P) {
  int i, k;
  oed cstate;

  k = i = hf_count(P);
  do {
    cstate = count_cache_table[k].state;
    if (occupied == cstate) {
      if (count_cache_table[k]._P == P) { return count_cache_table[k].sum; } /* find the input node */
    }
    k = (k+1) % COUNT_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);
  return -2; /* there is no input node */
}

void delete_count(const unsigned P) {
  int i, k;
  oed cstate;

  k = i = hf_count(P);
  do {
    cstate = count_cache_table[k].state;
    if (occupied == cstate) {
      if (count_cache_table[k]._P == P) { count_cache_table[k].state = deleted; return; } /* find the input node and delete it */
    }
    k = (k+1) % COUNT_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);
  return; /* there is no input node */
}

void insert_count(const int P, const int n) {
  int i, k, found = -1;
  oed cstate;

  k = i = hf_count(P);
  do {
    cstate = count_cache_table[k].state;
    if (empty == cstate || deleted == cstate) { if (found < 0) { found = k; } } /* there is an empty cell */
    else { if (count_cache_table[k]._P == P) { return; } } /* input node already exists */
    k = (k+1) % COUNT_CACHE_TABLE_SIZE;
  } while (empty != cstate && k != i);

  if (-1 == found) { found = i; } /* update the tables by new entry */

  count_cache_table[found].state = occupied;
  count_cache_table[found].sum = n;
}


uint32_t hf_count(const unsigned P) {
  return P % OP_CACHE_TABLE_SIZE;
}





/******************************************* ZDD manipulating routine *******************************************/

typedef struct {
  list_unsigned* ref0; 
} zdd_manager;


void zdd_init(const unsigned); /* initialize the table size */

unsigned topvar(const unsigned);
unsigned get_left(const unsigned);
unsigned get_right(const unsigned);
int getnode(const unsigned, const unsigned, const unsigned);
int union_(const int, const int);
int intersection(const int, const int);
int setminus(const int, const int);
unsigned count(const int);

unsigned topvar(const unsigned P) { return zdd_table[P]._var; }
unsigned get_left(const unsigned P) { return zdd_table[P]._left; }
unsigned get_right(const unsigned P) { return zdd_table[P]._right; }

int getnode(const unsigned var, const unsigned left, const unsigned right) {
  if (0 == right) { return left; }

  int P = member(var, left, right);
  if (-1 != P) {
    inc_refcounter(P);
    return P;
  }
  P = insert(var, left, right);
  return P;
}

int union_(const int P, const int Q) {
  if (0 == P) { return Q; }
  if (0 == Q || P == Q) { return P; }

  int R = member_op(_union, P, Q);
  if (-2 != R) { return R; }
  if (topvar(P) < topvar(Q)) { R = getnode(topvar(Q), union_(get_left(P), Q), get_right(P)); }
  if (topvar(P) > topvar(Q)) { R = getnode(topvar(Q), union_(P, get_left(Q)), get_right(Q)); }
  if (topvar(P) == topvar(Q)) { R = getnode(topvar(P), union_(get_left(P), get_left(Q)), union_(get_right(P), get_right(Q))); }
  insert_op(_union, P, Q, R);
  return R;
}

int intersection(const int P, const int Q) {
  if (0 == P || 0 == Q) { return 0; }
  if (P == Q) { return P; }
  int R = member_op(_intersection, P, Q);
  if (-2 != R) { return R; }
  if (topvar(P) < topvar(Q)) { R = intersection(get_left(P), Q); }
  if (topvar(P) > topvar(Q)) { R = intersection(P, get_left(Q)); }
  if (topvar(P) == topvar(Q)) { R = getnode(topvar(P), intersection(get_left(P), get_left(Q)), intersection(get_right(P), get_right(Q))); }
  insert_op(_intersection, P, Q, R);
  return R;
}

int setminus(const int P, const int Q) {
  if (0 == P || P == Q) { return 0; }
  if (0 == Q) { return P; }
  int R = member_op(_setminus, P, Q);
  if (-2 != R) { return R; }
  if (topvar(P) < topvar(Q)) { R = getnode(topvar(P), setminus(get_left(P), Q), get_right(P)); }
  if (topvar(P) > topvar(Q)) { R = setminus(P, get_left(Q)); }
  if (topvar(P) == topvar(Q)) { R = getnode(topvar(P), setminus(get_left(P), get_left(Q)), setminus(get_right(P), get_right(Q))); }
  insert_op(_setminus, P, Q, R);
  return R;
}

unsigned count(const int P) {
  if (0 == P) { return 0; }
  if (1 == P) { return 1; }
  int sum = member_count(P);
  if (-1 != sum) { return sum; }
  sum = count(get_left(P)) + count(get_right(P));
  insert_count(P, sum);
  return sum;
}

#endif
