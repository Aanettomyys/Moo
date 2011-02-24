#include <stdarg.h>
#include <assert.h>
#include <mpfr.h>

#include "utils.h"
#include "ast.h"

a_t * a_new(a_tt klass, ...)
{
	a_t * exp = malloc(sizeof(*exp));
	assert( exp != NULL );
	exp->klass = klass;
	exp->negate = false;
	va_list va;
	va_start(va, klass);
	switch(klass)
	{
		case AST_NUMERIC :
		{
			a_numeric_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			mpfr_init2(p->v, AST_MPFR_PREC);
			int r = mpfr_set_str(p->v, 
				va_arg(va, char *), 
				10, GMP_RNDN);
			assert( r == 0 );
			exp->p = p;
		} break;
		case AST_BIF1 :
		{
			a_bif1_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->klass = va_arg(va, a_bif1_tt);
			p->exp = NULL;
			exp->p = p;
		} break;
		case AST_VAR :
		{
			a_var_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->name = va_arg(va, char *);
			p->ds = va_arg(va, slist_t *);
			exp->p = p;
		} break;
		case AST_OP :
		{
			a_op_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->klass = va_arg(va, a_op_tt);
			p->lexp = va_arg(va, a_t *);
			p->rexp = va_arg(va, a_t *);
			exp->p = p;
		} break;
		case AST_EQL :
		{
			a_eql_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->lexp = va_arg(va, a_t *);
			p->rexp = va_arg(va, a_t *);
			exp->p = p;
		} break;
		case AST_DIFF :
		{
			a_diff_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->exp = va_arg(va, a_t *);
			p->by = va_arg(va, slist_t *);
			exp->p = p;
		} break;
		default : assert(0); break;
	};
	va_end(va);
	return exp;
}


a_t * a_clone(const a_t * exp)
{
	a_t * exp1 = malloc(sizeof(*exp1));
	assert( exp1 != NULL );
	exp1->klass = exp->klass;
	exp1->negate = exp->negate;
	switch(exp->klass)
	{
		case AST_EQL :
		{
			a_eql_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->lexp = a_clone(((a_eql_t *)(exp->p))->lexp);
			p->rexp = a_clone(((a_eql_t *)(exp->p))->rexp);
			exp1->p = p;
		} break;
		case AST_DIFF :
		{
			a_diff_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->exp = a_clone(((a_diff_t *)(exp->p))->exp);
			p->by = u_sl_clone(((a_diff_t *)(exp->p))->by);
			exp1->p = p;
		} break;
		case AST_NUMERIC :
		{
			a_numeric_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			mpfr_init2(p->v, AST_MPFR_PREC);
			int r = mpfr_set(p->v, ((a_numeric_t *)(exp->p))->v, 
				GMP_RNDN);
			assert( r == 0 );
			exp1->p = p;
		} break;
		case AST_BIF1 :
		{
			a_bif1_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->klass = ((a_bif1_t *)(exp->p))->klass;
			p->exp = a_clone(((a_bif1_t *)(exp->p))->exp);
			exp1->p = p;
		} break;
		case AST_OP :
		{
			a_op_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->klass = ((a_op_t *)(exp->p))->klass;
			p->lexp = a_clone(((a_op_t *)(exp->p))->lexp);
			p->rexp = a_clone(((a_op_t *)(exp->p))->rexp);
			exp1->p = p;
		} break;
		case AST_VAR :
		{
			a_var_t * p = malloc(sizeof(*p));
			assert( p != NULL );
			p->name = strdup(((a_var_t *)(exp->p))->name);
			assert( p->name != NULL );
			p->ds = u_sl_clone(((a_var_t *)(exp->p))->ds);
			exp1->p = p;
		} break;
		default : assert(0); break;
	}
	return exp1;
}

void a_delete(a_t * exp)
{
	switch(exp->klass)
	{
		case AST_EQL :
		{
			a_delete(((a_eql_t *)(exp->p))->lexp);
			a_delete(((a_eql_t *)(exp->p))->rexp);
		} break;
		case AST_DIFF :
		{
			a_delete(((a_diff_t *)(exp->p))->exp);
			u_sl_delete(((a_diff_t *)(exp->p))->by);
		} break;
		case AST_NUMERIC :
		{
			mpfr_clear(((a_numeric_t *)(exp->p))->v);
		} break;
		case AST_BIF1 :
		{
			a_delete(((a_bif1_t *)(exp->p))->exp);
		} break;
		case AST_OP :
		{
			a_delete(((a_op_t *)(exp->p))->lexp);
			a_delete(((a_op_t *)(exp->p))->rexp);
		} break;
		case AST_VAR :
		{
			free(((a_var_t *)(exp->p))->name);
			u_sl_delete(((a_var_t *)(exp->p))->ds);
		} break;
		default : assert(0); break;
	}
	free(exp->p);
	free(exp);
}

u_stack_t * a_iterate(a_t * exp)
{
	u_stack_t * s = u_s_new();
	queue_t * q = u_q_new();
	u_q_push(q, exp);
	while((exp = u_q_pop(q)) != NULL)
	{
		u_s_push(s, exp);
		switch(exp->klass)
		{
			case AST_EQL :
				u_q_push(q, ((a_eql_t *)(exp->p))->rexp);
				u_q_push(q, ((a_eql_t*)(exp->p))->lexp);
				break;
			case AST_BIF1 :
				u_q_push(q, ((a_bif1_t *)(exp->p))->exp);
				break;
			case AST_OP :
				u_q_push(q, ((a_op_t *)(exp->p))->rexp);
				u_q_push(q, ((a_op_t *)(exp->p))->lexp);
				break;
			case AST_DIFF :
				u_q_push(q, ((a_diff_t *)(exp->p))->exp);
				break;
			default : 
				break;
		};
	}
	free(q);
	return s;
}
