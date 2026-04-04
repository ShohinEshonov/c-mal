#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>







MalType *mal_new_list()
{
  MalType* node = malloc(sizeof(MalType));
  node->type = MAL_LIST;
  node->value.list.items = NULL;
  node->value.list.count = 0;
  node->value.list.capacity = 0;
  return node;
}


void mal_list_append(MalType *list, MalType *item) {
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


MalType *mal_new_string(char *start_ptr, size_t length)
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



void pr_str(MalType *node, bool print_readably) {
  switch (node->type) {
  case (MAL_NIL): {
    printf("nil");
    break;
  }
  case (MAL_TRUE): {
    printf("true");
    break;
  }
  case (MAL_FALSE): {
    printf("false");
    break;
  }
  case (MAL_STRING): {
    if (print_readably == false) {
      printf("%s", node->value.str);
      break;
    }
    char *s = node->value.str;
    while (*s) {
      if (*s == '\n') {
        printf("\\n");
      } else if (*s == '\\') {
        printf("\\\\");
      } else if (*s == '"') {
        printf("\\\"");
      } else {
        printf("%c", *s);
      }
      s++;
    }
    break;
  }
  case (MAL_SYMBOL): {
    printf("%s", node->value.symbol);
    break;
  }
  case (MAL_LIST): {
    printf("(");
    for (int i = 0; i < node->value.list.count; i++) {
      pr_str(node->value.list.items[i], print_readably);
      printf(" ");
    }
    printf(")");
    break;
  }
  }
}





