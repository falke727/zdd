#include <stdio.h>

#ifndef __TANAKALAB_ZDD_H__
#include "zdd.h"
#endif

int main(int argc, char** argv) {
  extern zdd_element* zdd_table;
  zdd_table = NULL;
  return 0;
}
