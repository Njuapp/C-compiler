%option yylineno
%{
	int yycolumn = 1;
	#define YY_USER_ACTION  \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1;\
		yycolumn += yyleng;
	#include <stdlib.h>
	#include "syntax.tab.h"
	#include "grammar_tree.h"
%}

delim	[ \t\n\r]
WS		{delim}+

/* punctuation */
SEMI	";"
COMMA	","
RELOP	">"|"<"|">="|"<="|"=="|"!="
ASSIGNOP	"="
PLUS	"+"
MINUS	"-"
STAR	"*"
DIV		"/"
AND		"&&"
OR		"||"
DOT		\.
NOT		\!
LP		\(
RP		")"
LB		"["
RB		"]"
LC		"{"
RC		"}"
COMM	"//".*
comment	([^\*]|(\*)*[^\*/])*
COMS	"/*"{comment}(\*)*"*/"


/* reserved words */
TYPE	"int"|"float"
STRUCT	"struct"
RETURN	"return"
IF		"if"
ELSE	"else"
WHILE	"while"

/* format */
digit [0-9]
INT	0|[1-9]{digit}*
FLOAT	{digit}+\.{digit}+
letter_ [_a-zA-Z]
/* wrong format?? */
ID	{letter_}({letter_}|{digit})*

%%
\n 	{ yycolumn = 1;}
{WS}	{ /* do nothing */ }
{SEMI}	{ yylval.a=create("SEMI",0,yylineno); return SEMI;} 
{COMMA}	{ yylval.a=create("COMMA",0,yylineno);return COMMA; }
{RELOP}	{ yylval.a=create("RELOP",0,yylineno);return RELOP; }
{ASSIGNOP}	{yylval.a=create("ASSIGNOP",0,yylineno); return ASSIGNOP; }
{PLUS}	{ yylval.a=create("PLUS",0,yylineno);return PLUS; }
{MINUS}	{ yylval.a=create("MINUS",0,yylineno); return MINUS; }
{STAR}	{ yylval.a=create("STAR",0,yylineno);return STAR; }
{DIV}	{ yylval.a=create("DIV",0,yylineno);return DIV; }
{AND}	{ yylval.a=create("AND",0,yylineno);return AND; }
{OR}	{ yylval.a=create("OR",0,yylineno);return OR; }
{DOT}	{ yylval.a=create("DOT",0,yylineno);return DOT; }
{NOT}	{ yylval.a=create("NOT",0,yylineno);return NOT; }
{LP}	{ yylval.a=create("LP",0,yylineno); return LP; }
{RP}	{ yylval.a=create("RP",0,yylineno); return RP; }
{LB}	{ yylval.a=create("LB",0,yylineno); return LB; }
{RB} 	{yylval.a=create("RB",0,yylineno); return RB;}
{LC} 	{yylval.a=create("LC",0,yylineno); return LC;}
{RC} 	{yylval.a=create("RC",0,yylineno); return RC;}
{COMM}	{ /*do nothing*/ }
{COMS}	{ /*do nothing*/ }
{TYPE}	{ yylval.a=create("TYPE",0,yylineno);return TYPE; }
{STRUCT}	{ yylval.a=create("STRUCT",0,yylineno);return STRUCT; }
{RETURN}	{ yylval.a=create("RETURN",0,yylineno);return RETURN; }
{IF}	{ yylval.a=create("IF",0,yylineno);return IF; }
{ELSE}	{ yylval.a=create("ELSE",0,yylineno);return ELSE; }
{WHILE}	{ yylval.a=create("WHILE",0,yylineno);return WHILE; }
{FLOAT}	{ yylval.a=create("FLOAT",0,yylineno);return FLOAT; }
{INT}	{ yylval.a=create("INT",0,yylineno);return INT; }
{ID}	{ yylval.a=create("ID",0,yylineno);  return ID; }
.		{ printf("Error type A at Line %d: Mysterious string \"%s\".\n", yylineno, yytext); }
%%
void donothing(){
}
