#include "tokenizer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void da_append(TokenArray *tokens, Token *token) {
  if (tokens->count >= tokens->capacity) {
    if (tokens->capacity == 0) {
      tokens->capacity = 16;
    } else {
      tokens->capacity *= 2;
    }
    void *tmp =
        realloc(tokens->items, tokens->capacity * sizeof(*tokens->items));
    if (!tmp) {
      perror("realloc failed");
      exit(1);
    }
    tokens->items = tmp;
  }
  tokens->items[tokens->count++] = *token;
}

void free_tokens(TokenArray *tokens) {
  free(tokens->items);
}

void tokenizer_free(Tokenizer *t) {
  free_tokens(&t->tokens);
  free(t);
}

static char next(Tokenizer *tokenizer) {
  return tokenizer->source[tokenizer->current++];
}

static bool is_at_end(Tokenizer *tokenizer) {
  return tokenizer->source[tokenizer->current] == '\0';
}

static char peek(Tokenizer *tokenizer) {
  if (is_at_end(tokenizer)) {
    return '\0';
  }
  return tokenizer->source[tokenizer->current];
}

static void scan_string(Tokenizer *tokenizer) {
  while (peek(tokenizer) != '"') {
    if (is_at_end(tokenizer)) {
      fprintf(stderr, "%d:%d Error: Unterminated string.\n", tokenizer->line,
              tokenizer->current);
      return;
    }
    if (peek(tokenizer) == '\\') {
      next(tokenizer);
      if (is_at_end(tokenizer)) {
        fprintf(stderr, "%d:%d Error: Undeterminated escape.\n",
                tokenizer->line, tokenizer->current);
      }
      next(tokenizer);
      continue;
    }
    next(tokenizer);
  }
  next(tokenizer);
  Token token = {.type = TOKEN_STRING,
                 .start_ptr = &tokenizer->source[tokenizer->start],
                 .length = tokenizer->current - 1 - tokenizer->start,
                 .line = tokenizer->line};
  da_append(&tokenizer->tokens, &token);
}

static void scan_symbol(Tokenizer *tokenizer) {
  while (!is_at_end(tokenizer) && !isspace(peek(tokenizer)) &&
         !strchr("()[]{}'`~^@", peek(tokenizer))) {
    next(tokenizer);
  }

  Token token = {.type = TOKEN_SYMBOL,
                 .start_ptr = &tokenizer->source[tokenizer->start],
                 .length = tokenizer->current - tokenizer->start,
                 .line = tokenizer->line};
  da_append(&tokenizer->tokens, &token);
}

void scan_number(Tokenizer *tokenizer) {
  TokenType type = TOKEN_NUMBER_INT;
  while (isdigit((unsigned char)peek(tokenizer))) {
    next(tokenizer);
  }
  if (peek(tokenizer) == '.') {
    next(tokenizer);
    if (!isdigit((unsigned char)peek(tokenizer))) {
      fprintf(stderr, "%d:%d Unexpected character after '.' .", tokenizer->line,
              tokenizer->current);
      return;
    }

    type = TOKEN_NUMBER_DOUBLE;

    while (isdigit((unsigned char)peek(tokenizer))) {
      next(tokenizer);
    }
  }

  Token token = {.type = type,
                 .start_ptr = &tokenizer->source[tokenizer->start],
                 .length = tokenizer->current - tokenizer->start,
                 .line = tokenizer->line};

  da_append(&tokenizer->tokens, &token);
}

static void scan_token(Tokenizer *tokenizer) {
  char c = next(tokenizer);

  if (c == ' ' || c == '\t' || c == '\r' || c == ',') {
  } else if (c == '\n') {
    tokenizer->line++;
  } else if (c == '~') {
    if (peek(tokenizer) == '@') {
      Token token = {.type = TOKEN_SPLICE, .line = tokenizer->line};
      da_append(&tokenizer->tokens, &token);
      next(tokenizer);
    } else {
      Token token = {.type = TOKEN_TILDE, .line = tokenizer->line};
      da_append(&tokenizer->tokens, &token);
    }
  } else if (c == '(') {
    Token token = {.type = TOKEN_L_PAREN, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == ')') {
    Token token = {.type = TOKEN_R_PAREN, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '[') {
    Token token = {.type = TOKEN_L_BRACKET, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == ']') {
    Token token = {.type = TOKEN_R_BRACKET, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '{') {
    Token token = {.type = TOKEN_L_BRACE, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '}') {
    Token token = {.type = TOKEN_R_BRACE, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '\'') {
    Token token = {.type = TOKEN_QUOTE, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '`') {
    Token token = {.type = TOKEN_BACKTICK, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '^') {
    Token token = {.type = TOKEN_CARET, .line = tokenizer->line};
    da_append(&tokenizer->tokens, &token);
  } else if (c == '"') {
    tokenizer->start++;
    scan_string(tokenizer);
  } else if (c == ';') {
    while (peek(tokenizer) != '\n' && !is_at_end(tokenizer)) {
      next(tokenizer);
    }
  } else if (!isspace(c) && !strchr("()[]{}'`~^@", c)) {
    scan_symbol(tokenizer);
  } else if (isdigit((unsigned char)c)) {
    scan_number(tokenizer);
  } else {
    fprintf(stderr, "%d:%d Error: Unexpected character\n", tokenizer->line,
            tokenizer->current);
  }
}

TokenArray tokenize(Tokenizer *tokenizer) {
  while (!is_at_end(tokenizer)) {
    tokenizer->start = tokenizer->current;
    scan_token(tokenizer);
  }
  Token eof = {.type = TOKEN_EOF, .line = tokenizer->line};
  da_append(&tokenizer->tokens, &eof);
  TokenArray result = tokenizer->tokens;
  return result;
}

Tokenizer *init_tokenizer(char *source) {
  Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
  if (!tokenizer) {
    perror("Cannot allocate tokenizer");
    exit(1);
  }
  tokenizer->source = source;
  TokenArray tokens;
  tokens.count = 0;
  tokens.capacity = 0;
  tokens.items = NULL;
  tokenizer->tokens = tokens;
  tokenizer->current = 0;
  tokenizer->start = 0;
  tokenizer->line = 1;
  return tokenizer;
}
