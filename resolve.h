#ifndef RESOLVE_HEADER
#define RESOLVE_HEADER
#include "types.h"
#include "compile.h"
CompileContext* resolve_code(TypedNode* node);
bool resolve_node(TypedNode* node, CompileContext* c);
#endif
