#include "ast.h"
#include "utils.h"

void a_reduce(a_t * exp, a_params_t * ap)
{
	u_stack_t * s = a_iterate(exp);
	while((exp = u_s_pop(s)) != NULL)
	{
/**----------START RULE SECTION----------**/
do {
/***---------- BEGIN RULE----------***/
/*-CONDITION-*/
if(
	exp->klass == AST_OP &&
	((a_op_t *)(exp->p))->lexp->klass == AST_NUMERIC && 
	((a_op_t *)(exp->p))->rexp->klass == AST_NUMERIC)
{
/*-BEGIN ACTION-*/
a_numeric_t * p = malloc(sizeof(*p));
mpfr_init2(p->v, AST_MPFR_PREC);
a_numeric_t * l = ((a_op_t *)(exp->p))->lexp->p;
a_numeric_t * r = ((a_op_t *)(exp->p))->rexp->p;
switch(((a_op_t *)(exp->p))->klass)
{
	case AST_OP_ADD :
	mpfr_add(p->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_SUB :
	mpfr_sub(p->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_MUL :
	mpfr_mul(p->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_DIV :
	mpfr_div(p->v, l->v, r->v, GMP_RNDN);
	break;

	case AST_OP_POW :
	mpfr_pow(p->v, l->v, r->v, GMP_RNDN);
	break;
}
a_delete(((a_op_t *)(exp->p))->lexp);
a_delete(((a_op_t *)(exp->p))->rexp);
free(exp->p);
exp->klass = AST_NUMERIC;
exp->p = p;
/*-END ACTION-*/
continue; }
/**----------END RULE---------**/

/**----------BEGIN RULE---------*/
/*-CONDITION-*/
if(
	exp->klass == AST_BIF1 &&
	((a_bif1_t *)(exp->p))->exp->klass == AST_NUMERIC)
{
/*-BEGIN ACTION-*/
a_numeric_t * p = malloc(sizeof(*p));
mpfr_init2(p->v, AST_MPFR_PREC);
a_numeric_t * arg = ((a_bif1_t *)(exp->p))->exp->p;
switch(((a_bif1_t *)(exp->p))->klass)
{
	case AST_BIF_SIN : mpfr_sin(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COS : mpfr_cos(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_TAN : mpfr_tan(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ACOS : mpfr_acos(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ASIN : mpfr_asin(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ATAN : mpfr_atan(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COSH : mpfr_cosh(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_SINH : mpfr_sinh(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_TANH : mpfr_tanh(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ACOSH : mpfr_acosh(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ASINH : mpfr_asinh(p->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ATANH : mpfr_atanh(p->v, arg->v, GMP_RNDN); break;
};
a_delete(((a_bif1_t *)(exp->p))->exp);
exp->klass = AST_NUMERIC;
free(exp->p);
exp->p = p;
/*-END ACTION-*/
continue; }
/**----------END RULE----------**/

/**----------BEGIN RULE----------*/
/*-CONDITION-*/
if(
	exp->klass == AST_NUMERIC &&
	exp->negate)
{
/*-BEGIN ACTION-*/
mpfr_neg(((a_numeric_t *)(exp->p))->v,
	((a_numeric_t *)(exp->p))->v, GMP_RNDN);
exp->negate = false;
/*-END ACTION-*/
continue; }
/**----------END RULE----------*/
break; } while(1);
/**----------END RULE SECTION----------**/
	}
	free(s);
}
