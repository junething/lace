#include "includes.h"
#include "types.h"
#ifndef MAIN_HEADER
#define MAIN_HEADER
int logVerbosity;
void print_node(const TypedNode* node);
void print_type(const SymType type);
void error(int code);
string opStr[12];
string opName[12];

#endif
