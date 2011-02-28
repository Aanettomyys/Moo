#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "parser.h"
#include "utils.h"

extern bool p_finish;
extern u_queue_t * p_queue;
extern FILE * yyin;
extern FILE * yyout;
extern int yyparse();

const char * pname;
const struct option lo[] = {
	{ "help",	0, NULL, 'h' },
	{ "output",	1, NULL, 'o' },
	{ "input",	1, NULL, 'i' },
	{ NULL,		0, NULL, 0 }
};
const char * so = "ho:i:";

void usage(FILE * o, int ec)
{
	fprintf(o, "Usage: %s options\n", pname);
	fprintf(o,
		" -i --input <filename>  Read input from file.\n"
		" -o --output <filename> Write output to file.\n"
		" -h --help              Print this usage.\n");
	exit(ec);
}

int main(int argc, char ** argv)
{
	yyin = stdin;
	yyout = stdout;
	pname = argv[0];
	int nextop;
	do
	{
		nextop = getopt_long(argc, argv, so, lo, NULL);
		switch(nextop)
		{
			case 'h' :
				usage(stdout, 0);
				break;
			case 'i' :
				yyin = fopen(optarg, "r");
				if(yyin == NULL)
				{
					perror("Moo: Cannot open input file.\n");
					exit(-1);
				}
				break;
			case 'o' :
				yyout = fopen(optarg, "w");
				if(yyout == NULL)
				{
					perror("Moo: Cannot open output file.");
					exit(-1);
				}
				break;
			case -1 :
				break;
			default : assert(0); break;
		}
	} while(nextop != -1);
	p_queue = u_q_new();
	int result = 0;
	while(!p_finish) 
	{
		result = yyparse();
		if(result != 0)
		{
			perror("Moo: Parse error.");
			exit(-1);
		}
	}
	u_queue_t * prepared = u_q_new();
	p_result_t * i;
	while((i = u_q_pop(p_queue)) != NULL)
	{
		if(i->is_ast && (i->p.ast.actn & AST_REDUCE))
			a_reduce(i->p.ast.exp, i->p.ast.ap);
		u_q_push(prepared, i);
	}
	free(p_queue);
	while((i = u_q_pop(prepared)) != NULL)
	{
		if(i->is_ast)
		{
			if(i->p.ast.actn & AST_SHOW)
			{
				fprintf(yyout, "%s", i->p.ast.ap->wrap);
				a_show(i->p.ast.exp, i->p.ast.ap);
				fprintf(yyout, "%s", i->p.ast.ap->wrap);
			}
			if(i->p.ast.actn & AST_DRAW)
			{
				fprintf(yyout, "\n\n");
				a_show_g(i->p.ast.exp, i->p.ast.ap);
				fprintf(yyout, "\n\n");
			}
			a_delete(i->p.ast.exp);
			free(i->p.ast.ap);
			fflush(yyout);
		}
		else
		{
			fprintf(yyout, "%s", i->p.text);
			free(i->p.text);
			fflush(yyout);
		}
	}
	free(prepared);
	if(yyin != stdin) fclose(yyin);
	if(yyout != stdout) fclose(yyout);
	return 0;
}
