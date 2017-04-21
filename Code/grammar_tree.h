#ifndef _GRAMMAR_TREE_H_
#define _GRAMMAR_TREE_H_
extern int yylineno;
extern char* yytext;
void yyerror(char *s,...);

struct GrammerTree{
	int line;
	char* name;
	int prod;//The production rule we used to generate this node
	int tag;//STRU(2) 表示结构
	struct GrammerTree* l;
	struct GrammerTree* r;

	struct Type* typeinfo;//在语法树节点中记录的类型信息
	char* funcname; // function name
	char* arrayname;
	int isLeft; // left value exp
	struct Param* param; // inher params

	char* typeName;
	struct FieldList * stru;

	union{
		char* idtype;
		int intgr;
		float flt;
	};
};

struct GrammerTree * create(char* name,int prod, int num, ...);

void eval(struct GrammerTree *, int level);
#endif
