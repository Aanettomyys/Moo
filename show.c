#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <mpfr.h>

#include "ast.h"
#include "utils.h"

void a_show(a_t * exp, a_params_t * ap, FILE * out)
{
	if(exp->negate) fprintf(out, "-");
	switch(exp->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(out, 10, ap->precision,
				((a_numeric_t *)(exp->p))->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((a_bif1_t *)(exp->p))->klass)
			{
				case AST_BIF_SIN : fprintf(out, "\\sin"); break;
				case AST_BIF_COS : fprintf(out, "\\cos"); break;
				case AST_BIF_TAN : fprintf(out, "\\tan"); break;
				case AST_BIF_ACOS : fprintf(out, "\\acos"); break;
				case AST_BIF_ASIN : fprintf(out, "\\asin"); break;
				case AST_BIF_ATAN : fprintf(out, "\\atan"); break;
				case AST_BIF_COSH : fprintf(out, "\\cosh"); break;
				case AST_BIF_SINH : fprintf(out, "\\sinh"); break;
				case AST_BIF_TANH : fprintf(out, "\\tanh"); break;
				case AST_BIF_ACOSH : fprintf(out, "\\acosh"); break;
				case AST_BIF_ASINH : fprintf(out, "\\asinh"); break;
				case AST_BIF_ATANH : fprintf(out, "\\atanh"); break;
				default : assert(0); break;
			}
			fprintf(out, "(");
			a_show(((a_bif1_t *)(exp->p))->exp, ap, out);
			fprintf(out, ")");
			break;
		case AST_OP :
			if(((a_op_t *)(exp->p))->klass != AST_OP_POW)
				fprintf(out, "(");
			a_show(((a_op_t *)(exp->p))->lexp, ap, out);
			switch(((a_op_t *)(exp->p))->klass)
			{
				case AST_OP_ADD :
					fprintf(out, " + "); break;
				case AST_OP_SUB :
					fprintf(out, " - "); break;
				case AST_OP_MUL :
					fprintf(out, " * "); break;
				case AST_OP_DIV :
					fprintf(out, " / "); break;
				case AST_OP_POW :
					fprintf(out, "^"); break;
				default : assert(0); break;
			};
			if(((a_op_t *)(exp->p))->klass == AST_OP_POW)
				fprintf(out, "{");
			a_show(((a_op_t *)(exp->p))->rexp, ap, out);
			if(((a_op_t *)(exp->p))->klass == AST_OP_POW)
				fprintf(out, "}");
			else
				fprintf(out, ")");
			break;
		case AST_VAR :
		{
			fprintf(out, ((a_var_t *)(exp->p))->name);
			slist_t * sl = ((a_var_t *)(exp->p))->ds;
			if(sl->size > 0)
			{
				fprintf(out, "(");
				for(size_t i = 0; i < sl->size; i++)
				{
					fprintf(out, "%s", sl->ss[i]);
				}
				fprintf(out, ")");
			}
		} break;
		case AST_EQL :
			a_show(((a_eql_t *)(exp->p))->lexp, ap, out);
			fprintf(out, " = ");
			a_show(((a_eql_t *)(exp->p))->rexp, ap, out);
			break;
		case AST_DIFF :
			fprintf(out, "\\frac{\\mathrm{d}}"
				"{\\mathrm{d}%s}", ((a_diff_t *)(exp->p))->by->ss[0]);
			a_show(((a_diff_t *)(exp->p))->exp, ap, out);
			break;
		default : assert(0); break;
	}
}

static void a_show_g_(a_t * exp, a_params_t * ap, FILE * toplot)
{
	if(exp->negate) fprintf(toplot, "-");
	switch(exp->klass)
	{
		case AST_NUMERIC :
			mpfr_out_str(toplot, 10, 
				ap->precision, ((a_numeric_t *)(exp->p))->v, GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(((a_bif1_t *)(exp->p))->klass)
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
				default: assert(0); break;
			};
			fprintf(toplot, "(");
			a_show_g_(((a_bif1_t *)(exp->p))->exp, ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_OP :
			fprintf(toplot, "(");
			a_show_g_(((a_op_t *)(exp->p))->lexp, ap, toplot);
			switch(((a_op_t *)(exp->p))->klass)
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
				default: assert(0); break;
			};
			a_show_g_(((a_op_t *)(exp->p))->rexp, ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_VAR :
			fprintf(toplot, "x");
			break;
		default: assert(0); break;
	};
}

void a_show_g(a_t * exp, a_params_t * ap, FILE * out)
{
	u_stack_t * s = a_iterate(exp);
	char * var = NULL;
	a_t * i;
	while((i = u_s_pop(s)) != NULL)
	{
		switch(i->klass)
		{
			case AST_EQL :
				yyerror("Worker: Draw: Operator `='.");
				exit(-1);
				break;
			case AST_VAR :
			{
				a_var_t * v = i->p;
				if(v->ds->size > 0)
				{
					yyerror("Worker: Draw: Nonfree varable.");
					exit(-1);
				}
				if(var == NULL)
				{
					var = strdup(v->name);
					continue;
				}
				if(strcmp(var, v->name))
				{
					yyerror("Worker: Draw: More than one varable.");
					exit(-1);
				}
			} break;
			case AST_DIFF :
				yyerror("Worker: Draw: Differential.");
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
		yyerror("Worker: Cannot call `gnuplot'.");
		exit(-1);
	}
	close(to_plot[0]);
	close(from_plot[1]);
	FILE * ftoplot = fdopen(to_plot[1], "w");
	assert(ftoplot != NULL );
	fprintf(ftoplot, "set terminal latex\n");
	fprintf(ftoplot, "set format y \"$%%g$\"\n");
	fprintf(ftoplot, "set format x \"$%%g$\"\n");
	fprintf(ftoplot, "plot [%lf:%lf] [%lf:%lf] ", 
		ap->x_min, ap->x_max, ap->y_min, ap->y_max);
	a_show_g_(exp, ap, ftoplot);
	fprintf(ftoplot, "\nexit\n");
	fflush(ftoplot);
	char b;
	while(read(from_plot[0], &b, 1) == 1) 
	{
		fprintf(out, "%c", b);
	}
	fflush(stdout);
	waitpid(pid, NULL, 0);
	close(to_plot[1]);
	close(from_plot[0]);
}
