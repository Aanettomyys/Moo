#include <stdio.h>
#include <mpfr.h>

#include "actions.h"

void ast_action_show(AST * p, FILE * o)
{
	fprintf(o, "(");
	switch(p->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(o, 10, 4, ((ASTNumeric *)p->p)->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((ASTBIF1 *)(p->p))->type)
			{
				case AST_BIF_SIN :
					fprintf(o, "\\sin");
					break;
				case AST_BIF_COS :
					fprintf(o, "\\cos");
					break;
			};
			ast_action_show(((ASTBIF1 *)(p->p))->arg, o);
			break;
		case AST_OP :
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
	fprintf(o, ")");
}
