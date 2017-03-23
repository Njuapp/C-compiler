extern int yylineno;
extern char* yytext;
void yyerror(char *s,...);

struct GrammerTree{
	int line;
	char* name;
	struct GrammerTree* l;
	struct GrammerTree* r;
	union{
		char* idtype;
		int intgr;
		float flt;
	};
};

struct GrammerTree * create(char* name, int num, ...);

void eval(struct GrammerTree *, int level);
