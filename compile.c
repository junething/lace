#include "compile.h"
#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "main.h"
#include "parse.h"
#include "types.h"
#include "utils.h"
bool compile_type(SymType* type, CompileContext* c) {
    fprintf(c->dest, "%s", type->str);
    if (type->pointer) fputc('*', c->dest);
    return true;
}
bool compile2file(TypedNode* code, CompileContext* c) {
    // Import lacelib.c
    if (c->dest != stdout) {
        char content[80];
        FILE* laceLibC = fopen("lacelib.c", "r");
        if (laceLibC == NULL) {
            perror("fopen");
            ERROR("'lacelib.c' not found");
        }
        while (fgets(content, sizeof(content), laceLibC) != NULL) {
            fprintf(c->dest, "%s", content);
        }
        fclose(laceLibC);
    }
    // Struct and union declararions and typedefs
    for (int a = 0; a < c->typeDefs->len; a++) {
        StructNode* structure = ((StructNode*)c->typeDefs->array[a].value);
        fprintf(c->dest, "typedef %s %s %s;\n",
                (structure->type == STRUCT) ? "struct" : "union",
                structure->name, structure->name);
    }
    // function and method declararions;
    for (int a = 0; a < c->functionDefs->len; a++) {
        MethodNode* method = ((MethodNode*)c->functionDefs->array[a].value);
        compile_type(&method->type, c);
        fprintf(c->dest, " ");
        if(method->parent != NULL) {
            fprintf(c->dest, "%s__", method->parent->node.structure.name);
        }
        fprintf(c->dest, "%s (", method->name);
        for (int a = 0; a < method->numArgs; a++) {
            compile_type(&method->arguments[a].type, c);
            fprintf(c->dest, " %s", method->arguments[a].name);
            if (a + 1 != method->numArgs) fprintf(c->dest, ", ");
        }
        fprintf(c->dest, ");\n");
    }

    // struct and union definitions
    for (int a = 0; a < c->typeDefs->len; a++) {
        StructNode* structure = ((StructNode*)c->typeDefs->array[a].value);
        fprintf(c->dest, "%s %s {\n",
                (structure->type == STRUCT) ? "struct" : "union",
                structure->name);
        for (int a = 0; a < structure->fields->len; a++) {
            TypedNode* subNode = (TypedNode*)structure->fields->array[a].value;
            if (subNode->nType == LET) {
                fprintf(c->dest, "\t");
                compile_type(&subNode->node.declare.type, c);
                fprintf(c->dest, " %s;\n", subNode->node.declare.name);
            }
        }
        fprintf(c->dest, "};\n");
    }
    // MEthod and function definitions
    for (int a = 0; a < c->functionDefs->len; a++) {
        MethodNode* method = ((MethodNode*)c->functionDefs->array[a].value);
        compile_type(&method->type, c);
        fprintf(c->dest, " ");
        if (method->parent != NULL) {
            fprintf(c->dest, "%s__", method->parent->node.structure.name);
        }
        fprintf(c->dest, "%s (", method->name);
        for (int a = 0; a < method->numArgs; a++) {
            compile_type(&method->arguments[a].type, c);
            fprintf(c->dest, " %s", method->arguments[a].name);
            if (a + 1 != method->numArgs) fprintf(c->dest, ", ");
        }
        fprintf(c->dest, ") {\n");
        compile_node(method->body, c);
        fprintf(c->dest, "}\n");
    }
    c->indentation = -1;
    compile_node(code, c);
    return true;
}

bool compile_node(TypedNode* node, CompileContext* c) {
    if (node == NULL) return false;
    switch (node->nType) {
        case BINOP:
            // LOG("Type of one is %s", node->node.binOp.one->rType.str);
            if (node->node.binOp.op == ADD &&
                node->node.binOp.one->rType.str != NULL &&
                strcmp(node->node.binOp.one->rType.str, "string") == 0) {
                fprintf(c->dest, "str_add(");
                compile_node(node->node.binOp.one, c);
                fprintf(c->dest, ", ");
                compile_node(node->node.binOp.two, c);
                fprintf(c->dest, ")");
            } else if (node->node.binOp.op == ASS &&
                       node->node.binOp.one->nType == IND) {
                compile_node(node->node.binOp.one, c);
                // fprintf(c->dest, "(%d)\n", node->node.binOp.op);
                compile_node(node->node.binOp.two, c);
                fprintf(c->dest, ")");
            } else {
                compile_node(node->node.binOp.one, c);
                // fprintf(c->dest, "(%d)\n", node->node.binOp.op);
                fprintf(c->dest, " %s ", opStr[node->node.binOp.op]);
                compile_node(node->node.binOp.two, c);
                    
            }
            if (node->statement) fprintf(c->dest, ";\n");
            break;
        case PRIMATIVE:
            switch (node->pType) {
                case INT:
                    fprintf(c->dest, "%d", node->node.primitive.integer);
                    break;
                case FLOAT:
                    fprintf(c->dest, "%s", node->node.primitive.string);
                    break;
                case CHAR:
                    fprintf(c->dest, "%c", node->node.primitive.character);
                    break;
                case STRING:
                    fprintf(c->dest, "\"%s\"", node->node.primitive.string);
                    break;
                case BOOL:
                    fprintf(c->dest, "%s",
                            (node->node.primitive.boolean) ? "true" : "false");
                    break;
                default:
                    ERROR("Not impemented %d", node->pType);
                    break;
            }
            break;
        case WORD:
            if(strcmp(node->node.word, "null") == 0)
                fprintf(c->dest, "NULL");
            else
                fprintf(c->dest, "%s", node->node.word);
                break;
            case IND:
            if(node->node.index.left->rType.structy) {
                fprintf(c->dest, "%s__index_%s(",
                        node->node.index.left->rType.str,
                        (node->node.index.set) ? "set" : "get");
                compile_node(node->node.index.left, c);
                fprintf(c->dest, ", ");
                compile_node(node->node.index.index, c);
                fprintf(c->dest, "%s", (node->node.index.set) ? ", " : ")");
            } else {
                compile_node(node->node.index.left, c);
                fprintf(c->dest, "[");
                compile_node(node->node.index.index, c);
                fprintf(c->dest, "]");
            }
            break;
            case KEYWORD:
                fprintf(c->dest, "%s", node->node.word);
                break;
                case NEW:;
                bool hasInit = false;
                dict_find(node->rType.typeNode->node.structure.fields, "init", &hasInit);
                if(hasInit) {
                    fprintf(c->dest, "%s__init(new (%s))", node->rType.str, node->rType.str);
                } else {
                   fprintf(c->dest, "new (%s)", node->rType.str); 
                }
                    break;
                case DOT:
                    // Type* type = (Type*)dict_find(c->)
                   
                        //      LOG("uhhhh %s",
                        //      node->node.access.left->rType.str);
                        compile_node(node->node.access.left, c);
                        fprintf(c->dest, "%s",
                                (node->node.access.left->rType.pointer) ? "->"
                                                                        : ".");
                        fprintf(c->dest, "%s", node->node.access.word);

                        // if(node->node.access->rType)
                        //                fprintf(stderr, "(");

                        //                fprintf(stderr, ")");
                        break;
                case LET:
                    compile_type(((node->node.declare.type.str == NULL)
                                        ? &node->rType
                                        : &node->node.declare.type),
                                    c);
                    fprintf(c->dest, " %s", node->node.declare.name);
                    if (node->node.declare.value != NULL) {
                        fprintf(c->dest, " = ");
                        compile_node(node->node.declare.value, c);
                    }
                    fprintf(c->dest, ";\n");
                    break;
                case IF:
                case WHILE:
                    fprintf(c->dest, "%s (", (node->nType == IF) ? "if" : "while");
                    compile_node(node->node.ifOrWhile.condition, c);
                    fprintf(c->dest, ") {\n");
                    compile_node(node->node.ifOrWhile.body, c);
                    for (int i = 0; i < c->indentation; i++)
                        fputc('\t', c->dest);
                    fprintf(c->dest, "}\n");
                    break;
                case RET:
                    fprintf(c->dest, "return ");
                    compile_node(node->node.node, c);
                    fprintf(c->dest, ";\n");
                    break;
                case MET_FUN:

                    break;
                case CODE: {
                    c->indentation++;
                    ListNode* current = node->node.code.start;
                    while ((current = current->next) != NULL) {
                        for (int i = 0; i < c->indentation; i++)
                            fputc('\t', c->dest);
                        compile_node((TypedNode*)current->value, c);
                        fflush(c->dest);
                    }
                    c->indentation--;
                    break;
            } break;
            case CALL:;
                CallNode* call = &node->node.call;
                if (call->method->nType == DOT) {
                    AccessNode* dot = &call->method->node.access;
                    if (dot->left->rType.structy) {
                         TypedNode* met  = (TypedNode*)dict_find(
                             dot->left->rType.typeNode->node.structure.fields,
                             dot->word, NULL);
                         if (met->nType == MET_FUN) {
                             fprintf(
                                 c->dest, "%s__%s (",
                                 met->node.method.parent->node.structure.name,
                                 call->method->node.access.word);
                             compile_node(dot->left, c);
                             fprintf(c->dest, ", ");
                         } else {
                             ERROR("afdasasdf");
                         }
                    } else {
                        ERROR("cannot call on");
                    }
                } else {
                    compile_node(node->node.call.method, c);
                    fprintf(c->dest, "(");
                }
                
                for (int a = 0; a < node->node.call.numArgs; a++) {
                    compile_node(node->node.call.arguments[a], c);
                    if (a + 1 != node->node.call.numArgs)
                        fprintf(c->dest, ", ");
                }
                fprintf(c->dest, ")");
                if (node->statement) fprintf(c->dest, ";\n");
                break;
            case UNION:
            case STRUCT:
                
                break;
            default:
                fflush(c->dest);
                ERROR("%d not implemented (or forgot break)", node->nType);
                break;
        }
    fflush(c->dest);
    return true;
}
