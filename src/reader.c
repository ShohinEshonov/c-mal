#include "reader.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void mal_list_append(MalType *list, MalType *item) {
  if (list->value.list.count >= list->value.list.capacity) {
    if (list->value.list.capacity == 0) {
      list->value.list.capacity = 16;
    } else {
      list->value.list.capacity *= 2;
    }
    void *tmp =
        realloc(list->value.list.items, list->value.list.capacity * sizeof(MalType*));
    if (!tmp) {
      perror("realloc failed");
      exit(1);
    }
    list->value.list.items = tmp;
  }
  list->value.list.items[list->value.list.count++] = item;
}


static MalType *mal_new_list()
{
  MalType* node = malloc(sizeof(MalType));
  node->type = MAL_LIST;
  node->value.list.items = NULL;
  node->value.list.count = 0;
  return node;
}

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
    MalType* item = malloc(sizeof(MalType));
    item->type = MAL_SYMBOL;
    item->value.symbol = strndup(t->lexeme_str, t->length);
    return item;
  }else if(t->type == TOKEN_STRING)
  {
    MalType* item = malloc(sizeof(MalType));
    item->type = MAL_STRING;
    item->value.str = strndup(t->lexeme_str, t->length);
    return item;
  }
  
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
      fprintf(stderr, "%d ERROR: Unclosed  parentesize.", t->line);
      exit(1);
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

