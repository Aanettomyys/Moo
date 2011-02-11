#include "ast.h"

LDepNames * ast_l_ldn_new(char * n1)
{
	LDepNames * ldn = malloc(sizeof(LDepNames));
	ldn->size = 1;
	ldn->names = malloc(sizeof(char *));
	ldn->names[0] = n1;
	return ldn;
}

LDepNames * ast_l_ldn_append(LDepNames * ldn, char * nn)
{
	ldn->size++;
	ldn->names = realloc(ldn->names, ldn->size * sizeof(char *));
	ldn->names[ldn->size - 1] = nn;
	return ldn;
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
