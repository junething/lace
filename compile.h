#ifndef COMPILE_HEADER
#define COMPILE_HEADER
#include "types.h"
typedef struct {
	FILE* dest;
	ListNode* definitions;

} CompileContext;
bool compile_node (TypedNode* code, CompileContext* context);
bool compile2file (TypedNode* code, FILE* dest);
#endif
