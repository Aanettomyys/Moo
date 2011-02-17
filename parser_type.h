#ifndef __PARSER_TYPE_H__
#define __PARSER_TYPE_H__

#include "ast.h"

typedef union _ParserType
{
	AST * ast;
	char * word;
	Strings * ldn;
	ASTActions actn;
	ParserRList * prl;
} ParserType;

int yyerror(const char * msg);

#define YYSTYPE ParserType

#endif // __PARSER_TYPE_H__
