#include "types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

MalType *sum(MalType *args) {
  double result = 0;
  int has_double = 0;
  for (int i = 0; i < args->value.list.count; i++) {
    if (args->value.list.items[i]->type == MAL_DOUBLE) {
      has_double = 1;
    }
    if (args->value.list.items[i]->type != MAL_INT &&
        args->value.list.items[i]->type != MAL_DOUBLE) {
      fprintf(stderr, "+ operator accepts only int or double.\n");
      return NULL;
    }
    result += to_double(args->value.list.items[i]);
  }
  if (has_double) {
    MalType *res = mal_new_num_double(result);
    return res;
  } else {
    MalType *res = mal_new_num_int((int64_t)result);
    return res;
  }
}

MalType *sub(MalType *args) {
  double result = 0;
  int has_double = 0;
  if (args->value.list.items[0]->type == MAL_INT) {
    result = args->value.list.items[0]->value.number_int;
  } else if (args->value.list.items[0]->type == MAL_DOUBLE) {
    result = args->value.list.items[0]->value.number_double;
    has_double = 1;
  }else
  {
    fprintf(stderr, "- operator accepts only int or double.\n");
    return NULL;
  }
  for (int i = 1; i < args->value.list.count; i++) {
    if (args->value.list.items[i]->type == MAL_DOUBLE) {
      has_double = 1;
    }
    if (args->value.list.items[i]->type != MAL_INT &&
        args->value.list.items[i]->type != MAL_DOUBLE) {
      fprintf(stderr, "- operator accepts only int or double.\n");
      return NULL;
    }
    result -= to_double(args->value.list.items[i]);
  }
  if (has_double) {
    MalType *res = mal_new_num_double(result);
    return res;
  } else {
    MalType *res = mal_new_num_int((int64_t) result);
    return res;
  }
}

MalType *mult(MalType *args) {
  double result = 1;
  int has_double = 0;
  for (int i = 0; i < args->value.list.count; i++) {
    if (args->value.list.items[i]->type == MAL_DOUBLE) {
      has_double = 1;
    }
    if (args->value.list.items[i]->type != MAL_INT &&
        args->value.list.items[i]->type != MAL_DOUBLE) {
      fprintf(stderr, "* operator accepts only int or double.\n");
      return NULL;
    }
    result *= to_double(args->value.list.items[i]);
  }
  if (has_double) {
    MalType *res = mal_new_num_double(result);
    return res;
  } else {
    MalType *res = mal_new_num_int((int64_t) result);
    return res;
  }
}

MalType *division(MalType *args) {
  double result = 1;
  int has_double = 0;
  if (args->value.list.items[0]->type == MAL_INT) {
    result = args->value.list.items[0]->value.number_int;
  } else if (args->value.list.items[0]->type == MAL_DOUBLE) {
    result = args->value.list.items[0]->value.number_double;
    has_double = 1;
  }else
  {
    fprintf(stderr, "/ operator accepts only int or double.\n");
    return NULL;
  }
  for (int i = 1; i < args->value.list.count; i++) {
    if (args->value.list.items[i]->type == MAL_DOUBLE) {
      has_double = 1;
    }
    if (args->value.list.items[i]->type != MAL_INT &&
        args->value.list.items[i]->type != MAL_DOUBLE) {
      fprintf(stderr, "/ operator accepts only int or double.\n");
      return NULL;
    }
    result /= to_double(args->value.list.items[i]);
  }
  if (has_double) {
    MalType *res = mal_new_num_double(result);
    return res;
  } else {
    MalType *res = mal_new_num_int((int64_t) result);
    return res;
  }
}
