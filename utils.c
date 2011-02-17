#include "ast.h"

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
		LASTActions * lasta = va_arg(ap, LASTActions *);
		nprl->p.a.ast = ast;
		nprl->p.a.lasta = lasta;
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


LASTActions * ast_l_lasta_new(ASTAction a1)
{
	LASTActions * lasta = malloc(sizeof(LASTActions));
	lasta->size = 1;
	lasta->actions = malloc(sizeof(ASTAction));
	lasta->actions[0] = a1;
	return lasta;
}

LASTActions * ast_l_lasta_append(LASTActions * lasta, ASTAction an)
{
	lasta->size++;
	lasta->actions = realloc(lasta->actions, lasta->size * sizeof(ASTAction));
	lasta->actions[lasta->size - 1] = an;
	return lasta;
}
