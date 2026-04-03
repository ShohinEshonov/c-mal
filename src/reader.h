#ifndef READER_H

#define READER_H


#include "tokenizer.h"
#include <stdint.h>

typedef struct
{
  TokenArray source;
  int current;  
}Reader;

typedef enum
{
  MAL_SYMBOL,
  MAL_STRING,
  MAL_LIST,
  MAL_TRUE,
  MAL_FALSE,
  MAL_NIL,
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
    struct
    {
      struct MalType **items;
      int count;
      int capacity;
    }list;
  }value;
}MalType;

Reader* init_reader(TokenArray tokens);
MalType* read_form(Reader* reader);

#endif
