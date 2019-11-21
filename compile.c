#include "includes.h"
#include "macros.h"
#include "lexer.h"
#include "parse.h"
#include "utils.h"
#include "types.h"
#include "main.h"
#include "compile.h"

bool compile2file (TypedNode* code, FILE* dest) {
	CompileContext c;
	fprintf(dest, "#include <stdio.h>\n");
	fprintf(dest, "int main(int argc, char **argv) {\n");
	fputs("printf(\"%d\", ", dest);
	compile_node(code, &c);
	fprintf(dest, ");\n");
	fprintf(dest, "return 0;");
	fprintf(dest, "}\n");
	return true;
}
bool compile_node (TypedNode* node, CompileContext* c) {
	switch(node->type) {
		case BINOP:
		compile_node(node->node.binOp.one, c);
		fprintf(c->dest, "(%d)\n", node->node.binOp.op);
		fprintf(c->dest, " %s ", opStr[node->node.binOp.op]);
		compile_node(node->node.binOp.two, c);
		break;
		case INT:
		printf("%d", node->node.integer);
		break;
	}
	return true;
}
