#include "semantic.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
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

