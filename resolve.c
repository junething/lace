#include "resolve.h"
#include "compile.h"
#include "dictionary.h"
#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "main.h"
#include "parse.h"
#include "sb.h"
#include "types.h"
#include "utils.h"
CompileContext* resolve_code(TypedNode* code) {
    CompileContext* c = new (CompileContext);
    c->types = dict_new();
    c->symbols = dict_new();
    c->functionDefs = dict_new();
    c->typeDefs = dict_new();
    string primitives[] = { "int", "float", "char", "string", "bool", "void", "FILE", "size_t", "ssize_t", (string)0 };
    //string primitivesC[] = { "int", "float", "char", "string", "bool", "void", (string)0 };
    for(int p = 0; primitives[p] != NULL; p++) {
        SymType* primitive = new (SymType);
        primitive->str = strdup(primitives[p]);
        dict_add(c->types, primitives[p], primitive);
    }
    string functions[] = {"fopen", "fclose", "fputs", "getline", "puts", "printf", (string)0};
    string functionTypes[] = {"FILE", "int", "int", "ssize_t", "int", "int"};
    for (int f = 0; functions[f] != NULL; f++) {
        SymType* function = new (SymType);
        function->type.ret = (SymType*)dict_find(c->types, functionTypes[f], NULL);
        dict_add(c->symbols, functions[f], (void*)function);
    }
    

    Symbol* methodSymbol = new (Symbol);
    methodSymbol->type = *(SymType*)dict_find(c->types, "int", NULL);
    methodSymbol->type.type.arguments = NULL;
    methodSymbol->type.type.numArgs = -1;
    dict_add(c->symbols, "printf", (void*)methodSymbol);

    resolve_node(code, c);
    return c;
}
bool first_resolve(CodeNode* code, CompileContext* c) {
    ListNode* current = code->start;
    while ((current = current->next) != NULL) {
        TypedNode* node = (TypedNode*)current->value;
        if (node->nType == MET_FUN) {
            Symbol* methodSymbol = new (Symbol);
            methodSymbol->type = node->node.method.type;
            methodSymbol->type.type.arguments = node->node.method.arguments;
            methodSymbol->type.type.numArgs = node->node.method.numArgs;
            dict_add(c->symbols, node->node.method.name, (void*)methodSymbol);
        }
    }
    return true;
}
bool resolve_node(TypedNode* node, CompileContext* c) {
    if (node == NULL) return false;
    //   LOG("Resolving: {");
    // print_node(node);
    // LOG("}")
    switch (node->nType) {
        case BINOP:
            resolve_node(node->node.binOp.one, c);
            resolve_node(node->node.binOp.two, c);
            node->rType = node->node.binOp.one->rType;
            if (node->node.binOp.op == ASS &&
                node->node.binOp.one->nType == IND)
                node->node.binOp.one->node.index.set = true;
            break;
        case PRIMATIVE:
            switch (node->pType) {
                case INT:
                    node->rType =
                        *((SymType*)dict_find_n(c->types, "int", NULL));
                    break;
                case FLOAT:
                    node->rType =
                        *((SymType*)dict_find_n(c->types, "float", NULL));
                    break;
                case CHAR:
                    node->rType =
                        *((SymType*)dict_find_n(c->types, "char", NULL));
                    break;
                case STRING:
                    node->rType =
                        *((SymType*)dict_find_n(c->types, "string", NULL));
                    break;
                case BOOL:
                    node->rType =
                        *((SymType*)dict_find_n(c->types, "bool", NULL));
                    break;
            }
            break;
        case WORD:;
            if (!strcmp(node->node.word, "null")) {
                node->rType = *((SymType*)dict_find_n(c->types, "void", NULL));
            } else {
                bool found = false;
                Symbol* word =
                    (Symbol*)dict_find_n(c->symbols, node->node.word, &found);
                if(found) {
                    LOG("Got word %s and its type is %s (%d)", node->node.word,
                        word->type.str, word->type.structy);
                        node->rType = word->type;
                } else {
                     node->rType = *(SymType*)dict_find_n(c->types, node->node.word, NULL); 
                }
                // if (!found) ERROR("'%s' not found", node->node.word);
                
            }
            break;
        case UNARY:
            resolve_node(node->node.unary.operand, c);
            node->rType = node->node.unary.operand->rType;
            break;
        case IND:
            resolve_node(node->node.index.left, c);
            resolve_node(node->node.index.index, c);
            if (node->node.index.left->rType.structy) {
                StructNode* structNode = (StructNode*)dict_find(
                    c->typeDefs, node->node.index.left->rType.str, NULL);
                TypedNode* field = (TypedNode*)dict_find(structNode->fields,
                                                         "index_get", NULL);
                node->rType = field->rType;
            } else {
                node->rType = node->node.index.left->rType;
            }
            break;
        case LET:
            resolve_node(node->node.declare.value, c);
            if (node->node.declare.value != NULL)
                node->rType = node->node.declare.value->rType;
            else {
                node->rType = node->rType =
                    *((SymType*)dict_find_n(c->types, node->node.declare.type.str, NULL));
            }
            Symbol* sym = new (Symbol);
            sym->type = node->rType;
            dict_add(c->symbols, node->node.declare.name, (void*)sym);
            LOG("Declaring that %s is a %s (%d)", node->node.declare.name,
                sym->type.str, sym->type.structy)
            break;
        case IF:
        case WHILE:
            resolve_node(node->node.ifOrWhile.condition, c);
            resolve_node(node->node.ifOrWhile.body, c);
            break;
        case RET:
            resolve_node(node->node.node, c);
            break;
        case DOT:;
            TypedNode* left = node->node.access.left;
            resolve_node(left, c);
            StructNode* structNode = (StructNode*)dict_find(
                c->typeDefs, node->node.access.left->rType.str, NULL);
            TypedNode* field = (TypedNode*)dict_find(
                structNode->fields, node->node.access.word, NULL);
            node->rType = field->rType;
            LOG("something.%s is a %s (%d)", node->node.access.word,
                node->rType.str, node->rType.structy);
            break;
        case MET_FUN: {
            node->rType = *((SymType*)dict_find_n(
                c->types, node->node.method.type.str, NULL));
            dict_add(c->functionDefs, node->node.method.name, &node->node);
            dict_new_division(c->symbols);
            for (int a = 0; a < node->node.method.numArgs; a++) {
                if (c->currentParent != NULL &&
                    !strcmp(node->node.method.arguments[a].name, "self")) {
                    node->node.method.arguments[a].type =
                        c->currentParent->rType;
                    if(c->currentParent->rType.structy == 1)
                        node->node.method.arguments[a].type.pointer = true;
                    LOG("set a self type as %s*!", c->currentParent->rType.str);
                }
                Symbol* sym = new (Symbol);
                sym->type = node->node.method.arguments[a].type;
                dict_add(c->symbols, node->node.method.arguments[a].name,
                         (void*)sym);
            }
            resolve_node(node->node.method.body, c);
            dict_remove_top_segment(c->symbols);
        } break;
        case CODE: {
            ListNode* current = node->node.code.start;
            first_resolve(&node->node.code, c);
            while ((current = current->next) != NULL) {
                resolve_node((TypedNode*)current->value, c);
            }
            break;
        } break;
        case NEW:
            LOG("ugg: %s", node->node.new.type.str);
            node->rType =
                *(SymType*)dict_find(c->types, node->node.new.type.str, NULL);

            if (node->rType.structy != 2) node->rType.pointer = true;
            break;
        case CALL:
            LOG("call!");
            resolve_node(node->node.call.method, c);
            // if (!found) ERROR("'%s' not found", node->node.word);
            node->rType = node->node.call.method->rType;
            LOG("type of method %s is %s", node->node.method.name,
                node->node.call.method->rType.str);
            for (int a = 0; a < node->node.call.numArgs; a++) {
                resolve_node(node->node.call.arguments[a], c);
                // if (strcmp(node->node.call.arguments[a], c->)) {
            }
            break;
        case CLASS:
        case UNION:
        case STRUCT:;
            SymType* type = new (SymType);
            //    char def[8 + strlen(node->node.structure.name)];
            //  strcat(def, (node->nType == STRUCT) ? "struct " : "union
            //  ");
            // strcat(def, node->node.structure.name);
            type->str = node->node.structure.name;
            type->structy = 1;
            if (node->nType == CLASS) type->structy = 2;
            type->typeNode = node;
            dict_add(c->types, node->node.structure.name, type);
            dict_add(c->typeDefs, node->node.structure.name,
                     &node->node.structure);

            node->rType = *type;
            //   StructNode* structure = &node->node.structure;
            dict_new_division(c->symbols);
            c->currentParent = node;
            for (int a = 0; a < node->node.structure.fields->len; a++) {
                TypedNode* subNode =
                    (TypedNode*)node->node.structure.fields->array[a].value;
                /*  Symbol* sym = new (Symbol);
                  sym->type = subNode->rType;
                  dict_add(c->symbols,
                  node->node.method.arguments[a].name, (void*)sym);*/
                resolve_node(subNode, c);
            }
            c->currentParent = NULL;
            dict_remove_top_segment(c->symbols);
            break;
        default:
            fflush(c->dest);
            ERROR("%d not implemented (or forgot break)", node->nType);
            break;
    }
    fflush(c->dest);
    return true;
}
