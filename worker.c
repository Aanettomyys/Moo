#include "worker.h"

int yyparse(void * data);

void worker_init(Worker * w, const char * in)
{
	w->in = strdup(in);
	yylex_init(&w->p.scanner);
	w->plr = NULL;
	w->p.head = NULL;
}

void worker_clear(Worker * w)
{
	free(w->in);
	free(w->out);
	//TODO parser_l_rlist_clear(w->plr);
	yylex_destroy(w->p.scanner);
}

void worker_run(Worker * w)
{
	YY_BUFFER_STATE st;
	st = yy_scan_string(w->in, w->p.scanner);
	yyparse(&w->p);
	yy_delete_buffer(st, w->p.scanner);
}
