#include <stdio.h>
#include <getopt.h>

#include "parser_param.h"
#include "parser.h"
#include "lexer.h"
#include "actions.h"
#include "worker.h"

const char * pname;
const struct option lo[] = {
	{ "help",	0, NULL, 'h' },
	{ "output",	1, NULL, 'o' },
	{ NULL,		0, NULL, 0 }
};
const char * so = "ho";

void usage(FILE * o, int ec)
{
	fprintf(o, "Usage: %s options inputfile\n", pname);
	fprintf(o,
		" -o --output <filename> Write output to file.\n"
		" -h --help              Print this usage.\n");
	exit(ec);
}

int main(int argc, char ** argv)
{
	FILE * in = NULL;
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
			case 'o' :
				out = fopen(optarg, "w");
				break;
			case -1 :
				break;
			default : /* crash? */
				exit(-1);
				break;
		}
	} while(nextop != -1);
	if(optind >= argc)
		usage(stderr, -1);
	in = fopen(argv[optind], "r");
	Worker w;
	worker_init(&w, in);
	worker_run(&w);
	worker_flush(&w, out);
	fclose(in);
	if(out != stdout)
		fclose(out);
	return 0;
}
