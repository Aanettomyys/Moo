#ifndef __PARSER_TYPE_H__
#define __PARSER_TYPE_H__

#include "ast.h"

typedef union _ParserType
{
	AST * ast;
	char * word;
	LDepNames * ldn;
	LASTActions * lasta;
	ASTAction asta;
} ParserType;

#define YYSTYPE ParserType

#endif // __PARSER_TYPE_H__
