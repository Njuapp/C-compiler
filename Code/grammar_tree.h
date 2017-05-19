#ifndef _GRAMMAR_TREE_H_
#define _GRAMMAR_TREE_H_
#include "intercode.h"
extern int yylineno;
extern char* yytext;
void yyerror(char *s,...);

struct GrammerTree{
	int line;
	char* name;
	int prod;//The production rule we used to generate this node
	int tag;//NOSTRU(0),表示不是结构；STRU(1) 表示结构
	struct GrammerTree* l;
	struct GrammerTree* r;

	struct Type* typeinfo;//在语法树节点中记录的类型信息
	char* funcname; // function name
	char* arrayname;
	int isLeft; // left value exp
	struct Param* param; // inher params

	char* typeName;
	struct FieldList * stru;

	// intermediate code on
	Operand place;
	int isParam;
	int isBoolOrValue;
	char* label_true;
	char* label_false;
	// intermediate code off

	union{
		char* idtype;
		int intgr;
		float flt;
	};
};

struct GrammerTree * create(char* name,int prod, int num, ...);

void eval(struct GrammerTree *, int level);
#endif
