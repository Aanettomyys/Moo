#include "ast.h"
#include "actions.h"

static ASTStack * ast_iterate(AST * a)
{
	ASTStack * s = ast_s_new();
	ASTQueue * q = ast_q_new();
	ast_q_push(q, a);
	while(q->head != NULL)
	{
		AST * a = ast_q_pop(q);
		ast_s_push(s, a);
		switch(a->klass)
		{
			case AST_EQL :
				ast_q_push(q, ((ASTEql*)a->p)->a1);
				ast_q_push(q, ((ASTEql*)a->p)->a2);
				break;
			case AST_BIF1 :
				ast_q_push(q, ((ASTBIF1*)a->p)->arg);
				break;
			case AST_OP :
				ast_q_push(q, ((ASTOp*)a->p)->left);
				ast_q_push(q, ((ASTOp*)a->p)->right);
				break;
			default :
				break;
		};
	}
	free(q);
	return s;
}
void ast_action_reduce(AST * a)
{
	ASTStack * s = ast_iterate(a);
	while(s->head != NULL)
	{
		AST * a = ast_s_pop(s);
		switch(a->klass)
		{
			case AST_OP :
			{
				ASTOp * op = a->p;
				if(op->left->klass == AST_NUMERIC && op->right->klass == AST_NUMERIC)
				{
					ASTNumeric * ai = malloc(sizeof(ASTNumeric));
					mpfr_init2(ai->v, 128);
					ASTNumeric * l = op->left->p;
					ASTNumeric * r = op->right->p;
					switch(op->type)
					{
						case AST_OP_ADD :
							mpfr_add(ai->v, l->v, r->v, GMP_RNDZ);
							break;
						case AST_OP_SUB :
							mpfr_sub(ai->v, l->v, r->v, GMP_RNDZ);
							break;
						case AST_OP_MUL :
							mpfr_mul(ai->v, l->v, r->v, GMP_RNDZ);
							break;
						case AST_OP_DIV :
							mpfr_div(ai->v, l->v, r->v, GMP_RNDZ);
							break;
						default: break;
					}
					mpfr_clear(l->v);
					mpfr_clear(r->v);
					free(l); free(r);
					a->klass = AST_NUMERIC;
					a->p = ai;
				}
			};
			break;
			default : break;
		}
	};
	free(s);
}
