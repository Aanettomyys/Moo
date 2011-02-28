#include <assert.h>

#include "ast.h"
#include "utils.h"

#define START_RULE_SECTION do {
#define CONDITION(c) if(c) {
#define END_RULE continue; }
#define END_RULE_SECTION break; } while(1);


static void a_diff(a_t * exp, const char * by, a_params_t * ap)
{
	switch(AKLASS(exp))
	{
		case AST_OP :
		{
			if(!a_depend_on(exp, by))
			{
				a_delete(AOPR(exp));
				a_delete(AOPL(exp));
				AKLASS(exp) = AST_NUMERIC;
				mpfr_init2(ANUM(exp), AST_MPFR_PREC);
				mpfr_set_d(ANUM(exp), 0.0, GMP_RNDN);
				break;
			}
			switch(AOPK(exp))
			{
				case AST_OP_ADD :
				{ 
					a_diff(AOPL(exp), by, ap);
					a_diff(AOPR(exp), by, ap);
					a_reduce(exp, ap);
				} break;
				case AST_OP_SUB :
				{ 
					a_diff(AOPL(exp), by, ap);
					a_diff(AOPR(exp), by, ap);
					a_reduce(exp, ap);
				} break;
				case AST_OP_MUL :
				{
					a_t * ll = AOPL(exp);
					a_t * lr = AOPR(exp);
					a_t * rl = a_clone(ll);
					a_t * rr = a_clone(lr);
					a_diff(ll, by, ap);
					a_diff(rr, by, ap);
					a_t * l = a_new(AST_OP, AST_OP_MUL, ll, lr);
					a_t * r = a_new(AST_OP, AST_OP_MUL, rl, rr);
					AOPK(exp) = AST_OP_ADD;
					AOPL(exp) = l;
					AOPR(exp) = r;
					a_reduce(exp, ap);
				} break;
				case AST_OP_DIV :
				{
					a_t * ll = AOPL(exp);
					a_t * lr = AOPR(exp);
					a_t * rl = a_clone(ll);
					a_t * rr = a_clone(lr);
					a_t * down = a_new(AST_OP, AST_OP_MUL, 
						a_clone(lr), a_clone(lr));
					a_diff(ll, by, ap);
					a_diff(rr, by, ap);
					a_t * l = a_new(AST_OP, AST_OP_MUL, ll, lr);
					a_t * r = a_new(AST_OP, AST_OP_MUL, rl, rr);
					a_t * up = a_new(AST_OP, AST_OP_SUB, l, r);
					AOPL(exp) = up;
					AOPR(exp) = down;
					a_reduce(exp, ap);
				} break;
				default : assert(0); break;
			}			
		} break;
		case AST_NUMERIC :
			mpfr_set_d(ANUM(exp), 0.0, GMP_RNDN);
			break;
		case AST_BIF1 :
		{
			if(!a_depend_on(ABIF1E(exp), by))
			{
				a_delete(ABIF1E(exp));
				mpfr_init2(ANUM(exp), AST_MPFR_PREC);
				mpfr_set_d(ANUM(exp), 0.0, GMP_RNDN);
				AKLASS(exp) = AST_NUMERIC;
				break;
			}
			switch(ABIF1K(exp))
			{
				case AST_BIF_SIN :
				{
					a_t * right = a_clone(ABIF1E(exp));
					a_t * left = a_new(AST_BIF1, AST_BIF_COS);
					a_diff(right, by, ap);
					a_reduce(right, ap);
					ABIF1E(left) = ABIF1E(exp);
					AKLASS(exp) = AST_OP;
					AOPK(exp) = AST_OP_MUL;
					AOPL(exp) = left;
					AOPR(exp) = right;
				} break;
				case AST_BIF_COS :
				{
					a_t * right = a_clone(ABIF1E(exp));
					right->negate = true;
					a_t * left = a_new(AST_BIF1, AST_BIF_SIN);
					a_diff(right, by, ap);
					a_reduce(right, ap);
					ABIF1E(left) = ABIF1E(exp);
					AKLASS(exp) = AST_OP;
					AOPK(exp) = AST_OP_MUL;
					AOPL(exp) = left;
					AOPR(exp) = right;
				} break;
				default : assert(0); break;
			}
		} break;
		case AST_VAR :
		{
			if(!strcmp(AVARN(exp), by))
			{
				free(AVARN(exp));
				u_sl_delete(AVARD(exp));
				AKLASS(exp) = AST_NUMERIC;
				mpfr_init2(ANUM(exp), AST_MPFR_PREC);
				mpfr_set_d(ANUM(exp), 1.0, GMP_RNDN);
				break;
			} 
			else if(a_depend_on(exp, by))
			{
				a_t * exp1 = malloc(sizeof(*exp1));
				AKLASS(exp1) = AST_VAR;
				AVARN(exp1) = AVARN(exp);
				AVARD(exp1) = AVARD(exp);
				AKLASS(exp) = AST_DIFF;
				ADIFFE(exp) = exp1;
				ADIFFB(exp) = strdup(by);
				break;
			} 
			else
			{
				AKLASS(exp) = AST_NUMERIC;
				free(AVARN(exp));
				u_sl_delete(AVARD(exp));
				mpfr_init2(ANUM(exp), AST_MPFR_PREC);
				mpfr_set_d(ANUM(exp), 0.0, GMP_RNDN);
				break;
			}
			assert(0);
		} break;
		default : assert(0); break;
	}
}


					

void a_reduce(a_t * exp, a_params_t * ap)
{
	u_stack_t * s = a_iterate(exp);
	while((exp = u_s_pop(s)) != NULL)
	{

START_RULE_SECTION

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_ADD &&
	AKLASS(AOPL(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPL(exp)), 0.0))

a_delete(AOPL(exp));
AKLASS(exp) = AKLASS(AOPR(exp));
a_t * clean = AOPR(exp); 
exp->p = AOPR(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_ADD &&
	AKLASS(AOPR(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPR(exp)), 0.0))

a_delete(AOPR(exp));
AKLASS(exp) = AKLASS(AOPL(exp));
a_t * clean = AOPL(exp); 
exp->p = AOPL(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_MUL &&
	AKLASS(AOPL(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPL(exp)), 0.0))

a_delete(AOPR(exp));
AKLASS(exp) = AKLASS(AOPL(exp));
a_t * clean = AOPL(exp); 
exp->p = AOPL(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_MUL &&
	AKLASS(AOPR(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPR(exp)), 0.0))

a_delete(AOPL(exp));
AKLASS(exp) = AKLASS(AOPR(exp));
a_t * clean = AOPR(exp); 
exp->p = AOPR(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_MUL &&
	AKLASS(AOPL(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPL(exp)), 1.0))

a_delete(AOPL(exp));
AKLASS(exp) = AKLASS(AOPR(exp));
a_t * clean = AOPR(exp); 
exp->p = AOPR(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_OP &&
	AOPK(exp) == AST_OP_MUL &&
	AKLASS(AOPR(exp)) == AST_NUMERIC &&
	!mpfr_cmp_d(ANUM(AOPR(exp)), 1.0))

a_delete(AOPR(exp));
AKLASS(exp) = AKLASS(AOPL(exp));
a_t * clean = AOPL(exp); 
exp->p = AOPL(exp)->p;
free(clean);

END_RULE

CONDITION(AKLASS(exp) == AST_DIFF && 
	AKLASS(ADIFFE(exp)) != AST_DIFF &&
	!(AKLASS(ADIFFE(exp)) == AST_VAR &&
		strcmp(ADIFFB(exp), AVARN(ADIFFE(exp))) &&
		a_depend_on(ADIFFE(exp), ADIFFB(exp))))

a_diff(ADIFFE(exp), ADIFFB(exp), ap);
AKLASS(exp) = AKLASS(ADIFFE(exp));
free(ADIFFB(exp));
exp->p = ADIFFE(exp)->p;

END_RULE

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

