#include "ast.h"
#include "actions.h"
#include "utils.h"

static Stack * ast_iterate(AST * a)
{
	Stack * s = u_s_new();
	Queue * q = u_q_new();
	u_q_push(q, a);
	while(q->head != NULL)
	{
		AST * a = u_q_pop(q);
		u_s_push(s, a);
		switch(a->klass)
		{
			case AST_EQL :
				u_q_push(q, ((ASTEql*)a->p)->a2);
				u_q_push(q, ((ASTEql*)a->p)->a1);
				break;
			case AST_BIF1 :
				u_q_push(q, ((ASTBIF1*)a->p)->arg);
				break;
			case AST_OP :
				u_q_push(q, ((ASTOp*)a->p)->right);
				u_q_push(q, ((ASTOp*)a->p)->left);
				break;
			default :
				break;
		};
	}
	free(q);
	return s;
}

void ast_action_reduce(AST * a, ActionsParams * ap)
{
	Stack * s = ast_iterate(a);
	while(s->head != NULL)
	{
		AST * a = u_s_pop(s);
/**----------START RULE SECTION----------**/

/***---------- BEGIN RULE----------***/
/*-CONDITION-*/
if(
	a->klass == AST_OP &&
	((ASTOp*)(a->p))->left->klass == AST_NUMERIC && 
	((ASTOp*)(a->p))->right->klass == AST_NUMERIC)
{
/*-BEGIN ACTION-*/
ASTNumeric * ai = malloc(sizeof(ASTNumeric));
mpfr_init2(ai->v, AST_MPFR_PREC);
ASTNumeric * l = ((ASTOp*)(a->p))->left->p;
ASTNumeric * r = ((ASTOp*)(a->p))->right->p;
switch(((ASTOp*)(a->p))->type)
{
	case AST_OP_ADD :
	mpfr_add(ai->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_SUB :
	mpfr_sub(ai->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_MUL :
	mpfr_mul(ai->v, l->v, r->v, GMP_RNDN);
	break;
	
	case AST_OP_DIV :
	mpfr_div(ai->v, l->v, r->v, GMP_RNDN);
	break;
}
mpfr_clear(l->v);
mpfr_clear(r->v);
free(l); free(r);
free(a->p);
a->klass = AST_NUMERIC;
a->p = ai;
/*-END ACTION-*/
}
/**----------END RULE---------**/

/**----------BEGIN RULE---------*/
/*-CONDITION-*/
if(
	a->klass == AST_BIF1 &&
	((ASTBIF1*)a->p)->arg->klass == AST_NUMERIC)
{
/*-BEGIN ACTION-*/
ASTNumeric * ai = malloc(sizeof(ASTNumeric));
mpfr_init2(ai->v, AST_MPFR_PREC);
ASTNumeric * arg = ((ASTBIF1*)a->p)->arg->p;
switch(((ASTBIF1*)a->p)->type)
{
	case AST_BIF_SIN : mpfr_sin(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COS : mpfr_cos(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_TAN : mpfr_tan(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_SEC : mpfr_sec(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_CSC : mpfr_csc(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COT : mpfr_cot(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ACOS : mpfr_acos(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ASIN : mpfr_asin(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ATAN : mpfr_atan(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COSH : mpfr_cosh(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_SINH : mpfr_sinh(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_TANH : mpfr_tanh(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_SECH : mpfr_sech(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_CSCH : mpfr_csch(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_COTH : mpfr_coth(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ACOSH : mpfr_acosh(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ASINH : mpfr_asinh(ai->v, arg->v, GMP_RNDN); break;
	case AST_BIF_ATANH : mpfr_atanh(ai->v, arg->v, GMP_RNDN); break;
};
mpfr_clear(arg->v);
a->klass = AST_NUMERIC;
free(arg);
free(a->p);
a->p = ai;
/*-END ACTION-*/
}
/**----------END RULE----------**/

/**----------BEGIN RULE----------*/
/*-CONDITION-*/
if(
	a->klass == AST_NUMERIC &&
	a->negate)
{
/*-BEGIN ACTION-*/
mpfr_neg(((ASTNumeric *)(a->p))->v, ((ASTNumeric *)(a->p))->v, GMP_RNDN);
a->negate = false;
/*-END ACTION-*/
}
/**----------END RULE----------*/

/**----------END RULE SECTION----------**/
	}
	free(s);
}
