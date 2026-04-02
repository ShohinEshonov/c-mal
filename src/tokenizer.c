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
  for (int i = 0; i < tokens->count; i++) {
    Token *t = &tokens->items[i];

    if (t->type == TOKEN_STRING) {
      free(t->lexeme_str);
    }
  }
  free(tokens->items);
}

static void tokenizer_free(Tokenizer *t) {
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

static void parse_string(Tokenizer *tokenizer) {
  char buffer[2048];
  int len = 0;
  while (peek(tokenizer) != '"') {
    if (is_at_end(tokenizer)) {
      fprintf(stderr, "%d:%d Error: Unterminated string.\n", tokenizer->current,
              tokenizer->line);
      tokenizer_free(tokenizer);
      exit(1);
    }

    char c = next(tokenizer);

    if (c == '\\') {
      c = next(tokenizer);

      if (is_at_end(tokenizer)) {
        fprintf(stderr, "%d:%d Error: Unterminated string.\n",
                tokenizer->current, tokenizer->line);
        tokenizer_free(tokenizer);
        exit(1);
      }

      switch (c) {
      case '"':
        buffer[len++] = '"';
        break;
      }
    } else {
      buffer[len++] = c;
    }

    if (len >= 2047) {
      fprintf(stderr, "String too long\n");
      tokenizer_free(tokenizer);
      exit(1);
    }
  }
  next(tokenizer);
  buffer[len] = '\0';

  char *str = strdup(buffer);
  Token token = {.type = TOKEN_STRING,
                 .lexeme_str = str,
                 .length = len,
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
    parse_string(tokenizer);
  } else if (c == ';') {
    while (peek(tokenizer) != '\n' && !is_at_end(tokenizer)) {
      next(tokenizer);
    }
  } else if (!isspace(c) && !strchr("()[]{}'`~^@", c)) {
    scan_symbol(tokenizer);
  } else {
    fprintf(stderr, "%d:%d Error: Unexpected character", tokenizer->line,
            tokenizer->current);
    tokenizer_free(tokenizer);
    exit(1);
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
