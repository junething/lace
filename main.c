#include "includes.h"
#include "macros.h"
#include "lexer.h"
#include "parse.h"
#include "utils.h"
#include "types.h"
#include "main.h"
#include "compile.h"
string opStr[] = {"+", "-", "*", "/", "x", "x", "x" };
int main(int argc, char **argv)
{
	ListNode* tokens = lex(argv[1]);
	ListNode* ListNode = tokens;
	fprintf(stderr, "Tokens:\n");
	while(ListNode != NULL) {
		fprintf(stderr, "%s  ", (char*)ListNode->value);
		ListNode = ListNode->next;
	}
	fprintf(stderr, "\n");
	TypedNode* code = parse_lace(tokens);
	fprintf(stderr, "\nAST Nodes:\n");
	print_node(code);
	fprintf(stderr, "\nC Code:\n");
	compile2file(code, stdout);
}
void print_node(TypedNode* node) {
	//fprintf(stderr, "(%d):", node->type);
	switch(node->type) {
		case BINOP:
		fprintf(stderr, "(");
		print_node(node->node.binOp.one);
		fprintf(stderr, " %s ", opStr[node->node.binOp.op]);
		print_node(node->node.binOp.two);
		fprintf(stderr, ")");
		break;
		case INT:
		fprintf(stderr, "%d", node->node.integer);
		break;
	}
}
/*void error(Error errorCode) {
    switch(errorCode) {
        case ERR_ARGS_NUM:
            ffprintf(stderr, stderr, "Usage: 2310depot name {goods qty}\n");
            break;
        case ERR_ARGS_NAME:
            ffprintf(stderr, stderr, "Invalid name(s)\n");
            break;
        case ERR_ARGS_QTY:
            ffprintf(stderr, stderr, "Invalid quantity\n");
            break;
        case ERR_ALL_G:
        case ERR_SERVER:
            break;
    }
    exit(errorCode);
}*/
void error(int code, string file, int line, string message) {
	fprintf(stderr, "\033[31;1;4m[%s:%d]\033[0m: ERROR: %s\n", file, line, message);
	exit(code);
}
