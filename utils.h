#ifndef __UTILS_H__
#define __UTILS_H__

typedef struct _u_slist_t
{
	size_t size;
	char ** ss;
} u_slist_t;

typedef struct _node_t
{
	void * p;
	struct _node_t * next;
} u_node_t;

typedef struct _u_queue_t
{
	u_node_t * head;
	u_node_t * tail;
} u_queue_t;

typedef struct _u_stack_t
{
	u_node_t * head;
} u_stack_t;

u_queue_t * 	u_q_new();
void *		u_q_pop(u_queue_t *);
void		u_q_push(u_queue_t *, void *);
u_stack_t *	u_s_new();
void *		u_s_pop(u_stack_t *);
void		u_s_push(u_stack_t *, void *);
u_slist_t *	u_sl_new();
void		u_sl_append(u_slist_t *, char *);
u_slist_t *	u_sl_clone(u_slist_t *);
void		u_sl_delete(u_slist_t *);

#endif // __UTILS_H__
