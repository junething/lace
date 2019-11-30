#include "includes.h"
#include "types.h"
#ifndef MAIN_HEADER
#define MAIN_HEADER
int logVerbosity;
void print_node(const TypedNode* node);
void error(int code);
string opStr[9];

#endif
