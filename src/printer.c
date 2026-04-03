#include "reader.h"
#include <stdbool.h>
#include <stdio.h>

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
