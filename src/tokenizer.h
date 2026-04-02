#ifndef TOKENIZER_H

#define TOKENIZER_H


#include <unistd.h>


typedef enum
{
  TOKEN_L_PAREN,
  TOKEN_R_PAREN,

  TOKEN_L_BRACKET,
  TOKEN_R_BRACKET,

  TOKEN_L_BRACE,
  TOKEN_R_BRACE,
  
  TOKEN_QUOTE,
  TOKEN_BACKTICK,
  TOKEN_TILDE,
  TOKEN_CARET,
  TOKEN_AT,
  TOKEN_SPLICE,

  TOKEN_STRING,
  TOKEN_SYMBOL,

  TOKEN_EOF,
}TokenType;


typedef struct{
  TokenType type;
  union{
    char* lexeme_str;
    char*  start_ptr;
  };
  size_t length;
  int line;
}Token;


typedef struct
{
  Token* items;
  size_t count;
  size_t capacity;
}TokenArray;

typedef struct
{
  char* source;
  TokenArray tokens;
  int start;
  int current;
  int line; 
}Tokenizer;

Tokenizer *init_tokenizer(char* source);
TokenArray tokenize(Tokenizer *tokenizer);
void free_tokens(TokenArray *tokens);


#endif
