#ifndef __PARSER_PARAM_H__
#define __PARSER_PARAM_H__

#ifndef YY_NO_UNISTD_H
#define YY_NO_UNISTD_H 1
#endif // YY_NO_UNISTD_H


#include "parser_type.h"
#include "ast.h"
#include "lexer.h"

typedef struct _ParserParam
{
	yyscan_t scanner;
	AST * ast;
	LASTActions * lasta;
} ParserParam;

#define YYPARSE_PARAM data
#define YYLEX_PARAM ((ParserParam *)data)->scanner


#endif // __PARSER_PARM_H__
