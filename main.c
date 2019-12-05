#include "main.h"
#include "compile.h"
#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "parse.h"
#include "types.h"
#include "utils.h"
#include "resolve.h"
string opStr[] = {"+", "-", "*", "/", "%", "=", "==", "!=", "*", "&", "x", "x"};
string opName[] = {"add", "sub", "mul", "mod", "ass", "equ", "neq", "ast", "amp", "-x-", "-x-"};
int logVerbosity = 1;
TypeStrings types;
int indentation = 0;
int main(int argc, char **argv) {
    if (argc < 2) ERROR("not enough args");
    bool run = argc > 2 && !strcmp(argv[1], "run");
    bool build = argc > 2 && !strcmp(argv[1], "build");
    
    FILE *source = fopen(argv[(run || build) ? 2 : 1], "r");
    ListNode *tokens = lex(source);
    fclose(source);
    ListNode *listNode = tokens;
    fprintf(stderr, "Tokens:\n");
    while (listNode != NULL) {
        fprintf(stderr, "%s  ", (char *)listNode->value);
        listNode = listNode->next;
    }
    fprintf(stderr, "\n");
    TypedNode *code = parse_lace(tokens);
    fprintf(stderr, "\nAST Nodes:\n");
    print_node(code);
    CompileContext* context = resolve_code(code);
    context->dest = stdout;
    //int devNull = open("/dev/null", O_WRONLY);
    int toGCC[2], fromGCC[2];
    if (argc > 3 && !strcmp(argv[2], "-o")) {
        context->dest = fopen(argv[3], "w");
    } else if (build || run) {
        if (pipe(toGCC) || pipe(fromGCC)) {
            ERROR("Making pipes failed");
        }
        pid_t pid = fork();
        if (pid == -1) {
            ERROR("gsfd");
        } else if (pid == 0) {  // player process
            close(fromGCC[0]);
            close(toGCC[1]);
            dup2(toGCC[0], 0);
            //dup2(fromGCC[1], 1);
            //dup2(fromGCC[1], 2);
            char *args[] = { "gcc", "-xc", "-g", "-", "-o", "lace.out", (char *)0};
            //char *args[] = { "cowsay", (char*)0 };
            execvp("gcc", args);
            LOG("Failed Exec");
            fflush(stdout);
            
            exit(0); // Shutdown if exec failed
        } else { // parent process
          //game->players[i].pid = pid;

//            game->players[i].read = fdopen(fromGCC[0], "r");
  //          game->players[i].write = fdopen(toGCC[1], "a");
          context->dest = fdopen(toGCC[1], "a");
        
          LOG("afdadfs");
       //   printf("char: %c", getc(from));
        
          close(fromGCC[1]);
          close(toGCC[0]);
        }
    }
    compile2file(code, context);
    if(context->dest != stdout)
    fclose(context->dest);
    if (run) {
        int gccStatus;
        wait(&gccStatus);
        if(gccStatus) {
            ERROR("GCC compile error");
        } else {
            char *args[] = {"./lace.out", "", (char *)0};
            // char *args[] = { "cowsay", (char*)0 };
            execvp("./lace.out", args);
            LOG("Failed Exec");
        }
    }
    // fclose(toGCC);
}
void print_type(SymType type) {
    //fprintf(stderr, "%s", (type.c != NULL) ? type.c : type.str);
    //fprintf(stderr, "%s", type.str);
    if (type.pointer) fputc('*', stderr);
    if (type.structy == 2) fputc('*', stderr);
    if(type.arrayLen != NULL) {
        fprintf(stderr, "[");
        print_node(type.arrayLen);
        fprintf(stderr, "]");
    }
 //   return true;
}
void print_node(const TypedNode *node) {
    if (node == NULL) {
        //hold();
        //ERROR("NULL node");
        return;
    }
    // fprintf(stderr, "(%d):", node->nType);
    switch (node->nType) {
        case BINOP:
            fprintf(stderr, "(");
            print_node(node->node.binOp.one);
            fprintf(stderr, " %s ", opStr[node->node.binOp.op]);
            print_node(node->node.binOp.two);
            fprintf(stderr, ")");
            break;
        case UNARY:
            fprintf(stderr, " %s", opStr[node->node.unary.op]);
            print_node(node->node.unary.operand);
            break;
        case IND:
            print_node(node->node.index.left);
            fprintf(stderr, "[");
            print_node(node->node.index.index);
            fprintf(stderr, "]");
            break;
        case DOT:
            fprintf(stderr, "(");
            print_node(node->node.access.left);
            fprintf(stderr, ".%s", node->node.access.word);
            fprintf(stderr, ")");
            break;
            case PRIMATIVE:
            switch (node->pType) {
                case INT:
                    fprintf(stderr, "%d", node->node.primitive.integer);
                    break;
                case FLOAT:
                    fprintf(stderr, "%s", node->node.primitive.string);
                    break;
                case CHAR:
                    fprintf(stderr, "%c", node->node.primitive.character);
                    break;
                case STRING:
                    fprintf(stderr, "\"%s\"", node->node.primitive.string);
                    break;
                case BOOL:
                    fprintf(stderr, "%s",
                            (node->node.primitive.boolean) ? "true" : "false");
                    break;
                default:
                    ERROR("Not impemented %d", node->pType);
                    break;
            }
            break;
            case WORD:
                fprintf(stderr, "%s", node->node.word);
                break;
            case KEYWORD:
                fprintf(stderr, "%s", node->node.word);
                break;
            case NEW:
                fprintf(stderr, "new ");
                print_type(node->node.new.type);
                // print_node(node->node.node);
                break;
            case LET:
                fprintf(stderr, "let %s : %s", node->node.declare.name,
                        node->node.declare.type.str);
                if (node->node.declare.value != NULL) {
                    fprintf(stderr, " = ");
                    print_node(node->node.declare.value);
                }
                break;
            case IF:
            case WHILE:
                fprintf(stderr, "%s ", (node->nType == IF) ? "if" : "while");
                print_node(node->node.ifOrWhile.condition);
                fprintf(stderr, " {\n");
                print_node(node->node.ifOrWhile.body);
                for (int i = 0; i < indentation; i++) fputc('\t', stderr);
                fprintf(stderr, "}");
                break;
            case RET:
                fprintf(stderr, "return ");
                print_node(node->node.node);
                break;
            case MET_FUN:
                fprintf(stderr, "fun %s (", node->node.method.name);
                for (int a = 0; a < node->node.method.numArgs; a++) {
                    fprintf(stderr, "%s : %s",
                            node->node.method.arguments[a].name,
                            node->node.method.arguments[a].type.str);
                    if (a + 1 != node->node.method.numArgs)
                        fprintf(stderr, ", ");
                }
                fprintf(stderr, ") -> %s {\n", node->node.method.type.str);
                print_node(node->node.method.body);
                for (int i = 0; i < indentation; i++) fputc('\t', stderr);
                fprintf(stderr, "}\n");
                break;
            case STRUCT:
            case UNION:
            case CLASS:
                fprintf(stderr, "%s %s {\n",
                        (node->nType == STRUCT) ? "struct" : (node->nType == CLASS) ? "class" : "union",
                        node->node.structure.name);
                for (int a = 0; a < node->node.structure.fields->len; a++) {
                    TypedNode *subNode =
                        (TypedNode *)node->node.structure.fields->array[a]
                            .value;
                    if (subNode->nType == LET) {
                        fprintf(stderr, "%s : %s",
                                node->node.structure.fields->array[a].key,
                                subNode->node.declare.type.str);
                        if (a + 1 != node->node.structure.fields->len)
                            fprintf(stderr, ",\n");
                    } else if (subNode->nType == MET_FUN) {
                        print_node(subNode);
                    }
                }
                fprintf(stderr, "\n}\n");
                break;
            case CODE: {
                indentation++;
                
                ListNode *current = node->node.code.start;
                while ((current = current->next) != NULL) {
                    for (int i = 0; i < indentation; i++) fputc('\t', stderr);
                    print_node((TypedNode *)current->value);
                    if(((TypedNode*)current->value)->statement)
                        fputc('\n', stderr);
                }
                indentation--;
            } break;
            case CALL:
                print_node(node->node.call.method);
                fprintf(stderr, " (");
                for (int a = 0; a < node->node.call.numArgs; a++) {
                    print_node(node->node.call.arguments[a]);
                    if (a + 1 != node->node.call.numArgs) fprintf(stderr, ", ");
                }
                fprintf(stderr, ")");
                break;
            default:
                ERROR("%d not implemented (or forgot break)", node->nType);
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
void error(int code) {
    hold();
    exit(code);
}
