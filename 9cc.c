
// ヘッダ
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>




// マクロ
enum {
	TK_NUM = 256,	// 整数トークン
	TK_EOF,			// EOF
	ND_NUM = 256,	// 整数のノードの型
};




// 構造体
typedef struct {
	int ty;				// トークンの型
	int val;			// tyがTK_NUMの場合、その数値
	char *input;		// トークン文字列（エラーメッセージ用）
} Token;

typedef struct Node {
	int ty;				// 演算子かND_NUM
	struct Node *lhs;	// 左辺
	struct Node *rhs;	// 右辺
	int val;			// tyがND_NUMの場合のみ使う
} Node;



// データ
Token Tokens[100];
int Pos = 0;




// 関数
void tokenize(char *p);
void error(char *s,char *i);
Node *new_node(int op, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *mul();
Node *term();
void gen(Node *node);




int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません。\n");
		return 1;
	}
	
	tokenize(argv[1]);
	Node *node = expr();
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main: \n");
	
	// 抽象構文木を下りながらコード生成
	gen(node);
	
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}


void tokenize(char *p)
{
int i = 0;


	while (*p) {
		// 空白スキップ
		if (isspace(*p)) {
			p++;
			continue;
		}
		
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
			Tokens[i].ty = *p;
			Tokens[i].input = p;
			i++;
			p++;
			continue;
		}
		
		if (isdigit(*p)) {
			Tokens[i].ty = TK_NUM;
			Tokens[i].input = p;
			Tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}
		
		fprintf(stderr, "トークナイズできません： %s\n", p);
		exit(1);
	}
	Tokens[i].ty = TK_EOF;
	Tokens[i].input = p;
}


void error(char *s, char *i)
{
	printf("%s %c\n", s, *i);
	exit(1);
}


Node *new_node(int op, Node *lhs, Node *rhs)
{
Node *node = malloc(sizeof(Node));
node->ty = op;
node->lhs = lhs;
node->rhs = rhs;
return node;
}


Node *new_node_num(int val)
{
Node *node = malloc(sizeof(Node));
node->ty = ND_NUM;
node->val = val;
return node;
}


Node *expr()
{
	Node *lhs = mul();
	
	if (Tokens[Pos].ty == TK_EOF)
		return lhs;
	
	if (Tokens[Pos].ty == '+') {
		Pos++;
		return new_node('+', lhs, expr());
	}
	if (Tokens[Pos].ty == '-') {
		Pos++;
		return new_node('-', lhs, expr());
	}
	error("(1)想定しないトークンです:", Tokens[Pos].input);
}


Node *mul()
{
Node *lhs = term();


	if (Tokens[Pos].ty == '+' || Tokens[Pos].ty == '-')		// add hara
		return lhs;											// add hara
	if (Tokens[Pos].ty == TK_EOF)
		return lhs;
	if (Tokens[Pos].ty == '*') {
		Pos++;
		return new_node('*', lhs, mul());
	}
	if (Tokens[Pos].ty == '/') {
		Pos++;
		return new_node('/', lhs, mul());
	}
	error("(2)想定しないトークンです:", Tokens[Pos].input);
}

Node *term()
{
	if (Tokens[Pos].ty == TK_NUM)
		return new_node_num(Tokens[Pos++].val);
	if (Tokens[Pos].ty == '(') {
		Pos++;
		Node *node = expr();
		if (Tokens[Pos].ty != ')') {
			error("開きカッコに対応する閉じカッコがありません:", Tokens[Pos].input);
			Pos++;
			return node;
		}
	}
	error("数値でも開きカッコでもないトークンです:", Tokens[Pos].input);
}


void gen(Node *node)
{
	if (node->ty == ND_NUM) {		// nodeは１つではない
		printf("	push %d\n", node->val);
		return;
	}
	
	gen(node->lhs);
	gen(node->rhs);
	
	printf("	pop rdi\n");
	printf("	pop rax\n");
	
	switch (node->ty) {
	case '+':
		printf("	add rax, rdi\n");
		break;
	case '-':
		printf("	sub rax, rdi\n");
		break;
	case '*':
		printf("	mul rdi\n");
		break;
	case '/':
		printf("	mov rdx, 0\n");
		printf("	div rdi\n");
	}
	printf("	push rax\n");
}


