#include <stdarg.h>
#include "ast.h"

#include <mpfr.h>

AST * ast_new(ASTClass klass, ...)
{
	AST * a = malloc(sizeof(AST));
	a->klass = klass;
	va_list va;
	va_start(va, klass);
	switch(klass)
	{
		case AST_NUMERIC :
		{
			ASTNumeric * p = malloc(sizeof(ASTNumeric));
			mpfr_init2(p->v, AST_MPFR_PREC);
			mpfr_set_str(p->v, va_arg(va, char *), 10, GMP_RNDZ);
			a->p = p;
		}; break;
		case AST_BIF1 :
		{
			ASTBIF1 * p = malloc(sizeof(ASTBIF1));
			p->type = va_arg(va, ASTBIFType1);
			p->arg = NULL;
			a->p = p;
		}; break;
		case AST_VAR :
		{
			ASTVar * p = malloc(sizeof(ASTVar));
			p->name = va_arg(va, char *);
			p->ldn = va_arg(va, LDepNames *);
			a->p = p;
		}; break;
		case AST_OP :
		{
			ASTOp * p = malloc(sizeof(ASTOp));
			p->type = va_arg(va, ASTOpType);
			p->left = va_arg(va, AST *);
			p->right = va_arg(va, AST *);
			a->p = p;
		}; break;
		case AST_EQL :
		{
			ASTEql * p = malloc(sizeof(ASTEql));
			p->a1 = va_arg(va, AST *);
			p->a2 = va_arg(va, AST *);
			a->p = p;
		}; break;
		default : /* cannot reach here */ break;
	};
	va_end(va);
	return a;
}


 AST * ast_bif1_set_arg(AST * a, AST * arg)
 {
	 ASTBIF1 * p = (ASTBIF1*) (a->p);
	 p->arg = arg;
	 return a;
 }

