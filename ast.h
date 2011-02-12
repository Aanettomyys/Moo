#ifndef __AST_H__
#define __AST_H__

#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>

#define AST_MPFR_PREC 256

typedef enum
{
	AST_ACTN_SHOW,
	AST_ACTN_REDUCE
} ASTAction;

typedef enum
{
	AST_OP_ADD,
	AST_OP_SUB,
	AST_OP_MUL,
	AST_OP_DIV,
	AST_OP_POW
} ASTOpType;

typedef enum
{
	AST_BIF_SIN,
	AST_BIF_COS,
	AST_BIF_TAN,
	AST_BIF_SEC,
	AST_BIF_CSC,
	AST_BIF_COT,
	AST_BIF_ACOS,
	AST_BIF_ASIN,
	AST_BIF_ATAN,
	AST_BIF_COSH,
	AST_BIF_SINH,
	AST_BIF_TANH,
	AST_BIF_SECH,
	AST_BIF_CSCH,
	AST_BIF_COTH,
	AST_BIF_ACOSH,
	AST_BIF_ASINH,
	AST_BIF_ATANH 
} ASTBIFType1;

typedef enum
{
	AST_EQL,
	AST_SYSTEM,
	AST_NUMERIC,
	AST_BIF1,
	AST_OP,
	AST_VAR
} ASTClass;

typedef struct 
{
	size_t size;
	char ** names;
} LDepNames;

typedef struct
{
	size_t size;
	ASTAction * actions;
} LASTActions;

typedef struct
{
	ASTClass klass;
	void * p;
} AST;

typedef struct
{
	ASTBIFType1 type;
	AST * arg;
} ASTBIF1;

typedef struct
{
	mpfr_t v;
} ASTNumeric;

typedef struct
{
	ASTOpType type;
	AST * left;
	AST * right;
} ASTOp;

typedef struct
{
	char * name;
	LDepNames * ldn;
} ASTVar;

typedef struct
{
	AST * a1;
	AST * a2;
} ASTEql;

typedef struct _ASTNode
{
	AST * ast;
	struct _ASTNode * next;
	
} ASTNode;

typedef struct
{
	ASTNode * head;
	ASTNode * tail;
} ASTQueue;

typedef struct _ASTStack
{
	ASTNode * head;
} ASTStack;

typedef struct _ParserRList
{
	bool is_ast;
	union 
	{
		char * s;
		struct
		{
			AST * ast;
			LASTActions * lasta;
		} a;
	} p;
	struct _ParserRList * next;
} ParserRList;

/**
 * parser_l_push_back(prl, is_ast, char c);
 * parser_l_push_back(prl, is_ast, AST * a, LASTActions * lasta);
 */
ParserRList *	parser_l_push_back(ParserRList * prl, bool is_ast, ...);

LDepNames *	ast_l_ldn_new(char * n1);
LDepNames *	ast_l_ldn_append(LDepNames * ldn, char * nn);
LASTActions *	ast_l_lasta_new(ASTAction a1);
LASTActions *	ast_l_lasta_append(LASTActions * lasta, ASTAction an);

ASTQueue * 	ast_q_new();
AST *		ast_q_pop(ASTQueue * q);
void		ast_q_push(ASTQueue * q, AST * a);
ASTStack *	ast_s_new();
AST *		ast_s_pop(ASTStack * s);
void		ast_s_push(ASTStack * s, AST * a);

/**
 * ast_new(AST_NUMERIC, const char * s);
 * ast_new(AST_OP, ASTOpType t, AST * l, AST * r);
 * ast_new(AST_VAR, char * name, NULL | LDepNames * ldn);
 * ast_new(AST_BIF1, ASTBIFType1 t);
 * ast_new(AST_EQL, AST * a1, AST * a2);
 */
AST * 	ast_new(ASTClass klass, ...);
AST *	ast_bif1_set_arg(AST * a, AST * arg);

#endif // __AST_H__
