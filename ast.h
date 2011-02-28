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
		ap.precision = 3; \
		ap.x_min = 0.0; \
		ap.x_max = 10.0; \
		ap.y_min = 0.0; \
		ap.y_max = 10.0; \
		ap.wrap = "$"; \
	} while(0)

#define AKLASS(e) (e)->klass
#define ANUM(e) (e)->p.num
#define ABIF1K(e) (e)->p.bif1.klass
#define ABIF1E(e) (e)->p.bif1.exp
#define AOPK(e) (e)->p.op.klass
#define AOPL(e) (e)->p.op.lexp
#define AOPR(e) (e)->p.op.rexp
#define AVARN(e) (e)->p.var.name
#define AVARD(e) (e)->p.var.ds
#define AEQLL(e) (e)->p.eql.lexp
#define AEQLR(e) (e)->p.eql.rexp
#define ADIFFE(e) (e)->p.diff.exp
#define ADIFFB(e) (e)->p.diff.by

typedef struct _a_params_t
{
	int precision;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	char * wrap;
} a_params_t;

typedef enum _a_actions_t
{
	AST_SHOW  	= 1 << 0,
	AST_REDUCE	= 1 << 1,
	AST_DRAW 	= 1 << 2
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

typedef struct _a_t
{
	a_tt klass;
	bool negate;
	union
	{
		struct 
		{
			struct _a_t * exp;
			char * by;
		} diff;
		struct
		{
			a_bif1_tt klass;
			struct _a_t * exp;
		} bif1;
		mpfr_t num;
		struct 
		{
			a_op_tt klass;
			struct _a_t * lexp;
			struct _a_t * rexp;
		} op;
		struct
		{
			char * name;
			u_slist_t * ds;
		} var;
		struct
		{
			struct _a_t * lexp;
			struct _a_t * rexp;
		} eql;
	} p;
} a_t;

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

a_t * 		a_new(a_tt, ...);
void		a_show(a_t *, a_params_t *);
void		a_reduce(a_t *, a_params_t *);
void		a_show_g(a_t *, a_params_t *);
void		a_delete(a_t *);
a_t *		a_clone(const a_t *);
u_stack_t *	a_iterate(a_t *);
bool		a_depend_on(a_t *, const char *);

#endif // __AST_H__
