#include "types.h"
#include <asm-generic/errno.h>
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void pr_value(MalType *node, bool print_readably);

MalType *mal_new_list() {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_LIST;
  node->value.list.items = NULL;
  node->value.list.count = 0;
  node->value.list.capacity = 0;
  node->meta = NULL;
  return node;
}

MalType *mal_new_vector() {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_VECTOR;
  node->value.list.items = NULL;
  node->value.list.count = 0;
  node->value.list.capacity = 0;
  node->meta = NULL;
  return node;
}

MalType *mal_new_hashmap() {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_HASHMAP;
  node->value.list.items = NULL;
  node->value.list.count = 0;
  node->value.list.capacity = 0;
  node->meta = NULL;
  return node;
}

MalType *wrap_meta(MalType *data, MalType *meta) {
  MalType *wrap = malloc(sizeof(MalType));
  *wrap = *data;
  wrap->meta = meta;
  free(data);
  return wrap;
}

void mal_list_append(MalType *list, MalType *item) {
  if (list->value.list.count >= list->value.list.capacity) {
    if (list->value.list.capacity == 0) {
      list->value.list.capacity = 16;
    } else {
      list->value.list.capacity *= 2;
    }
    void *tmp = realloc(list->value.list.items,
                        list->value.list.capacity * sizeof(MalType *));
    if (!tmp) {
      perror("realloc failed");
      exit(1);
    }
    list->value.list.items = tmp;
  }
  list->value.list.items[list->value.list.count++] = item;
}

MalType *mal_new_string(char *start_ptr, size_t length) {
  MalType *item = malloc(sizeof(MalType));
  item->type = MAL_STRING;
  item->value.str = malloc(length + 1);
  item->meta = NULL;
  size_t str_len = 0;

  for (size_t i = 0; i < length; i++) {
    char c = start_ptr[i];

    if (c == '\\' && i + 1 < length) {
      char next = start_ptr[++i];

      switch (next) {
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
    } else {
      item->value.str[str_len++] = c;
    }
  }

  item->value.str[str_len] = '\0';
  return item;
}

MalType *mal_new_symbol(char *symbol) {
  MalType *item = malloc(sizeof(MalType));
  item->type = MAL_SYMBOL;
  item->value.symbol = strdup(symbol);
  item->meta = NULL;
  return item;
}

MalType *mal_new_keyword(char *start_ptr, size_t length) {
  MalType *item = malloc(sizeof(MalType));
  item->type = MAL_KEYWORD;
  item->value.keyword = malloc(length + 1);
  item->meta = NULL;
  size_t str_len = 0;

  for (size_t i = 0; i < length; i++) {
    char c = start_ptr[i];
    item->value.str[str_len++] = c;
  }

  item->value.str[str_len] = '\0';
  return item;
}

MalType *mal_new_num_int(char *start_ptr, size_t length) {
  char *num_str = strndup(start_ptr, length);
  char *end;
  errno = 0;

  int64_t val = (int64_t)strtoll(num_str, &end, 10);

  if (errno == ERANGE) {
    fprintf(stderr, "Error: Number is out of range.\n");
  } else {
    MalType *item = malloc(sizeof(MalType));
    item->type = MAL_INT;
    item->value.number_int = val;
    item->meta = NULL;
    free(num_str);
    return item;
  }
  free(num_str);
  return NULL;
}

MalType *mal_new_num_double(char *start_ptr, size_t length) {
  char *num_str = strndup(start_ptr, length);
  char *end;
  errno = 0;

  double val = (double)strtod(num_str, &end);

  if (errno == ERANGE) {
    fprintf(stderr, "Error: Number is out of range.\n");
  } else {
    MalType *item = malloc(sizeof(MalType));
    item->type = MAL_DOUBLE;
    item->value.number_float = val;
    item->meta = NULL;
    free(num_str);
    return item;
  }
  free(num_str);
  return NULL;
}

void free_ast(MalType *node) {
  if (!node) {
    return;
  }
  if(node->meta) free_ast(node->meta);
  if (node->type == MAL_LIST || node->type == MAL_VECTOR || node->type == MAL_HASHMAP) {
    for (int i = 0; i < node->value.list.count; i++) {
      free_ast(node->value.list.items[i]);
    }
    free(node->value.list.items);
  } else if (node->type == MAL_STRING) {
    free(node->value.str);
  } else if (node->type == MAL_SYMBOL) {
    free(node->value.symbol);
  } else if (node->type == MAL_KEYWORD) {
    free(node->value.keyword);
  } else if (node->type == MAL_NIL || node->type == MAL_TRUE ||
             node->type == MAL_FALSE) {
    return;
  }
  free(node);
}

void pr_str(MalType *node, bool print_readably) {
  if (node->meta != NULL) {
    printf("(with meta ");
    pr_value(node->meta, print_readably);
    printf(" ");
    pr_value(node, print_readably);
    printf(")");
  } else {
    pr_value(node, print_readably);
  }
}

static void pr_value(MalType *node, bool print_readably) {
  switch (node->type) {
  case MAL_NIL:
    printf("nil");
    break;
  case MAL_TRUE:
    printf("true");
    break;
  case MAL_FALSE:
    printf("false");
    break;
  case MAL_INT:
    printf("%" PRId64, node->value.number_int);
    break;
  case MAL_DOUBLE:
    printf("%f", node->value.number_float);
    break;

  case MAL_SYMBOL:
    printf("%s", node->value.symbol);
    break;
  case MAL_KEYWORD:
    printf("%s", node->value.keyword);
    break;
  case MAL_STRING:
    printf("\"");
    if (!print_readably) {
      printf("%s", node->value.str);
    } else {
      for (char *s = node->value.str; *s; s++) {
        if (*s == '\n')
          printf("\n");
        else if (*s == '\\')
          printf("\\\\");
        else if (*s == '"')
          printf("\\\"");
        else
          printf("%c", *s);
      }
    }
    printf("\"");
    break;

  case MAL_LIST:
    printf("(");
    for (int i = 0; i < node->value.list.count; i++) {
      if (i > 0)
        printf(" ");
      pr_str(node->value.list.items[i], print_readably);
    }
    printf(")");
    break;

  case MAL_VECTOR:
    printf("[");
    for (int i = 0; i < node->value.list.count; i++) {
      if (i > 0)
        printf(" ");
      pr_str(node->value.list.items[i], print_readably);
    }
    printf("]");
    break;
  case MAL_HASHMAP:
    printf("{");
    for (int i = 0; i < node->value.list.count; i++) {
      if (i > 0)
        printf(" ");
      pr_str(node->value.list.items[i], print_readably);
    }
    printf("}");
    break;
  }
}
