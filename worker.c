#include "worker.h"
#include "actions.h"
#include "utils.h"

int yyparse(void * data);

void worker_init(Worker * w, FILE * in)
{
	yylex_init(&w->p.scanner);
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
	while(!feof(w->fin)) 
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
			ASTActions actn = it->p.a.actn;
			if(actn & AST_REDUCE)
			{
				ast_action_reduce(it->p.a.ast, it->p.a.ap);
			}
		}
		i = i->next;
	}
	return 0;
}

void worker_flush(Worker * w, FILE * out)
{
	Node * i = w->p.q->head;
	while(i != NULL)
	{
		ParserResult * it = i->p;
		if(it->is_ast)
		{
			ASTActions actn = it->p.a.actn;
			if(actn & AST_SHOW)
				ast_action_show(it->p.a.ast, it->p.a.ap, out);
		}
		else
		{
			fprintf(out, "%s", it->p.s);
		}
		i = i->next;
	}
}
