#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <mpfr.h>

#include "ast.h"
#include "utils.h"

extern FILE * yyin;
extern FILE * yyout;

void a_show(a_t * exp, a_params_t * ap)
{
	if(exp->negate) fprintf(yyout, "-");
	switch(AKLASS(exp))
	{
		case AST_NUMERIC :
			mpfr_out_str(yyout, 10, ap->precision, ANUM(exp), GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(ABIF1K(exp))
			{
				case AST_BIF_SIN : fprintf(yyout, "\\sin"); break;
				case AST_BIF_COS : fprintf(yyout, "\\cos"); break;
				case AST_BIF_TAN : fprintf(yyout, "\\tan"); break;
				case AST_BIF_ACOS : fprintf(yyout, "\\acos"); break;
				case AST_BIF_ASIN : fprintf(yyout, "\\asin"); break;
				case AST_BIF_ATAN : fprintf(yyout, "\\atan"); break;
				case AST_BIF_COSH : fprintf(yyout, "\\cosh"); break;
				case AST_BIF_SINH : fprintf(yyout, "\\sinh"); break;
				case AST_BIF_TANH : fprintf(yyout, "\\tanh"); break;
				case AST_BIF_ACOSH : fprintf(yyout, "\\acosh"); break;
				case AST_BIF_ASINH : fprintf(yyout, "\\asinh"); break;
				case AST_BIF_ATANH : fprintf(yyout, "\\atanh"); break;
				default : assert(0); break;
			}
			fprintf(yyout, "(");
			a_show(ABIF1E(exp), ap);
			fprintf(yyout, ")");
			break;
		case AST_OP :
			if(AOPK(exp) != AST_OP_POW)
				fprintf(yyout, "(");
			a_show(AOPL(exp), ap);
			switch(AOPK(exp))
			{
				case AST_OP_ADD :
					fprintf(yyout, " + "); break;
				case AST_OP_SUB :
					fprintf(yyout, " - "); break;
				case AST_OP_MUL :
					fprintf(yyout, " * "); break;
				case AST_OP_DIV :
					fprintf(yyout, " / "); break;
				case AST_OP_POW :
					fprintf(yyout, "^"); break;
				default : assert(0); break;
			};
			if(AOPK(exp) == AST_OP_POW)
				fprintf(yyout, "{");
			a_show(AOPR(exp), ap);
			if(AOPK(exp) == AST_OP_POW)
				fprintf(yyout, "}");
			else
				fprintf(yyout, ")");
			break;
		case AST_VAR :
		{
			fprintf(yyout, AVARN(exp));
			if(AVARD(exp)->size > 0)
			{
				fprintf(yyout, "(");
				for(size_t i = 0; i < AVARD(exp)->size; i++)
					fprintf(yyout, "%s", AVARD(exp)->ss[i]);
				fprintf(yyout, ")");
			}
		} break;
		case AST_EQL :
			a_show(AEQLL(exp), ap);
			fprintf(yyout, " = ");
			a_show(AEQLR(exp), ap);
			break;
		case AST_DIFF :
			fprintf(yyout, "\\frac{\\mathrm{d}}"
				"{\\mathrm{d}%s}", ADIFFB(exp));
			a_show(ADIFFE(exp), ap);
			break;
		default : assert(0); break;
	}
}

static void a_show_g_(a_t * exp, a_params_t * ap, FILE * toplot)
{
	if(exp->negate) fprintf(toplot, "-");
	switch(AKLASS(exp))
	{
		case AST_NUMERIC :
			mpfr_out_str(toplot, 10, ap->precision, ANUM(exp), GMP_RNDZ);
			break;
		case AST_BIF1 :
			switch(ABIF1K(exp))
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
			a_show_g_(ABIF1E(exp), ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_OP :
			fprintf(toplot, "(");
			a_show_g_(AOPL(exp), ap, toplot);
			switch(AOPK(exp))
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
			a_show_g_(AOPR(exp), ap, toplot);
			fprintf(toplot, ")");
			break;
		case AST_VAR :
			fprintf(toplot, "x");
			break;
		default: assert(0); break;
	};
}

void a_show_g(a_t * exp, a_params_t * ap)
{
	int to_plot[2];
	int from_plot[2];
	pipe(to_plot);
	pipe(from_plot);
	pid_t pid;
	pid = fork();
	if(pid == (pid_t) 0)
	{
		fclose(yyin);
		fclose(yyout);
		close(to_plot[1]);
		close(from_plot[0]);
		dup2(to_plot[0], STDIN_FILENO);
		dup2(from_plot[1], STDOUT_FILENO);
		execlp("gnuplot", "gnuplot", NULL);
		perror("Moo: Cannot call `gnuplot'.\n");
		exit(-1);
	}
	close(to_plot[0]);
	close(from_plot[1]);
	FILE * ftoplot = fdopen(to_plot[1], "w");
	assert(ftoplot != NULL );
	fprintf(ftoplot, 
		"set terminal latex\n"
		"set format y \"$%%g$\"\n"
		"set format x \"$%%g$\"\n"
		"set border 3\n"
		"set xtics nomirror\n"
		"set ytics nomirror\n"
		"unset key\n");
	fprintf(ftoplot, "plot [%lf:%lf] [%lf:%lf] ", 
		ap->x_min, ap->x_max, ap->y_min, ap->y_max);
	a_show_g_(exp, ap, ftoplot);
	fprintf(ftoplot, " with lines\nexit\n");
	fflush(ftoplot);
	char b;
	while(read(from_plot[0], &b, 1) == 1) 
	{
		fprintf(yyout, "%c", b);
	}
	fflush(yyout);
	waitpid(pid, NULL, 0);
	close(to_plot[1]);
	close(from_plot[0]);
}
