%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser_param.h"
#include "ast.h"
#include "utils.h"

%}

%locations
%pure_parser
%defines "parser.h"
%output "parser.c"

%token ENTRY TEXT ENDOFFILE
%token ACTN_BEGIN ACTN_ACTION ACTN_SEP ACTN_END
%token PRMS_BEG PRMS_SEP PRMS_SET PRMS_VAL PRMS_END
%token EXPR_BEG EXPR_END EXPR_LBR EXPR_RBR EXPR_SEP

/*
	Parametrs
*/
%token P_PRECISION
%token P_XMIN P_XMAX
%token P_WRAP P_WRAP_1 P_WRAP_2
%token P_YMIN P_YMAX

%left EXPR_EQL
%left EXPR_DIFF
%left EXPR_ADD EXPR_SUB
%left EXPR_DIF
%left EXPR_MUL EXPR_DIV
%left NEGATE
%right EXPR_POW

%token EXPR_BIF1
%token EXPR_VAR EXPR_NUM

%%

all:	|	ENDOFFILE 
		{
			((p_param_t *)data)->finish = true;
			YYACCEPT;
		}
	|	TEXT
		{ 
			p_result_t * pr = malloc(sizeof(*pr));
			assert( pr != NULL );
			pr->is_ast = false;
			pr->p.text = $<word>1;
			u_q_push(((p_param_t  *)data)->q, pr);
			YYACCEPT;
		} 
	|	parsable 
		{ 
			YYACCEPT;
		}
	;
parsable:	ENTRY actions params expression 
		{ 
			p_result_t * pr = malloc(sizeof(*pr));
			pr->is_ast = true;
			pr->p.ast.exp = $<exp>4;
			pr->p.ast.actn = $<actn>2;
			pr->p.ast.ap = malloc(sizeof(*(pr->p.ast.ap)));
			assert( pr->p.ast.ap != NULL );
			memcpy(pr->p.ast.ap, 
				((p_param_t *)data)->ap, 
				sizeof(*(pr->p.ast.ap)));
			AST_PARAMS_DEFAULT(((p_param_t *)data)->ap);
			u_q_push(((p_param_t *)data)->q, pr);
		}
	;
actions:	ACTN_BEGIN action_list ACTN_END 
		{ 
			$<actn>$ = $<actn>2; 
		}
	;
action_list:	ACTN_ACTION 
		{ 
			$<actn>$ = $<actn>1; 
		}
	|	action_list ACTN_SEP ACTN_ACTION
		{
			if($<actn>3 & $<actn>1)
			{
				yyerror("Duplicate actions.");
				YYABORT;
			}
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
			int prec = -1;
			if(sscanf($<word>3, "%d", 
				&prec) != 1 || prec <= 0)
			{
				yyerror("at %d:%d - %d:%d: Wrong precision.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column);
				exit(-1);
			}
			((p_param_t *)data)->ap->precision = prec;
			free($<word>3);
		}
	|	P_XMIN PRMS_SET PRMS_VAL
		{
			double xmin;
			if(sscanf($<word>3, "%lf", &xmin) != 1)
			{
				yyerror("at %d:%d - %d:%d: Wrong value of `xmin'.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column);
				exit(-1);
			}
			((p_param_t *)data)->ap->x_min = xmin;
			free($<word>3);
		}
	|	P_XMAX PRMS_SET PRMS_VAL
		{
			double xmax;
			if(sscanf($<word>3, "%lf", &xmax) != 1)
			{
				yyerror("at %d:%d - %d:%d: Wrong value of `xmax'.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column);
				exit(-1);
			}
			((p_param_t *)data)->ap->x_max = xmax;
			free($<word>3);
		}
	|	P_YMIN PRMS_SET PRMS_VAL
		{
			double ymin;
			if(sscanf($<word>3, "%lf", &ymin) != 1)
			{
				yyerror("at %d:%d - %d:%d: Wrong value of `ymin'.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column);
				exit(-1);
			}
			((p_param_t *)data)->ap->y_min = ymin;
			free($<word>3);
		}	
	|	P_YMAX PRMS_SET PRMS_VAL
		{
			double ymax;
			if(sscanf($<word>3, "%lf", &ymax) != 1)
			{
				yyerror("at %d:%d - %d:%d: Wrong value of `ymax'.", 
					@3.first_line,
					@3.first_column,
					@3.last_line,
					@3.last_column);
				exit(-1);
			}
			((p_param_t *)data)->ap->y_max = ymax;
			free($<word>3);
		}
	|	P_WRAP PRMS_SET P_WRAP_1
		{
			((p_param_t *)data)->ap->wrap = "$";
		}
	|	P_WRAP PRMS_SET P_WRAP_2
		{
			((p_param_t *)data)->ap->wrap = "$$";
		}
	;
expression:	EXPR_BEG expr_eq EXPR_END 
		{ 
			$<exp>$ = $<exp>2; 
		}
	|	EXPR_BEG expr EXPR_END 
		{ 
			$<exp>$ = $<exp>2; 
		}
	;

expr_eq:	expr EXPR_EQL expr 
		{ 
			$<exp>$ = a_new(AST_EQL, 
				$<exp>1, $<exp>3); 
		}
	;

expr:		expr EXPR_ADD expr 
		{ 
			$<exp>$ = a_new(AST_OP, 
				AST_OP_ADD, $<exp>1, $<exp>3); 
		}
	|	expr EXPR_SUB expr 
		{ 
			$<exp>$ = a_new(AST_OP, 
				AST_OP_SUB, $<exp>1, $<exp>3); 
		}
	|	expr EXPR_MUL expr 
		{ 
			$<exp>$ = a_new(AST_OP, 
				AST_OP_MUL, $<exp>1, $<exp>3); 
		}
	|	expr EXPR_DIV expr 
		{ 
			$<exp>$ = a_new(AST_OP, 
				AST_OP_DIV, $<exp>1, $<exp>3); 
		}
	|	EXPR_SUB expr %prec NEGATE 
		{ 
			$<exp>2->negate = true; 
			$<exp>$ = $<exp>2; 
		}
	|	EXPR_LBR expr EXPR_RBR 
		{ 
			$<exp>$ = $<exp>2; 
		}
	|	expr EXPR_POW expr 
		{
			$<exp>$ = a_new(AST_OP, 
				AST_OP_POW, $<exp>1, $<exp>3); 
		}
	|	var 
		{ 
			$<exp>$ = $<exp>1; 
		}
	|	EXPR_NUM 
		{ 
			$<exp>$ = $<exp>1; 
		}
	|	EXPR_BIF1 EXPR_LBR expr EXPR_RBR 
		{ 
			((a_bif1_t *)($<exp>1->p))->exp = $<exp>3;
			$<exp>$ = $<exp>1; 
		}
	|	EXPR_DIFF EXPR_VAR expr %prec EXPR_DIFF
		{
			slist_t * sl = u_sl_new();
			u_sl_append(sl, $<word>2);
			$<exp>$ = a_new(AST_DIFF, $<exp>3, sl);
		}
	;

var:		EXPR_VAR EXPR_LBR var_depends EXPR_RBR 
		{ 
			$<exp>$ = a_new(AST_VAR, $<word>1, $<ldn>3); 
		}
	|	EXPR_VAR 
		{ 
			$<exp>$ = a_new(AST_VAR, $<word>1, u_sl_new()); 
		}
	;

var_depends:	EXPR_VAR 
		{ 
			$<ldn>$ = u_sl_new();
			u_sl_append($<ldn>$, $<word>1);
		}
	|	EXPR_VAR EXPR_SEP var_depends 
		{ 
			u_sl_append($<ldn>3, $<word>1); 
			$<ldn>$ = $<ldn>3;
		}
	;
%%
