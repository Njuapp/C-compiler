#include "semantic.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
make_helper(StmtList1){
	switch(location){
		case 1:
			if(inh){
				node->funcname = parent->funcname;
			}
			break;
		case 2:
			if(inh){
				node->funcname = parent->funcname;
			}
			break;
	}
}

make_helper(Stmt2){
	assert(location == 1);
	if(inh){
		node->funcname = parent->funcname;
	}
	else{
		//TODO: some optimize ?
	}
}

make_helper(Stmt3){
	switch(location){
		case 1:
			break;
		case 2:
			if(inh){
				//intercode
				node->place = new_operand(VARIABLE, 0, 0.0, new_temp());
			}
			else{
				assert(parent->funcname);
				struct Func* func = findFunc(parent->funcname);
				if(!typeEqual(node->typeinfo, func->rettype))
					printf("Error type 8 at Line %d: Type dismatched for return.\n",node->line);
			
				//intercode
				Operand op = node->place;
				InterCode code = new_intercode(iRETURN);
				code->operate1.op = op;
				addCode(code, context);
			}
			break;
		case 3:
			break;
	}
}

make_helper(Stmt4){
	switch(location){
		case 1:
			break;
		case 2:
			break;
		case 3:
			if(inh) {
				//intercode
				Operand label1 = new_operand(oLABEL, 0, 0.0, new_label());
				Operand label2 = new_operand(oLABEL, 0, 0.0, new_label());
				node->label_true = label1;
				node->label_false = label2;
				node->isBoolOrValue = 1;
				parent->label_true = label1;
				parent->label_false = label2;
			   	return;
			}
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);

			//intercode
			Operand op = parent->label_true;
			InterCode code = new_intercode(iLABEL);
			code->operate1.op = op;
			addCode(code, context);

			break;
		case 4:
			break;
		case 5:
			if(inh)
				node->funcname = parent->funcname;
			else{
				//intercode
				Operand op = parent->label_false;
				InterCode code = new_intercode(iLABEL);
				code->operate1.op = op;
				addCode(code, context);
			}
			break;
	}
}

make_helper(Stmt5){
	switch(location){
		case 1:case 2:case 4:case 6:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 5:case 7:
			if(inh)
				node->funcname = parent->funcname;
	}
}

make_helper(Stmt6){
	switch(location){
		case 1:case 2:case 4:
			break;
		case 3:
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 5:
			if(inh)
				node->funcname = parent->funcname;
	}
}


