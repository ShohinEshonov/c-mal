#ifndef REPL_H
#define REPL_H

#include "types.h"
#include "uthash.h"

typedef struct {
  char *key;
  MalType *value;
  UT_hash_handle hh;
} Env;

#endif
