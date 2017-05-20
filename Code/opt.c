#include "intercode.h"

Operand* get_operand(InterCode code, int num){
	Operand* re = (Operand*)malloc(sizeof(Operand)*num);
	re[0] = code->operate1.op;
	if(num >= 2)
		re[1] = code->operate2.op2;
	if(num >= 3)
		re[2] = code->operate3.op3;
	if(num >= 4)
		re[3] = code->operate4.op4;
	return re;
}

void free_operand(Operand op){
	if(!is_constant(op->kind) && op->kind != oRELOP)
		free(op->var);
	free(op);
}

void free_intercode(InterCode code){
	int num = op_num(code->kind);
	Operand* ops = get_operand(code, num);
	for(int i = 0; i < num; i++)
		free_operand(ops[i]);
	free(ops);
	free(code);
}

void free_intercodes(InterCodes cds){
	free_intercode(cds->code);
	free(cds);
}

int opEqual(Operand op1, Operand op2){
	if(op1->kind == op2->kind && op1->intValue == op2->intValue)
		return 1;
	return 0;
}

int icEqual(InterCode code1, InterCode code2){
	if(code1->kind == code2->kind){
		int num = op_num(code1->kind);
		switch(num){
			case 1:
				if(opEqual(code1->operate1.op, code2->operate1.op))
					return 1;
				return 0;
			case 2:
				if(opEqual(code1->operate2.op1, code2->operate2.op1)
						&& opEqual(code1->operate2.op2, code2->operate2.op2))
					return 1;
				return 0;
			case 3:
				if(opEqual(code1->operate3.op1, code2->operate3.op1)
						&& opEqual(code1->operate3.op2, code2->operate3.op2)
						&& opEqual(code1->operate3.op3, code2->operate3.op3))
					return 1;
				return 0;
			case 4:
				if(opEqual(code1->operate4.op1, code2->operate4.op1)
						&& opEqual(code1->operate4.op2, code2->operate4.op2)
						&& opEqual(code1->operate4.op3, code2->operate4.op3)
						&& opEqual(code1->operate4.op4, code2->operate4.op4))
					return 1;
				return 0;
			default:
				return 0;
		}
	}
}

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
	label_list[index++]="label7";
	label_list[index++]="label8";
	label_list[index++]="label7";
	qsort(label_list, index, sizeof(char*), mystrcmp);
	
	for(int i = 0; i + 1 < index; i++)
	{
		while(i + 1 < index && !strcmp(label_list[i], label_list[i + 1])){
			for(int j = i + 1; j + 1 < index ;j++)
				label_list[j] = label_list[j + 1];
			index--;
		}
	}
	for(int i = 0; i< index; i++)
		;//	printf("%s\n", label_list[i]);
	
	p = start;
	while(p != end){
		InterCode code = p->code;
		if(code->kind == iLABEL){
			char *label = code->operate1.op->label;
			if(!bsearch(&label, label_list, index, sizeof(char*), mystrcmp)){
				p->next->prev = p->prev;
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

void interOptimize(){
	InterCodes root = codeField;
	controlOptimize(root->next, NULL);
}

