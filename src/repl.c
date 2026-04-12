#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "tokenizer.h"
#include "reader.h"
#include "types.h"




MalType *READ(TokenArray tokens)
{
  Reader* reader = init_reader(tokens);
  MalType* ast = read_form(reader);
  free(reader);
  return ast; 
}

char *EVAL(char *str) { return str; }

void PRINT(MalType* ast) { pr_str(ast, true); printf("\n");}

void rep(char *str) {

  Tokenizer *tokenizer = init_tokenizer(str);
  TokenArray tokens = tokenize(tokenizer);


  // for(size_t i = 0; i < tokens.count; i++)
  // {
  //   if(tokens.items[i].type == TOKEN_L_PAREN)
  //   {
  //     printf("TOKEN_L_PAREN\n");
  //   }else if(tokens.items[i].type == TOKEN_R_PAREN)
  //   {
  //     printf("TOKEN_R_PAREN\n");
  //   }else if(tokens.items[i].type == TOKEN_L_BRACKET)
  //   {
  //     printf("TOKEN_L_BRACKET\n");
  //   }else if(tokens.items[i].type == TOKEN_R_BRACKET)
  //   {
  //     printf("TOKEN_R_BRACKET\n");
  //   }else if(tokens.items[i].type == TOKEN_L_BRACE)
  //   {
  //     printf("TOKEN_L_BRACE");
        
  //   }else if(tokens.items[i].type == TOKEN_R_BRACE)
  //   {
  //     printf("TOKEN_R_BRACE\n");
  //   }else if(tokens.items[i].type == TOKEN_QUOTE)
  //   {
  //     printf("TOKEN_QUOTE\n");
  //   }else if(tokens.items[i].type == TOKEN_BACKTICK)
  //   {
  //     printf("TOKEN_BACKTICK\n");
  //   }else if(tokens.items[i].type == TOKEN_TILDE)
  //   {
  //     printf("TOKEN_TILDE\n");
  //   }else if(tokens.items[i].type == TOKEN_CARET)
  //   {
  //     printf("TOKEN_CARET\n");
  //   }else if(tokens.items[i].type == TOKEN_AT)
  //   {
  //     printf("TOKEN_AT\n");
  //   }else if(tokens.items[i].type == TOKEN_SPLICE)
  //   {
  //     printf("TOKEN_SPLICE");
  //   }else if(tokens.items[i].type == TOKEN_STRING)
  //   {
  //     char *str = strndup(tokens.items[i].start_ptr, tokens.items[i].length);
  //     printf("TOKEN_STRING: %s\n", str);
  //     free(str);
  //   }else if(tokens.items[i].type == TOKEN_SYMBOL)
  //   {
  //     char *symb =  strndup(tokens.items[i].start_ptr, tokens.items[i].length);
  //     printf("TOKEN_SYMBOL: %s\n", symb);
  //     free(symb);
  //   }else if(tokens.items[i].type == TOKEN_KEYWORD)
  //   {
  //     char *keyword =  strndup(tokens.items[i].start_ptr, tokens.items[i].length);
  //     printf("TOKEN_KEYWORD: %s\n", keyword);
  //     free(keyword);
  //   }else if(tokens.items[i].type == TOKEN_NUMBER_INT)
  //   {
  //     char *num_int = strndup(tokens.items[i].start_ptr, tokens.items[i].length);
  //     printf("TOKEN_NUMBER_INT: %s\n", num_int);
  //     free(num_int);
  //   }else if(tokens.items[i].type == TOKEN_NUMBER_DOUBLE)
  //   {
  //     char *num_dou = strndup(tokens.items[i].start_ptr, tokens.items[i].length);
  //     printf("TOKEN_NUMBER_DOUBLE: %s\n", num_dou);
  //     free(num_dou);
  //   }
  //   else if(tokens.items[i].type == TOKEN_EOF)
  //   {
  //     printf("TOKEN_EOF\n");
  //   }
  // }

  MalType* ast = READ(tokens);
  if(ast == NULL)
  {
    tokenizer_free(tokenizer);
    return;
  }
  PRINT(ast);
  tokenizer_free(tokenizer);
  free_ast(ast);
}

int main() {
  while (1) {
    char buffer[2048];
    printf("user> ");
    fflush(stdout);
    if(fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
      break;
    }
    rep(buffer);
  }
}
