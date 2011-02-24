#ifndef __AST_H__
#define __AST_H__

#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>

#include "utils.h"

#define AST_MPFR_PREC 32
#define AST_PARAMS_DEFAULT(ap) \
	do { \
		ap->precision = 3; \
		ap->x_min = 0.0; \
		ap->x_max = 10.0; \
		ap->y_min = 0.0; \
		ap->y_max = 10.0; \
		ap->wrap = "$"; \
	} while(0)

typedef struct
{
	int precision;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	char * wrap;
} a_params_t;

typedef enum
{
	AST_SHOW = 1,
	AST_REDUCE = 2,
	AST_DRAW = 4
} a_actions_t;

typedef enum
{
	AST_OP_ADD,
	AST_OP_SUB,
	AST_OP_MUL,
	AST_OP_DIV,
	AST_OP_POW
} a_op_tt;

typedef enum
{
	AST_BIF_SIN,
	AST_BIF_COS,
	AST_BIF_TAN,
	AST_BIF_ACOS,
	AST_BIF_ASIN,
	AST_BIF_ATAN,
	AST_BIF_COSH,
	AST_BIF_SINH,
	AST_BIF_TANH,
	AST_BIF_ACOSH,
	AST_BIF_ASINH,
	AST_BIF_ATANH 
} a_bif1_tt;

typedef enum
{
	AST_EQL,
	AST_DIFF,
	AST_NUMERIC,
	AST_BIF1,
	AST_OP,
	AST_VAR
} a_tt;

typedef struct
{
	a_tt klass;
	bool negate;
	void * p;
} a_t;

typedef struct
{
	a_t * exp;
	slist_t * by;
} a_diff_t;

typedef struct
{
	a_bif1_tt klass;
	a_t * exp;
} a_bif1_t;

typedef struct
{
	mpfr_t v;
} a_numeric_t;

typedef struct
{
	a_op_tt klass;
	a_t * lexp;
	a_t * rexp;
} a_op_t;

typedef struct
{
	char * name;
	slist_t * ds;
} a_var_t;

typedef struct
{
	a_t * lexp;
	a_t * rexp;
} a_eql_t;

typedef struct
{
	bool is_ast;
	union 
	{
		char * text;
		struct
		{
			a_t * exp;
			a_actions_t actn;
			a_params_t * ap;
		} ast;
	} p;
} p_result_t;

/**
 * ast_new(AST_NUMERIC, const char * s);
 * ast_new(AST_OP, ASTOpType t, AST * l, AST * r);
 * ast_new(AST_VAR, char * name, Strings * ldn);
 * ast_new(AST_BIF1, ASTBIFType1 t);
 * ast_new(AST_DIFF, AST * exp, Strings * by);
 * ast_new(AST_EQL, AST * a1, AST * a2);
 */
a_t * 		a_new(a_tt, ...);
void		a_show(a_t *, a_params_t *, FILE *);
void		a_reduce(a_t *, a_params_t *);
void		a_show_g(a_t *, a_params_t *, FILE *);
void		a_delete(a_t *);
a_t *		a_clone(const a_t *);
u_stack_t *	a_iterate(a_t *);

#endif // __AST_H__
