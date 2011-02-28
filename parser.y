%{

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "utils.h"

bool p_finish = false;
a_params_t p_ap;
u_queue_t * p_queue;
a_actions_t actn;
char * vname = NULL;
enum {
	P_PREC_SET = 1 << 0,
	P_XMIN_SET = 1 << 1,
	P_XMAX_SET = 1 << 2,
	P_YMIN_SET = 1 << 3,
	P_YMAX_SET = 1 << 4,
	P_WRAP_SET = 1 << 5
} p_param_ck;

#include "parser.h"

int lyyerror(YYLTYPE t, char * s, ...);
int yyerror(char * s, ...);
extern int yylex();

%}

%union
{
	a_t * exp;
	char * word;
	u_slist_t * ldn;
	a_actions_t actn;
}

%initial-action{
	AST_PARAMS_DEFAULT(p_ap);
	p_param_ck = 0;
	actn = 0;
	if(vname != NULL) free(vname);
}

%locations
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
			p_finish = true;
			YYACCEPT;
		}
	|	TEXT
		{ 
			p_result_t * pr = malloc(sizeof(*pr));
			assert( pr != NULL );
			pr->is_ast = false;
			pr->p.text = $<word>1;
			u_q_push(p_queue, pr);
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
			memcpy(pr->p.ast.ap, &p_ap, sizeof(p_ap));
			u_q_push(p_queue, pr);
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
				lyyerror(@3, "duplicate actions.");
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
			if(p_param_ck & P_PREC_SET)
				lyyerror(@1, "precision already set.");
			else
				p_param_ck |= P_PREC_SET;
			int prec = -1;
			if(sscanf($<word>3, "%d", &prec) != 1 || prec <= 0)
				lyyerror(@3, "wrong precision."); 
			p_ap.precision = prec;
			free($<word>3);
		}
	|	P_XMIN PRMS_SET PRMS_VAL
		{
			if(p_param_ck & P_XMIN_SET)
				lyyerror(@1, "`xmin' already set.");
			else
				p_param_ck |= P_XMIN_SET;
			double xmin;
			if(sscanf($<word>3, "%lf", &xmin) != 1)
				lyyerror(@3, "wrong value of `xmin'."); 
			p_ap.x_min = xmin;
			free($<word>3);
		}
	|	P_XMAX PRMS_SET PRMS_VAL
		{
			if(p_param_ck & P_XMAX_SET)
				lyyerror(@1, "`xmax' already set.");
			else
				p_param_ck |= P_XMAX_SET;
			double xmax;
			if(sscanf($<word>3, "%lf", &xmax) != 1)
				lyyerror(@3, "wrong value of `xmax'."); 
			p_ap.x_max = xmax;
			free($<word>3);
		}
	|	P_YMIN PRMS_SET PRMS_VAL
		{
			if(p_param_ck & P_YMIN_SET)
				lyyerror(@1, "`ymin' already set.");
			else
				p_param_ck |= P_YMIN_SET;
			double ymin;
			if(sscanf($<word>3, "%lf", &ymin) != 1)
				lyyerror(@3, "wrong value of `ymin'."); 
			p_ap.y_min = ymin;
			free($<word>3);
		}	
	|	P_YMAX PRMS_SET PRMS_VAL
		{
			if(p_param_ck & P_YMAX_SET)
				lyyerror(@1, "`ymax' already set.");
			else
				p_param_ck |= P_YMAX_SET;
			double ymax;
			if(sscanf($<word>3, "%lf", &ymax) != 1)
				lyyerror(@3, "wrong value of `ymax'.");
			p_ap.y_max = ymax;
			free($<word>3);
		}
	|	P_WRAP PRMS_SET P_WRAP_1
		{
			if(p_param_ck & P_WRAP_SET)
				lyyerror(@1, "`wrap' already set.");
			else
				p_param_ck |= P_WRAP_SET;
			p_ap.wrap = "$";
		}
	|	P_WRAP PRMS_SET P_WRAP_2
		{
			if(p_param_ck & P_WRAP_SET)
				lyyerror(@1, "`wrap' already set.");
			else
				p_param_ck |= P_WRAP_SET;
			p_ap.wrap = "$$";
		}
	;
expression:	EXPR_BEG expr_eq EXPR_END 
		{ 
			if(actn & AST_DRAW) lyyerror(@2, "`=' in graph.");
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
			ABIF1E($<exp>1) = $<exp>3;
			$<exp>$ = $<exp>1; 
		}
	|	EXPR_DIFF EXPR_VAR expr %prec EXPR_DIFF
		{
			if(actn & AST_DRAW) lyyerror(@1, "differential in grap.");
			u_slist_t * sl = u_sl_new();
			u_sl_append(sl, $<word>2);
			$<exp>$ = a_new(AST_DIFF, $<exp>3, sl);
		}
	;

var:		EXPR_VAR EXPR_LBR var_depends EXPR_RBR 
		{ 
			if(actn & AST_DRAW) lyyerror(@2, "nonfree varable in graph.");
			$<exp>$ = a_new(AST_VAR, $<word>1, $<ldn>3); 
		}
	|	EXPR_VAR 
		{
			if(actn & AST_DRAW)
			{
				if(vname != NULL && strcmp(vname, $<word>1))
					lyyerror(@1, "no unique varable in graph.");
				else if(vname == NULL)
					vname = strdup($<word>1);
			}
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

int lyyerror(YYLTYPE t, char * s, ...)
{
	va_list va;
	va_start(va, s);
	fprintf(stderr, "Error at line %d: ",
		t.first_line);
	vfprintf(stderr, s, va);
	fprintf(stderr, "\n");
	va_end(va);
	exit(-1);
}

int yyerror(char * s, ...)
{
	va_list va;
	va_start(va, s);
	fprintf(stderr, "Error at line %d: ",
		yylloc.first_line);
	vfprintf(stderr, s, va);
	fprintf(stderr, "\n");
	va_end(va);
	exit(-1);
}
