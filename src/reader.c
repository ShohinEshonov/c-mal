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




static MalType* read_macro(Reader* reader, char* name)
{
  MalType *form = read_form(reader);
  if(!form)
  {
    fprintf(stderr, "Error: Expected form after macros '%s'\n", name);
    return NULL;
  }

  
  MalType *list = mal_new_list();
  
  mal_list_append(list, mal_new_symbol(name));
  mal_list_append(list, form);

  return list;
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
    return mal_new_symbol(symbol);
  }else if(t->type == TOKEN_STRING)
  {
    return mal_new_string(t->start_ptr, t->length);
  }else if(t->type == TOKEN_KEYWORD)
  {
    return mal_new_keyword(t->start_ptr, t->length);
  }else if(t->type == TOKEN_NUMBER_INT)
  {
    return mal_new_num_int(t->start_ptr, t->length);
  }else if(t->type == TOKEN_NUMBER_DOUBLE)
  {
    return mal_new_num_double(t->start_ptr, t->length);
  }else if(t->type == TOKEN_AT)
  {
    return read_macro(reader, "deref");
  }else if(t->type == TOKEN_QUOTE)
  {
    return read_macro(reader, "quote");
  }else if(t->type == TOKEN_BACKTICK)
  {
    return read_macro(reader, "quasiquote"); 
  }else if(t->type == TOKEN_TILDE)
  {
    return read_macro(reader, "unquote");
  }else if(t->type == TOKEN_SPLICE)
  {
    return read_macro(reader, "splice-unquote");
  }
  return NULL;  
}


static MalType* read_list(Reader* reader, TokenType closingToken)
{
  next(reader);
  MalType *list;
  if(closingToken == TOKEN_R_PAREN)
  {    
    list = mal_new_list();
  }

  if(closingToken == TOKEN_R_BRACKET)
  {    
    list = mal_new_vector();
  }

  if(closingToken == TOKEN_R_BRACE)
  {
    list = mal_new_hashmap();
  }
  
  Token *t;
  while(1)
  {
    t = peek(reader);
    if(t->type == TOKEN_EOF)
    {
      fprintf(stderr, "%d ERROR: Unclosed parentesize.\n", t->line);
      free_ast(list);
      return NULL;
    
    }
    if(t->type == closingToken)
    {
      next(reader);
      break;
    }
    MalType *item = read_form(reader);
    if(!item)
    {
      free_ast(list);
      return NULL;
    }
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
    return  read_list(reader, TOKEN_R_PAREN);
  }else if(t->type == TOKEN_L_BRACKET)
  {
    return read_list(reader, TOKEN_R_BRACKET);
  }else if(t->type == TOKEN_L_BRACE)
  {
    return read_list(reader, TOKEN_R_BRACE);
  }else if(t->type == TOKEN_CARET)
  {
    next(reader);
    MalType* meta = read_form(reader);
    if(!meta)
    {
      fprintf(stderr, "Error: Expected form after ^.\n");
      return NULL;
    }
    MalType* data = read_form(reader);
    if(!data)
    {
      fprintf(stderr, "Error: Expected form after meta.\n");
      free_ast(meta);
      return NULL;
    }
    if(data->type == MAL_TRUE || data->type == MAL_FALSE || data->type == MAL_NIL)
    {
      fprintf(stderr, "Error: Cannot attach meta-data to  true/false/nil value.i\n");
      free_ast(meta);
      free_ast(data);
      return NULL;
    }
    return wrap_meta(data, meta);
  }
  else
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

