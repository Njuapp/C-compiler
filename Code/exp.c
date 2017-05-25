#include "semantic.h"
#include "symtable.h"
#include "intercode.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define OTHER_CASES if(parent->isBoolOrValue){ \
			assert(parent->label_true && parent->label_false); \
			Operand op1, op2, op3, op4; \
			op1 = parent->place; \
			if(is_constant(op1->kind)){ \
				assert(op1->kind == CONSTANT_INT); \
				op1 = (op1->intValue != 0)?parent->label_true:parent->label_false; \
				InterCode code = new_intercode(iGOTO); \
				code->operate1.op = op1; \
				addCode(code, context); \
			} \
			else{ \
				op2 = new_operand(oRELOP, 0, 0.0, "!="); \
				op3 = new_operand(CONSTANT_INT, 0, 0.0, NULL); \
				op4 = parent->label_true; \
				InterCode code = new_intercode(iREGOTO); \
				code->operate4.op1 = op1; \
				code->operate4.op2 = op2; \
				code->operate4.op3 = op3; \
				code->operate4.op4 = op4; \
				addCode(code, context); \
				op1 = parent->label_false; \
				code = new_intercode(iGOTO); \
				code->operate1.op = op1; \
				addCode(code, context); \
			} \
		}


make_helper(ExpASSIGNExp){
	switch(location){
		case 1:
		if(inh){
			node->leftside = 1;
		}
		else{
			if(!node->isLeft)
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",node->line);
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;
		}
		break;
		case 2:
		break;
		case 3:
		if(inh){
			node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
			return;
		}
		if(!typeEqual(parent->typeinfo, node->typeinfo))
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",node->line);

		//intercode
		InterCode code = NULL;
		if(parent->place->kind != ADDRESS)
			code = new_intercode(iASSIGN);
		else
			code = new_intercode(iPOST);
		code->operate2.op1 = parent->place;
		code->operate2.op2 = node->place;
		addCode(code, context);
		
		OTHER_CASES
		break;
		default:
		assert(0);
	}
}

make_helper(ExpAND){ // exp for bool
	Operand label = NULL;
	switch(location){
		case 1:
			if(inh){
				//intercode
				if(!parent->isBoolOrValue){
					Operand label1 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_true = label1;
					Operand label2 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_false = label2;
					Operand op1 = parent->place;
					Operand op2 = new_operand(CONSTANT_INT, 0, 0.0, NULL);
					InterCode code = new_intercode(iASSIGN);
					code->operate2.op1 = op1;
					code->operate2.op2 = op2;
					addCode(code, context);
				}
				label = new_operand(oLABEL, 0, 0.0, new_label());
				node->label_true = label;
				node->label_false = parent->label_false;
				node->isBoolOrValue = 1;
				parent->label3 = label;
				return;
			}
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			break;
		case 2:
			if(!inh){
				Operand op = parent->label3;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op;
				addCode(code, context);		
			}
			break;
		case 3:
			if(inh){
				//intercode
				node->label_true = parent->label_true;
				node->label_false = parent->label_false;
				node->isBoolOrValue = 1;
			   	return;
			}
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = node->typeinfo;
			
			//intercode
			if(!parent->isBoolOrValue){
				Operand op1 = parent->label_true;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
				
				op1 = parent->place;
				Operand op2 = new_operand(CONSTANT_INT, 1, 0.0, NULL);
				code = new_intercode(iASSIGN);
				code->operate2.op1 = op1;
				code->operate2.op2 = op2;
				addCode(code, context);

				op1 = parent->label_false;
				code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
			}
			break;

		default:
			assert(0);
	}
}

make_helper(ExpOR){ // exp for bool
	Operand label = NULL;
	switch(location){
		case 1:
			if(inh){
				//intercode
				if(!parent->isBoolOrValue){
					Operand label1 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_true = label1;
					Operand label2 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_false = label2;
					Operand op1 = parent->place;
					Operand op2 = new_operand(CONSTANT_INT, 0, 0.0, NULL);
					InterCode code = new_intercode(iASSIGN);
					code->operate2.op1 = op1;
					code->operate2.op2 = op2;
					addCode(code, context);
				}
				label = new_operand(oLABEL, 0, 0.0, new_label());
				node->label_true = parent->label_true;
				node->label_false = label;
				node->isBoolOrValue = 1;
				parent->label3 = label;
				return;
			}
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			break;
		case 2:
			if(!inh){
				Operand op = parent->label3;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op;
				addCode(code, context);		
			}
			break;
		case 3:
			if(inh){
				//intercode
				node->label_true = parent->label_true;
				node->label_false = parent->label_false;
				node->isBoolOrValue = 1;
			   	return;
			}
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = node->typeinfo;
			
			//intercode
			if(!parent->isBoolOrValue){
				Operand op1 = parent->label_true;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
				
				op1 = parent->place;
				Operand op2 = new_operand(CONSTANT_INT, 1, 0.0, NULL);
				code = new_intercode(iASSIGN);
				code->operate2.op1 = op1;
				code->operate2.op2 = op2;
				addCode(code, context);

				op1 = parent->label_false;
				code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
			}
			break;

		default:
			assert(0);
	}
}



make_helper(ExpRELOP){//Exp for relational operation such as <,>,=,etc.
	Operand t1 = NULL;
	Operand t2 = NULL;
	Operand relop = NULL;
	switch(location){
		case 1:
			if(inh){
				//intercode
				if(!parent->isBoolOrValue){
					Operand label1 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_true = label1;
					Operand label2 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_false = label2;
					Operand op1 = parent->place;
					Operand op2 = new_operand(CONSTANT_INT, 0, 0.0, NULL);
					InterCode code = new_intercode(iASSIGN);
					code->operate2.op1 = op1;
					code->operate2.op2 = op2;
					addCode(code, context);
				}
				t1 = new_operand(VARIABLE, 0, 0.0, new_temp());
				node->place = t1;
				parent->t1 = t1;
				return;
			}
			parent->typeinfo = node->typeinfo;
		
			//intercode
			parent->t1 = node->place;
			break;
		case 2:
			if(!inh){
				//intercode
				parent->relop = new_operand(oRELOP, 0, 0.0, node->idtype);		
			}
			break;
		case 3:
			if(inh){
				//intercode
				t2 = new_operand(VARIABLE, 0, 0.0, new_temp());
				node->place = t2;
				parent->t2 = t2;
			   	return;
			}
			struct Type* ltype = parent->typeinfo;
			struct Type* rtype = node->typeinfo;
			if(ltype->kind != BASIC || rtype->kind!= BASIC || ltype->basic != rtype->basic)
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = findType("int");

			//intercode
			parent->t2 = node->place;
			if(is_constant(parent->t1->kind) && is_constant(parent->t2->kind)){
				assert(parent->t1->kind == CONSTANT_INT && parent->t2->kind == CONSTANT_INT);
				int v1 = parent->t1->intValue;
				int v2 = parent->t2->intValue;
				int ret = 0;
				char* str = parent->relop->relop;
				if(!strcmp(str, "=="))
					ret = (v1==v2);
				else if(!strcmp(str, "!="))
					ret = (v1!=v2);
				else if(!strcmp(str, "<"))
					ret = (v1<v2);
				else if(!strcmp(str, ">="))
					ret = (v1>=v2);
				else if(!strcmp(str, ">"))
					ret = (v1>v2);
				else if(!strcmp(str, "<="))
					ret = (v1<=v2);
				else
					assert(0);
				InterCode code = new_intercode(iGOTO);
				code->operate1.op = (ret)?parent->label_true:parent->label_false;
				addCode(code, context);
			}else{
				InterCode code = new_intercode(iREGOTO);
				code->operate4.op1 = parent->t1;
				code->operate4.op2 = parent->relop;
				code->operate4.op3 = parent->t2;
				code->operate4.op4 = parent->label_true;
				addCode(code, context);
			}

			InterCode code = new_intercode(iGOTO);
			code->operate1.op = parent->label_false;
			addCode(code, context);
			if(!parent->isBoolOrValue){
				Operand op1 = parent->label_true;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
				
				op1 = parent->place;
				Operand op2 = new_operand(CONSTANT_INT, 1, 0.0, NULL);
				code = new_intercode(iASSIGN);
				code->operate2.op1 = op1;
				code->operate2.op2 = op2;
				addCode(code, context);

				op1 = parent->label_false;
				code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
			}
			break;
		default:
			assert(0);
	}	
}

make_helper(ExpPLUS){ // exp for plus minus star div 
	switch(location){
		case 1:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;
			
			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			struct Type* ltype = parent->typeinfo;
			struct Type* rtype = node->typeinfo;
			if(ltype->kind!=BASIC || !typeEqual(ltype, rtype))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			//intercode
			char* temp = new_temp();
			Operand op1 = new_operand(VARIABLE, 0, 0.0, temp);
			Operand op2 = parent->place;
			Operand op3 = node->place;
			INIT_3_OP(iADD)
			parent->place = op1;

			OTHER_CASES
			break;
		default: 
			assert(0);
	}
}

make_helper(ExpMINUS){ // exp for plus minus star div
	switch(location){
		case 1:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;
			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			struct Type* ltype = parent->typeinfo;
			struct Type* rtype = node->typeinfo;
			if(ltype->kind!=BASIC || !typeEqual(ltype, rtype))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			
			//intercode
			char* temp = new_temp();
			Operand op1 = new_operand(VARIABLE, 0, 0.0, temp);
			Operand op2 = parent->place;
			Operand op3 = node->place;
			INIT_3_OP(iSUB)
			parent->place = op1;

			OTHER_CASES
			break;
		default: 
			assert(0);
	}
}

make_helper(ExpSTAR){ // exp for plus minus star div
	switch(location){
		case 1:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;
			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			struct Type* ltype = parent->typeinfo;
			struct Type* rtype = node->typeinfo;
			if(ltype->kind!=BASIC || !typeEqual(ltype, rtype))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			//intercode
			char* temp = new_temp();
			Operand op1 = new_operand(VARIABLE, 0, 0.0, temp);
			Operand op2 = parent->place;
			Operand op3 = node->place;
			INIT_3_OP(iMUL)
			parent->place = op1;

			OTHER_CASES
			break;
		default: 
			assert(0);
	}
}

make_helper(ExpDIV){ // exp for plus minus star div
	switch(location){
		case 1:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;
			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
				return;
			}
			struct Type* ltype = parent->typeinfo;
			struct Type* rtype = node->typeinfo;
			if(ltype->kind!=BASIC || !typeEqual(ltype, rtype))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			//intercode
			char* temp = new_temp();
			Operand op1 = new_operand(VARIABLE, 0, 0.0, temp);
			Operand op2 = parent->place;
			Operand op3 = node->place;
			INIT_3_OP(iDIV)
			parent->place = op1;

			OTHER_CASES
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
			if(inh){
				node->isBoolOrValue = parent->isBoolOrValue;
				node->label_true = parent->label_true;
				node->label_false = parent->label_false;
				node->place = parent->place;
				return;
			}
			parent->typeinfo = node->typeinfo;
			parent->place = node->place;

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
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
			   	return;
			}
			if(node->typeinfo->kind != BASIC)
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			parent->typeinfo = node->typeinfo;
			Operand op1 = new_operand(VARIABLE, 0, 0.0, new_temp());
			Operand op2 = new_operand(CONSTANT_INT, 0, 0.0, NULL);
			Operand op3 = node->place;
			INIT_3_OP(iSUB)
			parent->place = op1;

			OTHER_CASES
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
			if(inh){
				//intercode
				if(!parent->isBoolOrValue){
					Operand label1 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_true = label1;
					Operand label2 = new_operand(oLABEL, 0, 0.0, new_label());
					parent->label_false = label2;
					Operand op1 = parent->place;
					Operand op2 = new_operand(CONSTANT_INT, 0, 0.0, NULL);
					InterCode code = new_intercode(iASSIGN);
					code->operate2.op1 = op1;
					code->operate2.op2 = op2;
					addCode(code, context);
				}
				node->label_true = parent->label_false;
				node->label_false = parent->label_true;
				node->isBoolOrValue = 1;
				return;
			}

			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			else
				parent->typeinfo = node->typeinfo;

			//intercode
			if(!parent->isBoolOrValue){
				Operand op1 = parent->label_true;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
				
				op1 = parent->place;
				Operand op2 = new_operand(CONSTANT_INT, 1, 0.0, NULL);
				code = new_intercode(iASSIGN);
				code->operate2.op1 = op1;
				code->operate2.op2 = op2;
				addCode(code, context);

				op1 = parent->label_false;
				code = new_intercode(iLABEL);
				code->operate1.op = op1;
				addCode(code, context);
			}
			break;
		default:
			break;
	}
}

make_helper(ExpFunc1){
	switch(location){
		case 1:
			if(!inh){
				char* name = node->idtype;
				if(findVar(name) || findType(name)){
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
			if(inh){
				node->paramlist = NULL;
				return;
			}
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


			//Intercode
			if(!strcmp(name,"write")){
				Operand op1 = node->paramlist->param;
				INIT_1_OP(iWRITE)
				parent->place = op1;
				OTHER_CASES
				return;
			}
			Operands q = node->paramlist;
			for(; q; q = q->next){
				Operand op1 = q->param;
				INIT_1_OP(iARG)
			}
			Operand op1 = new_operand(VARIABLE, 0, 0.0, new_temp());
			Operand op2 = new_operand(FUNC_NAME, 0, 0.0, name);
			INIT_2_OP(iCALL)
			parent->place = op1;

			OTHER_CASES
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

			//Inter code
			if(!strcmp(name, "read")){
				Operand op1 = new_operand(VARIABLE,0,0.0,new_temp());
				INIT_1_OP(iREAD);
				parent->place = op1;
				OTHER_CASES
				return;
			}
			Operand op1 = new_operand(VARIABLE, 0, 0.0, new_temp());
			Operand op2 = new_operand(FUNC_NAME, 0, 0.0, name);
			INIT_2_OP(iCALL)
			parent->place = op1;

			OTHER_CASES
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
				parent->place = node->place;
			}
			break;
		case 2:
			break;
		case 3:
			if(inh){
				node->place = new_operand(VARIABLE, 0, 0, new_temp());
			}
			else{
				struct Type* INT = findType("int");
				if(!typeEqual(node->typeinfo, INT))
					printf("Error type 12 at Line %d: The exp is not a integer.\n",node->line);
				Operand op1 = NULL;
				Operand op2 = node->place;
				Operand op3 = NULL;
				if(is_constant(op2->kind)){
					assert(op2->kind == CONSTANT_INT);
					op1 = new_operand(CONSTANT_INT, op2->intValue * 4, 0, NULL);
				}
				else
				{
					op1 = new_operand(VARIABLE, 0, 0, new_temp());
					if(parent->typeinfo->kind == ARRAY)
						op3 = new_operand(CONSTANT_INT, parent->typeinfo->array.offset, 0,NULL); 
					else
						op3 = new_operand(CONSTANT_INT, 4, 0, NULL);
					INIT_3_OP(iMUL)
				}
				op3 = op1;
				if(is_constant(op3->kind) && op3->intValue == 0){
					op1 = parent->place;
				}
				else{
					op1 = new_operand(ADDRESS, 0, 0, new_temp());
					InterCode code2 = new_intercode(iADD);
					code2->operate3.op1 = op1;
					code2->operate3.op2 = parent->place;
					code2->operate3.op3 = op3;
					addCode(code2, context);
					parent->place = op1;
				}
				if(parent->typeinfo->kind != ARRAY && parent->leftside == 0){
					InterCode code3 = new_intercode(iGET);
					code3->operate2.op1 = new_operand(VARIABLE, 0, 0, new_temp());
					code3->operate2.op2 = parent->place;
					addCode(code3, context);
					parent->place = code3->operate2.op1;
				}
			}

			OTHER_CASES;
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

		//intercode
		if(parent->place) free(parent->place);
		if(var->type->kind == ARRAY && var->temp_name->kind == VARIABLE){
			Operand op1 = new_operand(ADDRESS, 0, 0, new_temp());
			Operand op2 = var->temp_name;
			INIT_2_OP(iADDRESS)
			parent->place = op1;
		}
		else{
			parent->place = var->temp_name;
		}
		/*
		if(op1){
			Operand op2 = var->temp_name;
			InterCode code = new_intercode(iASSIGN);
			code->operate2.op1 = op1;
			code->operate2.op2 = op2;
			addCode(code, context);
		}
		*/
		OTHER_CASES;
	}
}

make_helper(ExpINT){
	assert(location == 1);
	if(!inh){
		parent->typeinfo = findType("int");
	}
	else{
		//intercode
		if(parent->place) free(parent->place);
		parent->place = new_operand(CONSTANT_INT, node->intgr, 0.0, NULL);

		OTHER_CASES;
	}
}

make_helper(ExpFLOAT){
	assert(location == 1);
	if(!inh){
		parent->typeinfo = findType("float");
	}
	else{
		//intercode
		if(parent->place) free(parent->place);
		parent->place = new_operand(CONSTANT_FLOAT, 0, node->flt, NULL);
	
		OTHER_CASES;
	}
}

