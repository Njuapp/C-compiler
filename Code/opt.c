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
				free_intercodes(pp, 1);
			}
		}
		p = p->next;
	}
}

struct InterCodesList{
	InterCodes codes;
	int type; // 0 for use, 1 for product, 2 for unoptimizable
	struct InterCodesList* next;
};

struct OperandTable{
	Operand op;
	struct InterCodesList *list;
} *varTable, *labelTable;
int varIndex = 0, labelIndex = 0;

void addCodeList(struct OperandTable *pos, InterCodes codes, int type){
	struct InterCodesList *newp = malloc(sizeof(struct InterCodesList));
	newp->codes = codes;
	newp->type = type;
	newp->next = NULL;
	if(pos->list){
		struct InterCodesList *p = pos->list;
		while(p->next)
			p = p->next;
		p->next = newp;
	}
	else{
		pos->list = newp;
	}
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

InterCodes findLabel(struct InterCodesList *list){
	struct InterCodesList *p = list;
	while(p){
		if(p->type)
			return p->codes;
		p = p->next;
	}
}

// type for use/product, flag for var/label
void addTable(Operand op, int type, int flag, InterCodes p){
	if(op->kind == VARIABLE || op->kind == oLABEL || op->kind == ADDRESS){
		struct OperandTable *point;
		int *endIndex;
   	
		int index = findIndex(op, flag); 
		if(flag){
			point = &(labelTable[index]);
			endIndex = &(labelIndex);
		}else{
			point = &(varTable[index]);
			endIndex = &(varIndex);
		}
		if(point->op == NULL){
			point->op = op;
			(*endIndex)++;
		}
		addCodeList(point, p, type);
	}
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
		switch(code->kind){
			case iREGOTO:
				op = code->operate4.op4;
				addTable(op, 0, 1, p);
				addTable(code->operate4.op1, 2, 0, p);
				addTable(code->operate4.op3, 2, 0, p);
				break;
			case iGOTO:
				op = code->operate1.op;
				addTable(op, 0, 1, p);
				break;
			case iLABEL:
				op = code->operate1.op;
				addTable(op, 1, 1, p);
				break;
			case iASSIGN:
				op = code->operate2.op1;
				addTable(op, 1, 0, p);
				op = code->operate2.op2;
				addTable(op, 0, 0, p);
				break;
			case iADD:case iSUB:case iMUL:case iDIV:
				op = code->operate3.op1;
				addTable(op, 1, 0, p);
				addTable(code->operate3.op2, 0, 0, p);
				addTable(code->operate3.op3, 0, 0, p);
				break;
			case iADDRESS:
				addTable(code->operate2.op1, 2, 0, p);
				addTable(code->operate2.op2, 2, 0, p);
				break;
			case iGET:
				addTable(code->operate2.op1, 1, 0, p);
				addTable(code->operate2.op2, 2, 0, p);
				break;
			case iPOST:
				addTable(code->operate2.op1, 2, 0, p);
				addTable(code->operate2.op2, 2, 0, p);
				break;
			case iRETURN:
				addTable(code->operate1.op, 2, 0, p);
				break;
			case iDEC:
				addTable(code->operate1.op, 1, 0, p);
				break;
			case iARG:
				addTable(code->operate1.op, 2, 0, p);
				break;
			case iCALL:
				addTable(code->operate1.op, 1, 0, p);
				break;
			case iPARAM:
				addTable(code->operate1.op, 2, 0, p);
				break;
			case iREAD:
				addTable(code->operate1.op, 2, 0, p);
				break;
			case iWRITE:
				addTable(code->operate1.op, 2, 0, p);
				break;
			default:
				break;
		}
		p = p->next;
	}	
}

void listDelete(struct OperandTable *op, InterCodes codes){
	struct InterCodesList *list = op->list;
	if(list->codes == codes){
		op->list = list->next;
		free(list);
	}
	else{
		for(struct InterCodesList *p = list; p->next; p = p->next){
			if(p->next->codes == codes){
				struct InterCodesList *pp = p->next;
				p->next = pp->next;
				free(pp);
				break;
			}
		}
	}
	if(op->op->kind == oLABEL){
		struct InterCodesList *p = op->list;
		if(p->next == NULL){
			InterCodes codes = p->codes;
			codes->prev->next = codes->next;
			if(codes->next)
				codes->next->prev = codes->prev;
			free_intercodes(codes, 1);
		}
	}

}

void labelOptimize(InterCodes start, InterCodes end){
	InterCodes p = start;
	while(p != end){
		InterCode code = p->code;
		Operand op_label = NULL;
		switch(code->kind){
			case iGOTO:
				op_label = code->operate1.op;
				break;
			case iREGOTO:
				op_label = code->operate4.op4;
				break;
			default:
				break;
		}
		if(op_label){
			int index = findIndex(op_label, 1);
			struct OperandTable *opp = &(labelTable[index]);
			struct InterCodesList *list = opp->list;
			InterCodes pos = findLabel(list);

			if(pos->prev == p){
				InterCodes pp = p;
				p->prev->next = p->next;
				p->next->prev = p->prev;
				p = p->prev;
				listDelete(opp, pp);
				free_intercodes(pp, 0);
			}else if(pos->next && pos->next->code->kind == iGOTO){
				code->operate1.op = pos->next->code->operate1.op;
				listDelete(opp, p);
				opp = &(labelTable[findIndex(code->operate1.op, 1)]);
				addCodeList(opp, p, 0);
			}
		}
		p = p->next;
	}
}

void interOptimize(){
	InterCodes root = codeField;
	controlOptimize(root->next, NULL);
	varTable = (struct OperandTable*)malloc(sizeof(struct OperandTable)*getVarCount());
	labelTable = (struct OperandTable*)malloc(sizeof(struct OperandTable)*getLabelCount());

	printf("start to scan\n");
	scanCode(root->next, NULL);

/*
	for(int i=0;i<labelIndex;i++){
		printf("%s\t", labelTable[i].op->label);
		struct InterCodesList *list = labelTable[i].list;
		for(struct InterCodesList *p = list; p!=NULL; p = p->next){
			printf("%d, %d.\t", p->type, p->codes->code->kind);
		}
		printf("\n");
	}
	for(int i=0;i<varIndex;i++){
		printf("%s\t", varTable[i].op->var);
		struct InterCodesList *list = varTable[i].list;
		for(struct InterCodesList *p = list; p!=NULL; p = p->next){
			printf("%d, %d.\t", p->type, p->codes->code->kind);
		}
		printf("\n");
	}
*/

	labelOptimize(root->next, NULL);
	
	
	printf("%d, %d\n", labelIndex, varIndex);
	for(int i=0;i<labelIndex;i++){
		if(labelTable[i].op){
			printf("%s\t", labelTable[i].op->label);
			struct InterCodesList *list = labelTable[i].list;
			for(struct InterCodesList *p = list; p!=NULL; p = p->next){
				printf("%d, %d.\t", p->type, p->codes->code->kind);
			}
		}
		printf("\n");
	}
	for(int i=0;i<varIndex;i++){
		if(varTable[i].op){
			printf("%s\t", varTable[i].op->var);
			struct InterCodesList *list = varTable[i].list;
			for(struct InterCodesList *p = list; p!=NULL; p = p->next){
				printf("%d, %d.\t", p->type, p->codes->code->kind);
			}
		}
		printf("\n");
	}
	printf("DEBUG END\n\n");

}

