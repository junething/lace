#include "lexer.h"
#include "includes.h"
#include "macros.h"
#include "parse.h"
#include "types.h"
#include "utils.h"
bool one_of(char ch, string list) {
    for (int i = 0; i < strlen(list); i++) {
        if (ch == list[i]) return true;
    }
    return false;
}
void add_token(ListNode** currentListNode, string str) {
    fprintf(stderr, "%s  ", str);
    if (strlen(str) == 0) hold();
    (*currentListNode)->next = new (ListNode);
    *currentListNode = (*currentListNode)->next;
    (*currentListNode)->value = (void*)strdup(str);
}
ListNode* lex(FILE* input) {
    ListNode* list = new (ListNode);
    ListNode* currentListNode = list;
    char buf[100] = "";
    int i = 0;
    char ch;
    bool inComment = false;
    bool inString = false;
    while ((ch = getc(input)) != EOF) {
        if (inComment) {
            if (ch == '\n') inComment = false;
            continue;
        }
        if (inString) {
            if (ch == '"') inString = false;
            buf[i++] = ch;
            continue;
        }
        if (ch == '"') {
            inString = true;
            buf[i++] = ch;
            continue;
        }
        if (one_of(ch, "(){}[],:.*\n")) {
            if (strlen(buf) > 0) add_token(&currentListNode, buf);
            memset(buf, 0, sizeof(buf));
            buf[0] = ch;
            if (strlen(buf) > 0) add_token(&currentListNode, buf);
            memset(buf, 0, sizeof(buf));
            i = 0;
            //} else if(one_of(ch, "+-*/%~")) {

        } else if (!inComment &&
                   (ch == ' ' || ch == '\0' || ch == '\t')) {
            if (strlen(buf) > 0) add_token(&currentListNode, buf);
            memset(buf, 0, sizeof(buf));
            i = 0;
        } else {
            if (ch == '#')
                inComment = true;
            else if (ch == '\n')
                inComment = false;
            else if (!inComment)
                buf[i++] = ch;
        }
    };
    fprintf(stderr, "\n");
    return list;
}
