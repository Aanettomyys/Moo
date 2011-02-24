#ifndef __PARSER_PARAM_H__
#define __PARSER_PARAM_H__

#include <stdbool.h>

#include "ast.h"
#include "utils.h"

typedef struct
{
	void * scanner;
	queue_t * q;
	a_params_t * ap;
	bool finish;
} p_param_t;

typedef union
{
	a_t * exp;
	char * word;
	slist_t * ldn;
	a_actions_t actn;
} p_t;
 
#define YYSTYPE p_t

#define YYPARSE_PARAM data
#define YYLEX_PARAM ((p_param_t *)data)->scanner


#endif // __PARSER_PARM_H__
