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
	StmtList1, inv, inv, Stmt2,
	Stmt3, Stmt4, Stmt5, Stmt6,
	//32-47
	DefList1, inv, Def, DecList1,
	DecList2, Dec1, Dec2, ExpASSIGNExp,
	ExpBOOL, ExpBOOL, ExpBOOL, ExpPMSD,
	ExpPMSD, ExpPMSD, ExpPMSD, ExpLP,
	//48-63
	ExpUMINUS, ExpNOT, ExpFunc1, ExpFunc2,
	ExpArray, ExpDOTID, ExpID, ExpINT,
	ExpFLOAT, Args1, Args2, inv,
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
int arrayEqual(struct Type* left, struct Type* right);
int structEqual(struct Type* left, struct Type* right);

int typeEqual(struct Type* left, struct Type* right){
	if(!left || !right)
		return 0;
	if(left->kind != right->kind)
		return 0;
	if(left->kind == BASIC)
		return left->basic == right->basic;
	if(left->kind == ARRAY)
		return arrayEqual(left, right);
	if(left->kind == STRUCTURE)
		return structEqual(left, right);
}

int arrayEqual(struct Type* left, struct Type* right){
	assert(left->kind == ARRAY && right->kind == ARRAY);
	for(; left->kind == ARRAY && right->kind == ARRAY; left = left->array.elem, right = right->array.elem)
		;
	return typeEqual(left, right);
}

int structEqual(struct Type* left, struct Type* right){
	assert(left->kind == STRUCTURE&& right->kind == STRUCTURE);
	struct FieldList* lp = left->structure;
	struct FieldList* rp = right->structure;
	for(; lp && rp; lp = lp->next, rp = rp ->next){
		if(!typeEqual(lp->type, rp->type))
			return 0;
	}
	if(lp || rp)
		return 0;
	return 1;
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
	nwnode->kind = BASIC;
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
	assert(typeName);
	unsigned int h = hash_pjw(typeName);
	struct Type* nwnode = (struct Type*)malloc(sizeof(struct Type));
	nwnode->kind = 2;
	nwnode->structure = stru;
	nwnode->typeName = typeName;
	struct FieldList* t = stru;
	printf("----------\n");
	printf("Structure %s contains:\n",typeName);
	for( ; t; t = t->next)
		printf("field name:%s type:%s\n",t->name, t->type->typeName);
	printf("----------\n");
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
	addStruType(0,"INCOMPLETE STRUCT");
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

make_helper(ExtDef3){
	switch(location){
		case 1:
		if(!inh)
			parent->typeinfo = node->typeinfo;
		break;
		case 2:
		if(inh)
			node->typeinfo = parent->typeinfo;
		else
			parent->funcname = node->funcname;
		break;
		case 3:
		if(inh)
			node->funcname = parent->funcname;
		break;
		default:
		assert(0);
	}
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
//	printf("Tag %s.\n",node->idtype);	
	parent->typeName = node->idtype;
}

make_helper(Def){
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
		if(inh) return;
		if(!typeEqual(parent->typeinfo, node->typeinfo))
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",node->line);
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
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",node->line,node->idtype);
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
		else{
			if(parent->typeinfo->kind == STRUCTURE)
				printf("struct-type variable %s.\n",node->idtype);
			addVar(node->idtype, parent->typeinfo);
			parent->arrayname = node->idtype;
		}
	}
}

make_helper(VarDec2){
	switch(location){
		case 1:
			if(inh){
				node->typeinfo = parent->typeinfo;
			}
			else{
				if(node->arrayname){
					struct Type* type = (struct Type*)malloc(sizeof(struct Type));
					type->kind = ARRAY;
					struct Var* var = findVar(node->arrayname);
					type->array.elem = var->type;
					var->type = type;
					parent->arrayname = node->arrayname;
					printf("array %s\n",node->arrayname);
				}
			}
			break;
		case 2:
			break;
		case 3:
			if(!inh){
				if(parent->arrayname){
					struct Var* var = findVar(parent->arrayname);
					struct Type* type = var->type;
					assert(type->kind == ARRAY);
					assert(!strcmp(node->name, "INT"));
					type->array.size = node->intgr;
					struct Type* t = type;
					char* name = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
					name[0] = '\0';
					while(t->kind == ARRAY){
						char *number = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
						sprintf(number, "[%d]", t->array.size);
						strcat(name, number);
						free(number);
						t = t->array.elem;
					}
					char *temp = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
					strcpy(temp, t->typeName);
					strcat(temp, name);
					free(name);
					type->typeName = temp;
					var->type = type;
				}
			}
			break;
		case 4:
			break;
	}
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
		if(inh){
			node->funcname = parent->funcname;
		}
		break;
		case 4:
		break;
		default:
		assert(0);
	}
}

make_helper(StmtList1){
	switch(location){
		case 1:
			if(inh){
				node->funcname = parent->funcname;
			}
			break;
		case 2:
			if(inh){
				node->funcname = parent->funcname;
			}
			break;
	}
}

make_helper(Stmt2){
	assert(location == 1);
	if(inh){
		node->funcname = parent->funcname;
	}
}

make_helper(Stmt3){
	switch(location){
		case 1:
			break;
		case 2:
			if(!inh){
				assert(parent->funcname);
				struct Func* func = findFunc(parent->funcname);
				if(!typeEqual(node->typeinfo, func->rettype))
					printf("Error type 8 at Line %d: Type dismatched for return.\n",node->line);
				}
			break;
		case 3:
			break;
	}
}

make_helper(Stmt4){
	switch(location){
		case 1:
			break;
		case 2:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 4:
			break;
		case 5:
			if(inh)
				node->funcname = parent->funcname;
			break;
	}
}

make_helper(Stmt5){
	switch(location){
		case 1:case 2:case 4:case 6:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 5:case 7:
			if(inh)
				node->funcname = parent->funcname;
	}
}

make_helper(Stmt6){
	switch(location){
		case 1:case 2:case 4:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 5:
			if(inh)
				node->funcname = parent->funcname;
	}
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
		//TODO:question
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

make_helper(ExpASSIGNExp){
	switch(location){
		case 1:
		if(!inh){
			if(!node->isLeft)
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",node->line);
			parent->typeinfo = node->typeinfo;
		}
		break;
		case 2:
		break;
		case 3:
		if(inh)return;
		if(!typeEqual(parent->typeinfo, node->typeinfo))
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",node->line);
		break;
		default:
		assert(0);
	}
}

make_helper(ExpBOOL){ // exp for bool
	switch(location){
		case 1:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 2:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = node->typeinfo;
			break;
		default:
			assert(0);
	}
}

make_helper(ExpPMSD){ // exp for plus minus star div
	switch(location){
		case 1:
			if(inh) return;
			if(node->typeinfo->kind != BASIC)
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			parent->typeinfo = node->typeinfo;
			break;
		case 2:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, parent->typeinfo))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		default:
			assert(0);
	}
}

make_helper(ExpLP){ // exp for (exp)
	switch(location){
		case 1:
			break;
		case 2:
			if(inh) return;
			parent->typeinfo = node->typeinfo;
			break;
		case 3:
			break;
		default:
			assert(0);
	}
}

make_helper(ExpUMINUS){ // exp for minus exp
	switch(location){
		case 1:
			break;
		case 2:
			if(inh) return;
			if(node->typeinfo->kind != BASIC)
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			parent->typeinfo = node->typeinfo;
			break;
		default:
			assert(0);
	}
}

make_helper(ExpNOT){ // exp for not
	switch(location){
		case 1:
			break;
		case 2:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = node->typeinfo;
			break;
	}
}

make_helper(ExpFunc1){
	switch(location){
		case 1:
			if(!inh){
				char* name = node->idtype;
				if(findVar(name)){
					printf("Error type 11 at Line %d: \"%s\" is not a function.\n",node->line,node->idtype);
					return;
				}
				struct Func* func = findFunc(name);
				if(!func)
					printf("Error type 2 at Line %d: Undefined function \"%s\".\n",node->line,node->idtype);
				else{
					parent->funcname = name;
					parent->typeinfo = func->rettype;
				}
			}
			break;
		case 2:
			break;
		case 3:
			if(inh) return;
			char* name = parent->funcname;
			if(!name) return;
			struct Func* func = findFunc(name);
			struct Param* p1 = func->head, *p2 = node->param;
			for(; p1 && p2; p1 = p1->next, p2 = p2->next){
				if(!typeEqual(p1->type, p2->type))
					printf("Error type 9 at Line %d: Function is not applicable for arguments.\n",node->line);
			}
			if(p1 || p2)
				printf("Error type 9 at Line %d: Function is not applicable for arguments.\n",node->line);

			break;
		case 4:
			break;
	}
}

make_helper(ExpFunc2){
	switch(location){
		case 1:
			if(inh) return;
			char *name = node->idtype;
			if(findVar(name)){
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",node->line,node->idtype);
				return;
			}
			struct Func* func = findFunc(name);
			if(!func)
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",node->line,node->idtype);
			else{
				if(func->numOfParams != 0)
					printf("Error type 9 at Line %d: Function is not applicable for arguments.\n",node->line);
				parent->typeinfo = func->rettype;
			}

			break;
		case 2:
			break;
		case 3:
			break;
	}
}

make_helper(ExpArray){
	switch(location){
		case 1:
			if(!inh){
				if(node->typeinfo->kind != ARRAY)
					printf("Error type 10 at Line %d: The exp is not a array.\n",node->line);
				parent->typeinfo = node->typeinfo->array.elem;
				parent->isLeft = 1;
			}
			break;
		case 2:
			break;
		case 3:
			if(!inh){
				struct Type* INT = findType("int");
				if(!typeEqual(node->typeinfo, INT))
					printf("Error type 12 at Line %d: The exp is not a integer.\n",node->line);
			}
			break;
		case 4:
			break;
	}
}

make_helper(ExpDOTID){
	switch(location){
		case 1:
		if(!inh){
			parent->typeinfo = node->typeinfo;
			parent->isLeft = 1;
		}
		break;
		case 2:
		break;
		case 3:
		if(!inh){
			struct Type* typeinfo = parent->typeinfo;
			if(typeinfo->kind != STRUCTURE)
				printf("Error type 13 at Line %d: Illegal use of \".\".\n",node->line);
			else{
				struct FieldList* field = typeinfo->structure;
				for( ; field; field = field->next)
					if(!strcmp(node->idtype, field->name))
						break;
				if(!field)
					printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",node->line,node->idtype);
				else
					parent->typeinfo = field->type;
			}
		}
		break;
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
		parent->isLeft = 1;
	}
}

make_helper(ExpINT){
	assert(location == 1);
	if(!inh)
		parent->typeinfo = findType("int");
}

make_helper(ExpFLOAT){
	assert(location == 1);
	if(!inh){
		parent->typeinfo = findType("float");
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

void print_table(){
	for(int i=0;i<0x3fff;i++){
		if(typeTable[i])
			printf("%d %s\n", typeTable[i]->kind, typeTable[i]->typeName);
	}
	printf("\n");
	
	for(int i=0;i<0x3fff;i++){
		if(varTable[i] && varTable[i]->type && varTable[i]->name){
			if(varTable[i]->type->kind == ARRAY){
				struct Type* type = varTable[i]->type;
				while(type->array.size){
					printf("%d ",type->array.size);
					type = type->array.elem;
				}
			}
			printf("%d %s %s\n", varTable[i]->type->kind, varTable[i]->name, varTable[i]->type->typeName);
		}
	}
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
