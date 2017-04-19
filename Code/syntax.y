%{
#include "lex.yy.c"
#include <stdarg.h>
#include "semantic.h"
void yyerror(char* msg,...);
extern int lexerr;
extern struct GrammerTree* root;
int synerr = 0;
char *errstr = NULL;
int yydebug = 1;
%}
%union{
	struct GrammerTree* a;
}
/*%debug*/
%locations
%token LOWEST
%token <a> INT FLOAT
%token <a>ID SEMI COMMA TYPE STRUCT RETURN IF ELSE WHILE ASSIGNOP
%token <a>RELOP PLUS MINUS STAR DIV OR AND NOT DOT LP RP LB RB LC RC
%token LineB

/*priority definition*/
%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  MINUS PLUS
%left  STAR DIV
%right NOT 
%left  DOT
%left  LP RP LB RB 
%right UMINUS


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%type  <a> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag  Tag VarDec  FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args
%%

Program :ExtDefList{
		$$=create("Program",1,1,$1);
		sdt($$);
		}
	;
ExtDefList:ExtDef ExtDefList{
		$$=create("ExtDefList",2,2,$1,$2);
		}
	|{$$=create("ExtDefList",3,0,-1);}
	;
ExtDef:Specifier ExtDecList SEMI    {$$=create("ExtDef",4,3,$1,$2,$3);}    
	|Specifier SEMI	{$$=create("ExtDef",5,2,$1,$2);}
	|Specifier FunDec Compst	{$$=create("ExtDef",6,3,$1,$2,$3);}
	|Specifier error { errstr = "ExtDef missing \";\".";}
	;
ExtDecList:VarDec {$$=create("ExtDecList",7,1,$1);}
	|VarDec COMMA ExtDecList {$$=create("ExtDecList",8,3,$1,$2,$3);}
	;

/*Specifier*/
Specifier:TYPE {$$=create("Specifier",9,1,$1);}
	|StructSpecifier {$$=create("Specifier",10,1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=create("StructSpecifier",11,5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=create("StructSpecifier",12,2,$1,$2);}
	|STRUCT OptTag LC error RC {}
	;
OptTag:ID {$$=create("OptTag",13,1,$1);}
	|{$$=create("OptTag",14,0,-1);}
	;
Tag:ID {$$=create("Tag",15,1,$1);}
	;

/*Declarators*/
VarDec:ID {$$=create("VarDec",16,1,$1);}
	|VarDec LB INT RB {$$=create("VarDec",17,4,$1,$2,$3,$4);}
	|VarDec LB error RB {}
	|VarDec LB INT error {errstr="VarDec missing \"]\".";}
	;
FunDec:ID LP VarList RP {$$=create("FunDec",18,4,$1,$2,$3,$4);}
	|ID LP RP {$$=create("FunDec",19,3,$1,$2,$3);}
	|ID LP error RP {}
	|ID LP VarList error { errstr="FunDec missing \")\".";}
	;
VarList:ParamDec COMMA VarList {$$=create("VarList",20,3,$1,$2,$3);}
	|ParamDec {$$=create("VarList",21,1,$1);}
	;
ParamDec:Specifier VarDec {$$=create("ParamDec",22,2,$1,$2);}
    ;

/*Statement*/
Compst:LC DefList StmtList RC {$$=create("Compst",23,4,$1,$2,$3,$4);}
/*	|LC DefList StmtList error {errstr="Compst missing \"}\".";}*/
	;
StmtList:Stmt StmtList{$$=create("StmtList",24,2,$1,$2);}
	| {$$=create("StmtList",25,0,-1);}
	;
Stmt:Exp SEMI {$$=create("Stmt",26,2,$1,$2);}
	|error SEMI{}  
/*	|error { errstr="Stmt missing \";\"."; }*/
	|Compst {$$=create("Stmt",27,1,$1);}
	|RETURN Exp SEMI {$$=create("Stmt",28,3,$1,$2,$3);}
	|RETURN error SEMI {}
	|RETURN Exp error {errstr="Stmt missing \";\".";}
	|IF LP Exp RP Stmt {$$=create("Stmt",29,5,$1,$2,$3,$4,$5);}
	|IF LP error RP Stmt {}
	|IF LP Exp error Stmt %prec LOWER_THAN_ELSE {errstr="Stmt missing \")\".";}
	|IF LP Exp RP Stmt ELSE Stmt {$$=create("Stmt",30,7,$1,$2,$3,$4,$5,$6,$7);}
	|IF LP error RP Stmt ELSE Stmt {}
	|IF LP Exp error Stmt ELSE Stmt {errstr="Stmt missing \")\".";}
	|WHILE LP Exp RP Stmt {$$=create("Stmt",31,5,$1,$2,$3,$4,$5);}
	|WHILE LP error RP Stmt {}
	|WHILE LP Exp error Stmt {errstr="Stmt missing \")\".";}
	;

/*Local Definitions*/
DefList:Def DefList{$$=create("DefList",32,2,$1,$2);}
	| {$$=create("DefList",33,0,-1);}
	;
Def:Specifier DecList SEMI {$$=create("Def",34,3,$1,$2,$3);}
	|Specifier error SEMI {}
	;
DecList:Dec {$$=create("DecList",35,1,$1);}
	|Dec COMMA DecList {$$=create("DecList",36,3,$1,$2,$3);}
	;
Dec:VarDec {$$=create("Dec",37,1,$1);}
	|VarDec ASSIGNOP Exp {$$=create("Dec",38,3,$1,$2,$3);}
	|VarDec ASSIGNOP error {}
	;

Exp:Exp ASSIGNOP Exp{$$=create("Exp",39,3,$1,$2,$3);}
        |Exp AND Exp{$$=create("Exp",40,3,$1,$2,$3);}
        |Exp OR Exp{$$=create("Exp",41,3,$1,$2,$3);}
        |Exp RELOP Exp{$$=create("Exp",42,3,$1,$2,$3);}
        |Exp PLUS Exp{$$=create("Exp",43,3,$1,$2,$3);}
        |Exp MINUS Exp{$$=create("Exp",44,3,$1,$2,$3);}
        |Exp STAR Exp{$$=create("Exp",45,3,$1,$2,$3);}
        |Exp DIV Exp{$$=create("Exp",46,3,$1,$2,$3);}
        |LP Exp RP{$$=create("Exp",47,3,$1,$2,$3);}
		|LP error RP {}
		|LP Exp error {errstr = "Exp missing \")\".";}
		|MINUS Exp %prec UMINUS{$$=create("Exp",48,2,$1,$2);}
		|MINUS error %prec UMINUS {}
        |NOT Exp {$$=create("Exp",49,2,$1,$2);}
		|NOT error {}
        |ID LP Args RP {$$=create("Exp",50,4,$1,$2,$3,$4);}
        |ID LP RP {$$=create("Exp",51,3,$1,$2,$3);}
		|ID LP Args error { errstr = "Exp missing \")\".";}
        |Exp LB Exp RB {$$=create("Exp",52,4,$1,$2,$3,$4);}
		|Exp LB error RB {}
		|Exp LB Exp error { errstr = "Exp missing \"]\".";}
        |Exp DOT ID {$$=create("Exp",53,3,$1,$2,$3);}
        |ID {$$=create("Exp",54,1,$1);}
        |INT {$$=create("Exp",55,1,$1);}
        |FLOAT{$$=create("Exp",56,1,$1);}
/*		|error %prec LOWEST { errstr = "Invalid Expression."; }*/
        ;
Args:Exp COMMA Args {$$=create("Args",57,3,$1,$2,$3);}
		|error COMMA Args {}
        |Exp {$$=create("Args",58,1,$1);}
        ;

%%

void yyerror(char *format, ...){
	static int lastline = -1;
	if(yylineno > lastline){
		lastline = yylineno;
		synerr++;
		va_list args;
		va_start(args, format);
		fprintf(stderr, "Error type B at Line %d:", yylineno);
		if(errstr){
			fprintf(stderr,"%s", errstr);
			errstr = NULL;
		}
		else
			vfprintf(stderr, format, args);
		va_end(args);
		fprintf(stderr, "\n");
	}
}
