#ifndef __AST_H__
#define __AST_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <gmp.h>
#include <mpfr.h>

#define PARAMS_DEFAULT(op) \
	do { \
		op.precision = 3; \
		op.x_min = 0.0; \
		op.x_max = 10.0; \
		op.y_min = 0.0; \
		op.y_max = 10.0; \
		op.wrap = "$"; \
	} while(0)

#define MPFR_PREC 128

typedef struct
{
	int precision;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	char * wrap;
} OutParams;

typedef enum 
{
	SHOW  	= 1 << 0,
	REDUCE	= 1 << 1,
	DRAW 	= 1 << 2
} Actions;

void showTex(const void * _self, OutParams * _op);
void showPlot(const void * _self, FILE * _fp, OutParams * _op);
void * domainCast(const void * _self, const void * _class);

const void * Object(void);
const void * Class(void);
/* Props */
int isA(const void * _self, const void * _class);
int isOf(const void * _self, const void * _class);
const void * classOf(const void * _self);
size_t sizeOf(const void * _self);
void * cast(const void * _self, const void * _class);
/* Generics */
void * new(const void * _class, ...);
void delete(void * _self);

const void * MathClass(void);
/* Generics */
void * derive(const void * _self, const char * _by); 
int isDependOn(const void * _self, const char * _by);
int equal(const void * _self, const void * _another);
void switchNegated(void * _self);
void * copy(const void * _self);
void append(void * _self, void * _arg);

const void * MathObject(void);
/* Props */
char negated(const void * _self);
char reversed(const void * _self);
void setReversed(void * _self, char r);

const void * MathShowClass(void);
/* Generics */
void showTex(const void * _self, OutParams * _op);

const void * MathShowPlotClass(void);
/* Generics */
void showPlot(const void * _self, FILE * _fp, OutParams * _op);

const void * DomainClass(void);
/* Generics */
void * domainCast(const void * _self, const void * _class);

const void * Integer(void);
/* Props */
void iVal(const void * _self, mpz_t v);
void setIVal(void * _self, mpz_t i);
void setIValString(void * _self, const char * s);

const void * Real(void);
/* Props */
void rVal(const void * _self, mpfr_t v);
void setRVal(void * _self, mpfr_t d);
void setRValString(void * _self, const char * s);

const void * Apply_1(void);
/* Props */
void * arg(const void * _self);
void setArg(void * _self, void * _arg);

const void * Combinator(void);
/* Props */
void * argv(const void * _self, const size_t i);
size_t size(const void * _self);
void * setArgv(void * _self, const size_t i, void * _arg);


const void * Sum(void);
const void * Product(void);

const void * Pow(void);
/* Props */
void * power(const void * _self);
void setPower(void * _self, void * _power);
void * base(const void * _self);
void setBase(void * _self, void * _base);

const void * Ln(void);
const void * Sin(void);
const void * Cos(void);
const void * Tan(void);

const void * Var(void);
/* Props */
const char * name(const void * _self);
void setName(void * _self, char * _name);

const void * Diff(void);
void diffBy(void * _self, const char * _by);
void * diffExec(const void * _self);

const void * Function(void);
const char * fname(const void * _self);
void setFName(void * _self, char * _name);

void * reduce_phase1(const void * _exp);
void * reduce_phase2(const void * _exp);
#endif
