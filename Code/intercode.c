#include "intercode.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define STRING_LENGTH 20

InterCodes codeField, context;

Operand new_operand(int kind, int intValue, float floatValue, char* name){
	Operand target = (Operand)malloc(sizeof(struct Operand_));
	target->kind = kind;
	switch(kind){
		case CONSTANT_FLOAT:
			target->floatValue = floatValue;
			break;
		case CONSTANT_INT:
			target->intValue = intValue;
			break;
		case VARIABLE:
			target->var = name;
			break;
		case FUNC_NAME:
			target->func_name = name;
			break;
		case ADDRESS:
			target->addr = name;
			break;
		case oLABEL:
			target->label = name;
			break;
		case oRELOP:
			target->relop = name;
			break;
		default:
			assert(0);
	}
	return target;
}

InterCode new_intercode(int kind){
	InterCode target = (InterCode)malloc(sizeof(struct InterCode_));
	target->kind = kind;
	return target;
}

void intercodeInit(){
	codeField = (InterCodes)malloc(sizeof(struct InterCodes_));
	codeField->code = NULL;
	codeField->prev = NULL;
	codeField->next = NULL;
	context = codeField;
}

void addCode(InterCode code, InterCodes position){
	InterCodes t = (InterCodes)malloc(sizeof(struct InterCodes_));
	t->code = code;
	t->prev = position;
	t->next = position->next;
	position->next = t;
	if(position == context)
		context = context->next;
}

int op_num(int kind){
	if(kind >= OP1_OFF && kind < OP2_OFF)
		return 1;
	else if(kind >= OP2_OFF && kind < OP3_OFF)
		return 2;
	else if(kind >= OP3_OFF && kind < OP4_OFF)
		return 3;
	else
		return 4;
}

char* new_temp(){
	static int count = 1;
	char* target = (char*)malloc(sizeof(char)*STRING_LENGTH);
	sprintf(target, "temp%d", count++);
	return target;
}

char* new_label(){
	static int count = 1;
	char* target = (char*)malloc(sizeof(char)*STRING_LENGTH);
	sprintf(target, "label%d", count++);
	return target;
}

char* new_var(){
	static int count = 1;
	char* target = (char*)malloc(sizeof(char)*STRING_LENGTH);
	sprintf(target, "var%d", count++);
	return target;
}

#define CODE_LENGTH 50

char* get_str(Operand op){
	char *text = malloc(sizeof(char)*CODE_LENGTH);
	if(op->kind == VARIABLE)
		sprintf(text, "%s", op->var);
	else if(op->kind == CONSTANT_INT)
		sprintf(text, "#%d", op->intValue);
	else if(op->kind == CONSTANT_FLOAT)
		sprintf(text, "#%f", op->floatValue);
	else if(op->kind == FUNC_NAME)
		sprintf(text, "%s", op->func_name);
	return text;
}

void print_intercode(){
	printf("start to print code\n");
	InterCodes p = codeField->next;
	while(p){
		InterCode code = p->code;
		char text[CODE_LENGTH];
		Operand op1, op2, op3, op4;
		int op_number = op_num(code->kind);
		if(op_number == 1)
			op1 = code->operate1.op;
		else if(op_number == 2){
			op1 = code->operate2.op1;
			op2 = code->operate2.op2;
		}
		else if(op_number == 3){
			op1 = code->operate3.op1;
			op2 = code->operate3.op2;
			op3 = code->operate3.op3;
		}
		else{
			op1 = code->operate4.op1;
			op2 = code->operate4.op2;
			op3 = code->operate4.op3;
			op4 = code->operate4.op4;
		}
		char *t1 = NULL, *t2 = NULL;
		switch(code->kind){
			case iLABEL:
				assert(op1->kind == oLABEL);
				sprintf(text, "LABEL %s :\n", op1->label);
				break;
			case iFUNC:
				assert(op1->kind == FUNC_NAME);
				sprintf(text, "FUNCTION %s :\n", op1->func_name);			
				break;
			case iGOTO:
				assert(op1->kind == oLABEL);
				sprintf(text, "GOTO %s\n", op1->label);
				break;
			case iRETURN:
				assert(op1->kind == VARIABLE || is_constant(op1->kind));
				t1 = get_str(op1);
				sprintf(text, "RETURN %s\n", t1);
				free(t1);
				break;
			case iPARAM:
				assert(op1->kind == VARIABLE);
				sprintf(text, "PARAM %s\n", op1->var);
				break;
			case iASSIGN:
				assert(op1->kind == VARIABLE);
				assert(op2->kind == VARIABLE || is_constant(op2->kind));
				t1 = get_str(op2);
				sprintf(text, "%s := %s\n", op1->var, t1);
				free(t1);
				break;
			case iDEC:
				assert(op1->kind == VARIABLE && op2->kind == CONSTANT_INT);
				sprintf(text, "DEC %s %d\n", op1->var, op2->intValue * WORD_LENGTH);
				break;
			case iREGOTO:
				assert(op1->kind == VARIABLE || is_constant(op1->kind));
				assert(op3->kind == VARIABLE || is_constant(op3->kind));
				assert(op2->kind == oRELOP || op4->kind == oLABEL);
				t1 = get_str(op1);
				t2 = get_str(op3);
				sprintf(text, "IF %s %s %s GOTO %s\n", t1, op2->relop, t2, op4->label);
				break;
			default:
				printf("Print wrong type %d\n", code->kind);
				assert(0);
		}
		printf("%s", text);
		p = p->next;
	}
}
