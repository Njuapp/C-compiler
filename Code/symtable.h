#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_
#include "semantic.h"
unsigned int hash_pjw(char* name);
int arrayEqual(struct Type* left, struct Type* right);
int structEqual(struct Type* left, struct Type* right);
int typeEqual(struct Type* left, struct Type* right);
int arrayEqual(struct Type* left, struct Type* right);
int structEqual(struct Type* left, struct Type* right);
void addVar(char* varname, struct Type* typeinfo, char* tempname);
struct Var* findVar(char* varname);
struct Func* findFunc(char *funcname);
void addFunc(char* funcname, struct Type* typeinfo);
void addBasicType(char* typeName);
void addStruType(struct FieldList* stru, char* typeName);
struct Type* findType(char* typeName);
#endif
