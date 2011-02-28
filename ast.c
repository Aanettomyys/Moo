#include <stdarg.h>
#include <assert.h>
#include <stdbool.h>
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
			mpfr_init2(exp->p.num, AST_MPFR_PREC);
			int r = mpfr_set_str(exp->p.num, 
				va_arg(va, char *), 
				10, GMP_RNDN);
			assert( r == 0 );
		} break;
		case AST_BIF1 :
		{
			exp->p.bif1.klass = va_arg(va, a_bif1_tt);
			exp->p.bif1.exp = NULL;
		} break;
		case AST_VAR :
		{
			exp->p.var.name = va_arg(va, char *);
			exp->p.var.ds = va_arg(va, u_slist_t *);
		} break;
		case AST_OP :
		{
			exp->p.op.klass = va_arg(va, a_op_tt);
			exp->p.op.lexp = va_arg(va, a_t *);
			exp->p.op.rexp = va_arg(va, a_t *);
		} break;
		case AST_EQL :
		{
			exp->p.eql.lexp = va_arg(va, a_t *);
			exp->p.eql.rexp = va_arg(va, a_t *);
		} break;
		case AST_DIFF :
		{
			exp->p.diff.exp = va_arg(va, a_t *);
			exp->p.diff.by = va_arg(va, char *);
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
			exp1->p.eql.lexp = a_clone(exp->p.eql.lexp);
			exp1->p.eql.rexp = a_clone(exp->p.eql.rexp);
		} break;
		case AST_DIFF :
		{
			exp1->p.diff.exp = a_clone(exp->p.diff.exp);
			exp1->p.diff.by = strdup(exp->p.diff.by);
		} break;
		case AST_NUMERIC :
		{
			mpfr_init2(exp1->p.num, AST_MPFR_PREC);
			int r = mpfr_set(exp1->p.num, exp->p.num, 
				GMP_RNDN);
			assert( r == 0 );
		} break;
		case AST_BIF1 :
		{
			exp1->p.bif1.klass = exp->p.bif1.klass;
			exp1->p.bif1.exp = a_clone(exp->p.bif1.exp);
		} break;
		case AST_OP :
		{
			exp1->p.op.klass = exp->p.op.klass;
			exp1->p.op.lexp = a_clone(exp->p.op.lexp);
			exp1->p.op.rexp = a_clone(exp->p.op.rexp);
		} break;
		case AST_VAR :
		{
			exp1->p.var.name = strdup(exp->p.var.name);
			assert( exp1->p.var.name != NULL );
			exp1->p.var.ds = u_sl_clone(exp->p.var.ds);
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
			a_delete(exp->p.eql.lexp);
			a_delete(exp->p.eql.rexp);
		} break;
		case AST_DIFF :
		{
			a_delete(exp->p.diff.exp);
			free(exp->p.diff.by);
		} break;
		case AST_NUMERIC :
		{
			mpfr_clear(exp->p.num);
		} break;
		case AST_BIF1 :
		{
			a_delete(exp->p.bif1.exp);
		} break;
		case AST_OP :
		{
			a_delete(exp->p.op.lexp);
			a_delete(exp->p.op.rexp);
		} break;
		case AST_VAR :
		{
			free(exp->p.var.name);
			u_sl_delete(exp->p.var.ds);
		} break;
		default : assert(0); break;
	}
	free(exp);
}

u_stack_t * a_iterate(a_t * exp)
{
	u_stack_t * s = u_s_new();
	u_queue_t * q = u_q_new();
	u_q_push(q, exp);
	while((exp = u_q_pop(q)) != NULL)
	{
		u_s_push(s, exp);
		switch(exp->klass)
		{
			case AST_EQL :
				u_q_push(q, exp->p.eql.rexp);
				u_q_push(q, exp->p.eql.lexp);
				break;
			case AST_BIF1 :
				u_q_push(q, exp->p.bif1.exp);
				break;
			case AST_OP :
				u_q_push(q, exp->p.op.rexp);
				u_q_push(q, exp->p.op.lexp);
				break;
			case AST_DIFF :
				u_q_push(q, exp->p.diff.exp);
				break;
			default : 
				break;
		};
	}
	free(q);
	return s;
}

bool a_depend_on(a_t * exp, const char * var)
{
	switch(AKLASS(exp))
	{
		case AST_DIFF :
			return a_depend_on(ADIFFE(exp), var);
		case AST_NUMERIC :
			return false;
		case AST_BIF1 :
			return a_depend_on(ABIF1E(exp), var);
		case AST_OP :
			return a_depend_on(AOPL(exp), var) ||
				a_depend_on(AOPR(exp), var);
		case AST_VAR :
		{
			if(!strcmp(AVARN(exp), var))
				return true;
			for(size_t i = 0; i < AVARD(exp)->size; ++i)
				if(!strcmp(AVARD(exp)->ss[i], var))
					return true;
			return false;
		} break;
		default : assert(0); break;
	}
	assert(0);
	return false;
}
