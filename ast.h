#ifndef __AST_H__
#define __AST_H__

#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>

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
	AST_BIF_COS
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

AST * 	ast_integer_new(const char * str);
AST * 	ast_op_new(ASTOpType t, AST * l, AST * r);
AST * 	ast_var_new(char * name);
AST *	ast_var_new_with_ldn(char * name, LDepNames * ldn);
AST *	ast_bif1_new(ASTBIFType1 t);
AST *	ast_bif1_set_arg(AST * a, AST * arg);
AST *	ast_eql_new(AST * a1, AST * a2);

#endif // __AST_H__
