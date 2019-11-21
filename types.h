#ifndef TYPES_HEADER
#define TYPES_HEADER

typedef char* string;
struct ListNode;
typedef struct ListNode ListNode;
struct ListNode
{
	void *value;
	struct ListNode *next;
};
typedef enum { BINOP, INT, CHAR, STRING, BOOL, WORD, KEYWORD } TokType;
typedef enum { ADD, SUB, MUL, DIV, ASS/*, NEG, INV, AND, OR, NOR, XOR*/ } OpType;
struct TypedNode;
typedef struct TypedNode TypedNode;
typedef struct  {
	TokType type;
	//string text;
} CallNode;
typedef struct  {
	TypedNode* one;
	OpType op;
	TypedNode* two;
} BinOpNode;
typedef union  {
	CallNode call;
	BinOpNode binOp;
	int integer;
	float floating;
	char character;
	bool boolean;
	string word;
} AnyNode;
struct TypedNode {
	TokType type;
	AnyNode node;
};

#endif
