#include <stdio.h>
#include "reader.h"


void pr_str(MalType *node)
{
  switch(node->type)
  {
    case(MAL_STRING):
    {
      printf("%s", node->value.str);
      break; 
    }
    case(MAL_SYMBOL):
    {
      printf("%s", node->value.symbol);
      break; 
    }
    case(MAL_LIST):
    {
      printf("(");
      for(int i = 0; i < node->value.list.count; i++)
      {
        pr_str(node->value.list.items[i]);
        printf(" ");        
      }
      printf(")");
      break; 
    }
  }
}




