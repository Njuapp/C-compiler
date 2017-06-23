#include "assemble.h"
#define ASSEMBLE_LENGTH 100
#define ASSEMBLE_LENGTH_EXT 1000

char* get_str(Operand op);

char *warp_assemble(char *str){
	char *result = (char*)malloc(sizeof(char)*ASSEMBLE_LENGTH);
	sprintf(result, "\t%s\n", str);
	return result;
}

struct varName {
	char *name;
	struct varName* next;
};

void define_var(FILE *f){
	InterCodes p = codeField->next;
	struct varName *root = (struct varName*)malloc(sizeof(struct varName));
	root->next = NULL;
	while(p){
		InterCode code = p->code;
		char *name = NULL;
		switch(code->kind){
			case iPARAM:case iREAD:
				name = code->operate1.op->var;
				break;
			case iASSIGN:case iCALL:case iADDRESS:case iGET:
				name = code->operate2.op1->var;
				break;
			case iADD:case iSUB:case iMUL:case iDIV:
				name = code->operate3.op1->var;
				break;
			case iDEC:
				fprintf(f, "%s: .space %d\n", code->operate2.op1->var, code->operate2.op2->intValue);
				break;
			default:
				break;
		}
		if(name){
			struct varName* pv = root;
			while(pv->next){
				if(!strcmp(name, pv->next->name)){
					name = NULL;
					break;
				}
				pv = pv->next;
			}
			if(name){
				pv->next = (struct varName*)malloc(sizeof(struct varName));
				pv->next->next = NULL;
				pv->next->name = name;
				fprintf(f, "%s: .word 0\n", name);
			}
		}
		p = p->next;
	}
}

struct Inference {
	char* name;
	int inReg;
	char* reg;
	char* mem;
};

char* get_var(Operand op, int reg){
	if(op->kind == CONSTANT_FLOAT || op->kind == FUNC_NAME)
		assert(0);
	char sreg[20];
	sprintf(sreg, "$t%d", reg);
	char result[ASSEMBLE_LENGTH];
	//TODO:ADDRESS
	if(op->kind == CONSTANT_INT){
		sprintf(result, "li %s, %d", sreg, op->intValue);
	}
	else if(op->kind == VARIABLE){
		sprintf(result, "lw %s, %s", sreg, op->var);
	}
	return warp_assemble(result);
}

char* store_var(Operand op) {
	if (op->kind == CONSTANT_FLOAT || op->kind == CONSTANT_INT || op->kind == FUNC_NAME)
		assert(0);
	//TODO:ADDRESS
	char result[ASSEMBLE_LENGTH];
	sprintf(result, "sw $t1, %s", op->var);
	return warp_assemble(result);
}

char *intercodeToAssemble(InterCode code) {
	char *text = malloc(sizeof(char)*CODE_LENGTH);
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
	printf("intercode kind: %d\n", code->kind);
	switch(code->kind){
		case iLABEL:
			sprintf(text, "%s:\n", op1->label);
			break;
		case iFUNC:
			sprintf(text, "%s:\n", op1->func_name);
			break;
		case iGOTO:
			sprintf(text, "j %s", op1->label);
			text = warp_assemble(text);
			break;
		case iRETURN:
			sprintf(text, "%s%s%s\n", get_var(op1, 1), warp_assemble("move $v0, $t1"), warp_assemble("jr $ra"));
			free(t1);
			break;
		case iPARAM:
			sprintf(text, "PARAM %s\n", op1->var);
			break;
		case iASSIGN:
			sprintf(text, "%s%s", get_var(op2, 1), store_var(op1));
			break;
		case iADDRESS:
			sprintf(text, "%s := &%s\n", op1->addr, op2->var);
			break; 
		case iGET:
			sprintf(text, "%s := *%s\n",op1->var, get_str(op2));
			break;
		case iPOST:
			sprintf(text, "*%s := %s\n",op1->addr, get_str(op2));
			break;
		case iWRITE:
			sprintf(text, "%s%s%s%s%s%s%s", get_var(op1, 1), warp_assemble("move $a0, $t1"), warp_assemble("addi $sp, $sp, -4"), warp_assemble("sw $ra, 0($sp)"), warp_assemble("jal write"), warp_assemble("lw $ra, 0($sp)"), warp_assemble("addi $sp, $sp, 4"));
			break;
		case iREAD:
			sprintf(text, "%s%s%s%s%s%s%s", warp_assemble("addi $sp, $sp, -4"), warp_assemble("sw $ra, 0($sp)"), warp_assemble("jal read"), warp_assemble("lw $ra, 0($sp)"), warp_assemble("addi $sp, $sp, 4"), warp_assemble("move $t1, $v0"), store_var(op1));
			break;
		case iCALL:
			sprintf(text, "%s := CALL %s\n", op1->var, get_str(op2));
			break;
		case iDEC:
			//sprintf(text, "DEC %s %d\n", op1->var, op2->intValue);
			text = NULL;
			break;
		case iADD:
			sprintf(text, "%s%s%s%s", get_var(op2, 1), get_var(op3, 2), warp_assemble("add $t1, $t1, $t2"), store_var(op1));
			break;
		case iSUB:
			sprintf(text, "%s := %s - %s\n", op1->var , get_str(op2), get_str(op3));
			break;
		case iMUL:
			sprintf(text, "%s := %s * %s\n", op1->var , get_str(op2), get_str(op3));
			break;
		case iDIV:
			sprintf(text, "%s := %s / %s\n", op1->var , get_str(op2), get_str(op3));
			break;
		case iREGOTO:
			if(!strcmp(op2->relop, ">=")){
				t1 = "bge";
			}
			char generateLine[CODE_LENGTH];
			sprintf(generateLine, "%s $t1, $t2, %s", t1, op4->label);
			sprintf(text, "%s%s%s", get_var(op1, 1), get_var(op3, 2), warp_assemble(generateLine));
//			t1 = get_str(op1);
//			t2 = get_str(op3);
//			sprintf(text, "IF %s %s %s GOTO %s\n", t1, op2->relop, t2, op4->label);
			break;
		case iARG:
			sprintf(text, "ARG %s\n",get_str(op1));
			break;
		default:
			assert(0);
	}
	return text;
}

void generate_assemble(FILE *f){
	// data
	fprintf(f, ".data\n");
	fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(f, "_ret: .asciiz \"\\n\"\n");
	define_var(f);
	fprintf(f, ".globl main\n");
	
	// text
	fprintf(f, ".text\n");
	fprintf(f, "read:\n");
	fprintf(f, "%s", warp_assemble("li $v0, 4"));
	fprintf(f, "%s", warp_assemble("la $a0, _prompt"));
	fprintf(f, "%s", warp_assemble("syscall"));
	fprintf(f, "%s", warp_assemble("li $v0, 5"));
	fprintf(f, "%s", warp_assemble("syscall"));
	fprintf(f, "%s", warp_assemble("jr $ra"));
	fprintf(f, "\n");
	fprintf(f, "write:\n");
	fprintf(f, "%s", warp_assemble("li $v0, 1"));
	fprintf(f, "%s", warp_assemble("syscall"));
	fprintf(f, "%s", warp_assemble("li $v0, 4"));
	fprintf(f, "%s", warp_assemble("la $a0, _ret"));
	fprintf(f, "%s", warp_assemble("syscall"));
	fprintf(f, "%s", warp_assemble("move $v0, $0"));
	fprintf(f, "%s", warp_assemble("jr $ra"));
	fprintf(f, "\n");
	// code
	InterCodes p = codeField->next;
	while(p){
		InterCode code = p->code;
		char *text = intercodeToAssemble(code);
		if(!text){
			p = p -> next;
			continue;
		}
		if(f)
			fprintf(f, "%s", text);
		else
			printf("%s", text);
		free(text);
		p = p->next;
	}
}

