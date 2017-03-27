%{
#include "lex.yy.c"
void yyerror(char* msg,...);
extern int lexerr;
int synerr = 0;
int yydebug=1;
%}
%union{
	struct GrammerTree* a;
}
%debug
%locations
%token <a> INT FLOAT
%token <a>ID SEMI COMMA TYPE STRUCT RETURN IF ELSE WHILE ASSIGNOP
%token <a>RELOP PLUS MINUS STAR DIV OR AND NOT DOT LP RP LB RB LC RC

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
		$$=create("Program",1,$1);
		if(!lexerr&&!synerr)
		eval($$,0);
		}
	;
ExtDefList:ExtDef ExtDefList{
		$$=create("ExtDefList",2,$1,$2);
		}
	|{$$=create("ExtDefList",0,-1);}
	;
ExtDef:Specifier ExtDecList SEMI    {$$=create("ExtDef",3,$1,$2,$3);}    
	|Specifier SEMI	{$$=create("ExtDef",2,$1,$2);}
	|Specifier FunDec Compst	{$$=create("ExtDef",3,$1,$2,$3);}
	|Specifier error SEMI {}
	;
ExtDecList:VarDec {$$=create("ExtDecList",1,$1);}
	|VarDec COMMA ExtDecList {$$=create("ExtDecList",3,$1,$2,$3);}
	;

/*Specifier*/
Specifier:TYPE {$$=create("Specifier",1,$1);}
	|StructSpecifier {$$=create("Specifier",1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=create("StructSpecifier",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=create("StructSpecifier",2,$1,$2);}
	|STRUCT OptTag LC error RC {}
	;
OptTag:ID {$$=create("OptTag",1,$1);}
	|{$$=create("OptTag",0,-1);}
	;
Tag:ID {$$=create("Tag",1,$1);}
	;

/*Declarators*/
VarDec:ID {$$=create("VarDec",1,$1);}
	| VarDec LB INT RB {$$=create("VarDec",4,$1,$2,$3,$4);}
	|VarDec LB error RB {}
	;
FunDec:ID LP VarList RP {$$=create("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=create("FunDec",3,$1,$2,$3);}
	|ID LP error RP {}
	;
VarList:ParamDec COMMA VarList {$$=create("VarList",3,$1,$2,$3);}
	|ParamDec {$$=create("VarList",1,$1);}
	;
ParamDec:Specifier VarDec {$$=create("ParamDec",2,$1,$2);}
    ;

/*Statement*/
Compst:LC DefList StmtList RC {$$=create("Compst",4,$1,$2,$3,$4);}
	|LC DefList error RC {}
	;
StmtList:Stmt StmtList{$$=create("StmtList",2,$1,$2);}
	| {$$=create("StmtList",0,-1);}
	;
Stmt:Exp SEMI {$$=create("Stmt",2,$1,$2);}
	|error SEMI{}  
	|Compst {$$=create("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=create("Stmt",3,$1,$2,$3);}
	|RETURN error SEMI {}
	|IF LP Exp RP Stmt {$$=create("Stmt",5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt {$$=create("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt {$$=create("Stmt",5,$1,$2,$3,$4,$5);}
	|WHILE LP error RP Stmt {}
	|IF LP error RP Stmt {}
	|IF LP error RP Stmt ELSE Stmt {}
	;

/*Local Definitions*/
DefList:Def DefList{$$=create("DefList",2,$1,$2);}
	| {$$=create("DefList",0,-1);}
	;
Def:Specifier DecList SEMI {$$=create("Def",3,$1,$2,$3);}
	|Specifier error SEMI {}
	;
DecList:Dec {$$=create("DecList",1,$1);}
	|Dec COMMA DecList {$$=create("DecList",3,$1,$2,$3);}
	;
Dec:VarDec {$$=create("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=create("Dec",3,$1,$2,$3);}
	;

Exp:Exp ASSIGNOP Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp AND Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp OR Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp RELOP Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp PLUS Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp MINUS Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp STAR Exp{$$=create("Exp",3,$1,$2,$3);}
        |Exp DIV Exp{$$=create("Exp",3,$1,$2,$3);}
        |LP Exp RP{$$=create("Exp",3,$1,$2,$3);}
        |LP error RP {}
		|MINUS Exp %prec UMINUS{$$=create("Exp",2,$1,$2);}
        |NOT Exp {$$=create("Exp",2,$1,$2);}
        |ID LP Args RP {$$=create("Exp",4,$1,$2,$3,$4);}
        |ID LP RP {$$=create("Exp",3,$1,$2,$3);}
		|ID LP error RP {}
        |Exp LB Exp RB {$$=create("Exp",4,$1,$2,$3,$4);}
		|Exp LB error RB {}
        |Exp DOT ID {$$=create("Exp",3,$1,$2,$3);}
        |ID {$$=create("Exp",1,$1);}
        |INT {$$=create("Exp",1,$1);}
        |FLOAT{$$=create("Exp",1,$1);}
        ;
Args:Exp COMMA Args {$$=create("Args",3,$1,$2,$3);}
        |Exp {$$=create("Args",1,$1);}
        ;
%%
