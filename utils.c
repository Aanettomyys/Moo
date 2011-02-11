#include "ast.h"

ASTQueue * ast_q_new()
{
	ASTQueue * q = malloc(sizeof(ASTQueue));
	q->head = NULL;
	q->tail = NULL;
	return q;
}

void ast_q_push(ASTQueue * q, AST * a)
{
	ASTNode * qn = malloc(sizeof(ASTNode));
	qn->ast = a;
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

AST * ast_q_pop(ASTQueue * q)
{
	ASTNode * qn = q->head;
	q->head = qn->next;
	if(qn->next == NULL)
		q->tail = NULL;
	AST * a = qn->ast;
	free(qn);
	return a;
}

ASTStack * ast_s_new()
{
	ASTStack * s = malloc(sizeof(ASTStack));
	s->head = NULL;
	return s;
}

void ast_s_push(ASTStack * s, AST * a)
{
	ASTNode * sn = malloc(sizeof(ASTNode));
	sn->ast = a;
	sn->next = s->head;
	s->head = sn;
}

AST * ast_s_pop(ASTStack * s)
{
	ASTNode * sn = s->head;
	s->head = sn->next;
	AST * a = sn->ast;
	free(sn);
	return a;
}
