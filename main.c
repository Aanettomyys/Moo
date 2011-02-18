#include <stdio.h>
#include <getopt.h>

#include "parser_param.h"
#include "parser.h"
#include "actions.h"
#include "worker.h"

const char * pname;
const struct option lo[] = {
	{ "help",	0, NULL, 'h' },
	{ "output",	1, NULL, 'o' },
	{ "input",	1, NULL, 'i' },
	{ NULL,		0, NULL, 0 }
};

const char * so = "hoi";

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
	FILE * in = stdin;
	FILE * out = stdout;
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
				in = fopen(optarg, "r");
				if(in == NULL)
				{
					yyerror("Cannot open input file.");
				}
				break;
			case 'o' :
				out = fopen(optarg, "w");
				if(out == NULL)
				{
					yyerror("Cannot open output file.");
				}
				break;
			case -1 :
				break;
			default : /* crash? */
				exit(-1);
				break;
		}
	} while(nextop != -1);
	Worker w;
	worker_init(&w, in);
	worker_run(&w);
	worker_flush(&w, out);
	fclose(in);
	if(out != stdout)
		fclose(out);
	return 0;
}
