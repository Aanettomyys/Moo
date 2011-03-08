%{

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
/*#include "utils.h"*/

int finish = 0;
OutParams op;

enum {
	P_PREC_SET = 1 << 0,
	P_XMIN_SET = 1 << 1,
	P_XMAX_SET = 1 << 2,
	P_YMIN_SET = 1 << 3,
	P_YMAX_SET = 1 << 4,
	P_WRAP_SET = 1 << 5
} paramCk;

enum {
	REAL,
	INTEGER
} domain;

Actions actions;

#include "parser.h"

int lyyerror(YYLTYPE t, char * s, ...);
int yyerror(char * s, ...);
extern int yylex();
extern FILE * yyin;
extern FILE * yyout;


struct
{
	char * name;
	const void * class;
} * varTable;
size_t varTableSize;


%}

%union
{
	void * p;
	Actions a;
}

%initial-action{
	size_t i;
	PARAMS_DEFAULT(op);
	paramCk = 0;
	actions = 0;
	domain = REAL;
	for(i = 0; i < varTableSize; ++i)
	{
		free(varTable[i].name);
	}
	free(varTable);
	varTable = NULL;
	varTableSize = 0;
}

%locations
%defines "parser.h"
%output "parser.c"

%token ENTRY TEXT ENDOFFILE
%token ACTN_BEGIN ACTN_ACTION ACTN_SEP ACTN_END
%token PRMS_BEG PRMS_SEP PRMS_SET PRMS_VAL PRMS_END
%token EXPR_BEG EXPR_END EXPR_LBR EXPR_RBR EXPR_SEP
%token DOM_REAL DOM_INTEGER

%token P_PRECISION
%token P_XMIN P_XMAX
%token P_WRAP P_WRAP_1 P_WRAP_2
%token P_YMIN P_YMAX

%left EXPR_DIFF
%left EXPR_ADD EXPR_SUB
%left EXPR_DIF
%left EXPR_MUL EXPR_DIV
%left NEGATE
%right EXPR_POW

%token EXPR_BIF1
%token EXPR_VAR EXPR_INT EXPR_REAL

%%

all:	|	ENDOFFILE 
		{
			finish = 1;
			YYACCEPT;
		}
	|	TEXT
		{ 
			fprintf(yyout, "%s", (char *)$<p>1);
			free($<p>1);
			YYACCEPT;
		} 
	|	parsable 
		{ 
			YYACCEPT;
		}
	;
parsable:	ENTRY actions params domain expression 
		{ 
			if(actions & REDUCE)
			{
				void * reduced = reduce_phase1($<p>5);
				if(domain == REAL)
				{
					void * domProp = reduce_phase2(reduced);
					delete(reduced);
					reduced = domProp;
				}
				delete($<p>5);
				$<p>5 = reduced;
			}
			if(actions & SHOW)
			{
				fprintf(yyout, "%s", op.wrap);
				if(negated($<p>5) && !isA($<p>5, Pow()))
					fprintf(yyout, "-");
				showTex($<p>5, &op);
				fprintf(yyout, "%s", op.wrap);
			}
			if(actions & DRAW)
			{
				int to_plot[2];
				int from_plot[2];
				fflush(yyout);
				fprintf(yyout, "\n\n");
				pipe(to_plot);
				pipe(from_plot);
				if(fork() == (pid_t) 0)
				{
					fclose(yyin);
					fclose(yyout);
					close(to_plot[1]);
					close(from_plot[0]);
					dup2(to_plot[0], STDIN_FILENO);
					dup2(from_plot[1], STDOUT_FILENO);
					execlp("gnuplot", "gnuplot", NULL);
					perror("Moo: Cannot call `gnuplot'.\n");
					exit(-1);
				}
				else
				{
					FILE * ftoplot = fdopen(to_plot[1], "w");
					char b;
					close(to_plot[0]);
					close(from_plot[1]);
					assert(ftoplot != NULL );
					fprintf(ftoplot, 
						"set terminal latex\n"
						"set format y \"$%%g$\"\n"
						"set format x \"$%%g$\"\n"
						"set border 3\n"
						"set xtics nomirror\n"
						"set ytics nomirror\n"
						"unset key\n");
					fprintf(ftoplot, "plot [%f:%f] [%f:%f] ", 
						(float)op.x_min, (float)op.x_max, 
						(float)op.y_min, (float)op.y_max);
					showPlot($<p>5, ftoplot, &op);
					fprintf(ftoplot, " with lines\nexit\n");
					fflush(ftoplot);
					while(read(from_plot[0], &b, 1) == 1) 
					{
						fprintf(yyout, "%c", b);
					}
					close(to_plot[1]);
					close(from_plot[0]);
					fprintf(yyout, "\n\n");
				}
			}
			delete($<p>5);
		}
	;
domain:		/* REAL domain */
	|	DOM_REAL { domain = REAL; }
	|	DOM_INTEGER { domain = INTEGER; }
actions:	ACTN_BEGIN action_list ACTN_END {}
	;
action_list:	ACTN_ACTION 
		{ 
			actions = $<a>1; 
		}
	|	action_list ACTN_SEP ACTN_ACTION
		{
			if($<a>3 & actions)
				lyyerror(@3, "duplicate actions.");
			actions = ($<a>3 | actions); 
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
			if(paramCk & P_PREC_SET)
				lyyerror(@1, "precision already set.");
			else
				paramCk |= P_PREC_SET;
			if(sscanf($<p>3, "%d", &prec) != 1 || prec <= 0)
				lyyerror(@3, "wrong precision."); 
			op.precision = prec;
			free($<p>3);
		}
	|	P_XMIN PRMS_SET PRMS_VAL
		{
			double xmin;
			if(paramCk & P_XMIN_SET)
				lyyerror(@1, "`xmin' already set.");
			else
				paramCk |= P_XMIN_SET;
			if(sscanf($<p>3, "%lf", &xmin) != 1)
				lyyerror(@3, "wrong value of `xmin'."); 
			op.x_min = xmin;
			free($<p>3);
		}
	|	P_XMAX PRMS_SET PRMS_VAL
		{
			double xmax;
			if(paramCk & P_XMAX_SET)
				lyyerror(@1, "`xmax' already set.");
			else
				paramCk |= P_XMAX_SET;
			if(sscanf($<p>3, "%lf", &xmax) != 1)
				lyyerror(@3, "wrong value of `xmax'."); 
			op.x_max = xmax;
			free($<p>3);
		}
	|	P_YMIN PRMS_SET PRMS_VAL
		{
			double ymin;
			if(paramCk & P_YMIN_SET)
				lyyerror(@1, "`ymin' already set.");
			else
				paramCk |= P_YMIN_SET;
			if(sscanf($<p>3, "%lf", &ymin) != 1)
				lyyerror(@3, "wrong value of `ymin'."); 
			op.y_min = ymin;
			free($<p>3);
		}	
	|	P_YMAX PRMS_SET PRMS_VAL
		{
			double ymax;
			if(paramCk & P_YMAX_SET)
				lyyerror(@1, "`ymax' already set.");
			else
				paramCk |= P_YMAX_SET;
			if(sscanf($<p>3, "%lf", &ymax) != 1)
				lyyerror(@3, "wrong value of `ymax'.");
			op.y_max = ymax;
			free($<p>3);
		}
	|	P_WRAP PRMS_SET P_WRAP_1
		{
			if(paramCk & P_WRAP_SET)
				lyyerror(@1, "`wrap' already set.");
			else
				paramCk |= P_WRAP_SET;
			op.wrap = "$";
		}
	|	P_WRAP PRMS_SET P_WRAP_2
		{
			if(paramCk & P_WRAP_SET)
				lyyerror(@1, "`wrap' already set.");
			else
				paramCk |= P_WRAP_SET;
			op.wrap = "$$";
		}
	;
expression:	EXPR_BEG expr EXPR_END 
		{ 
			$<p>$ = $<p>2; 
		}
	;

expr:		expr EXPR_ADD expr 
		{ 
			if(isA($<p>1, Sum()))
			{
				append($<p>1, $<p>3);
				$<p>$ = $<p>1;
			}
			else if(isA($<p>3, Sum()))
			{
				append($<p>3, $<p>1);
				$<p>$ = $<p>3;
			}
			else
			{
				$<p>$ = new(Sum());
				append($<p>$, $<p>1);
				append($<p>$, $<p>3);
			}
		}
	|	expr EXPR_SUB expr 
		{
			switchNegated($<p>3);
			if(isA($<p>1, Sum()))
			{
				append($<p>1, $<p>3);
				$<p>$ = $<p>1;
			}
			else if(isA($<p>3, Sum()))
			{
				append($<p>3, $<p>1);
				$<p>$ = $<p>3;
			}
			else
			{
				$<p>$ = new(Sum());
				append($<p>$, $<p>1);
				append($<p>$, $<p>3);
			}

		}
	|	expr EXPR_MUL expr 
		{ 
			if(isA($<p>1, Product()))
			{
				append($<p>1, $<p>3);
				$<p>$ = $<p>1;
			}
			else if(isA($<p>3, Product()))
			{
				append($<p>3, $<p>1);
				$<p>$ = $<p>3;
			}
			else
			{
				$<p>$ = new(Product());
				append($<p>$, $<p>1);
				append($<p>$, $<p>3);
			}
		}
	|	expr EXPR_DIV expr 
		{ 
			setReversed($<p>3, 1);
			if(isA($<p>1, Product()))
			{
				append($<p>1, $<p>3);
				$<p>$ = $<p>1;
			}
			else if(isA($<p>3, Product()))
			{
				append($<p>3, $<p>1);
				$<p>$ = $<p>3;
			}
			else
			{
				$<p>$ = new(Product());
				append($<p>$, $<p>1);
				append($<p>$, $<p>3);
			}
		}
	|	EXPR_SUB expr %prec NEGATE 
		{ 
			switchNegated($<p>2);
			$<p>$ = $<p>2; 
		}
	|	EXPR_LBR expr EXPR_RBR 
		{ 
			$<p>$ = $<p>2; 
		}
	|	expr EXPR_POW expr 
		{
			$<p>$ = new(Pow());
			setBase($<p>$, $<p>1);
			setPower($<p>$, $<p>3);
		}
	|	EXPR_INT
		{ 
			if(domain == REAL)
			{
				$<p>$ = domainCast($<p>1, Real());
				delete($<p>1);
			}
			else
				$<p>$ = $<p>1; 
		}
	|	EXPR_REAL
		{
			if(domain == INTEGER) lyyerror(@1, 
				"real number in integer domain");
			$<p>$ = $<p>1;
		}
	|	EXPR_BIF1 EXPR_LBR expr EXPR_RBR 
		{ 
			setArg($<p>1, $<p>3);
			$<p>$ = $<p>1; 
		}
	|	EXPR_DIFF EXPR_VAR expr %prec EXPR_DIFF
		{
			if(actions & DRAW) lyyerror(@1, 
				"differential in graphic.");
			if(isA($<p>3, Diff()))
			{
				diffBy($<p>3, $<p>2);
				$<p>$ = $<p>3;
			}
			else
			{
				$<p>$ = new(Diff());
				setArg($<p>$, $<p>3);
				diffBy($<p>$, $<p>2);
			}
			free($<p>2);
		}
	|	EXPR_VAR EXPR_LBR expr EXPR_RBR 
		{ 
			size_t i;
			for(i = 0; i < varTableSize; ++i)
				if(!strcmp($<p>1, varTable[i].name) && 
					varTable[i].class != Function())
					lyyerror(@1, "different class for varable `%s'",
						varTable[i].name);
			if(i == varTableSize)
			{
				varTableSize++;
				varTable = realloc(varTable, varTableSize * sizeof(*varTable));
				varTable[varTableSize - 1].name = strdup($<p>1);
				varTable[varTableSize - 1].class = Function();
			}
			if(actions & DRAW) lyyerror(@2, 
				"nonfree varable in graphic.");
			$<p>$ = new(Function());
			setFName($<p>$, $<p>1);
			setArg($<p>$, $<p>3);
		}
	|	EXPR_VAR 
		{
			size_t i;
			for(i = 0; i < varTableSize; ++i)
				if(!strcmp($<p>1, varTable[i].name) && 
					varTable[i].class != Var())
					lyyerror(@1, "different class for varable `%s'",
						varTable[i].name);
			if(i == varTableSize)
			{
				varTableSize++;
				varTable = realloc(varTable, varTableSize * sizeof(*varTable));
				varTable[varTableSize - 1].name = strdup($<p>1);
				varTable[varTableSize - 1].class = Var();
			}
			if(varTableSize > 1 && (actions & DRAW))
				lyyerror(@1, "more than one varable for graphic");
			$<p>$ = new(Var());
			setName($<p>$, $<p>1);
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
