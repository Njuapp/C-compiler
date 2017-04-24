#include "semantic.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
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

make_helper(Specifier2){
	assert(location==1);
	if(!inh)//synthesize only
		parent->typeinfo = node->typeinfo;	
}

make_helper(StructDef){
	switch(location){
		case 1:
		break;
		case 2:
		if(inh)
			node->typeName = 0;//有时候结构体是没有名字的，所以需要先初始化OptTag的typeName为空串
		else
			parent->typeName = node->typeName;
		break;
		case 3:
		break;
		case 4:
		if(inh){
			node->tag = STRU;
			node->stru = 0;
		}
		else{
			struct Type* struType = findType(parent->typeName);
			struct Var* var = findVar(parent->typeName);
			if(struType|| var)
				printf("Error Type 16 at Line %d: Duplicated name \"%s\".\n", parent->line, parent->typeName);
			else{
				if(!parent->typeName){
					struType = (struct Type*)malloc(sizeof(struct Type));
					struType->kind = STRUCTURE;
					struType->structure = node->stru;
					struType->next = 0;
					struType->typeName=0;
					parent->typeinfo = struType;
				}
				else{
					addStruType(node->stru,parent->typeName);
					parent->typeinfo = findType(parent->typeName);
				}
			}
		}
		break;
		case 5:
		break;
		default:
		assert(0);
	}
}

make_helper(StructRef){
	switch(location){
		case 1:
		break;
		case 2:
		if(!inh){
			struct Type* struType = findType(node->typeName);
			if(!struType){
				printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",node->line, node->typeName);
				//如果使用了没有定义的结构类型，把这个结构类型设置为预订义好的空的结构类型
				parent->typeinfo = findType("INCOMPLETE STRUCT");
			}
			else{
				parent->typeinfo = struType;
			}
		}
		break;
		default:
		assert(0);
	}
}

make_helper(OptTag1){
	assert(location == 1);
	if(inh)return;
	parent->typeName = node->idtype;
}

make_helper(OptTag2){
}

make_helper(Tag){
	assert(location==1);
	if(inh)return;
	parent->typeName = node->idtype;
}
