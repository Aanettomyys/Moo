#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "utils.h"

int yyerror(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	exit(-1);
	return -1;
}

slist_t * u_sl_new()
{
	slist_t * sl = malloc(sizeof(*sl));
	assert( sl != NULL );
	sl->size = 0;
	sl->ss = NULL;
	return sl;
}

slist_t * u_sl_clone(slist_t * sl1)
{
	slist_t * sl2 = malloc(sizeof(*sl2));
	assert( sl2 != NULL );
	sl2->size = sl1->size;
	sl2->ss = malloc(sizeof(char *) * sl2->size);
	assert( sl2->ss != NULL );
	for(size_t i = 0; i < sl2->size; i++)
	{
		sl2->ss[i] = strdup(sl1->ss[i]);
		assert( sl2->ss[i] != NULL );
	}
	return sl2;
}

void u_sl_delete(slist_t * sl)
{
	for(size_t i = 0; i < sl->size; i++)
	{
		free(sl->ss[i]);
	}
	free(sl->ss);
	free(sl);
}


void u_sl_append(slist_t * sl, char * s)
{
	sl->size++;
	sl->ss = realloc(sl->ss, sl->size * sizeof(char *));
	assert( sl->ss != NULL );
	sl->ss[sl->size - 1] = s;
}

queue_t * u_q_new()
{
	queue_t * q = malloc(sizeof(*q));
	assert( q != NULL );
	q->head = NULL;
	q->tail = NULL;
	return q;
}

void u_q_push(queue_t * q, void * p)
{
	node_t * n = malloc(sizeof(*n));
	assert( n != NULL );
	n->p = p;
	n->next = NULL;
	if(q->head == NULL)
	{
		q->head = n;
	}
	if(q->tail == NULL) 
	{
		q->tail = n;
	}
	else
	{
		q->tail->next = n;
		q->tail = n;
	}
}

void * u_q_pop(queue_t * q)
{
	node_t * n = q->head;
	if(n == NULL)
		return NULL;
	q->head = n->next;
	if(n->next == NULL)
		q->tail = NULL;
	void * p = n->p;
	free(n);
	return p;
}

u_stack_t * u_s_new()
{
	u_stack_t * s = malloc(sizeof(*s));
	assert( s != NULL );
	s->head = NULL;
	return s;
}

void u_s_push(u_stack_t * s, void * p)
{
	node_t * n = malloc(sizeof(*n));
	assert( n != NULL );
	n->p = p;
	n->next = s->head;
	s->head = n;
}

void * u_s_pop(u_stack_t * s)
{
	node_t * n = s->head;
	if(n == NULL)
		return NULL;
	s->head = n->next;
	void * p = n->p;
	free(n);
	return p;
}
