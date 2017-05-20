#include "semantic.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define NAME_MAX_LENGTH 40
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
	ExpBOOL, ExpBOOL, ExpRELOP, ExpPLUS,
	ExpMINUS, ExpSTAR, ExpDIV, ExpLP,
	//48-63
	ExpUMINUS, ExpNOT, ExpFunc1, ExpFunc2,
	ExpArray, ExpDOTID, ExpID, ExpINT,
	ExpFLOAT, Args1, Args2, inv,
	inv, inv, inv, inv,
};

void sdt(struct GrammerTree* Program){
	//把内置的基本类型int和float放入类型表里面
	addBasicType("int");
	addBasicType("float");
	addFunc("read",findType("int"));
	addFunc("write",findType("int"));
	addStruType(0,"INCOMPLETE STRUCT");
	SDT(Program->l, Program, 1);
}

void SDT(struct GrammerTree* node, struct GrammerTree* parent, int location){
	if(!node)return;
//	printf("%s %s %d start\n", node->name, parent->name, location);
	int prod = parent->prod;
	semantics[prod](node, parent, location ,1);// node inherits something from left nodes or parent
	SDT(node->l, node, 1);
//	printf("%s %s %d middle\n", node->name, parent->name, location);
	semantics[prod](node, parent, location ,0);// parent synthesizes something from child node
	SDT(node->r, parent, location + 1);
//	printf("%s %s %d end\n", node->name, parent->name, location);
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
	else{
		parent->stru = node->stru;

		//intercode
		if(node->arrayname){
			struct Var* var = findVar(node->arrayname);
			assert(var);
			if(var->type->kind == ARRAY){
				Operand op1 = var->temp_name;
				Operand op2 = new_operand(CONSTANT_INT, var->type->array.offset, 0.0, NULL);
				InterCode code = new_intercode(iDEC);
				code->operate2.op1 = op1;
				code->operate2.op2 = op2;
				addCode(code, context);
			}
		}
	}
}

make_helper(Dec2){
	switch(location){
		case 1:
		if(inh){
			node->stru = parent->stru;
			node->tag = parent->tag;
			if(parent->tag == STRU)
				printf("Error type 15 at Line %d: Trying to initialize a field in definition of a structure type.\n",node->line);
			node->typeinfo = parent->typeinfo;
		}
		else{
			parent->arrayname = node->arrayname;
		}
		break;
		case 2:
		break;
		case 3:
		if(inh){
			//intercode
			char* temp = new_temp();
			Operand op = new_operand(VARIABLE, 0, 0.0, temp);
			node->place = op;
			return;
		}
		if(!typeEqual(parent->typeinfo, node->typeinfo))
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",node->line);
		
		//intercode
		struct Var* var = findVar(parent->arrayname);
		assert(var);
		assert(var->type->kind == BASIC);
		Operand op1 = var->temp_name;
		Operand op2 = node->place;
		InterCode code = new_intercode(iASSIGN);
		code->operate2.op1 = op1;
		code->operate2.op2 = op2;
		addCode(code, context);

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
		parent->arrayname = node->idtype;
	}
	else{
		//不是在结构体里面定义的，说明是定义的变量
		//先检查变量符号表看是否出现过：
		//有出现过报错，重复定义
		//否则，添加进变量符号表里去
		struct Var * var = findVar(node->idtype);
		struct Type * struType = findType(node->idtype);
		if(var || struType)
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->line,node->idtype);
		else{
			//intercode
			char *temp = new_var();
			addVar(node->idtype, parent->typeinfo, temp);
			if(parent->isParam){
				Operand var = new_operand(VARIABLE, 0, 0.0, temp);
				InterCode code = new_intercode(iPARAM);
				code->operate1.op = var;
				addCode(code, context);
			}

			parent->arrayname = node->idtype;
		}
	}
}

make_helper(VarDec2){
	switch(location){
		case 1:
			if(inh){
				node->typeinfo = parent->typeinfo;
				node->stru = parent->stru;
				node->tag = parent->tag;

				//intercode
				node->isParam = parent->isParam;
			}
			else{
				parent->stru = node->stru;
				if(node->arrayname){
					if(node->tag == STRU){
						struct FieldList* field = node->stru;
						while(field->next)
							field = field->next;
						struct Type* fieldType = field->type;
						struct Type* type = (struct Type*)malloc(sizeof(struct Type));
						type->kind = ARRAY;
						type->array.elem = fieldType;
						type->next =0;
						type->typeName = 0;
						field->type = type;
						parent->arrayname = node->arrayname;
					}
					else{
						parent->arrayname = node->arrayname;
					}
				}
			}
			break;
		case 2:
			break;
		case 3:
			if(!inh){
				if(parent->arrayname){
					struct Type* type = 0;
					/*
					if(parent->tag == STRU){
						struct FieldList* field = parent->stru;
						while(field->next)
							field = field->next;
						type = field->type;
						type->array.size = node->intgr;
					}
					else{*/
					struct Var* var = findVar(parent->arrayname);
					type = var->type;
					struct Type* newdim = (struct Type*)malloc(sizeof(struct Type));
					newdim->next = NULL;
					newdim->typeName = "ARRAY";
					newdim->kind = ARRAY;
					newdim->array.size = node->intgr;
					if(type->kind != ARRAY){
						newdim->array.elem = type;
						var->type = newdim;
					}
					else{
						while(type->array.elem->kind == ARRAY){
							type = type->array.elem;
						}
						newdim->array.elem = type->array.elem;
						type->array.elem = newdim;
					}
					int top = 0;
					struct Type* offsets[100];
					struct Type* p; 
					for(p= var->type ; p->kind == ARRAY; p = p->array.elem){
						offsets[top] = p;
						top++;
					}
					int sz = 4;
					while(top>0){
						top--;
						sz *= offsets[top]->array.size;
						offsets[top]->array.offset = sz;
					}
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

