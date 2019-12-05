#ifndef TYPES_HEADER
#define TYPES_HEADER
#include "dictionary.h"
#include "includes.h"
typedef char* string;
struct ListNode;
typedef struct ListNode ListNode;
struct ListNode {
    void* value;
    struct ListNode* next;
};
typedef struct List List;
struct List {
    ListNode* start;
    ListNode* end;
};
typedef enum { INT, FLOAT, STRING, BOOL, CHAR } Primitives;
typedef enum {
    BINOP,
    LET,
    IF,
    WHILE,
    RET,
    MET_FUN,
    CALL,
    DOT,
    STRUCT,
    UNION,
    CLASS,
    IND,
    PRIMATIVE,
    WORD,
    KEYWORD,
    NEW,
    CODE,
    UNARY
} TokType;
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    ASS,
    EQU,
    NEQ,
    AST,
    AMP
     /*, NEG, INV, AND, OR, NOR, XOR*/
} OpType;
struct TypedNode;
typedef struct TypedNode TypedNode;
struct Argument;
typedef struct Argument Argument;
struct SymType;
typedef struct SymType SymType;
typedef struct {
    string c;
    SymType* ret;
    int numArgs;
    Argument* arguments;
} Type;
struct SymType {
    string str;
    Type base;
    Type type;
    int structy;
    TypedNode* typeNode;
    int pointer;
    
    TypedNode* arrayLen;
    bool fun;
    bool met;
    string c;
    //  SymType* down;
};

typedef struct {
    SymType type;
} Symbol;
struct Argument {
    string name;
    SymType type;
};
typedef struct {
    TokType type;
    TypedNode* method;
    TypedNode** arguments;
    int numArgs;
} CallNode;
typedef struct {
    SymType type;
} NewNode;
typedef struct {
    OpType op;
    TypedNode* operand;
} UnaryNode;
typedef struct {
    TypedNode* one;
    OpType op;
    TypedNode* two;
} BinOpNode;
typedef struct {
    string name;
    SymType type;
    TypedNode* value;
} DeclareNode;
typedef struct {
    ListNode* start;
} CodeNode;
typedef struct {
    TypedNode* condition;
    TypedNode* body;
    TypedNode* elseNode;
} IfNode;
typedef struct {
    TypedNode* condition;
    TypedNode* body;
} WhileNode;
typedef struct {
    TypedNode* word;
    TypedNode* collection;
    TypedNode* body;
} ForNode;
typedef struct {
    string name;
    SymType type;
    int numArgs;
    Argument* arguments;
    TypedNode* body;
    bool fun;
    TypedNode* parent;
} MethodNode;
typedef struct {
    TypedNode* left;
    string word;
} AccessNode;
typedef struct {
    TypedNode* left;
    TypedNode* index;
    bool set;
} IndexNode;
typedef struct {
    TokType type;
    string name;
    Dict* fields;
    int fieldCount;
} StructNode;  // <- shared with union
typedef struct {
    string name;
    Argument* fields;
} ClassNode;
typedef union {
    int integer;
    float floating;
    char character;
    bool boolean;
    string string;
} PrimativeNode;
typedef union {
    CallNode call;
    BinOpNode binOp;
    CodeNode code;
    DeclareNode declare;
    IfNode ifOrWhile;
    TypedNode* node;
    MethodNode method;
    MethodNode function;
    StructNode structure;
    AccessNode access;
    IndexNode index;
    PrimativeNode primitive;
    UnaryNode unary;
    NewNode new;
    string word;
} AnyNode;
struct TypedNode {
    TokType nType;
    Primitives pType;
    SymType rType;
    bool statement;
    AnyNode node;
};

#endif
