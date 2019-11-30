#include "parse.h"
#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "main.h"
#include "types.h"
#include "utils.h"
// type.basedef enum { NodeBinOp, NodeCall, NodeAss } NodeType;

string peek;
ListNode* tokenNode;
int line = 1;
bool wasNewline;
TypedNode* expression(void);
TypedNode* function_or_method (void);
TypedNode* primary(void);
bool check(string tok) { return (strcmp(tok, peek) == 0); }
void next(void) {
    if (tokenNode->next != NULL) {
        tokenNode = tokenNode->next;
        //free(peek);
        peek = (string)tokenNode->value;
        if(check("\n")) {
            line++;
            next();
            wasNewline = true;
        } else {
            wasNewline = false;
            fprintf(stderr, "peek: %s\n", peek);
        }
    } else {
        tokenNode = NULL;
        peek = strdup("end");
    }
    
}
void consume(string tok) {
    if (strcmp(tok, peek) != 0) {
        hold();
        ERROR("Expected '%s'(%lu) got '%s'(%lu): %d", tok, strlen(tok), peek,
              strlen(peek), strcmp(tok, peek))
    }
    free(peek);
    next();
}

void discard(void) {
    free(peek);
    next();
}
bool consume_if(string tok) {
   // fprintf(stderr, "is '%s' == '%s'", peek, tok);
    if (check(tok)) {
        free(peek);
        next();
        return true;
    }
    return false;
}
string yank(void) {
    string p = peek;
    next();
    return p;
}
ListNode* code_list(void) {
    ListNode* code = new (ListNode);
    ListNode* current = code;
    while (tokenNode != NULL && strcmp("}", peek)) {
        current->next = new (ListNode);
        current = current->next;
        current->value = (void*)start();
        //LOG("new");
        //fprintf(stderr, ">");
        LOG("NODE:")
        print_node(current->value);
    }
    return code;
}
TypedNode* code(void) {
    TypedNode* code = new (TypedNode);
    code->nType = CODE;
    code->node.code.start = code_list();
    return code;
}
TypedNode* parse_lace(ListNode* tokens) {
    tokenNode = tokens;
    next();
    return code();
}

TypedNode* word(void) {
    TypedNode* node = new (TypedNode);
    if (check(",") || check("}")) ERROR("stray '%s'! on line %d", peek, line);
    if (consume_if("new")) {
        node->nType = NEW;
        
    } else {
        node->nType = WORD;
     //   LOG("Making a word:");
    }
    
    node->node.word = yank();
    //print_node(node);
    return node;
}
TypedNode* value(void) {
    int num;
    TypedNode* node = new (TypedNode);
    node->nType = PRIMATIVE;
    if (valint(peek, &num)) {
        node->pType = INT;
        node->node.primitive.integer = num;
    } else if (peek[0] == '\"') {
        string old = peek;
        peek = &old[1];
		peek[strlen(peek) - 1] = '\0';
        peek = strdup(peek);
        free(old);
        node->pType = STRING;
        node->node.primitive.string = peek;
    } else if (peek[0] == '\'') {
        node->pType = CHAR;

        if (peek[1] == '\\' && peek[3] == '\'') {
            switch (peek[2]) {
                case 'n':
                    node->node.primitive.character = '\n';
                    break;
                default:
                    ERROR("Invalid escaped special char");
            }
        } else if (peek[2] == '\'') {
            node->node.primitive.character = peek[1];
        } else {
            free(node);
            fprintf(stderr, "%s", peek);
            ERROR("invalid char");
            return NULL;
        }
    } else if (!strcmp(peek, "true") || !strcmp(peek, "false")) {
        node->pType = BOOL;
        node->node.primitive.boolean = (peek[0] == 't') ? true : false;
    } else {
        return word();
    }
    next();
    return node;
}

TypedNode* primary(void) {
    TypedNode* left = value();
    while (true) {
        //bool wasParen;
        if (consume_if("(")) {
            TypedNode* call = new (TypedNode);
            call->nType = CALL;
            call->node.call.method = left;

            TypedNode** args = (TypedNode**)calloc(1, sizeof(TypedNode*) * 5);
            int count = 5;
            int a = 0;
            while (!check(")")) {
                args[a] = expression();
                // print_node(args[a]);
                // ERROR("fs");
                // fprintf(stderr, "arg: %s : %s\n", args[a].name,
                // args[a].type.str);
                a++;
                if (a == count) {
                    count += 2;
                    args = realloc(args, sizeof(TypedNode*) * count);
                }
                if (!consume_if(",")) break;
            }
            call->node.call.arguments =
                (TypedNode**)realloc(args, sizeof(TypedNode*) * a);
            call->node.call.numArgs = a;
            call->statement = false;
            consume(")");
            left = call;
        } else if (consume_if(".")) {
            TypedNode* access = new (TypedNode);
            access->nType = DOT;
            access->node.access.left = left;
            access->node.access.word = yank();
            print_node(access);
            fprintf(stderr, "\n");
            left = access;
        } else if(consume_if("[")) {
            TypedNode* index = new (TypedNode); 
            index->nType = IND;
            index->node.index.left = left;
            index->node.index.index = expression();
            consume("]");
            left = index;
        } else {
            return left;
        }
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* multiplicative(void) {
    TypedNode* left = primary();
    while (true) {
        if (!strcmp(peek, "*") || !strcmp(peek, "*") || !strcmp(peek, "%")) {
            // string op = peek;
            next();
            TypedNode* newLeft = new (TypedNode);
            newLeft->nType = BINOP;
            newLeft->node.binOp.one = left;
            newLeft->node.binOp.op = (peek[0] == '*') ? MUL : ((peek[0] == '-') ? SUB : MOD);
            newLeft->node.binOp.two = primary();
            fprintf(stderr, "BinOp: ");
            print_node(newLeft);
            fprintf(stderr, "\n");
            left = newLeft;
        } else {
            return left;
        }
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* additive(void) {
    TypedNode* left = multiplicative();
    while (true) {
        if (!strcmp(peek, "+") || !strcmp(peek, "-")) {
            // string op = peek;
            TypedNode* newLeft = new (TypedNode);
            newLeft->nType = BINOP;
            newLeft->node.binOp.one = left;
            newLeft->node.binOp.op = (peek[0] == '+') ? ADD : SUB;
            next();
            newLeft->node.binOp.two = multiplicative();
            fprintf(stderr, "BinOp: ");
            print_node(newLeft);
            fprintf(stderr, "\n");
            left = newLeft;
        } else {
            return left;
        }
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* equality(void) {
    TypedNode* left = additive();
    while (true) {
        if (check("==") || check("!=")) {
            // string op = peek;
            
            TypedNode* newLeft = new (TypedNode);
            newLeft->nType = BINOP;
            newLeft->node.binOp.one = left;
            newLeft->node.binOp.op = (check("==")) ? EQU : NEQ;
            next();
            newLeft->node.binOp.two = additive();
            fprintf(stderr, "equality?: ");
            print_node(newLeft);
            fprintf(stderr, "\n");
            left = newLeft;
        } else {
            return left;
        }
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* assignment(void) {
    TypedNode* left = equality();
    while (true) {
        if (check("=")) {
            // string op = peek;
            next();
            TypedNode* newLeft = new (TypedNode);
            newLeft->nType = BINOP;
            newLeft->node.binOp.one = left;
            newLeft->node.binOp.op = ASS;
            newLeft->node.binOp.two = equality();
            fprintf(stderr, "assign: ");
            print_node(newLeft);
            fprintf(stderr, "\n");
            left = newLeft;
        } else {
            return left;
        }
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* expression(void) { return assignment(); }
TypedNode* statement(void) {
    // TypedNode* left = assignment();
   // LOG("Is %s a statement?", peek);
    if (consume_if("let")) {
        TypedNode* newLeft = new (TypedNode);
        newLeft->nType = LET;
        newLeft->node.declare.name = yank();
        if(consume_if(":")) {
           newLeft->node.declare.type.str = yank();
        }
        if(consume_if("=")) {
        newLeft->node.declare.value = expression();
        }
        newLeft->statement = true;
        return newLeft;
    } else if (check("fun") || check("met") || check("def")) {
        return function_or_method();
    } else if (check("if") || check("while")) {
        TypedNode* ifNode = new (TypedNode);
        ifNode->nType = (check("if")) ? IF : WHILE;
        next();
        ifNode->node.ifOrWhile.condition = expression();
        consume("{");
        ifNode->node.ifOrWhile.body = code();
        consume("}");
        return ifNode;
    } else if (consume_if("return")) {
        TypedNode* ret = new (TypedNode);
        ret->nType = RET;
        if(!wasNewline)
            ret->node.node = expression();
        return ret;
    } else if (check("struct") || check("union")) {
        bool isUnion = !strcmp(peek, "union");
        next();
        TypedNode* struc = new(TypedNode);
        struc->nType = (isUnion) ? UNION : STRUCT;
        struc->node.structure.type = struc->nType;
        struc->node.structure.name = yank();
        consume("{");
        struc->node.structure.fields = dict_new();
        int a = 0;
        while (!consume_if(")")) {
            if(check("met") || check("def")) {
                    TypedNode* method = function_or_method();
                    method->node.method.parent = struc;
                    dict_add(struc->node.structure.fields,
                             method->node.method.name, method);
            } else {
                TypedNode* dec = new (TypedNode);
                dec->nType = LET;
                dec->node.declare.name = yank();
                consume(":");
                dec->node.declare.type.str = yank();
                dec->node.declare.type.pointer = consume_if("*");
                dict_add(struc->node.structure.fields, dec->node.declare.name,
                         dec);
            }
            a++;
            if (!consume_if(",")) break;
            if (check("}")) break;
            
        }
        consume_if(",");
        consume("}");
        print_node(struc);
        return struc;
    } else {
        TypedNode* node = expression();
        node->statement = true;
        return node;
    }
    ERROR("Should not be here");
    return NULL;
}
TypedNode* start(void) {
    TypedNode* state = statement();
    state->statement = true;
    return state;
}

TypedNode* function_or_method (void) {
    bool def = check("def");
    string open = (def) ? "[" : "(";
    string close = (def) ? "]" : ")";
    TypedNode* funNode = new (TypedNode);
    funNode->nType = MET_FUN;
    funNode->node.method.fun = (peek[0] == 'f') ? true : false;
    next();
   
    if(def) {
            funNode->node.method.name = "index_get";
        consume("self");
    } else {
        funNode->node.method.name = yank();
    }
    consume(open);
    Argument* args = (Argument*)calloc(1, sizeof(Argument) * 5);
    int count = 5;
    int a = 0;
    if(def) {
        args[0].name = strdup("self");
        a++;
    }
    while (!consume_if(close)) {
        if (check("self")) {
            args[a].name = yank();
            if(consume_if(":")) {
                args[a].type.str = yank();
            }
        } else {
            args[a].name = yank();
            consume(":");
            args[a].type.str = yank();
            //a++;
        }

        args[a].type.pointer = consume_if("*");
        fprintf(stderr, "arg: %s : %s\n", args[a].name, args[a].type.str);
        a++;
        if (a == count) args = realloc(args, sizeof(Argument*) * (count += 5));
        if (!consume_if(",")) break;
    }
    
    consume_if(close);
    if(def &&  consume_if("=")) {
        args[a].name = yank();
        consume(":");
        args[a].type.str = yank();
        args[a].type.pointer = consume_if("*");
        funNode->node.method.name = "index_set";
        LOG("%d  self[%s : %s] = %s : %s", a, args[1].name, args[1].type.str,
            args[2].name, args[2].type.str);
        a++;
    }
    funNode->node.method.arguments =
        (Argument*)realloc(args, sizeof(Argument) * a);
    funNode->node.method.numArgs = a;
    if(consume_if("->"))
        funNode->node.method.type.str = yank();
    else
        funNode->node.method.type.str = strdup("void");
    funNode->node.method.type.pointer = consume_if("*");
    
    consume("{");
    funNode->node.method.body = code();
    consume("}");
    fprintf(stderr, "\n\n");
    return funNode;
}
