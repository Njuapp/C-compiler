#include "intercode.h"

InterCodes find_next(InterCode target, InterCodes start, InterCodes end){
	while(start != end){
		InterCode code = start->code;
		if(icEqual(code, target)){
			return start;
		}
	}
	return NULL;
}

Operand oppo_relop(Operand relop){
	if(relop->kind != oRELOP) return NULL;
	char* str = relop->relop;
	if(!strcmp(str, "=="))
		relop->relop = "!=";
	else if(!strcmp(str, "!="))
		relop->relop = "==";
	else if(!strcmp(str, "<"))
		relop->relop = ">=";
	else if(!strcmp(str, ">="))
		relop->relop = "<";
	else if(!strcmp(str, ">"))
		relop->relop = "<=";
	else if(!strcmp(str, "<="))
		relop->relop = ">";
	return relop;
}

int mystrcmp(const void* a, const void* b){
	return strcmp(*(char**)a, *(char**)b);
}

void controlOptimize(InterCodes start, InterCodes end){
	InterCodes p = start;
	while(p != end){
		InterCode code = p->code;
		if(code->kind == iREGOTO && p->next){
			InterCode ncode = p->next->code;
			Operand target;
			while(ncode->kind == iGOTO){
				code->operate4.op4 = ncode->operate1.op;
				oppo_relop(code->operate2.op2);
				p->next = p->next->next;
				p->next->prev = p;
				free(ncode);
				if(!p->next)
					break;
				ncode = p->next->code;
			}
		}
		p = p->next;
	}
	
	char* label_list[LABEL_MAX];
	int index = 0;
	p = start;
	while(p != end){
		InterCode code = p->code;
		if(code->kind == iREGOTO){
			label_list[index++] = code->operate4.op4->label;
		}
		else if(code->kind == iGOTO){
			label_list[index++] = code->operate1.op->label;
		}
		p = p->next;
	}
	qsort(label_list, index, sizeof(char*), mystrcmp);
	
	for(int i = 0; i + 1 < index; i++)
	{
		while(i + 1 < index && !strcmp(label_list[i], label_list[i + 1])){
			for(int j = i + 1; j + 1 < index ;j++)
				label_list[j] = label_list[j + 1];
			index--;
		}
	}
	
	p = start;
	while(p != end){
		InterCode code = p->code;
		if(code->kind == iLABEL){
			char *label = code->operate1.op->label;
			if(!bsearch(&label, label_list, index, sizeof(char*), mystrcmp)){
				if(p->next != end){
					p->next->prev = p->prev;
				}
				p->prev->next = p->next;
				InterCodes pp = p;
				p = p->prev;
				free_intercodes(pp);
			}
		}
		p = p->next;
	}
}

#define get_value(op) ((op->kind==CONSTANT_FLOAT)?op->floatValue:op->intValue)
void arithOptimize(InterCodes start, InterCodes end){
	InterCodes p = start;
	while(p != end){
		InterCode code = p->code;
		Operand sub = NULL;
		Operand target = NULL;
		Operand op1, op2, op3;
		switch(code->kind){
			case iASSIGN:
				op2 = code->operate2.op2;
				if(is_constant(op2->kind)){
					sub = code->operate2.op1;
					target = op2;
				}
				break;
			case iADD:
				op2 = code->operate3.op2;
				op3 = code->operate3.op3;
				if(is_constant(op2->kind) && is_constant(op3->kind)){
					sub = code->operate3.op1;
					if(op2->kind == CONSTANT_FLOAT || op3->kind == CONSTANT_FLOAT)
					{
						target = new_operand(CONSTANT_FLOAT, 0, get_value(op2) + get_value(op3), NULL);
					}
				}
				break;

			case iSUB:
				op2 = code->operate3.op2;
				op3 = code->operate3.op3;
				if(is_constant(op2->kind) && is_constant(op3->kind)){
					sub = code->operate3.op1;
					if(op2->kind == CONSTANT_FLOAT || op3->kind == CONSTANT_FLOAT)
					{
						target = new_operand(CONSTANT_FLOAT, 0, get_value(op2) - get_value(op3), NULL);
					}
				}
				break;

			case iMUL:
				op2 = code->operate3.op2;
				op3 = code->operate3.op3;
				if(is_constant(op2->kind) && is_constant(op3->kind)){
					sub = code->operate3.op1;
					if(op2->kind == CONSTANT_FLOAT || op3->kind == CONSTANT_FLOAT)
					{
						target = new_operand(CONSTANT_FLOAT, 0, get_value(op2) * get_value(op3), NULL);
					}
				}
				break;

			case iDIV:
				op2 = code->operate3.op2;
				op3 = code->operate3.op3;
				if(is_constant(op2->kind) && is_constant(op3->kind)){
					sub = code->operate3.op1;
					if(op2->kind == CONSTANT_FLOAT || op3->kind == CONSTANT_FLOAT)
					{
						target = new_operand(CONSTANT_FLOAT, 0, get_value(op2) / get_value(op3), NULL);
					}
				}
				break;
			case iREGOTO:
				break;
		}
		/*
		if(!sub && !target){
			InterCodes point = p->next;
			p = p->prev;
			p->next = point;
			if(point) point->prev = p;

			while(point != end){
				InterCode code = point->code;
				switch(code->kind){
					//TODO: unknown
				}
				point = point->next;
			}
		}
		*/
		p = p->next;
	}
}


struct InterCodesList{
	InterCodes code;
	int type;
	struct InterCodesList* next;
};

struct OperandTable{
	Operand op;
	struct InterCodesList *list;
} *varTable, *labelTable;
int varIndex = 0, labelIndex = 0;

void addCodeList(struct OperandTable *pos, InterCodes code, int type){
	struct InterCodesList *newp = malloc(sizeof(struct InterCodesList));
	newp->code = code;
	newp->type = type;
	newp->next = NULL;
	if(pos->list){
		struct InterCodesList *p = pos->list;
		while(p->next)
			p = p->next;
		p->next = newp->next;	
	}
	else
		pos->list = newp;
}

// flag 0 for var, 1 for label
int findIndex(Operand op, int flag){
	struct OperandTable *table = (flag)?labelTable:varTable;
	int endIndex = (flag)?labelIndex:varIndex;
	for(int i = 0; i < endIndex; i++){
		if(opEqual(table[i].op, op))
			return i;
	}
	return endIndex;
}

void scanCode(InterCodes start, InterCodes end){
	InterCodes p = start;
	while(p != end){
		InterCode code = p->code;
		Operand op = NULL;
		int type;
		struct OperandTable *table;
		int index;
		int *endIndex;
		if(code->kind == iREGOTO){
			op = code->operate4.op4;
			type = 0;
			table = labelTable;
			index = findIndex(op, 1);
			endIndex = &labelIndex;
		}else if(code->kind == iGOTO){
			op = code->operate1.op;
			type = 0;
			table = labelTable;
			index = findIndex(op, 1);
			endIndex = &labelIndex;
		}else if(code->kind == iLABEL){
			op = code->operate1.op;
			type = 1;
			table = labelTable;
			index = findIndex(op, 1);
			endIndex = &labelIndex;
		}
		if(op != NULL){
			if(table[index].op == NULL){
				table[index].op = op;
				*endIndex++;
			}
			addCodeList(&(table[index]), p, type);
		}
		p = p->next;
	}	
}

void interOptimize(){
	InterCodes root = codeField;
	controlOptimize(root->next, NULL);
	varTable = (struct OperandTable*)malloc(sizeof(struct OperandTable)*getVarCount());
	labelTable = (struct OperandTable*)malloc(sizeof(struct OperandTable)*getLabelCount());
	scanCode(root->next, NULL);
}

