#ifndef MAL_TYPES_H
#define MAL_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef enum {
  MAL_SYMBOL,
  MAL_STRING,
  MAL_LIST,
  MAL_TRUE,
  MAL_FALSE,
  MAL_NIL,
  MAL_KEYWORD,
  MAL_INT,
  MAL_DOUBLE,
  MAL_VECTOR,
  MAL_HASHMAP,
  MAL_FUNC,
} MalTypeTag;

struct MalType;

typedef struct MalType MalType;

typedef MalType *(*MalFunc)(struct MalType *args);

typedef struct MalType {
  MalTypeTag type;
  union {
    int64_t number_int;
    double number_double;
    char *str;
    char *symbol;
    char *keyword;
    struct {
      struct MalType **items;
      int count;
      int capacity;
    } list;
    MalFunc func_ptr;
  } value;
  struct MalType *meta;
} MalType;

MalType *mal_new_list();
MalType *mal_new_vector();
MalType *mal_new_hashmap();
void mal_list_append(MalType *list, MalType *item);
MalType *mal_new_string(char *start_ptr, size_t length);
MalType *mal_new_symbol(char *symbol);
MalType *mal_new_keyword(char *start_ptr, size_t length);
MalType *mal_new_num_int(int64_t value);
MalType *mal_new_num_double(double value);
double to_double(MalType *val);
int to_int(MalType *val);
MalType *wrap_meta(MalType *data, MalType *meta);
void pr_str(MalType *node, bool print_readably);
void free_ast(MalType *node);

#endif
