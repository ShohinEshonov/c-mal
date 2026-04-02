#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tokenizer.h"
#include "reader.h"
#include "printer.h"

MalType *READ(TokenArray tokens)
{
  Reader* reader = init_reader(tokens);
  MalType* ast = read_form(reader);
  free_tokens(&tokens);
  free(reader);
  return ast; 
}

char *EVAL(char *str) { return str; }

void PRINT(MalType* ast) { pr_str(ast); printf("\n"); }

void rep(char *str) {

  Tokenizer *tokenizer = init_tokenizer(str);
  TokenArray tokens = tokenize(tokenizer);


  for(int i = 0; i < tokens.count; i++)
  {
    if(tokens.items[i].type == TOKEN_L_PAREN)
    {
      printf("TOKEN_L_PAREN\n");
    }else if(tokens.items[i].type == TOKEN_R_PAREN)
    {
      printf("TOKEN_R_PAREN\n");
    }else if(tokens.items[i].type == TOKEN_L_BRACKET)
    {
      printf("TOKEN_L_BRACKET\n");
    }else if(tokens.items[i].type == TOKEN_R_BRACKET)
    {
      printf("TOKEN_R_BRACKET\n");
    }else if(tokens.items[i].type == TOKEN_L_BRACE)
    {
      printf("TOKEN_L_BRACE");
        
    }else if(tokens.items[i].type == TOKEN_R_BRACE)
    {
      printf("TOKEN_R_BRACE\n");
    }else if(tokens.items[i].type == TOKEN_QUOTE)
    {
      printf("TOKEN_QUOTE\n");
    }else if(tokens.items[i].type == TOKEN_BACKTICK)
    {
      printf("TOKEN_BACKTICK\n");
    }else if(tokens.items[i].type == TOKEN_TILDE)
    {
      printf("TOKEN_TILDE\n");
    }else if(tokens.items[i].type == TOKEN_CARET)
    {
      printf("TOKEN_CARET\n");
    }else if(tokens.items[i].type == TOKEN_AT)
    {
      printf("TOKEN_AT\n");
    }else if(tokens.items[i].type == TOKEN_SPLICE)
    {
      printf("TOKEN_SPLICE");
    }else if(tokens.items[i].type == TOKEN_STRING)
    {
      printf("TOKEN_STRING: %s\n", tokens.items[i].lexeme_str);
    }else if(tokens.items[i].type == TOKEN_SYMBOL)
    {
      char *symb =  strndup(tokens.items[i].start_ptr, tokens.items[i].length);
      printf("TOKEN_SYMBOL: %s\n", symb);
      free(symb);
    }else if(tokens.items[i].type == TOKEN_EOF)
    {
      printf("TOKEN_EOF\n");
    }
  }

  MalType* ast = READ(tokens);
  PRINT(ast);
  free(tokenizer);
}

int main() {
  while (1) {
    char buffer[2048];
    printf("user> ");

    if(fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
      break;
    }
    rep(buffer);
  }
}
