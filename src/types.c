#include "types.h"
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
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_STRING;
  node->value.str = malloc(length + 1);
  node->meta = NULL;
  size_t str_len = 0;

  for (size_t i = 0; i < length; i++) {
    char c = start_ptr[i];

    if (c == '\\' && i + 1 < length) {
      char next = start_ptr[++i];

      switch (next) {
      case 'n':
        node->value.str[str_len++] = '\n';
        break;
      case '"':
        node->value.str[str_len++] = '"';
        break;
      case '\\':
        node->value.str[str_len++] = '\\';
        break;
      default:
        node->value.str[str_len++] = next;
        break;
      }
    } else {
      node->value.str[str_len++] = c;
    }
  }

  node->value.str[str_len] = '\0';
  return node;
}

MalType *mal_new_symbol(char *symbol) {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_SYMBOL;
  node->value.symbol = strdup(symbol);
  node->meta = NULL;
  return node;
}

MalType *mal_new_keyword(char *start_ptr, size_t length) {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_KEYWORD;
  node->value.keyword = malloc(length + 1);
  node->meta = NULL;
  size_t str_len = 0;

  for (size_t i = 0; i < length; i++) {
    char c = start_ptr[i];
    node->value.str[str_len++] = c;
  }

  node->value.str[str_len] = '\0';
  return node;
}

MalType *mal_new_num_int(int64_t value) {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_INT;
  node->value.number_int = value;
  node->meta = NULL;
  return node;
}

MalType *mal_new_num_double(double value) {
  MalType *node = malloc(sizeof(MalType));
  node->type = MAL_DOUBLE;
  node->value.number_double = value;
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

double to_double(MalType *val) {
  if (val->type == MAL_INT)
    return (double)val->value.number_int;
  return val->value.number_double;
}

int to_int(MalType *val) {
  if (val->type == MAL_DOUBLE)
    return (int)val->value.number_double;
  return val->value.number_int;
}

void free_ast(MalType *node) {
  if (!node) {
    return;
  }
  if (node->meta)
    free_ast(node->meta);
  if (node->type == MAL_LIST || node->type == MAL_VECTOR ||
      node->type == MAL_HASHMAP) {
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
             node->type == MAL_FALSE || node->type == MAL_FUNC) {
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
    printf("%f", node->value.number_double);
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

  case MAL_FUNC:
    printf("<function>");
    break;
  }
}
