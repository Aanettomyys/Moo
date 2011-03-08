%{

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
/*#include "utils.h"*/

extern int yyerror(char * s, ...);


#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

%}

%option noyywrap yylineno
%option outfile="lexer.c"
%x common params expression actions

WS	[ \n\r\t]*
WORD	[a-zA-Z]+
INT	[0-9]+
REAL	[0-9]*\.?[0-9]+

%%

<INITIAL>{
	"@@"		BEGIN(common); return ENTRY;
	([^\@]){1,64}	yylval.p = strdup(yytext); return TEXT;
	"@"		yylval.p = strdup(yytext); return TEXT;
	<<EOF>>		return ENDOFFILE;
}
<common>{
	{WS}		/* eat */
	"<"		BEGIN(actions); return ACTN_BEGIN;
	"["		BEGIN(params); return PRMS_BEG;
	"{"		BEGIN(expression); return EXPR_BEG;
	.		{ 
				yyerror("unexpected character `%c'.", yytext[0]); 
				return -1; 
			}
}
<actions>{
	{WS}		/* eat */
	">"		BEGIN(common); return ACTN_END;
	","		return ACTN_SEP;
	"show"		yylval.a = SHOW; return ACTN_ACTION;
	"reduce"	yylval.a = REDUCE; return ACTN_ACTION;
	"draw"		yylval.a = DRAW; return ACTN_ACTION;
	.		{ 
				yyerror("unexpected character `%c'.", yytext[0]); 
				return -1; 
			}
}
<params>{
	{WS}		/* eat */
	","		return PRMS_SEP;
	"]"		BEGIN(common); return PRMS_END;
	"="		return PRMS_SET;
	"prec"		return P_PRECISION;
	"xmin"		return P_XMIN;
	"xmax"		return P_XMAX;
	"ymin"		return P_YMIN;
	"ymax"		return P_YMAX;
	"wrap"		return P_WRAP;
	"$$"		return P_WRAP_2;
	"$"		return P_WRAP_1;
	{REAL}		yylval.p = strdup(yytext); return PRMS_VAL;
	.		{ 
				yyerror("unexpected character `%c'.", yytext[0]); 
				return -1; 
			}
}
<expression>{
	{WS}		/* eat */
	"}"		BEGIN(INITIAL); return EXPR_END;
	"+"		return EXPR_ADD;
	"-"		return EXPR_SUB;
	"/"		return EXPR_DIV;
	"*"		return EXPR_MUL;
	"^"		return EXPR_POW;
	"("		return EXPR_LBR;
	")"		return EXPR_RBR;
	"sin"		yylval.p = new(Sin()); return EXPR_BIF1;
	"cos"		yylval.p = new(Cos()); return EXPR_BIF1;
	"tan"		yylval.p = new(Tan()); return EXPR_BIF1;
	"diff"		return EXPR_DIFF;
	{INT}		{
				yylval.p = new(Integer()); 
				setIValString(yylval.p, yytext);
				return EXPR_INT;
			}
	{REAL}		{
				yylval.p = new(Real()); 
				setRValString(yylval.p, yytext);
				return EXPR_REAL;
			}
	","		return EXPR_SEP;
	{WORD}		yylval.p = strdup(yytext); return EXPR_VAR;
	.		{ 
				yyerror("unexpected character `%c'.", yytext[0]); 
				return -1; 
			}

}

%%
