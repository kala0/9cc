
// ヘッダ
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>




// マクロ
enum {
	TK_NUM = 256,	// 整数トークン
	TK_EOF,			// EOF
};




// 構造体
typedef struct {
	int ty;			// トークンの型
	int val;		// tyがTK_NUMの場合、その数値
	char *input;	// トークン文字列（エラーメッセージ用）
} Token;




// データ
Token Tokens[100];




// 関数
void tokenize(char *p);
void error(int i);




int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません。\n");
		return 1;
	}
	
	tokenize(argv[1]);
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main: \n");
	
	if (Tokens[0].ty != TK_NUM)
		error(-1);
	printf("	mov rax, %d\n", Tokens[0].val);
	
	int i = 1;
	while (Tokens[i].ty != TK_EOF) {
		if (Tokens[i].ty == '+') {
			i++;
			if (Tokens[i].ty != TK_NUM)
				error(i);
			printf("	add rax, %d\n", Tokens[i].val);
			i++;
			continue;
		}
		if (Tokens[i].ty == '-') {
			i++;
			if (Tokens[i].ty != TK_NUM)
				error(i);
			printf("	sub rax, %d\n", Tokens[i].val);
			i++;
			continue;
		}
		error(i);
	}
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
		
		if (*p == '+' || *p == '-') {
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


void error(int i)
{
	fprintf(stderr, "予期せぬトークンです： %s\n", Tokens[i].input);
	exit(1);
}


