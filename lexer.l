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

%%

<INITIAL>{
	"%%"		BEGIN(common); return ENTRY;
	.		yylval->word = strdup(yytext); return TEXT;
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
	"sin"		return EXPR_FUNC_SIN;
	"cos"		return EXPR_FUNC_COS;
	{INT}		yylval->word = strdup(yytext); return EXPR_INT; 
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
