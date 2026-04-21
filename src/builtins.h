#ifndef BUILTINS_H
#define BUILTINS_H

#include "types.h"

MalType *sum(MalType *args);
MalType *sub(MalType *args);
MalType *mult(MalType *args);
MalType *division(MalType *args);

#endif
