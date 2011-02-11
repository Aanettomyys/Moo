%{

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser_param.h"
#include "parser_type.h"
#include "ast.h"

%}

%define api.pure

%token ENTRY TEXT
%token ACTN_BEGIN ACTN_ACTION ACTN_SEP ACTN_END
%token PRMS_BEG PRMS_SEP PRMS_SET PRMS_PRM PRMS_INT PRMS_END
%token EXPR_BEG EXPR_END EXPR_LBR EXPR_RBR EXPR_SEP

%left EXPR_EQL
%left EXPR_ADD EXPR_SUB
%left EXPR_MUL EXPR_DIV
%right EXPR_POW

%token EXPR_FUNC_SIN EXPR_FUNC_COS
%token EXPR_VAR EXPR_INT

%%

all:		/* empty */
	|	TEXT all 
	|	parsable all
	;
parsable:	ENTRY actions params expression 
		{ 
			printf("Парсинг успешно завершен!\n");
			printf("Получено выражение: ");
			ast_action_show($<ast>4, stdout);
			if($<lasta>2 != NULL)
			{
				printf("\nПолучены следующие действия:");
				for(size_t i = 0; i < $<lasta>2->size; i++)
				{
					switch($<lasta>2->actions[i])
					{
					case AST_ACTN_SHOW :
						printf(" SHOW");
						break;
					case AST_ACTN_REDUCE :
						printf(" REDUCE");
						break;
					};
				};
			}
			else
			{
				printf("\nНет переданных действий");
			}
			printf("\nРедуцируем: ");
			ast_action_reduce($<ast>4);
			ast_action_show($<ast>4, stdout);
			printf("\nКонец\n");
		}
	;
actions:	ACTN_BEGIN action_list ACTN_END { $<lasta>$ = $<lasta>2; }
	;
action_list:	{ $<lasta>$ = NULL; }
	|	ACTN_ACTION { $<lasta>$ = ast_l_lasta_new($<asta>1); }
	|	ACTN_ACTION ACTN_SEP action_list { $<lasta>$ = ast_l_lasta_append($<lasta>3, $<asta>1); } 
	;
params:		PRMS_BEG param_list PRMS_END
	;
param_list:	/* Empty */
	|	param PRMS_SEP param_list
	|	param
	;
param:		PRMS_PRM PRMS_SET param_right
	;
param_right:	PRMS_INT
	;
expression:	EXPR_BEG expr_eq EXPR_END { $<ast>$ = $<ast>2; }
	|	EXPR_BEG expr EXPR_END { $<ast>$ = $<ast>2; }
	;

expr_eq:	expr EXPR_EQL expr { $<ast>$ = ast_eql_new($<ast>1, $<ast>3); }
	;

expr:		expr EXPR_ADD expr { $<ast>$ = ast_op_new(AST_OP_ADD, $<ast>1, $<ast>3); }
	|	expr EXPR_SUB expr { $<ast>$ = ast_op_new(AST_OP_SUB, $<ast>1, $<ast>3); }
	|	expr EXPR_MUL expr { $<ast>$ = ast_op_new(AST_OP_MUL, $<ast>1, $<ast>3); }
	|	expr EXPR_DIV expr { $<ast>$ = ast_op_new(AST_OP_DIV, $<ast>1, $<ast>3); }
	|	EXPR_LBR expr EXPR_RBR { $<ast>$ = $<ast>2; }
	|	expr EXPR_POW expr { $<ast>$ = ast_op_new(AST_OP_POW, $<ast>1, $<ast>3); }
	|	expr_atom { $<ast>$ = $<ast>1; }
	|	expr_fun EXPR_LBR expr EXPR_RBR { $<ast>$ = ast_bif1_set_arg($<ast>1, $<ast>3); }
	;

expr_atom:	EXPR_INT { $<ast>$ = ast_integer_new($<word>1); free($<word>1); }
	|	EXPR_VAR EXPR_LBR var_depends EXPR_RBR { $<ast>$ = ast_var_new_with_ldn($<word>1, $<ldn>3); }
	|	EXPR_VAR { $<ast>$ = ast_var_new($<word>1); }
	;

var_depends:	EXPR_VAR { $<ldn>$ = ast_l_ldn_new($<word>1); }
	|	EXPR_VAR EXPR_SEP var_depends { $<ldn>$ = ast_l_ldn_append($<ldn>3, $<word>1); }
	;

expr_fun:	EXPR_FUNC_SIN { $<ast>$ = ast_bif1_new(AST_BIF_SIN); }
	|	EXPR_FUNC_COS { $<ast>$ = ast_bif1_new(AST_BIF_COS); }
	;
%%
