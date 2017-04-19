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
	ExtDef1, ExtDef2, ExtDef3, ExtDecList1,
	ExtDecList2, Specifier1, Specifier2, StructDef,
	StructRef, OptTag1, OptTag2, Tag,
	//16-31
	VarDec1, VarDec2, FunDec1, FunDec2,
	VarList1, VarList2, ParamDec, Compst,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	//32-47
	DefList1, inv, def, DecList1,
	DecList2, Dec1, Dec2, inv,
	inv, inv, inv, inv,
	inv, inv, inv, inv,
	//48-63
	inv, inv, inv, inv,
	inv, ExpDOTID, ExpID, inv,
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

void addStruType(struct FieldList* stru, char* typeName){
	static int dummy = 0;
	if(!typeName){
		//有的类是没有名字的，比如匿名结构体
		//还有数组类型，这个时候就把一个“数字+(array/struct)”作为虚设的typeName
		//因为typeName作为ID，肯定不是以数字开头的，这样就可以与已有类型的区分开
		typeName = (char*)malloc(sizeof(char)*40);
		sprintf(typeName,"%dstruct",dummy);
		dummy ++;
	}
	unsigned int h = hash_pjw(typeName);
	struct Type* nwnode = (struct Type*)malloc(sizeof(struct Type));
	nwnode->kind = 2;
	nwnode->structure = stru;
	nwnode->typeName = typeName;
	struct FieldList* t = stru;
	printf("Structure %s contains:\n",typeName);
	for( ; t; t = t->next)
		printf("field name:%s type:%s\n",t->name, t->type->typeName);
	nwnode->next = typeTable[h];
	typeTable[h] = nwnode;
}

struct Type* findType(char* typeName){
	if(!typeName)//这是个没有名字的类型，比如匿名结构体，直接返回NULL(0),表示以前未定过
		return 0;
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

make_helper(inv){
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

make_helper(ExtDef1){
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

make_helper(ExtDef2){
}

make_helper(ExtDecList1){
	assert(location == 1);
	if(inh){
		node->tag = NOSTRU;
		node->typeinfo = parent->typeinfo;
	}
}

make_helper(ExtDecList2){
	switch(location){
		case 1:
		if(inh){
			node->tag = NOSTRU;
			node->typeinfo = parent->typeinfo;
		}
		break;

		case 2:
		break;

		case 3:
		if(inh){
			node->tag = NOSTRU;
			node->typeinfo = parent->typeinfo;
		}
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
			if(struType)
				printf("Error Type 16 at Line %d: Duplicated name \"%s\".\n", parent->line, parent->typeName);
			else
				addStruType(node->stru,parent->typeName);
			parent->typeinfo = findType(parent->typeName);
			
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
			if(!struType)
				printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",node->line, node->typeName);
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

make_helper(def){
	switch(location){
		case 1:
		//从Specifier获取一个综合属性，也就是类型信息typeinfo
		if(!inh)//only synthesize
			parent->typeinfo = node->typeinfo;
		break;
		
		case 2:
		if(inh){//inherits only
			//把typeinfo传递给DecList，也就是一系列逗号隔开的变量名
			node->typeinfo = parent->typeinfo;
			node->tag = parent->tag;
			node->stru = parent->stru;
		}
		else{
			parent->stru = node->stru;
		}
		break;
		
		case 3:
		break;

		default:
		assert(0);
	}
}

make_helper(DecList1){
	assert(location==1);
	if(inh){//inherits only
		node->stru = parent->stru;
		node->tag = parent->tag;
		node->typeinfo = parent->typeinfo;
	}
	else{
		parent->stru = node->stru;
	}
}

make_helper(DecList2){
	switch(location){
		case 1:
		if(inh){
			node->stru = parent->stru;
			node->tag = parent->tag;
			node->typeinfo = parent->typeinfo;
		}
		else
			parent->stru = node->stru;
		break;
		case 2:
		break;
		case 3:
		if(inh){
			node->stru = parent->stru;
			node->tag = parent->tag;
			node->typeinfo = parent->typeinfo;
		}
		else
			parent->stru = node->stru;
		break;
		default:
		assert(0);
	}
}

make_helper(Dec1){
	assert(location == 1);
	if(inh){
		node->stru = parent->stru;
		node->tag = parent->tag;
		node->typeinfo = parent->typeinfo;
	}
	else
		parent->stru = node->stru;
}

make_helper(Dec2){
	switch(location){
		case 1:
		if(inh){
			node->tag = parent->tag;
			node->typeinfo = parent->typeinfo;
		}
		break;
		case 2:
		break;
		case 3:
		//TODO:查看复制语句两边是否类型相同
		break;
		default:
		assert(0);
	}
}

make_helper(VarDec1){
	assert(location == 1);
	if(!inh)return;// inheritance only
	if(parent->tag == STRU){
		//变量声明是在结构体里面,要做的就是把这个域的
		//名字，类型信息插入到FieldList这个链表中去

		//先检查这个域的名字是否在这个结构体里面出现过
		//如果出现过，报错且不添加这个域
		struct FieldList* temp;
		for(temp = parent->stru ; temp; temp = temp->next){
			if(!strcmp(temp->name,node->idtype)){
				printf("Error type 15 at Line %d:Redefined field \"%s\".\n",node->line,node->idtype);
				return;
			}
		}
		struct FieldList* nwnode = (struct FieldList*)malloc(sizeof(struct FieldList));
		nwnode->name = node->idtype;
		nwnode->type = parent->typeinfo;
		nwnode->next = 0;
		if(!parent->stru){
			parent->stru = nwnode;
		}
		else{
			temp = parent->stru;
			while(temp->next)
				temp = temp ->next;
			temp->next = nwnode;
		}
	}
	else{
		//不是在结构体里面定义的，说明是定义的变量
		//先检查变量符号表看是否出现过：
		//有出现过报错，重复定义
		//否则，添加进变量符号表里去
		struct Var * var = findVar(node->idtype);
		if(var)
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->line,node->idtype);
		else
			addVar(node->idtype, parent->typeinfo);
	}
}

make_helper(VarDec2){
	
}

make_helper(Compst){
	switch(location){
		case 1:
		break;
		case 2:
		if(inh){
			node->tag = NOSTRU;
			//为DefList设置一个Tag域，DefList出现在结构体中，Tag域为STRU；否则为NOSTRU
			//此处的DefList出现在函数局部定义中，所以tag设置为NOSTRU
			node->stru = 0;
			//虽然此处不涉及结构体定义，但为了安全起见，还是把stru这个域设为空
		}
		break;
		case 3:
		break;
		case 4:
		break;
		default:
		assert(0);
	}
}

make_helper(ExpDOTID){
	//TODO:检测错误类型13,14,最好支持递归取结构体的域
}

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
	}
}

void addParam(struct GrammerTree* node, struct Param* param){
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

make_helper(DefList1){
	switch(location){
		case 1:
		//进入Def节点，也就是单个以分号结尾的定义语句
		if(inh){
			node->tag = parent->tag;
			node->stru = parent->stru;
		}
		else{
			parent->stru = node->stru;
		}
		break;
		case 2:
		//进入下一个DefList节点
		if(inh){
			node->stru = parent->stru;
			node->tag = parent->tag;
		}
		else{
			parent->stru = node->stru;
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

make_helper(ExpID){
	assert(location == 1);
	if(inh)return;//synthesize information from child-node
	struct Var * var = findVar(node->idtype);
	if(!var)
		printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",node->line,node->idtype);
	else{
		parent->typeinfo = var->type;
	}
}

void print_table(){
	for(int i=0;i<0x3fff;i++){
		if(typeTable[i])
			printf("%d %s\n", typeTable[i]->kind, typeTable[i]->typeName);
	}
	printf("\n");
	/*
	for(int i=0;i<0x3fff;i++){
		if(varTable[i])
			printf("%d %s\n", varTable[i]->type, varTable[i]->name);
	}*/
	printf("\n");
	for(int i=0;i<0x3fff;i++){
		if(funcTable[i]){
			printf("%d %s %d", funcTable[i]->rettype->kind, funcTable[i]->name, funcTable[i]->numOfParams);
			struct Param* p = funcTable[i]->head;
			while(p){
				printf(" %s", p->type->typeName);
				p=p->next;
			}
			printf("\n");
		}
	}
}
