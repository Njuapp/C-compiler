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
			target->u.floatValue = floatValue;
			break;
		case CONSTANT_INT:
			target->u.intValue = intValue;
			break;
		case VARIABLE:
			target->u.var = name;
			break;
		case FUNC_NAME:
			target->u.func_name = name;
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
	else if(kind >= OP3_OFF)
		return 3;
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

void print_intercode(){
	printf("start to print code\n");
	InterCodes p = codeField->next;
	while(p){
		InterCode code = p->code;
		char text[CODE_LENGTH];
		Operand op1, op2, op3;
		switch(code->kind){
			case iFUNC:
				op1 = code->u.operate1.op;
				assert(op1->kind == FUNC_NAME);
				sprintf(text, "FUNCTION %s :\n", op1->u.func_name);			
				break;
			case iPARAM:
				op1 = code->u.operate1.op;
				assert(op1->kind == VARIABLE);
				sprintf(text, "PARAM %s\n", op1->u.var);
				break;
			default:
				printf("Print wrong type %d\n", code->kind);
				assert(0);
		}
		printf("%s", text);
		p = p->next;
	}
}
