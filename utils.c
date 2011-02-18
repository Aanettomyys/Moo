#include "ast.h"
#include "utils.h"
#include <stdarg.h>
#include <strings.h>

int yyerror(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	return -1;
}

Strings * u_strings_new(char * s)
{
	Strings * ss = malloc(sizeof(Strings));
	ss->size = 1;
	ss->ss = malloc(sizeof(char *));
	ss->ss[0] = s;
	return ss;
}

Strings * u_strings_append(Strings * ss, char * s)
{
	ss->size++;
	ss->ss = realloc(ss->ss, ss->size * sizeof(char *));
	ss->ss[ss->size - 1] = s;
	return ss;
}

Queue * u_q_new()
{
	Queue * q = malloc(sizeof(Queue));
	q->head = NULL;
	q->tail = NULL;
	return q;
}

void u_q_push(Queue * q, void * p)
{
	Node * qn = malloc(sizeof(Node));
	qn->p = p;
	qn->next = NULL;
	if(q->head == NULL)
	{
		q->head = qn;
	}
	if(q->tail == NULL) 
	{
		q->tail = qn;
	}
	else
	{
		q->tail->next = qn;
		q->tail = qn;
	}
}

void * u_q_pop(Queue * q)
{
	Node * qn = q->head;
	q->head = qn->next;
	if(qn->next == NULL)
		q->tail = NULL;
	void * p = qn->p;
	free(qn);
	return p;
}

Stack * u_s_new()
{
	Stack * s = malloc(sizeof(Stack));
	s->head = NULL;
	return s;
}

void u_s_push(Stack * s, void * p)
{
	Node * sn = malloc(sizeof(Node));
	sn->p = p;
	sn->next = s->head;
	s->head = sn;
}

void * u_s_pop(Stack * s)
{
	Node * sn = s->head;
	s->head = sn->next;
	void * p = sn->p;
	free(sn);
	return p;
}


ParserRList * parser_l_push_back(ParserRList * prl, bool is_ast, ...)
{
	ParserRList * nprl = malloc(sizeof(ParserRList));
	nprl->is_ast = is_ast;
	va_list ap;
	nprl->next = prl;
	va_start(ap, is_ast);
	if(is_ast)
	{
		AST * ast = va_arg(ap, AST *);
		ASTActions actn = va_arg(ap, ASTActions);
		ActionsParams * aps = va_arg(ap, ActionsParams *);
		nprl->p.a.ast = ast;
		nprl->p.a.actn = actn;
		nprl->p.a.ap = malloc(sizeof(ActionsParams));
		memmove(nprl->p.a.ap, aps, sizeof(ActionsParams));
	}
	else
	{
		char c = va_arg(ap, int); // compile warning
		if(prl != NULL && !prl->is_ast)
		{
			char * s;
			size_t len = strlen(prl->p.s) + 2;
			s = malloc(sizeof(char) * len);
			s[0] = c;
			memcpy(s + 1, prl->p.s, sizeof(char) * (len - 1));  
			prl->p.s = s;
			va_end(ap);
			free(nprl);
			return prl;
		}
		char * s = malloc(2 * sizeof(char));
		s[0] = c; s[1] = 0;
		nprl->p.s = s;
	}
	va_end(ap);
	return nprl;
}
