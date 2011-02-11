#include <stdio.h>
#include <mpfr.h>

#include "ast.h"
#include "actions.h"

void ast_action_show(AST * p, FILE * o)
{
	switch(p->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(o, 10, 4, ((ASTNumeric *)p->p)->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((ASTBIF1 *)(p->p))->type)
			{
				case AST_BIF_SIN : fprintf(o, "\\sin"); break;
				case AST_BIF_COS : fprintf(o, "\\cos"); break;
				case AST_BIF_TAN : fprintf(o, "\\tan"); break;
				case AST_BIF_SEC : fprintf(o, "\\sec"); break;
				case AST_BIF_CSC : fprintf(o, "\\csc"); break;
				case AST_BIF_COT : fprintf(o, "\\cot"); break;
				case AST_BIF_ACOS : fprintf(o, "\\acos"); break;
				case AST_BIF_ASIN : fprintf(o, "\\asin"); break;
				case AST_BIF_ATAN : fprintf(o, "\\atan"); break;
				case AST_BIF_COSH : fprintf(o, "\\cosh"); break;
				case AST_BIF_SINH : fprintf(o, "\\sinh"); break;
				case AST_BIF_TANH : fprintf(o, "\\tanh"); break;
				case AST_BIF_SECH : fprintf(o, "\\sech"); break;
				case AST_BIF_CSCH : fprintf(o, "\\csch"); break;
				case AST_BIF_COTH : fprintf(o, "\\coth"); break;
				case AST_BIF_ACOSH : fprintf(o, "\\acosh"); break;
				case AST_BIF_ASINH : fprintf(o, "\\asinh"); break;
				case AST_BIF_ATANH : fprintf(o, "\\atanh"); break;
			};
			ast_action_show(((ASTBIF1 *)(p->p))->arg, o);
			break;
		case AST_OP :
			fprintf(o, "(");
			ast_action_show(((ASTOp *)(p->p))->left, o);
			switch(((ASTOp *)(p->p))->type)
			{
				case AST_OP_ADD :
					fprintf(o, " + ");
					break;
				case AST_OP_SUB :
					fprintf(o, " - ");
					break;
				case AST_OP_MUL :
					fprintf(o, " * ");
					break;
				case AST_OP_DIV :
					fprintf(o, " / ");
					break;
				case AST_OP_POW :
					fprintf(o, " ^ ");
					break;
			};
			ast_action_show(((ASTOp *)(p->p))->right, o);
			fprintf(o, ")");
			break;
		case AST_VAR :
			fprintf(o, ((ASTVar *)(p->p))->name);
			LDepNames * ldn = ((ASTVar *)(p->p))->ldn;
			if(ldn != NULL)
				fprintf(o, "(");
			for(size_t it = 0; it < ldn->size; it++)
			{
				fprintf(o, "%s", ldn->names[it]);
			};
			if(ldn != NULL)
				fprintf(o, ")");
			break;
	};
}
