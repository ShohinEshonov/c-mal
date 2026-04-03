#include "reader.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// some singlton objects
MalType mal_nil = {.type = MAL_NIL};
MalType mal_true = {.type = MAL_TRUE};
MalType mal_false = {.type = MAL_FALSE};


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
  node->value.list.capacity = 0;
  return node;
}



static MalType *mal_new_string(char *start_ptr, size_t length)
{   
  MalType* item = malloc(sizeof(MalType));
  item->type = MAL_STRING;
  item->value.str = malloc(length + 1);
  size_t str_len = 0;

  
  for(size_t i = 0; i < length; i++)
  {
    char c = start_ptr[i];

    if(c == '\\' && i + 1 < length)
    {
      char next = start_ptr[++i];

      switch(next)
      {
        case 'n':
          item->value.str[str_len++] = '\n';
          break;
        case '"':
          item->value.str[str_len++] = '"';
          break;
        case '\\':
          item->value.str[str_len++] = '\\';
          break;
        default:
          item->value.str[str_len++] = next;
          break; 
      }
    }else{
      item->value.str[str_len++] = c;
    }
  }

  item->value.str[str_len] = '\0';
  return item;
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

