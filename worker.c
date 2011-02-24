#include <assert.h>

#include "worker.h"
#include "utils.h"

extern int yyparse(void * data);
extern int yylex_init(void **);
extern int yylex_destroy(void *);
extern int yyset_in(FILE *, void *);

void w_init(worker_t * w, FILE * in)
{
	yylex_init(&w->p.scanner);
	w->p.finish = false;
	w->p.q = u_q_new();
	w->p.ap = malloc(sizeof(*(w->p.ap)));
	assert( w->p.ap != NULL );
	AST_PARAMS_DEFAULT(w->p.ap);
	w->fin = in;
}



int w_run(worker_t * w)
{
	yyset_in(w->fin, w->p.scanner);
	int result = 0;
	while(!w->p.finish) 
	{
		result = yyparse(&w->p);
		if(result != 0)
		{
			fprintf(stderr, "Worker: Parse error.\n");
			return result;
		}
	}
	node_t * i = w->p.q->head;
	while(i != NULL)
	{
		p_result_t * it = i->p;
		if(it->is_ast)
		{
			if(it->p.ast.actn & AST_REDUCE)
			{
				a_reduce(it->p.ast.exp, it->p.ast.ap);
			}
		}
		i = i->next;
	}
	return 0;
}

void w_flush(worker_t * w, FILE * out)
{
	p_result_t * pr;
	while((pr = u_q_pop(w->p.q)) != NULL)
	{
		if(pr->is_ast)
		{
			if(pr->p.ast.actn & AST_SHOW)
			{
				fprintf(out, "%s", pr->p.ast.ap->wrap);
				a_show(pr->p.ast.exp, pr->p.ast.ap, out);
				fprintf(out, "%s", pr->p.ast.ap->wrap);
			}
			if(pr->p.ast.actn & AST_DRAW)
			{
				fprintf(out, "\n\n");
				a_show_g(pr->p.ast.exp, pr->p.ast.ap, out);
				fprintf(out, "\n\n");
			}
			a_delete(pr->p.ast.exp);
			free(pr->p.ast.ap);
		}
		else
		{
			fprintf(out, "%s", pr->p.text);
			free(pr->p.text);
		}
	}
	free(w->p.q);
	free(w->p.ap);
	yylex_destroy(w->p.scanner);
}
