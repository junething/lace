#ifndef COMPILE_HEADER
#define COMPILE_HEADER
#include "types.h"
#include "dictionary.h"
#include "sb.h"
typedef struct {
    string integer;
    string string;
    string floating;
    string boolean;
    string character;
} TypeStrings;
typedef struct {
	FILE* dest;
	ListNode* definitions;
    //Dictionary types;
    Dict* types;
    Dict* functionDefs;
    Dict* typeDefs;
    Dict* symbols;
    int indentation;
    TypedNode* currentParent;
    // StringBuilder* functionDefs;
} CompileContext;
bool compile_node (TypedNode* code, CompileContext* context);
bool compile2file(TypedNode* code, CompileContext* c);

#endif
