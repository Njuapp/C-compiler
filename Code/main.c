#include <stdio.h>
#include <stdlib.h>
int yyparse();
void yyrestart(FILE* f);

extern int yylineno;

int main(int argc, char** argv) {
	if(argc <= 1) return 1;
	int i;
	for (i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if(!f) {
			perror(argv[i]);
			return 1;
		}
		yyrestart(f);
		yylineno = 1;
		yyparse();
		fclose(f);
	}
	return 0;
}
