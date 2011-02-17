#include "worker.h"
#include "actions.h"

int yyparse(void * data);

void worker_init(Worker * w, FILE * in)
{
	yylex_init(&w->p.scanner);
	w->plr = NULL;
	w->p.head = NULL;
	w->fin = in;
}

void worker_clear(Worker * w)
{
	//TODO parser_l_rlist_clear(w->plr);
	yylex_destroy(w->p.scanner);
}

void worker_run(Worker * w)
{
	yyset_in(w->fin, w->p.scanner);
	yyparse(&w->p);
}

void worker_flush(Worker * w, FILE * out)
{
	ParserRList * it = w->p.head;
	while(it != NULL)
	{
		if(it->is_ast)
		{
			LASTActions * lasta = it->p.a.lasta;
			for(size_t i = 0; i < lasta->size; ++i)
			{
				switch(lasta->actions[i])
				{
					case AST_ACTN_SHOW :
						ast_action_show(it->p.a.ast, out);
						break;
					case AST_ACTN_REDUCE :
						ast_action_reduce(it->p.a.ast);
						break;
				}
			}
		}
		else
		{
			printf("%s", it->p.s);
		}
		it = it->next;
	}
}
