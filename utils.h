#ifndef __UTILS_H__
#define __UTILS_H__

typedef struct _Node
{
	void * p;
	struct _Node * next;
	
} Node;

typedef struct
{
	Node * head;
	Node * tail;
} Queue;

typedef struct _ASTStack
{
	Node * head;
} Stack;

int yyerror(char *s, ...);

Queue * 	u_q_new();
void *		u_q_pop(Queue * q);
void		u_q_push(Queue * q, void * p);
Stack *		u_s_new();
void *		u_s_pop(Stack * s);
void		u_s_push(Stack * s, void * p);

#endif // __UTILS_H__
