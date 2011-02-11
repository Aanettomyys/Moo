#include "ast.h"

AST * ast_numeric_new(const char * str)
{
	AST * a = malloc(sizeof(AST));
	ASTNumeric * p = malloc(sizeof(ASTNumeric));
	a->klass = AST_NUMERIC;
	a->p = p;
	mpfr_init_set_str(p->v, str, 10, MPFR_RNDZ);
	return a;
}

AST * ast_bif1_new(ASTBIFType1 t)
{
	AST * a = malloc(sizeof(AST));
	ASTBIF1 * p = malloc(sizeof(ASTBIF1));
	a->klass = AST_BIF1;
	a->p = p;
	p->type = t;
	p->arg = NULL;
	return a;
}
 AST * ast_bif1_set_arg(AST * a, AST * arg)
 {
	 ASTBIF1 * p = (ASTBIF1*) (a->p);
	 p->arg = arg;
	 return a;
 }

 AST * ast_var_new(char * name)
{
	AST * a = malloc(sizeof(AST)); 
	ASTVar * p = malloc(sizeof(ASTVar));
	a->klass = AST_VAR;
	a->p = p;
	p->ldn = NULL;
	p->name = name;
	return a;
}

AST * ast_var_new_with_ldn(char * name, LDepNames * ldn)
{
	AST * a = malloc(sizeof(AST)); 
	ASTVar * p = malloc(sizeof(ASTVar));
	a->klass = AST_VAR;
	a->p = p;
	p->ldn = ldn;
	p->name = name;
	return a;
}

AST * ast_op_new(ASTOpType t, AST * l, AST * r)
{
	AST * a = malloc(sizeof(AST));
	ASTOp * p = malloc(sizeof(ASTOp));
	a->klass = AST_OP;
	a->p = p;
	p->left = l;
	p->right = r;
	p->type = t;
	return a;
}

AST * ast_eql_new(AST * a1, AST * a2)
{
	AST * a = malloc(sizeof(AST));
	ASTEql * p = malloc(sizeof(ASTEql));
	p->a1 = a1;
	p->a2 = a2;
	a->klass = AST_EQL;
	a->p = p;
	return a;
}

