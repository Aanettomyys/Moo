#include "worker.h"
#include "utils.h"

extern int yyparse(void * data);
extern int yylex_init(void **);
extern int yylex_destroy(void *);
extern int yyset_in(FILE *, void *);

void worker_init(Worker * w, FILE * in)
{
	yylex_init(&w->p.scanner);
	w->p.finish = false;
	w->p.q = u_q_new();
	w->p.ap = malloc(sizeof(ActionsParams));
	w->p.ap->precision = DEFAULT_PRECISION;
	w->fin = in;
}

void worker_clear(Worker * w)
{
	free(w->p.ap);
	yylex_destroy(w->p.scanner);
}

int worker_run(Worker * w)
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
	Node * i = w->p.q->head;
	while(i != NULL)
	{
		ParserResult * it = i->p;
		if(it->is_ast)
		{
			if(it->p.a.actn & AST_REDUCE)
			{
				ast_reduce(it->p.a.ast, it->p.a.ap);
			}
		}
		i = i->next;
	}
	return 0;
}

void worker_flush(Worker * w, FILE * out)
{
	ParserResult * pr;
	while((pr = u_q_pop(w->p.q)) != NULL)
	{
		if(pr->is_ast)
		{
			ASTActions actn = pr->p.a.actn;
			if(actn & AST_SHOW)
			{
				fprintf(out, "$");
				ast_show(pr->p.a.ast, pr->p.a.ap, out);
				fprintf(out, "$");
			}
			if(actn & AST_DRAW)
			{
				fprintf(out, "\n\n");
				ast_show_g(pr->p.a.ast, pr->p.a.ap, out);
				fprintf(out, "\n\n");
			}
		}
		else
		{
			fprintf(out, "%s", pr->p.s);
		}
	}
}
