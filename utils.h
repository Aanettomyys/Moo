#ifndef __UTILS_H__
#define __UTILS_H__

typedef struct
{
	size_t size;
	char ** ss;
} slist_t;

typedef struct _node_t
{
	void * p;
	struct _node_t * next;
	
} node_t;

typedef struct
{
	node_t * head;
	node_t * tail;
} queue_t;

typedef struct
{
	node_t * head;
} u_stack_t;

int yyerror(char *s, ...);

queue_t * 	u_q_new();
void *		u_q_pop(queue_t *);
void		u_q_push(queue_t *, void *);
u_stack_t *	u_s_new();
void *		u_s_pop(u_stack_t *);
void		u_s_push(u_stack_t *, void *);
slist_t *	u_sl_new();
void		u_sl_append(slist_t *, char *);
slist_t *	u_sl_clone(slist_t *);
void		u_sl_delete(slist_t *);

#endif // __UTILS_H__
