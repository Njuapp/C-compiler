#include "semantic.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

make_helper(FunDec1){
	switch(location){
		case 1:
			if(!inh)return;
			struct Func* func = findFunc(node->idtype);
			if(func)
				printf("Error type 4 at Line %d: Redefined function \"%s\".\n",node->line,node->idtype);
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
			printf("Error type 4 at Line %d: Redefined function \"%s\".\n",node->line,node->idtype);
		else
			addFunc(node->idtype, parent->typeinfo);
		parent->funcname = node->idtype;
	}
}

void addParam(struct GrammerTree* node, struct Param* param){
	struct Param* root = node->param;
	if(root)
	{
		struct Param* p = root;
		while(p->next){
			p = p->next;
		}
		p->next = param;
	}
	else
		root = param;
	node->param = root;
}

make_helper(ParamDec){
	switch(location){
		case 1:
			if(!inh)
				parent->typeinfo = node->typeinfo;
			break;
		case 2:
			if(inh){
				node->typeinfo = parent->typeinfo;
			}
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

make_helper(VarList1){
	switch(location){
		case 1:
			if(!inh){
				addParam(parent, node->param);
			}
			break;
		case 2:
			break;
		case 3:
			if(!inh){
				addParam(parent, node->param);
			}
			break;
		default:
			assert(0);
	}
}

make_helper(VarList2){
	assert(location == 1);
	if(!inh){
		addParam(parent, node->param);
	}
}

make_helper(Args1){
	switch(location){
		case 1:
			if(inh) return;
			struct Param* param = (struct Param*)malloc(sizeof(struct Param));
			param->type = node->typeinfo;
			param->next = NULL;
			addParam(parent, param);
			break;
		case 2:
			break;
		case 3:
			if(inh) return;
			addParam(parent, node->param);
			break;
		default:
			assert(0);
	}
}

make_helper(Args2){
	assert(location == 1);
	if(inh) return;
	struct Param* param = (struct Param*)malloc(sizeof(struct Param));
	param->type = node->typeinfo;
	param->next = NULL;
	addParam(parent, param);
}
