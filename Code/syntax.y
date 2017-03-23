%{
#include "lex.yy.c"
int yyerror(char *);
%}
%union{
	int type_int;
	float type_float;
	double type_double;
}
%locations
%token <type_int> INT
%token ID
%token <type_float> FLOAT
%token SEMI
%token COMMA
%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  MINUS PLUS
%left  STAR DIV
%right NOT 
%left  DOT
%left  LP RP LB RB 
%token LC RC
%token COMM COMS
%token TYPE
%token STRUCT
%token RETURN
%token IF WHILE
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <type_double> Exp
%%
Program :ExtDefList
	;
ExtDefList:ExtDef ExtDefList
	|
	;
ExtDef	:Specifier ExtDecList SEMI
	|Specifier SEMI
	|Specifier FunDec CompSt
	;
ExtDecList:VarDec
	|VarDec COMMA ExtDecList
	;

Specifier:TYPE
	|StructSpecifier
	;
StructSpecifier:STRUCT OptTag LC DefList RC
	|STRUCT Tag
	;
OptTag	:ID
	|
	;
Tag	:ID
	;

VarDec	:ID
	|VarDec LB INT RB
	;
FunDec	:ID LP VarList RP
	|ID LP RP
	;
VarList	:ParamDec COMMA VarList
	|ParamDec
	;
ParamDec:Specifier VarDec
	;

CompSt	:LC DefList StmtList RC
	;
StmtList:Stmt StmtList
	|
	;
Stmt 	:Exp SEMI
	|CompSt
	|RETURN Exp SEMI
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	|IF LP Exp RP Stmt ELSE Stmt
	|WHILE LP Exp RP Stmt
	;

DefList	:Def DefList
	|
	;
Def	:Specifier DecList SEMI
	;
DecList	:Dec
	|Dec COMMA DecList
	;
Dec	:VarDec
	|VarDec ASSIGNOP Exp
	;
Exp	:Exp ASSIGNOP Exp {$1 = $3;}
	|Exp AND Exp 	{$$ = $1 && $3;}
	|Exp OR Exp	{$$ = $1 || $3;}
	|Exp RELOP Exp	
	|Exp PLUS Exp	{$$ = $1 + $3;}
	|Exp MINUS Exp	{$$ = $1 - $3;}
	|Exp STAR Exp	{$$ = $1 * $3;}
	|Exp DIV Exp	{$$ = $1 / $3;}
	|LP Exp RP	{$$ = $2;}	
	|MINUS Exp	{$$ = - $2;}
	|NOT Exp	{$$ = !$2;}
	|ID LP Args RP
	|ID LP RP	
	|Exp LB Exp RB
	|Exp DOT ID	
	|ID
	|INT		{$$ = $1;}
	|FLOAT		{$$ = $1;}
	;
Args	:Exp COMMA Args
	|Exp
	;
%%
void do_nothing(){
}
