#include "ast.h"
#include "utils.h"

#define START_RULE_SECTION do {
#define CONDITION(c) if(c) {
#define END_RULE continue; }
#define END_RULE_SECTION break; } while(1);

void a_reduce(a_t * exp, a_params_t * ap)
{
	u_stack_t * s = a_iterate(exp);
	while((exp = u_s_pop(s)) != NULL)
	{

START_RULE_SECTION

CONDITION(exp->klass == AST_OP &&
	exp->p.op.lexp->klass == AST_NUMERIC && 
	exp->p.op.rexp->klass == AST_NUMERIC)

a_t * l = exp->p.op.lexp;
a_t * r = exp->p.op.rexp;
a_op_tt klass = exp->p.op.klass;
mpfr_init2(exp->p.num, AST_MPFR_PREC);
switch(klass)
{
	case AST_OP_ADD :
	mpfr_add(exp->p.num, l->p.num, r->p.num, GMP_RNDN);
	break;
	
	case AST_OP_SUB :
	mpfr_sub(exp->p.num, l->p.num, r->p.num, GMP_RNDN);
	break;
	
	case AST_OP_MUL :
	mpfr_mul(exp->p.num, l->p.num, r->p.num, GMP_RNDN);
	break;
	
	case AST_OP_DIV :
	mpfr_div(exp->p.num, l->p.num, r->p.num, GMP_RNDN);
	break;

	case AST_OP_POW :
	mpfr_pow(exp->p.num, l->p.num, r->p.num, GMP_RNDN);
	break;
}
a_delete(l);
a_delete(r);
exp->klass = AST_NUMERIC;

END_RULE

CONDITION(exp->klass == AST_BIF1 &&
	exp->p.bif1.exp->klass == AST_NUMERIC)

a_t * expa = exp->p.bif1.exp;
a_bif1_tt klass = exp->p.bif1.klass;
mpfr_init2(exp->p.num, AST_MPFR_PREC);
switch(klass)
{
	case AST_BIF_SIN : mpfr_sin(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_COS : mpfr_cos(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_TAN : mpfr_tan(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ACOS : mpfr_acos(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ASIN : mpfr_asin(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ATAN : mpfr_atan(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_COSH : mpfr_cosh(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_SINH : mpfr_sinh(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_TANH : mpfr_tanh(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ACOSH : mpfr_acosh(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ASINH : mpfr_asinh(exp->p.num, expa->p.num, GMP_RNDN); break;
	case AST_BIF_ATANH : mpfr_atanh(exp->p.num, expa->p.num, GMP_RNDN); break;
};
a_delete(expa);
exp->klass = AST_NUMERIC;

END_RULE

CONDITION(exp->klass == AST_NUMERIC && exp->negate)

mpfr_neg(exp->p.num, exp->p.num, GMP_RNDN);
exp->negate = false;

END_RULE

END_RULE_SECTION
	}
	free(s);
}
