#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "grammar_tree.h"
int i;
struct GrammerTree * create(char* name, int num, ...){
	va_list valist;
	struct GrammerTree * a = (struct GrammerTree*)malloc(sizeof(struct GrammerTree));
	if(!a){
		yyerror("space allocation failed.\n");
		exit(0);
	}
	a->name = name;
	va_start(valist, num);
	struct GrammerTree * temp = (struct GrammerTree*)malloc(sizeof(struct GrammerTree));
	if(num>0){
		temp = va_arg(valist, struct GrammerTree*);
		a->l = temp;
		a->line = temp->line;
		if(num>=2){
			for(i=0;i<num-1;i++){
				temp->r=va_arg(valist,struct GrammerTree*);
				temp = temp->r;
			}
		}
	}
	else{
		int t = va_arg(valist, int);
		a->line = t;
		a->l = a->r = NULL;
		if( (!strcmp(a->name,"ID")) || (!strcmp(a->name,"TYPE"))){
			char* t = (char*)malloc(sizeof(char*)*40);
			strcpy(t,yytext);
			a->idtype = t;
		}
		else if (!strcmp(a->name, "INT"))
			a->intgr = atoi(yytext);
		else if (!strcmp(a->name, "FLOAT"))
			a->flt = atof(yytext);
	}
	return a;
}
void eval(struct GrammerTree*a, int level){
	if(!a||a->line==-1)return;
	for(int i = 0; i < level; i++)
		printf("  ");
	if(a->line!=-1){
		printf("%s ",a->name);
		if((!strcmp(a->name,"ID"))||(!strcmp(a->name,"TYPE")))
			printf(":%s ",a->idtype);
		else if(!strcmp(a->name,"INT"))
			printf(":%d ",a->intgr);
		else if(!strcmp(a->name,"FLOAT"))
			printf(":%.6f ",a->flt);
		else if(a->l!=NULL)
			printf("(%d) ",a->line);
		printf("\n");
	}
	eval(a->l, level+1);
	eval(a->r, level  );
}
void yyerror(char*s, ...){
	va_list ap;
	va_start(ap,s);
	fprintf(stderr,"Error type B at Line %d:",yylineno);
	vfprintf(stderr,s,ap);
	fprintf(stderr,"\n");
}
