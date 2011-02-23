#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <mpfr.h>

#include "ast.h"
#include "utils.h"

void ast_show(AST * p, ActionsParams * ap, FILE * o)
{
	if(p->negate)
		fprintf(o, "-");
	switch(p->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(o, 10, ap->precision, ((ASTNumeric *)p->p)->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((ASTBIF1 *)(p->p))->type)
			{
				case AST_BIF_SIN : fprintf(o, "\\sin"); break;
				case AST_BIF_COS : fprintf(o, "\\cos"); break;
				case AST_BIF_TAN : fprintf(o, "\\tan"); break;
				case AST_BIF_ACOS : fprintf(o, "\\acos"); break;
				case AST_BIF_ASIN : fprintf(o, "\\asin"); break;
				case AST_BIF_ATAN : fprintf(o, "\\atan"); break;
				case AST_BIF_COSH : fprintf(o, "\\cosh"); break;
				case AST_BIF_SINH : fprintf(o, "\\sinh"); break;
				case AST_BIF_TANH : fprintf(o, "\\tanh"); break;
				case AST_BIF_ACOSH : fprintf(o, "\\acosh"); break;
				case AST_BIF_ASINH : fprintf(o, "\\asinh"); break;
				case AST_BIF_ATANH : fprintf(o, "\\atanh"); break;
			};
			fprintf(o, "(");
			ast_show(((ASTBIF1 *)(p->p))->arg, ap, o);
			fprintf(o, ")");
			break;
		case AST_OP :
			if(((ASTOp *)(p->p))->type != AST_OP_POW)
				fprintf(o, "(");
			ast_show(((ASTOp *)(p->p))->left, ap, o);
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
			if(((ASTOp *)(p->p))->type == AST_OP_POW)
				fprintf(o, "{");
			ast_show(((ASTOp *)(p->p))->right, ap, o);
			if(((ASTOp *)(p->p))->type == AST_OP_POW)
				fprintf(o, "}");
			else
				fprintf(o, ")");
			break;
		case AST_VAR :
			fprintf(o, ((ASTVar *)(p->p))->name);
			Strings * ldn = ((ASTVar *)(p->p))->ldn;
			if(ldn != NULL)
			{
				fprintf(o, "(");
				for(size_t it = 0; it < ldn->size; it++)
				{
					fprintf(o, "%s", ldn->ss[it]);
				}
				fprintf(o, ")");
			}
			break;
		case AST_EQL :
			ast_show(((ASTEql *)p->p)->a1, ap, o);
			fprintf(o, " = ");
			ast_show(((ASTEql *)p->p)->a2, ap, o);
			break;
	};
}

static void ast_show_g_(AST * a, ActionsParams * ap, FILE * toplot)
{
	if(a->negate) fprintf(toplot, "-");
	switch(a->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(toplot, 10, ap->precision, ((ASTNumeric *)a->p)->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((ASTBIF1 *)(a->p))->type)
			{
				case AST_BIF_SIN : fprintf(toplot, "sin"); break;
				case AST_BIF_COS : fprintf(toplot, "cos"); break;
				case AST_BIF_TAN : fprintf(toplot, "tan"); break;
				case AST_BIF_ACOS : fprintf(toplot, "acos"); break;
				case AST_BIF_ASIN : fprintf(toplot, "asin"); break;
				case AST_BIF_ATAN : fprintf(toplot, "atan"); break;
				case AST_BIF_COSH : fprintf(toplot, "cosh"); break;
				case AST_BIF_SINH : fprintf(toplot, "sinh"); break;
				case AST_BIF_TANH : fprintf(toplot, "tanh"); break;
				case AST_BIF_ACOSH : fprintf(toplot, "acosh"); break;
				case AST_BIF_ASINH : fprintf(toplot, "asinh"); break;
				case AST_BIF_ATANH : fprintf(toplot, "atanh"); break;
			};
			fprintf(toplot, "(");
			ast_show_g_(((ASTBIF1 *)(a->p))->arg, ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_OP :
			fprintf(toplot, "(");
			ast_show_g_(((ASTOp *)(a->p))->left, ap, toplot);
			switch(((ASTOp *)(a->p))->type)
			{
				case AST_OP_ADD :
					fprintf(toplot, " + ");
					break;
				case AST_OP_SUB :
					fprintf(toplot, " - ");
					break;
				case AST_OP_MUL :
					fprintf(toplot, " * ");
					break;
				case AST_OP_DIV :
					fprintf(toplot, " / ");
					break;
				case AST_OP_POW :
					fprintf(toplot, " ** ");
					break;
			};
			ast_show_g_(((ASTOp *)(a->p))->right, ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_VAR :
			fprintf(toplot, "x");
			break;
		default: break;
	};
}

extern Stack * ast_iterate(AST * a);

void ast_show_g(AST * a, ActionsParams * ap, FILE * out)
{
	Stack * s = ast_iterate(a);
	char * var = NULL;
	while(s->head != NULL)
	{
		AST * ai = u_s_pop(s);
		switch(ai->klass)
		{
			case AST_EQL :
				yyerror("Worker: Operator `=' in drawable function.\n");
				exit(-1);
				break;
			case AST_VAR :
			{
				ASTVar * v = ai->p;
				if(v->ldn != NULL)
				{
					yyerror("Worker: Dependable varable in drawable function.\n");
					exit(-1);
				}
				if(var == NULL)
				{
					var = strdup(v->name);
					continue;
				}
				if(strcmp(var, v->name))
				{
					yyerror("Worker: More than one varable in drawable function.\n");
					exit(-1);
				}
			} break;
			default : break;
		}
	}

	int to_plot[2];
	int from_plot[2];
	pipe(to_plot);
	pipe(from_plot);
	pid_t pid;
	pid = fork();
	if(pid == (pid_t) 0)
	{
		close(to_plot[1]);
		close(from_plot[0]);
		dup2(to_plot[0], STDIN_FILENO);
		dup2(from_plot[1], STDOUT_FILENO);
		execlp("gnuplot", "gnuplot", NULL);
		yyerror("Worker: Cannot call `gnuplot'.\n");
		exit(-1);
	}
	close(to_plot[0]);
	close(from_plot[1]);
	FILE * ftoplot;
	ftoplot = fdopen(to_plot[1], "w");
	assert(ftoplot != NULL );
	fprintf(ftoplot, "set terminal latex\n");
	fprintf(ftoplot, "set format y \"$%%g$\"\n");
	fprintf(ftoplot, "set format x \"$%%.2f$\"\n");
	fprintf(ftoplot, "unset key\n");
	fprintf(ftoplot, "plot [-5:5] [-5:5] ");
	ast_show_g_(a, ap, ftoplot);
	fprintf(ftoplot, "\nexit\n");
	fflush(ftoplot);
	char b;
	while(read(from_plot[0], &b, 1) == 1) {
		fprintf(out, "%c", b);
	}
	fflush(stdout);
	waitpid(pid, NULL, 0);

	close(to_plot[1]);
	close(from_plot[0]);
}
