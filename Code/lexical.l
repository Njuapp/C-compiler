%option yylineno
%{
	/* NONE */
%}

delim	[ \t\n\r]
ws		{delim}+

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
ANNO	"//"
LA		"/*"
RA		"*/"

/* reserved words */
TYPE	"int"|"float"
STRUCT	"struct"
RETURN	"return"
IF		"if"
ELSE	"else"
WHILE	"while"

/* formal */
DIGIT [0-9]
INT	0|[1-9]{DIGIT}*
FLOAT	{DIGIT}+\.{DIGIT}+
LETTER_ [_a-zA-Z]
ID	{LETTER_}({LETTER_}|{DIGIT})*

%%
{ws}	{ /* do nothing */ }
{SEMI}	{ printf("SEMI\n"); }
{COMMA}	{ printf("COMMA\n"); }
{RELOP}	{ printf("RELOP\n"); }
{ASSIGNOP}	{ printf("ASSIGNOP\n"); }
{PLUS}	{ printf("PLUS\n"); }
{MINUS}	{ printf("MINUS\n"); }
{STAR}	{ printf("STAR\n"); }
{DIV}	{ printf("DIV\n"); }
{AND}	{ printf("AND\n"); }
{OR}	{ printf("OR\n"); }
{DOT}	{ printf("DOT\n"); }
{NOT}	{ printf("NOT\n"); }
{LP}	{ printf("LP\n"); }
{RP}	{ printf("RP\n"); }
{LB}	{ printf("LB\n"); }
{RB}	{ printf("RB\n"); }
{LC}	{ printf("LC\n"); }
{RC}	{ printf("RC\n"); }
{ANNO}	{ printf("ANNO\n"); }
{LA}	{ printf("LA\n"); }
{RA}	{ printf("RA\n"); }
{TYPE}	{ printf("TYPE\n"); }
{STRUCT}	{ printf("STRUCT\n"); }
{RETURN}	{ printf("RETURN\n"); }
{IF}	{ printf("IF\n"); }
{ELSE}	{ printf("ELSE\n"); }
{WHILE}	{ printf("WHILE\n"); }
{FLOAT}	{ printf("FLOAT\n"); }
{INT}	{ printf("INT\n"); }
{ID}	{ printf("ID\n"); }
.		{ printf("Error type A at Line %d: Mysterious string \"%s\".\n", yylineno, yytext); }
%%
void donothing(){
}
