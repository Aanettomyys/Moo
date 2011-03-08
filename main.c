#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "parser.h"

extern int finish;
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
	int result = 0;
	int nextop;
	yyin = stdin;
	yyout = stdout;
	pname = argv[0];
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
	while(!finish) 
	{
		result = yyparse();
		if(result != 0)
		{
			perror("Moo: Parse error.");
			exit(-1);
		}
	}
	if(yyin != stdin) fclose(yyin);
	if(yyout != stdout) fclose(yyout);
	return 0;
}
