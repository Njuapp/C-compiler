#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_
#include "grammar_tree.h"
#define make_helper(name) void name(struct GrammerTree* node, struct GrammerTree* parent, int location, int inh)
typedef void (*helper_fun)(struct GrammerTree*, struct GrammerTree*, int,int);
#define NOSTRU 0
#define STRU 1
struct Type{
	enum{ BASIC, ARRAY, STRUCTURE} kind;
	union{
		int basic;//1 is for int , 2 is for float
		struct { struct Type* elem; int size; }array;
		struct FieldList *structure;
	};
	char* typeName;
	struct Type* next;
};
struct Var{
	char* name;
	struct Type* type;
	struct Var* next;
};


struct FieldList{
	char* name;
	struct Type* type;
	struct FieldList* next;
};
struct Param{
	struct Type* type;
	struct Param* next;
};
struct Func{
	char *name;
	struct Type* rettype;
	int numOfParams;
	struct Param* head;
	struct Func* next;
};
void sdt(struct GrammerTree* Program);
void SDT(struct GrammerTree* node, struct GrammerTree* parent, int location);
make_helper(inv);
make_helper(ExtDef1);
make_helper(ExtDef2);
make_helper(ExtDef3);
make_helper(ExtDecList1);
make_helper(ExtDecList2);
make_helper(Specifier1);
make_helper(Specifier2);
make_helper(StructDef);
make_helper(StructRef);
make_helper(OptTag1);
make_helper(OptTag2);
make_helper(Tag);
make_helper(VarDec1);
make_helper(VarDec2);
make_helper(FunDec1);
make_helper(FunDec2);
make_helper(VarList1);
make_helper(VarList2);
make_helper(ParamDec);
make_helper(Compst);
make_helper(DefList1);
make_helper(def);
make_helper(DecList1);
make_helper(DecList2);
make_helper(Dec1);
make_helper(Dec2);
make_helper(ExpASSIGNExp);
make_helper(ExpDOTID);
make_helper(ExpID);
#endif
