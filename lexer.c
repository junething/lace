#include "includes.h"
#include "types.h"
#include "macros.h"
#include "lexer.h"
#include "parse.h"
#include "utils.h"
ListNode* lex(char* code) {
	fprintf(stderr, "Code: '%s'\n", code);
	int ch = 0;
	ListNode *list = new(ListNode);
	ListNode *currentListNode = list;
	char buf[100] = "";
	int i = 0;
	do {
		//fprintf(stderr, "%c-", code[ch]);
		if(code[ch] == ' ' || code[ch] == '\n' || code[ch] == '\0') {
			fprintf(stderr, "Token: '%s'", buf);
			currentListNode->value = (void *)strdup(buf);
			currentListNode->next = new(ListNode);
			currentListNode = currentListNode->next;
			memset(buf, 0, sizeof(buf));
			i = 0;
		} else {
			buf[i++] = code[ch];
		}
		//ch++;
	} while (code[ch++] != '\0');
	fprintf(stderr, "\n");
	return list;
}
