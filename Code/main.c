#include <stdio.h>
#include <stdlib.h>
#include "grammar_tree.h"
#include "assemble.h"
int yyparse();
void yyrestart(FILE* f);
extern int yylineno;
int main(int argc, char** argv) {
	if(argc <= 2) return 1;
	int i;
	FILE *f = fopen(argv[1], "r");
	if(!f) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yylineno = 1;
	yyparse();
	//eval(root,0);
	fclose(f);

	f = fopen(argv[2], "w+");
	print_intercode(f);
	fprintf(f,"\n\n\n");
	generate_assemble(f);	
	fclose(f);
	return 0;
}

