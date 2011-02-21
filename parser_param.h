#ifndef __PARSER_PARAM_H__
#define __PARSER_PARAM_H__

#include "ast.h"
#include "utils.h"

typedef struct
{
	void * scanner;
	Queue * q;
	ActionsParams * ap;
} ParserParam;

typedef union
{
	AST * ast;
	char * word;
	Strings * ldn;
	ASTActions actn;
} ParserType;
 
#define YYSTYPE ParserType

#define YYPARSE_PARAM data
#define YYLEX_PARAM ((ParserParam *)data)->scanner


#endif // __PARSER_PARM_H__
