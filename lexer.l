%{

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "parser_param.h"
#include "ast.h"
#include "utils.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
	yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
	yycolumn += yyleng;

%}

%option noyywrap reentrant yylineno
%option bison-bridge bison-locations
%option outfile="lexer.c"
%x common params expression actions

WS	[ \n\r\t]*
WORD	[a-zA-Z]+
INT	[0-9]+
NUM	([0-9]+)(("."([0-9]*))?)
%%

<INITIAL>{
	"@@"		BEGIN(common); return ENTRY;
	([^\@]){1,64}	yylval->word = strdup(yytext); return TEXT;
	"@"		yylval->word = strdup(yytext); return TEXT;
	<<EOF>>		return ENDOFFILE;
}
<common>{
	{WS}		/* eat */
	"<"		BEGIN(actions); return ACTN_BEGIN;
	"["		BEGIN(params); return PRMS_BEG;
	"{"		BEGIN(expression); return EXPR_BEG;
	.		{ yyerror("Lexer error at %d:%d - %d:%d: Unexpected character `%c'.", 
				yylloc->first_line,
				yylloc->first_column,
				yylloc->last_line,
				yylloc->last_column,
				yytext[0]); return -1; }
}
<actions>{
	{WS}		/* eat */
	">"		BEGIN(common); return ACTN_END;
	","		return ACTN_SEP;
	"show"		yylval->actn = AST_SHOW; return ACTN_ACTION;
	"reduce"	yylval->actn = AST_REDUCE; return ACTN_ACTION;
	"draw"		yylval->actn = AST_DRAW; return ACTN_ACTION;
	.		{ yyerror("Lexer error at %d:%d - %d:%d: Unexpected character `%c'.", 
				yylloc->first_line,
				yylloc->first_column,
				yylloc->last_line,
				yylloc->last_column,
				yytext[0]); return -1; }
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
	{NUM}		yylval->word = strdup(yytext); return PRMS_VAL;
	.		{ yyerror("Lexer error at %d:%d - %d:%d: Unexpected character `%c'.", 
				yylloc->first_line,
				yylloc->first_column,
				yylloc->last_line,
				yylloc->last_column,
				yytext[0]); return -1; }
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
	"cosh"		yylval->exp = a_new(AST_BIF1, AST_BIF_COSH); return EXPR_BIF1;
	"sinh"		yylval->exp = a_new(AST_BIF1, AST_BIF_SINH); return EXPR_BIF1;
	"tanh"		yylval->exp = a_new(AST_BIF1, AST_BIF_TANH); return EXPR_BIF1;
	"acosh"		yylval->exp = a_new(AST_BIF1, AST_BIF_ACOSH); return EXPR_BIF1;
	"asinh"		yylval->exp = a_new(AST_BIF1, AST_BIF_ASINH); return EXPR_BIF1;
	"atanh"		yylval->exp = a_new(AST_BIF1, AST_BIF_ATANH); return EXPR_BIF1;
	"sin"		yylval->exp = a_new(AST_BIF1, AST_BIF_SIN); return EXPR_BIF1;
	"cos"		yylval->exp = a_new(AST_BIF1, AST_BIF_COS); return EXPR_BIF1;
	"tan"		yylval->exp = a_new(AST_BIF1, AST_BIF_TAN); return EXPR_BIF1;
	"acos"		yylval->exp = a_new(AST_BIF1, AST_BIF_ACOS); return EXPR_BIF1;
	"asin"		yylval->exp = a_new(AST_BIF1, AST_BIF_ASIN); return EXPR_BIF1;
	"atan"		yylval->exp = a_new(AST_BIF1, AST_BIF_ATAN); return EXPR_BIF1;
	"diff"		return EXPR_DIFF;
	{NUM}		yylval->exp = a_new(AST_NUMERIC, yytext); return EXPR_NUM; 
	"="		return EXPR_EQL;
	","		return EXPR_SEP;
	{WORD}		yylval->word = strdup(yytext); return EXPR_VAR;
	.		{ yyerror("Lexer error at %d:%d - %d:%d: Unexpected character `%c'.", 
				yylloc->first_line,
				yylloc->first_column,
				yylloc->last_line,
				yylloc->last_column,
				yytext[0]);  return -1; }
}

%%
