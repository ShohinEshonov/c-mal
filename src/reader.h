#ifndef READER_H

#define READER_H


#include "tokenizer.h"
#include "types.h"
#include <stdint.h>

typedef struct
{
  TokenArray source;
  int current;  
}Reader;


Reader* init_reader(TokenArray tokens);
MalType* read_form(Reader* reader);

#endif
