#ifndef MAL_TYPES_H
#define MAL_TYPES_H


#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>



typedef enum
{
  MAL_SYMBOL,
  MAL_STRING,
  MAL_LIST,
  MAL_TRUE,
  MAL_FALSE,
  MAL_NIL,
  MAL_KEYWORD,
  MAL_INT,
  MAL_DOUBLE,

}MalTypeTag;


typedef struct MalType
{
  MalTypeTag type;
  union
  {
    int64_t number_int;
    double number_float;
    char* str;
    char* symbol;
    char* keyword;
    struct
    {
      struct MalType **items;
      int count;
      int capacity;
    }list;
  }value;
}MalType;

MalType *mal_new_list();
void mal_list_append(MalType *list, MalType *item);
MalType* mal_new_string(char *start_ptr, size_t length);
MalType* mal_new_symbol(char *symbol);
MalType* mal_new_keyword(char *start_ptr, size_t length);
MalType* mal_new_num_int(char *start_ptr, size_t length);
MalType* mal_new_num_double(char *start_ptr, size_t length);
void pr_str(MalType *node, bool print_readably);
void free_ast(MalType *node);


#endif
