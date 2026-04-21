#include "repl.h"
#include "builtins.h"
#include "reader.h"
#include "string.h"
#include "tokenizer.h"
#include "types.h"
#include "uthash.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Env *init_env();
static void free_env(Env **env);

MalType *READ(TokenArray tokens) {
  Reader *reader = init_reader(tokens);
  MalType *ast = read_form(reader);
  free(reader);
  return ast;
}

MalType *EVAL(MalType *ast, Env *repl_env) {
  if (ast->type == MAL_SYMBOL) {
    Env *entry = NULL;
    HASH_FIND_STR(repl_env, ast->value.symbol, entry);
    if (entry == NULL) {
      fprintf(stderr, "Not found symbol: %s\n", ast->value.symbol);
      return NULL;
    } else {
      return entry->value;
    }
  }

  if (ast->type == MAL_LIST) {
    if (ast->value.list.count == 0) {
      fprintf(stderr, "Cannot eval empty list\n");
      return NULL;
    }
    MalType *f = EVAL(ast->value.list.items[0], repl_env);
    if (f && f->type == MAL_FUNC) {
      MalType *args = mal_new_list();
      for (int i = 1; i < ast->value.list.count; i++) {
        MalType *arg = EVAL(ast->value.list.items[i], repl_env);
        if (arg == NULL)
          return NULL;
        mal_list_append(args, arg);
      }
      return f->value.func_ptr(args);
    } else {
      fprintf(stderr, "Not a fucntion\n");
      return NULL;
    }
  }

  return ast;
}

void PRINT(MalType *ast) {
  pr_str(ast, true);
  printf("\n");
}

void rep(char *str, Env* repl_env) {

  Tokenizer *tokenizer = init_tokenizer(str);
  TokenArray tokens = tokenize(tokenizer);

  MalType *ast = READ(tokens);
  if (ast == NULL) {
    tokenizer_free(tokenizer);
    return;
  }
  MalType *evaluated = EVAL(ast, repl_env);
  if (evaluated == NULL) {
    tokenizer_free(tokenizer);
    free_ast(ast);
    return;
  }
  PRINT(evaluated);
  tokenizer_free(tokenizer);
  free_ast(ast);
}

void add_func(Env **env, char *key, MalType *value) {
  Env *entry = malloc(sizeof(Env));
  entry->key = strdup(key);
  entry->value = value;
  HASH_ADD_STR(*env, key, entry);
}

static Env *init_env() {
  Env *env = NULL;
  MalType *plus = malloc(sizeof(MalType));
  plus->type = MAL_FUNC;
  plus->value.func_ptr = &sum;
  plus->meta = NULL;
  add_func(&env, "+", plus);

  MalType *minus = malloc(sizeof(MalType));
  minus->type = MAL_FUNC;
  minus->value.func_ptr = &sub;
  minus->meta = NULL;
  add_func(&env, "-", minus);

  MalType *mul = malloc(sizeof(MalType));
  mul->type = MAL_FUNC;
  mul->value.func_ptr = &mult;
  mul->meta = NULL;
  add_func(&env, "*", mul);

  MalType *div = malloc(sizeof(MalType));
  div->type = MAL_FUNC;
  div->value.func_ptr = &division;
  div->meta = NULL;
  add_func(&env, "/", div);

  return env;
}

static void free_env(Env **env) {
    Env *entry, *tmp;
    HASH_ITER(hh, *env, entry, tmp) {
        HASH_DEL(*env, entry);
        free(entry->value);
        free(entry);
    }
}

int main() {
  Env *repl_env = init_env();
  while (1) {
    char buffer[2048];
    printf("user> ");
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      break;
    }
    rep(buffer, repl_env);
  }
  free_env(&repl_env);
}
