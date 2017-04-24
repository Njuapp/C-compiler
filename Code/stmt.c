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
}

make_helper(Stmt3){
	switch(location){
		case 1:
			break;
		case 2:
			if(!inh){
				assert(parent->funcname);
				struct Func* func = findFunc(parent->funcname);
				if(!typeEqual(node->typeinfo, func->rettype))
					printf("Error type 8 at Line %d: Type dismatched for return.\n",node->line);
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
			if(inh) return;
			if(!typeEqual(node->typeinfo, findType("int")))
				printf("Error type 7 at Line %d: Type dismatched for operands.\n",node->line);
			break;
		case 4:
			break;
		case 5:
			if(inh)
				node->funcname = parent->funcname;
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

