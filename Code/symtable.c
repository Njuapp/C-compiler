#include "semantic.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define NAME_MAX_LENGTH 40
struct Var*  varTable[0x3fff];
struct Type* typeTable[0x3fff];
struct Func* funcTable[0x3fff];
unsigned int hash_pjw(char* name){
	unsigned int val = 0, i;
//	printf("hash: %s\n", name);
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
void addVar(char* varname, struct Type* typeinfo, char* tempname){
	unsigned int h = hash_pjw(varname);
	struct Var* nwnode = (struct Var*)malloc(sizeof(struct Var));
	nwnode->name = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
	strcpy(nwnode->name, varname);
	nwnode->type = typeinfo;
	nwnode->temp_name = new_operand(VARIABLE, 0, 0.0, tempname);
	nwnode->next = varTable[h];
	varTable[h] = nwnode;
}

struct Var* findVar(char* varname){
	if(!varname) return NULL;
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
	if(!funcname) return NULL;
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
	if(!strcmp(funcname, "write")){
		nwnode->numOfParams = 1;
		struct Param* p = (struct Param*)malloc(sizeof(struct Param));
		p->next = NULL;
		p->type = typeinfo;
		nwnode->head = p;
	}
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
	nwnode->typeName =(char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
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
				while(type->kind== ARRAY){
					printf("[%d]offset:%d ",type->array.size,type->array.offset);
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

