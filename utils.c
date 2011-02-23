#include "ast.h"
#include "utils.h"
#include <stdarg.h>
#include <strings.h>

int yyerror(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	vfprintf(stderr, s, ap);
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
	if(qn == NULL)
		return NULL;
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
