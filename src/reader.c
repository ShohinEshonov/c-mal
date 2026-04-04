#include "types.h"
#include "reader.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// some singlton objects
MalType mal_nil = {.type = MAL_NIL};
MalType mal_true = {.type = MAL_TRUE};
MalType mal_false = {.type = MAL_FALSE};



static Token* peek(Reader* reader)
{
  return &reader->source.items[reader->current];
}

static Token* next(Reader* reader)
{
  return &reader->source.items[reader->current++];
}






static MalType* read_atom(Reader* reader)
{
  Token* t = next(reader);


  if(t->type == TOKEN_SYMBOL)
  {
    char *symbol = strndup(t->start_ptr, t->length);
    if(strcmp(symbol, "true") == 0)
    {
      free(symbol);
      return &mal_true;
    }
    if(strcmp(symbol, "false") == 0)
    {
      free(symbol);
      return &mal_false;
    }
    if(strcmp(symbol, "nil") == 0)
    {
      free(symbol);
      return &mal_nil;
    }
    MalType *node = malloc(sizeof(MalType));
    node->type = MAL_SYMBOL;
    node->value.symbol = symbol;
    return node;
  }else if(t->type == TOKEN_STRING)
  {
    return mal_new_string(t->start_ptr, t->length);
  }
  return NULL;  
}


static MalType* read_list(Reader* reader)
{
  next(reader);
  MalType *list = mal_new_list();

  Token *t;
  while(1)
  {
    t = peek(reader);
    if(t->type == TOKEN_EOF)
    {
      fprintf(stderr, "%d ERROR: Unclosed  parentesize.\n", t->line);
      return NULL;
    
    }
    if(t->type == TOKEN_R_PAREN)
    {
      next(reader);
      break;
    }
    MalType *item = read_form(reader);
    mal_list_append(list, item);
  }
  return list;
}


MalType* read_form(Reader* reader)
{
  Token *t = peek(reader);
  if(t->type == TOKEN_EOF) return NULL;
  if(t->type == TOKEN_L_PAREN)
  {
    return  read_list(reader);
  }else
  {
     return read_atom(reader);
  }
}




Reader* init_reader(TokenArray tokens)
{
  Reader* reader = malloc(sizeof(Reader));
  reader->source = tokens;
  reader->current = 0;
  return reader;
}

