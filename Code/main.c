#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int i, totchars = 0, totwords = 0,totlines = 0;
	if (argc < 2) {
		yylex();
		return 0;
	}
	for (i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if(!f) {
			perror(argv[i]);
			return 1;
		}
		yyrestart(f);
		yylex();
		fclose(f);
	}
	if(argc > 1)
		;//printf("%8d%8d%8d total\n", totlines, totwords, totchars);
	return 0;
}
