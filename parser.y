%{

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser_param.h"
#include "parser_type.h"
#include "ast.h"
#include "actions.h"

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

%token EXPR_BIF1
%token EXPR_VAR EXPR_NUM

%%

all:		/* empty */
	|	TEXT all
		{ 
			((ParserParam*)data)->head = parser_l_push_back(((ParserParam*)data)->head, false, $<word>1[0]);
			free($<word>1);
		} 
	|	parsable all 
		{
			$<prl>1->next = ((ParserParam *)data)->head;
			((ParserParam *)data)->head = $<prl>1;
		}
	;
parsable:	ENTRY actions params expression 
		{ 
			$<prl>$ = parser_l_push_back(NULL, true, $<ast>4, $<lasta>2);
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

expr_eq:	expr EXPR_EQL expr { $<ast>$ = ast_new(AST_EQL, $<ast>1, $<ast>3); }
	;

expr:		expr EXPR_ADD expr { $<ast>$ = ast_new(AST_OP, AST_OP_ADD, $<ast>1, $<ast>3); }
	|	expr EXPR_SUB expr { $<ast>$ = ast_new(AST_OP, AST_OP_SUB, $<ast>1, $<ast>3); }
	|	expr EXPR_MUL expr { $<ast>$ = ast_new(AST_OP, AST_OP_MUL, $<ast>1, $<ast>3); }
	|	expr EXPR_DIV expr { $<ast>$ = ast_new(AST_OP, AST_OP_DIV, $<ast>1, $<ast>3); }
	|	EXPR_LBR expr EXPR_RBR { $<ast>$ = $<ast>2; }
	|	expr EXPR_POW expr { $<ast>$ = ast_new(AST_OP, AST_OP_POW, $<ast>1, $<ast>3); }
	|	var { $<ast>$ = $<ast>1; }
	|	EXPR_NUM { $<ast>$ = $<ast>1; }
	|	EXPR_BIF1 EXPR_LBR expr EXPR_RBR { $<ast>$ = ast_bif1_set_arg($<ast>1, $<ast>3); }
	;

var:		EXPR_VAR EXPR_LBR var_depends EXPR_RBR { $<ast>$ = ast_new(AST_VAR, $<word>1, $<ldn>3); }
	|	EXPR_VAR { $<ast>$ = ast_new(AST_VAR, $<word>1, NULL); }
	;

var_depends:	EXPR_VAR { $<ldn>$ = ast_l_ldn_new($<word>1); }
	|	EXPR_VAR EXPR_SEP var_depends { $<ldn>$ = ast_l_ldn_append($<ldn>3, $<word>1); }
	;
%%
