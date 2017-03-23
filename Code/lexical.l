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
COME	"*/"


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
{SEMI}	{ return SEMI;} 
{COMMA}	{ return COMMA; }
{RELOP}	{ return RELOP; }
{ASSIGNOP}	{ return ASSIGNOP; }
{PLUS}	{ return PLUS; }
{MINUS}	{ return MINUS; }
{STAR}	{ return STAR; }
{DIV}	{ return DIV; }
{AND}	{ return AND; }
{OR}	{ return OR; }
{DOT}	{ return DOT; }
{NOT}	{ return NOT; }
{LP}	{ return LP; }
{RP}	{ return RP; }
{LB}	{ return LB; }
{RB}	{ return RB; }
{LC}	{ return LC; }
{RC}	{ return RC; }
{COMM}	{ return ANNO; }
{COMS}	{ return ANNS; }
{COME}	{ printf("Error type A at Line %d: Wrong */.\n",yylineno); }
{TYPE}	{ return TYPE; }
{STRUCT}	{ return STRUCT; }
{RETURN}	{ return RETURN; }
{IF}	{ return IF; }
{ELSE}	{ return ELSE; }
{WHILE}	{ return WHILE; }
{FLOAT}	{ 
	yylval = atof(yytext);
	return FLOAT; }
{INT}	{ 
	yylval = atoi(yytext); 
	return INT; }
{ID}	{ return ID; }
.		{ printf("Error type A at Line %d: Mysterious string \"%s\".\n", yylineno, yytext); }
%%
void donothing(){
}
