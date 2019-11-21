#include "includes.h"
#include "parse.h"
#include "types.h"
#include "lexer.h"
#include "utils.h"
#include "macros.h"
#include "main.h"
//typedef enum { NodeBinOp, NodeCall, NodeAss } NodeType;


string peek;
ListNode* tokenNode;
void next(void) {
	if(tokenNode->next != NULL) {
		tokenNode = tokenNode->next;
		peek = (string)tokenNode->value;
	} else {
		tokenNode = NULL;
		peek = NULL;
	}
	fprintf(stderr, "peek: %s\n", peek);
}
TypedNode* parse_lace (ListNode* tokens) {
	tokenNode = tokens;
	peek = (string)tokenNode->value;
	fprintf(stderr, "first peek: %s\n", peek);
	return start();
}

TypedNode* word(void) {
	TypedNode* node = new(TypedNode);
	node->type = WORD;
	node->node.word = strdup(peek);
	return node;
}
TypedNode* value(void) {
	int num;
	TypedNode* node = new(TypedNode);
	if(valint(peek, &num)) {
		node->type = INT;
		node->node.integer = num;
	} else if (peek[0] == '\"') {

	} else if (peek[0] == '\'') {
		node->type = CHAR;
		
		if(peek[1] == '\\' && peek[3] == '\'') {
			switch(peek[2]) {
				case 'n':
				node->node.character = '\n';
				break;
				default:
				ERROR("Invalid escaped char");
			}
		} else if(peek[2] == '\'') {
			node->node.character = peek[1];
		} else {
			free(node);
			fprintf(stderr, "%s", peek);
			ERROR("invalid char");
			return NULL;
		}
	} else if (!strcmp(peek, "true") || !strcmp(peek, "false")) {
		node->type = BOOL;
		node->node.boolean = (peek[0] == 't') ? true : false;
	} else {
		free(node);
		fprintf(stderr, "%s", peek);
		ERROR("Not value");
		return NULL;
	}
	next();
	return node;
	
}
TypedNode* multiplicative(void) {
	TypedNode* left = value();
	while (true) {
		if (!strcmp(peek, "*")) {
			//string op = peek;
			next ();
			TypedNode* newLeft = new(TypedNode);
			newLeft->type = BINOP;
			newLeft->node.binOp.one = left;
			newLeft->node.binOp.op = MUL;
			newLeft->node.binOp.two = value();
			left = newLeft;
		} else {
			break;
		}
	}
	return left;
}
TypedNode* additive(void) {
	TypedNode* left = multiplicative();
	while (true) {
		if (!strcmp(peek, "+")) {
			//string op = peek;
			next ();
			TypedNode* newLeft = new(TypedNode);
			newLeft->type = BINOP;
			newLeft->node.binOp.one = left;
			newLeft->node.binOp.op = ADD;
			newLeft->node.binOp.two = multiplicative();
			fprintf(stderr, "BinOp: ");
			print_node(newLeft);
			fprintf(stderr, "\n");
			left = newLeft;
		} else {
			break;
		}
	}
	return left;
}
TypedNode* assignment(void) {
	TypedNode* left = additive();
	while (true) {
		if (!strcmp(peek, "=")) {
			//string op = peek;
			next ();
			TypedNode* newLeft = new(TypedNode);
			newLeft->type = BINOP;
			newLeft->node.binOp.one = left;
			newLeft->node.binOp.op = ASS;
			newLeft->node.binOp.two = additive();
			fprintf(stderr, "BinOp: ");
			print_node(newLeft);
			fprintf(stderr, "\n");
			left = newLeft;
		} else {
			break;
		}
	}
	return left;
}
TypedNode* start(void) {
	return additive ();
}
