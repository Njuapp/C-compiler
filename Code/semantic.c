#include "semantic.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define NAME_MAX_LENGTH 40
struct Var*  varTable[0x3fff];
struct Type* typeTable[0x3fff];
struct Func* funcTable[0x3fff];
helper_fun semantics [64]={
	//0-15
	inv, inv, inv, inv,
	inv, inv, ExtDef3, inv,
	inv, Specifier1, inv, inv,
	inv, inv, inv, inv,
	//16-31
	VarDec1, VarDec2, FunDec1, FunDec2,
	VarList1, VarList2, ParamDec, inv,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	//32-47
	inv, inv, def, DecList1,
	DecList2, Dec1, Dec2, inv,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	//48-63
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
};


unsigned int hash_pjw(char* name){
	unsigned int val = 0, i;
	for(; *name ; name++){
		val = (val << 2 ) + *name;
		if( i = val & ~0x3fff) val = (val ^ (i >> 12) ) & 0x3fff;
	}
	return val;
}

void addVar(char* varname, struct Type* typeinfo){
	unsigned int h = hash_pjw(varname);
	struct Var* nwnode = (struct Var*)malloc(sizeof(struct Var));
	nwnode->name = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
	strcpy(nwnode->name, varname);
	nwnode->type = typeinfo;
	nwnode->next = varTable[h];
	varTable[h] = nwnode;
}

struct Var* findVar(char* varname){
	unsigned int h = hash_pjw(varname);
	struct Var* temp = varTable[h];
	while(temp){
		if(!strcmp(varname, temp->name))
			return temp;
		temp = temp->next;
	}
	return 0;
}

struct Func* findFunc(char *funcname){
	unsigned int h = hash_pjw(funcname);
	struct Func* temp = funcTable[h];
	while(temp){
		if(!strcmp(funcname, temp->name))
			return temp;
		temp = temp->next;
	}
	return 0;
}

void addFunc(char* funcname, struct Type* typeinfo){
	unsigned int h = hash_pjw(funcname);
	struct Func* nwnode = (struct Func*)malloc(sizeof(struct Func));
	nwnode->name = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
	strcpy(nwnode->name, funcname);
	nwnode->rettype = typeinfo;
	nwnode->numOfParams = 0;
	nwnode->head = NULL;
	nwnode->next = funcTable[h];
	funcTable[h] = nwnode;
}

void addBasicType(char* typeName){
	unsigned int h = hash_pjw(typeName);
	struct Type* nwnode = (struct Type*)malloc(sizeof(struct Type));
	nwnode->kind = 0;
	if(!strcmp(typeName,"int"))
		nwnode->basic = 1;
	else if(!strcmp(typeName, "float"))
		nwnode->basic = 2;
	else
		assert(0);
	nwnode->typeName =(char*)malloc(sizeof(char)*40);
	strcpy(nwnode->typeName, typeName);
	nwnode->next = typeTable[h];
	typeTable[h] = nwnode;
}

struct Type* findType(char* typeName){
	unsigned int h = hash_pjw(typeName);
	struct Type* temp = typeTable[h];
	while(temp){
		if(!strcmp(typeName, temp->typeName))
			return temp;
		temp = temp->next;
	}
	return 0;
}

void sdt(struct GrammerTree* Program){
	//把内置的基本类型int和float放入类型表里面
	addBasicType("int");
	addBasicType("float");
	SDT(Program->l, Program, 1);
}

void SDT(struct GrammerTree* node, struct GrammerTree* parent, int location){
	if(!node)return;
	int prod = parent->prod;
	printf("%s %d %d\n", node->name, prod, location);
	semantics[prod](node, parent, location ,1);// node inherits something from left nodes or parent
	SDT(node->l, node, 1);
	semantics[prod](node, parent, location ,0);// parent synthesizes something from child node
	SDT(node->r, parent, location + 1);
}

make_helper(ExtDef3){
	switch(location){
		case 1:
		if(!inh)
			parent->typeinfo = node->typeinfo;
		break;
		case 2:
		if(inh)
			node->typeinfo = parent->typeinfo;
		break;
		case 3:
		break;
		default:
		assert(0);
	}
}

make_helper(Specifier1){
	assert(location == 1);
	if(inh == 1)return;// Specifier synthesizes type info from TYPE node
	struct Type* typeinfo = findType(node->idtype);
	if(!typeinfo){
		addBasicType(node->idtype);
		typeinfo = findType(node->idtype);
	}
	parent->typeinfo = typeinfo;
}

make_helper(inv){
}

make_helper(def){
	switch(location){
		case 1:
		if(!inh)//only synthesize
			parent->typeinfo = node->typeinfo;
		break;
		
		case 2:
		if(inh)//inherits only
			node->typeinfo = parent->typeinfo;
		break;
		
		case 3:
		break;

		default:
		assert(0);
	}
}

make_helper(DecList1){
	assert(location==1);
	if(inh)//inherits only
		node->typeinfo = parent->typeinfo;
}

make_helper(DecList2){
	switch(location){
		case 1:
		if(inh)
			node->typeinfo = parent->typeinfo;
		break;
		case 2:
		break;
		case 3:
		if(inh)
			node->typeinfo = parent->typeinfo;
		break;
		default:
		assert(0);
	}
}

make_helper(Dec1){
	assert(location == 1);
	if(inh)
		node->typeinfo = parent->typeinfo;
}

make_helper(Dec2){
	switch(location){
		case 1:
		if(inh)
			node->typeinfo = parent->typeinfo;
		break;
		case 2:
		break;
		case 3:
		break;
		default:
		assert(0);
	}
}

make_helper(VarDec1){
	assert(location == 1);
	if(!inh)return;// inheritance only
	struct Var * var = findVar(node->idtype);
	if(var)
		printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->line,node->idtype);
	else
		addVar(node->idtype, parent->typeinfo);
}

make_helper(VarDec2){

}

make_helper(FunDec1){
	switch(location){
		case 1:
			if(!inh)return;
			struct Func* func = findFunc(node->idtype);
			if(func)
				printf("Error type 3 at Line %d: Redefined function \"%s\".\n",node->line,node->idtype);
			else{
				addFunc(node->idtype, parent->typeinfo);
				parent->funcname = node->idtype;
			}

			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->funcname = parent->funcname;
			}
			else{
				char* funcname = parent->funcname;
				if(!funcname) return;
				struct Func* func = findFunc(funcname);
				func->head = node->param;
				int count = 0;
				struct Param* p = node->param;
				while(p){
					count++;
					p=p->next;
				}
				func->numOfParams = count;
			}
			break;
		case 4:
			break;
		default:
			assert(0);
	}
}

make_helper(FunDec2){
	if(location == 1){
		if(!inh)return;
		struct Func* func = findFunc(node->idtype);
		if(func)
			printf("Error type 3 at Line %d: Redefined function \"%s\".\n",node->line,node->idtype);
		else
			addFunc(node->idtype, parent->typeinfo);
	}
}

void addParam(struct GrammerTree* node, struct GrammerTree* parent, struct Param* param){
	struct Param* root = node->param;
	if(root)
	{
		struct Param* p = root;
		while(p->next)
			p = p->next;
		p->next = param;
	}
	else
		root = param;
	node->param = root;
}

make_helper(VarList1){
	switch(location){
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			assert(0);
	}
}

make_helper(VarList2){
	if(!inh){
		assert(location == 1);

	}
}

make_helper(ParamDec){
	switch(location){
		case 1:
			if(!inh)
				parent->typeinfo = node->typeinfo;
			break;
		case 2:
			if(inh)
				node->typeinfo = parent->typeinfo;
			else{
				struct Param* param = (struct Param*)malloc(sizeof(struct Param));
				param->type = node->typeinfo;
				param->next = NULL;
				parent->param = param;
			}
			break;
		default:
			assert(0);
	}
}

