%{

#include <stdio.h>
#include <stdlib.h>
#include "parser_type.h"
#include "parser.h"
#include "ast.h"

%}

%option noyywrap reentrant never-interactive nounistd
%option bison-bridge

%x common params expression actions

WS	[ \n\r\t]*
WORD	[a-zA-Z]+
INT	[0-9]+
NUM	[0-9]+

%%

<INITIAL>{
	"%%"		BEGIN(common); return ENTRY;
	.|\n		yylval->word = strdup(yytext); return TEXT;
}
<common>{
	{WS}		/* eat */
	"<"		BEGIN(actions); return ACTN_BEGIN;
	"["		BEGIN(params); return PRMS_BEG;
	"{"		BEGIN(expression); return EXPR_BEG;
	.		{ yyerror(yytext); }
}
<actions>{
	{WS}		/* eat */
	">"		BEGIN(common); return ACTN_END;
	","		return ACTN_SEP;
	"show"		yylval->asta = AST_ACTN_SHOW; return ACTN_ACTION;
	"reduce"	yylval->asta = AST_ACTN_REDUCE; return ACTN_ACTION;
	.		{ yyerror(yytext); }
}
<params>{
	{WS}		/* eat */
	","		return PRMS_SEP;
	"]"		BEGIN(common); return PRMS_END;
	"="		return PRMS_SET;
	{WORD}		return PRMS_PRM;
	{INT}		return PRMS_INT;
	.		{ yyerror(yytext); }
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
	"cosh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_COSH); return EXPR_BIF1;
	"sinh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_SINH); return EXPR_BIF1;
	"tanh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_TANH); return EXPR_BIF1;
	"sech"		yylval->ast = ast_new(AST_BIF1, AST_BIF_SECH); return EXPR_BIF1;
	"csch"		yylval->ast = ast_new(AST_BIF1, AST_BIF_CSCH); return EXPR_BIF1;
	"coth"		yylval->ast = ast_new(AST_BIF1, AST_BIF_COTH); return EXPR_BIF1;
	"acosh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ACOSH); return EXPR_BIF1;
	"asinh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ASINH); return EXPR_BIF1;
	"atanh"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ATANH); return EXPR_BIF1;
	"sin"		yylval->ast = ast_new(AST_BIF1, AST_BIF_SIN); return EXPR_BIF1;
	"cos"		yylval->ast = ast_new(AST_BIF1, AST_BIF_COS); return EXPR_BIF1;
	"tan"		yylval->ast = ast_new(AST_BIF1, AST_BIF_TAN); return EXPR_BIF1;
	"sec"		yylval->ast = ast_new(AST_BIF1, AST_BIF_SEC); return EXPR_BIF1;
	"csc"		yylval->ast = ast_new(AST_BIF1, AST_BIF_CSC); return EXPR_BIF1;
	"cot"		yylval->ast = ast_new(AST_BIF1, AST_BIF_COT); return EXPR_BIF1;
	"acos"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ACOS); return EXPR_BIF1;
	"asin"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ASIN); return EXPR_BIF1;
	"atan"		yylval->ast = ast_new(AST_BIF1, AST_BIF_ATAN); return EXPR_BIF1;
	{NUM}		yylval->ast = ast_new(AST_NUMERIC, yytext); return EXPR_NUM; 
	"="		return EXPR_EQL;
	","		return EXPR_SEP;
	{WORD}		yylval->word = strdup(yytext); return EXPR_VAR;
	.		{ yyerror(yytext); }
}

%%

int yyerror(const char * msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	exit(-1);
	return 0;
}
