#ifndef _GRAMMAR_TREE_H_
#define _GRAMMAR_TREE_H_
extern int yylineno;
extern char* yytext;
void yyerror(char *s,...);

struct GrammerTree{
	int line;
	char* name;
	int prod;//The production rule we used to generate this node
	struct GrammerTree* l;
	struct GrammerTree* r;
	struct Type* typeinfo;//在语法树节点中记录的类型信息
	union{
		char* idtype;
		int intgr;
		float flt;
	};
};

struct GrammerTree * create(char* name,int prod, int num, ...);

void eval(struct GrammerTree *, int level);
#endif
