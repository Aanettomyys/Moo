%{

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser_param.h"
#include "ast.h"
#include "actions.h"
#include "utils.h"

%}

%locations
%pure_parser

%token ENTRY TEXT
%token ACTN_BEGIN ACTN_ACTION ACTN_SEP ACTN_END
%token PRMS_BEG PRMS_SEP PRMS_SET PRMS_VAL PRMS_END
%token EXPR_BEG EXPR_END EXPR_LBR EXPR_RBR EXPR_SEP

/*
	Availabel parametrs
*/
%token P_PRECISION

%left EXPR_EQL
%left EXPR_ADD EXPR_SUB
%left EXPR_DIF
%left EXPR_MUL EXPR_DIV
%left NEGATE
%right EXPR_POW

%token EXPR_BIF1
%token EXPR_VAR EXPR_NUM

%%

all:		/* empty */
	|	TEXT
		{ 
			ParserResult * pr = malloc(sizeof(ParserResult));
			pr->is_ast = false;
			pr->p.s = $<word>1;
			u_q_push(((ParserParam*)data)->q, pr);
			YYACCEPT;
		} 
	|	parsable { YYACCEPT; }
	;
parsable:	ENTRY actions params expression 
		{ 
			ParserResult * pr = malloc(sizeof(*pr));
			pr->is_ast = true;
			pr->p.a.ast = $<ast>4;
			pr->p.a.actn = $<actn>2;
			pr->p.a.ap = malloc(sizeof(*(pr->p.a.ap)));
			memcpy(pr->p.a.ap, ((ParserParam *)data)->ap, sizeof(*(pr->p.a.ap)));
			((ParserParam *)data)->ap->precision = DEFAULT_PRECISION;
			u_q_push(((ParserParam*)data)->q, pr);
		}
	;
actions:	ACTN_BEGIN action_list ACTN_END { $<actn>$ = $<actn>2; }
	;
action_list:	ACTN_ACTION { $<actn>$ = $<actn>1; }
	|	action_list ACTN_SEP ACTN_ACTION
		{
			if($<actn>3 & $<actn>1)
				yyerror("Duplicate actions.");
			YYABORT;
			$<actn>$ = ($<actn>3 | $<actn>1); 
		} 
	;
params:		/* empty */
	|	PRMS_BEG param_list PRMS_END
	;
param_list:	/* empty */
	|	param
	|	param_list PRMS_SEP param
	;
param:		P_PRECISION PRMS_SET PRMS_VAL
		{
			int prec =  atoi($<word>3);
			if(prec <= 0)
			{
				yyerror("at %d:%d - %d:%d: Wrong precision `%d'.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column,
					prec);
				YYABORT;
			}
			((ParserParam *)data)->ap->precision = prec;
			free($<word>3);
		}
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
	|	EXPR_SUB expr %prec NEGATE { $<ast>2->negate = true; $<ast>$ = $<ast>2; }
	|	EXPR_LBR expr EXPR_RBR { $<ast>$ = $<ast>2; }
	|	expr EXPR_POW expr { $<ast>$ = ast_new(AST_OP, AST_OP_POW, $<ast>1, $<ast>3); }
	|	var { $<ast>$ = $<ast>1; }
	|	EXPR_NUM { $<ast>$ = $<ast>1; }
	|	EXPR_BIF1 EXPR_LBR expr EXPR_RBR { $<ast>$ = ast_bif1_set_arg($<ast>1, $<ast>3); }
	;

var:		EXPR_VAR EXPR_LBR var_depends EXPR_RBR { $<ast>$ = ast_new(AST_VAR, $<word>1, $<ldn>3); }
	|	EXPR_VAR { $<ast>$ = ast_new(AST_VAR, $<word>1, NULL); }
	;

var_depends:	EXPR_VAR { $<ldn>$ = u_strings_new($<word>1); }
	|	EXPR_VAR EXPR_SEP var_depends { $<ldn>$ = u_strings_append($<ldn>3, $<word>1); }
	;
%%
