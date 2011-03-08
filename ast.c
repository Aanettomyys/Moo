/*#include "utils.h"*/
#include "ast.h"

extern FILE * yyin;
extern FILE * yyout;

#ifndef __GNUC__
#define __attribute__(x) /* USE GCC, LUKE! */
#endif

/****************************  Core Objects  *****************************/

struct Object
{
	const struct Class * class;
};

struct Class
{
	const struct Object _;
	const struct Class * super;
	size_t size;
	void * (*ctor)(void *, va_list *);
	void * (*dtor)(void *);
};

/******************************  Properties  *****************************/

const void * classOf(const void * _self)
{
	const struct Object * self = _self;
	assert(self && self->class);
	return self->class;
}

void * cast(const void * _self, const void * _class)
{
	assert(isOf(_self, _class));
	return (void *) _self;
}

size_t sizeOf(const void * _self)
{
	const struct Class * class = classOf(_self);
	return class->size;
}

void * ctor(void * _self, va_list * _app)
{
	const struct Class * class = classOf(_self);
	assert(class->ctor);
	return class->ctor(_self, _app);
}

void * dtor(void * _self)
{
	const struct Class * class = classOf(_self);
	assert(class->dtor);
	return class->dtor(_self);
}

const void * super(const void * _self)
{
	const struct Class * self = _self;
	assert(self && self->super);
	return self->super;
}

void * super_ctor(const void * _class, void * _self, va_list * _app)
{
	const struct Class * superclass = super(_class);
	assert(_self && superclass->ctor);
	return superclass->ctor(_self, _app);
}

void * super_dtor(const void * _class, void * _self)
{
	const struct Class * superclass = super(_class);
	assert(_self && superclass->dtor);
	return superclass->dtor(_self);
}

int isA(const void * _self, const void * _class)
{
	return _self && classOf(_self) == _class;
}

int isOf(const void * _self, const void * _class)
{
	if(_self)
	{
		const struct Class * class = classOf(_self);
		if(class != Object())
			while(class != _class)
				if(class != Object())
					class = super(class);
				else
					return 0;
		return 1;
	}
	return 0;
}

/***************************** Object method's ***************************/

static void * Object_ctor(void * _self, 
	va_list * _app __attribute__((unused)))
{
	return _self;
}

static void * Object_dtor(void * _self)
{
	return _self;
}


/***************************** Class method's ****************************/

static void * Class_dtor(void * _self __attribute__((unused)))
{
	assert(0);
	return 0;
}

static void * Class_ctor(void * _self, va_list * _app)
{
	struct Class * self = _self;
	const size_t offset = offsetof(struct Class, ctor);
	self->super = va_arg(* _app, struct Class *);
	self->size = va_arg(* _app, size_t);
	assert(self->super);
	memcpy((char *) self + offset, (char *) self->super + offset, 
		sizeOf(self->super) - offset);
	{
		typedef void (* fptr)();
		fptr selector;
		va_list ap;
#ifdef __va_copy
		__va_copy(ap, * _app);
#else
		ap = * _app;
#endif
		while((selector = va_arg(ap, fptr)))
		{
			fptr method = va_arg(ap, fptr);
			if(selector == (fptr)ctor)
				* (fptr *) & self->ctor = method;
			else if(selector == (fptr)dtor)
				* (fptr *) & self->dtor = method;
		}
		return self;
	}
}

/****************************** Core instance ****************************/

static const struct Class object [] = {
	{ 
		{object + 1},
		object, sizeof(struct Object),
		Object_ctor, Object_dtor
	},
	{
		{object + 1},
		object, sizeof(struct Class),
		Class_ctor, Class_dtor
	}
};

static const void * _Object = object;
static const void * _Class = object + 1;

const void * Object(void)
{
	return _Object;
}

const void * Class(void)
{
	return _Class;
}


/******************************* Core generics ***************************/

void delete(void * _self)
{
	if(_self) free(dtor(_self));
}

void * new(const void * _class, ...)
{
	const struct Class * class = _class;
	struct Object * object;
	va_list ap;
	assert(class && class->size);
	object = calloc(1, class->size);
	assert(object);
	object->class = _class;
	va_start(ap, _class);
	object = ctor(object, &ap);
	va_end(ap);
	return object;
}

/****************************** Math metaclass ***************************/

struct MathClass
{
	const struct Class _;
	void * (*derive)(const void *, const char *);
	int (*isDependOn)(const void *, const char *);
	int (*equal)(const void *, const void *);
	void * (*copy)(const void *);
	void (*append)(void *, void *);
	void (*switchNegated)(void *);
};


/******************************** Methods ********************************/

static void * MathClass_ctor(void * _self, va_list * _app)
{
	struct MathClass * self = 
		super_ctor(MathClass(), _self, _app);
	typedef void (*fptr)();
	fptr selector;
	va_list ap;
#ifdef __va_copy
	__va_copy(ap, * _app);
#else
	ap = * _app;
#endif
	while((selector = va_arg(ap, fptr)))
	{
		fptr method = va_arg(ap, fptr);
		if(selector == (fptr) derive)
			* (fptr *) & self->derive = method;
		else if(selector == (fptr) isDependOn)
			* (fptr *) & self->isDependOn = method;
		else if(selector == (fptr) copy)
			* (fptr *) & self->copy = method;
		else if(selector == (fptr) equal)
			* (fptr *) & self->equal = method; 
		else if(selector == (fptr) switchNegated)
			* (fptr *) & self->switchNegated = method;
		else if(selector == (fptr) append)
			* (fptr *) & self->append = method;
	}
	return self;
}


/******************************** Generics *******************************/

void * derive(const void * _self, const char * _by) 
{
	const struct MathClass * const * class = _self;
	void * r;
	assert(_self && *class && (*class)->derive);
	if(!isDependOn(_self, _by))
		return new(Integer());
	if(reversed(_self))
	{
		struct Product * p = new(Product());
		struct MathObject * _copy = copy(_self);
		struct Pow * pw = new(Pow());
		struct Integer * ipow = new(Integer());
		setIValString(ipow, "2");
		switchNegated(ipow);
		switchNegated(_copy);
		setBase(pw, _copy);
		setPower(pw, ipow);
		append(p, pw);
		append(p, (*class)->derive(_copy, _by));
		return p;
	}
	r = (*class)->derive(_self, _by);
	if(negated(_self) != negated(r))
		switchNegated(r);
	return r;
}

int isDependOn(const void * _self, const char * _by)
{
	const struct MathClass * const * class = _self;
	assert(_self && *class && (*class)->isDependOn);
	return (*class)->isDependOn(_self, _by);
}

void switchNegated(void * _self)
{
	const struct MathClass * const * class = _self;
	assert(_self && *class && (*class)->switchNegated);
	(*class)->switchNegated(_self);
}

static int super_equal(const void * _class, 
	const void * _self, const void * _another)
{
	const struct MathClass * superclass = 
		cast(super(_class), MathClass());
	assert(_self && _another && superclass->equal);
	return superclass->equal(_self, _another);
}

int equal(const void * _self, const void * _another)
{
	const struct MathClass * const * class = _self;
	assert(_self && *class && (*class)->equal);
	return isA(_another, classOf(_self)) &&
		(*class)->equal(_self, _another);
}

static void * super_copy(const void * _class, const void * _self)
{
	const struct MathClass * superclass =
		cast(super(_class), MathClass());
	assert(_self && superclass->copy);
	return superclass->copy(_self);
}

void * copy(const void * _self)
{
	const struct MathClass * const * class = _self;
	assert(_self && *class && (*class)->copy);
	return (*class)->copy(_self);
}

void append(void * _self, void * _arg)
{
	const struct MathClass * const * class = _self;
	assert(_self && *class && (*class)->append);
	(*class)->append(_self, _arg);
}


/****************************** Instance *********************************/

static const void * _MathClass;

const void * MathClass(void)
{
	return _MathClass ? _MathClass :
		(_MathClass = new(Class(),
			Class(), sizeof(struct MathClass),
			ctor, MathClass_ctor,
			0));
}

/***************************** Math object *******************************/

struct MathObject
{
	const struct Object _;
	char negated;
	char reversed;
};


/********************************* Methods *******************************/

static void * MathObject_ctor(void * _self, va_list * _app)
{
	struct MathObject * self = super_ctor(MathObject(), _self, _app);
	self->negated = 0;
	self->reversed = 0;
	return self;
}

static void * MathObject_copy(const void * _self)
{
	const struct MathObject * self = cast(_self, MathObject());
	struct MathObject * copy = cast(new(classOf(_self)), MathObject());
	copy->negated = self->negated;
	copy->reversed = self->reversed;
	return copy;
}

static int MathObject_equal(const void * _self, const void * _another)
{
	const struct MathObject * self = cast(_self, MathObject());
	const struct MathObject * another = cast(_another, MathObject());
	return self->negated == another->negated &&
		self->reversed == another->reversed;
}

/******************************* Properties ******************************/
char negated(const void * _self)
{
	const struct MathObject * self = cast(_self, MathObject());
	return self->negated;
}

void MathObject_switchNegated(void * _self)
{
	struct MathObject * self = cast(_self, MathObject());
	self->negated = !self->negated;
}

char reversed(const void * _self)
{
	const struct MathObject * self = cast(_self, MathObject());
	return self->reversed;
}

void setReversed(void * _self, char r)
{
	struct MathObject * self = cast(_self, MathObject());
	self->reversed = r;
}

/********************************** Instance *****************************/

static const void * _MathObject;

const void * MathObject(void)
{
	return _MathObject ? _MathObject :
		(_MathObject = new(MathClass(), Object(),
			sizeof(struct MathObject),
			ctor, MathObject_ctor, 
			copy, MathObject_copy,
			equal, MathObject_equal,
			switchNegated, MathObject_switchNegated,
			0));

}


/******************************* Show metaclass **************************/

struct MathShowClass
{
	const struct MathClass _;
	void (*showTex)(const void *, OutParams *);
};


/********************************* Methods *******************************/

static void * MathShowClass_ctor(void * _self, va_list * _app)
{
	struct MathShowClass * self = 
		super_ctor(MathShowClass(), _self, _app);
	typedef void (*fptr)();
	fptr selector;
	va_list ap;
#ifdef __va_copy
	__va_copy(ap, * _app);
#else
	ap = * _app;
#endif
	while((selector = va_arg(ap, fptr)))
	{
		fptr method = va_arg(ap, fptr);
		if(selector == (fptr) showTex)
			* (fptr *) & self->showTex = method;
	}
	return self;
}


/******************************** Generics *******************************/

void showTex(const void * _self, OutParams * _op) 
{
	const struct MathShowClass * const * class = _self;
	assert(_self && *class && (*class)->showTex);
	(*class)->showTex(_self, _op);
}


/****************************** Instance *********************************/

static const void * _MathShowClass;

const void * MathShowClass(void)
{
	return _MathShowClass ? _MathShowClass :
		(_MathShowClass = new(Class(),
			MathClass(), sizeof(struct MathShowClass),
			ctor, MathShowClass_ctor,
			0));
}



/************************ ShowPlot metaclass *****************************/

struct MathShowPlotClass
{
	const struct MathShowClass _;
	void (*showPlot)(const void * , FILE *, OutParams *);
};


/********************************** Methods ******************************/

static void * MathShowPlotClass_ctor(void * _self, va_list * _app)
{
	struct MathShowPlotClass * self =
		super_ctor(MathShowPlotClass(), _self, _app);
	typedef void (*fptr)();
	fptr selector;
	va_list ap;
#ifdef __va_copy
	__va_copy(ap, * _app);
#else
	ap = * _app;
#endif	
	while((selector = va_arg(ap, fptr)))
	{
		fptr method = va_arg(ap, fptr);
		if(selector == (fptr) showPlot)
			* (fptr *) & self->showPlot = method;
	}
	return self;
}


/******************************** Generics *******************************/

void showPlot(const void * _self, FILE * _fp, OutParams * _op) 
{
	const struct MathShowPlotClass * const * class = _self;
	assert(_self && *class && (*class)->showPlot);
	(*class)->showPlot(_self, _fp, _op);
}


/******************************* Instance ********************************/

static const void * _MathShowPlotClass;

const void * MathShowPlotClass(void)
{
	return _MathShowPlotClass ? _MathShowPlotClass :
		(_MathShowPlotClass = new(Class(),
			MathShowClass(),
			sizeof(struct MathShowPlotClass),
			ctor, MathShowPlotClass_ctor,
			0));
}


/************************ Domain metaclass *******************************/

struct DomainClass
{
	const struct MathShowPlotClass _;
	void * (*domainCast)(const void * , const void *);
};


/********************************** Methods ******************************/

static void * DomainClass_ctor(void * _self, va_list * _app)
{
	struct DomainClass * self =
		super_ctor(DomainClass(), _self, _app);
	typedef void (*fptr)();
	fptr selector;
	va_list ap;
#ifdef __va_copy
	__va_copy(ap, * _app);
#else
	ap = * _app;
#endif
	while((selector = va_arg(ap, fptr)))
	{
		fptr method = va_arg(ap, fptr);
		if(selector == (fptr) domainCast)
			* (fptr *) & self->domainCast = method;
	}
	return self;
}


/********************************* Generics *******************************/

void * domainCast(const void * _self, const void * _class) 
{
	const struct DomainClass * const * class = _self;
	assert(_self && *class && (*class)->domainCast);
	return (*class)->domainCast(_self, _class);
}


/******************************** Instance *******************************/

static const void * _DomainClass;

const void * DomainClass(void)
{
	return _DomainClass ? _DomainClass :
		(_DomainClass = new(Class(),
			MathShowPlotClass(),
			sizeof(struct DomainClass),
			ctor, DomainClass_ctor,
			0));
}


/******************************* Integer class ***************************/

struct Integer
{
	const struct MathObject _;
	mpz_t x;
};


/******************************** Methods ********************************/

static void * Integer_ctor(void * _self, va_list * _app)
{
	struct Integer * self = super_ctor(Integer(), _self, _app);
	mpz_init(self->x);
	return self;
}

static int Integer_isDependOn(
	const void * _self __attribute__((unused)), 
	const void * _by __attribute__((unused)))
{
	return 0;
}

static void * Integer_derive(
	const void * _self __attribute__((unused)), 
	const void * _by __attribute__((unused)))
{
	return new(Integer());
}

static void Integer_showTex(const void * _self, 
	OutParams * _op __attribute__((unused)))
{
	const struct Integer * self = cast(_self, Integer());
	if(reversed(_self))
	{
		fprintf(yyout, "\\frac{1}{");		
		mpz_out_str(yyout, 10, self->x);
		fprintf(yyout, "}");
	}
	else mpz_out_str(yyout, 10, self->x);
}

static void Integer_showPlot(const void * _self, 
	FILE * _fp, 
	OutParams * _op __attribute__((unused)))
{
	const struct Integer * self = cast(_self, Integer());
	if(negated(_self)) fprintf(_fp, "(-");
	if(reversed(_self))
	{
		fprintf(_fp, "(1/");
		mpz_out_str(_fp, 10, self->x);
		fprintf(_fp, ")");
	}
	else mpz_out_str(_fp, 10, self->x);
	if(negated(_self)) fprintf(_fp, ")");
}

static int Integer_equal(const void * _self, const void * _another)
{
	const struct Integer * self = cast(_self, Integer());
	const struct Integer * another = cast(_another, Integer());
	return super_equal(Integer(), _self, _another) &&
		!mpz_cmp(self->x, another->x);
}

static void * Integer_copy(const void * _self)
{
	const struct Integer * self = cast(_self, Integer());
	struct Integer * copy = super_copy(Integer(), _self);
	mpz_set(copy->x, self->x);
	return copy;
}

static void * Integer_domainCast(const void * _self, const void * _class)
{
	if(_class == Real())
	{
		const struct Integer * self = cast(_self, Integer());
		void * rval = new(Real());
		mpfr_t x;
		mpfr_init2(x, MPFR_PREC);
		mpfr_set_z(x, self->x, GMP_RNDZ);
		setRVal(rval, x);
		mpfr_clear(x);
		return rval;
	} 
	assert(0);
	return 0;
}

static void * Integer_dtor(void * _self)
{
	struct Integer * self = super_dtor(Integer(), _self);
	mpz_clear(self->x);
	return self;
}


/***************************** Properties ********************************/

void iVal(const void * _self, mpz_t v)
{
	const struct Integer * self = cast(_self, Integer());
	mpz_set(v , self->x);
}

void setIVal(void * _self, mpz_t i)
{
	struct Integer * self = cast(_self, Integer());
	mpz_set(self->x, i);
}

void setIValString(void * _self, const char * s)
{
	struct Integer * self = cast(_self, Integer());
	int r = mpz_set_str(self->x, s, 10);
	assert(!r);
}


/****************************** Instance *********************************/

static const void * _Integer;

const void * Integer(void)
{
	return _Integer ? _Integer :
		(_Integer = new(DomainClass(),
			MathObject(), sizeof(struct Integer),
			ctor, Integer_ctor,
			dtor, Integer_dtor,
			derive, Integer_derive,
			equal, Integer_equal,
			copy, Integer_copy,
			showTex, Integer_showTex,
			showPlot, Integer_showPlot,
			isDependOn, Integer_isDependOn,
			domainCast, Integer_domainCast,
			0));
}


/******************************* Real class ******************************/

struct Real
{
	const struct MathObject _;
	mpfr_t x;
};


/********************************* Methods *******************************/

static void * Real_ctor(void * _self, va_list * _app)
{
	struct Real * self = super_ctor(Real(), _self, _app);
	mpfr_init2(self->x, MPFR_PREC);
	mpfr_set_d(self->x, 0.0, GMP_RNDZ);
	return self;
}

static int Real_isDependOn(void * _self __attribute__((unused)))
{
	return 0;
}

static void * Real_derive(
	const void * _self __attribute__((unused)), 
	const void * _by __attribute__((unused)))
{
	return new(Real());
}

static void * Real_copy(const void * _self)
{
	const struct Real * self = cast(_self, Real());
	struct Real * copy = super_copy(Real(), _self);
	mpfr_set(copy->x, self->x, GMP_RNDZ);
	return copy;
}

static int Real_equal(const void * _self, const void * _another)
{
	const struct Real * self = cast(_self, Real());
	const struct Real * another = cast(_another, Real());
	return super_equal(Real(), self, another)
		&& !mpfr_cmp(self->x, another->x);
}

static void Real_showTex(const void * _self, OutParams * _op)
{
	const struct Real * self = cast(_self, Real());
	if(reversed(_self))
	{
		fprintf(yyout, "\\frac{1}{");
		mpfr_out_str(yyout, 10, _op->precision,	
			self->x, GMP_RNDZ);
		fprintf(yyout, "}");
	}
	else mpfr_out_str(yyout, 10, _op->precision,
		self->x, GMP_RNDZ);
}

static void Real_showPlot(const void * _self, FILE * _fp, 
	OutParams * _op)
{
	const struct Real * self = cast(_self, Real());
	if(negated(_self)) fprintf(_fp, "(-");
	if(reversed(_self))
	{
		fprintf(_fp, "(1.0/");
		mpfr_out_str(_fp, 10, _op->precision,
			self->x, GMP_RNDZ);
		fprintf(_fp, ")");
	}
	else mpfr_out_str(_fp, 10, _op->precision,
		self->x, GMP_RNDZ);
	if(negated(_self)) fprintf(_fp, ")");
}


static void * Real_dtor(void * _self)
{
	struct Real * self = super_dtor(Real(), _self);
	mpfr_clear(self->x);
	return self;
}


/***************************** Properties ********************************/

void rVal(const void * _self, mpfr_t v)
{
	const struct Real * self = cast(_self, Real());
	mpfr_set(v , self->x, GMP_RNDZ);
}

void setRVal(void * _self, mpfr_t d)
{
	struct Real * self = cast(_self, Real());
	mpfr_set(self->x, d, GMP_RNDZ);
}

void setRValString(void * _self, const char * s)
{
	struct Real * self = cast(_self, Real());
	mpfr_set_str(self->x, s, 10, GMP_RNDZ);
}


/***************************** Instance **********************************/

static const void * _Real;

const void * Real(void)
{
	return _Real ? _Real :
		(_Real = new(DomainClass(),
			MathObject(), sizeof(struct Real),
			ctor, Real_ctor,
			dtor, Real_dtor,
			equal, Real_equal,
			copy, Real_copy,
			showTex, Real_showTex,
			showPlot, Real_showPlot,
			derive, Real_derive,
			isDependOn, Real_isDependOn,
			0));
}


/**************************** Apply_1 class ******************************/

struct Apply_1 
{
	const struct MathObject _;
	void * arg;
};


/********************************* Methods *******************************/

void * Apply_1_ctor(void * _self, va_list * _app)
{
	struct Apply_1 * self = 
		super_ctor(Apply_1(), _self, _app);
	self->arg = NULL;
	return self;
}

void * Apply_1_dtor(void * _self)
{
	struct Apply_1 * self = 
		super_dtor(Apply_1(), _self);
	if(self->arg) delete(self->arg);
	return self;
}

void * Apply_1_copy(const void * _self)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	struct Apply_1 * _copy = super_copy(Apply_1(), _self);
	if(self->arg) _copy->arg = copy(self->arg);
	return _copy;
}

int Apply_1_equal(const void * _self, const void * _another)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	const struct Apply_1 * another = cast(_another, Apply_1());
	return super_equal(Apply_1(), _self, _another) &&
		self->arg && another->arg &&
		equal(self->arg, another->arg);
}

int Apply_1_isDependOn(const void * _self, const void * _by)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	return self->arg && isDependOn(self->arg, _by);
}

/***************************** Properties ********************************/

void * arg(const void * _self)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	return self->arg;
}

void setArg(void * _self, void * _arg)
{
	struct Apply_1 * self = cast(_self, Apply_1());
	self->arg = _arg;
}

/************************** Instance *************************************/

static const void * _Apply_1;

const void * Apply_1(void)
{
	return _Apply_1 ? _Apply_1 :
		(_Apply_1 = new(MathShowClass(),
			MathObject(), sizeof(struct Apply_1),
			ctor, Apply_1_ctor,
			dtor, Apply_1_dtor,
			equal, Apply_1_equal,
			copy, Apply_1_copy,
			isDependOn, Apply_1_isDependOn,
			0));
}


/*************************** Combinator class ****************************/

struct Combinator
{
	struct MathObject _;
	size_t argc;
	void ** argv;
};


/************************** Methods **************************************/

static void * Combinator_ctor(void * _self, va_list * _app)
{
	struct Combinator * self = 
		super_ctor(Combinator(), _self, _app);
	self->argc = 0;
	self->argv = NULL;
	return self;
}

static int Combinator_isDependOn(const void * _self, const char * _by)
{
	const struct Combinator * self = cast(_self, Combinator());
	size_t i;
	int ds = 0;
	for(i = 0; i < self->argc; ++i)
		ds = ds || isDependOn(self->argv[i], _by);
	return ds;
}

static int Combinator_equal(const void * _self, const void * _another)
{
	const struct Combinator * self = cast(_self, Combinator());
	const struct Combinator * another = cast(_another, Combinator());
	int eql = 1;
	if(self->argc == another->argc)
	{
		size_t i;
		for(i = 0; i < self->argc; ++i)
			eql = eql && equal(self->argv[i], 
				another->argv[i]);
	}
	else
		eql = 0;
	return eql && super_equal(Combinator(), self, another);
}

static void * Combinator_copy(const void * _self)
{
	const struct Combinator * self = cast(_self, Combinator());
	struct Combinator * _copy = super_copy(Combinator(), _self);
	size_t i;
	_copy->argc = self->argc;
	_copy->argv = calloc(self->argc, sizeof(void*));
	for(i = 0; i < self->argc; ++i)
		_copy->argv[i] = copy(self->argv[i]);
	return _copy;
}

void * Combinator_dtor(void * _self)
{
	struct Combinator * self =
		super_dtor(Combinator(), _self);
	size_t i;
	for(i = 0; i < self->argc; ++i)
		delete(self->argv[i]);
	if(i) free(self->argv);
	return self;
}

/****************************** Properties *******************************/

void * argv(const void * _self, const size_t i)
{
	const struct Combinator * self = cast(_self, Combinator());
	assert(i < self->argc);
	return self->argv[i];
}

void * setArgv(void * _self, const size_t i, void * _arg)
{
	struct Combinator * self = cast(_self, Combinator());
	struct MathObject * arg = cast(_arg, MathObject());
	void * pushed;
	assert(i < self->argc);
	pushed = self->argv[i];
	self->argv[i] = arg;
	return pushed;
}


/**************************** Instance ***********************************/

static const void * _Combinator;
const void * Combinator(void)
{
	return _Combinator ? _Combinator :
		(_Combinator = new(MathShowPlotClass(),
			MathObject(), sizeof(struct Combinator),
			ctor, Combinator_ctor,
			dtor, Combinator_dtor,
			isDependOn, Combinator_isDependOn,
			copy, Combinator_copy,
			equal, Combinator_equal,
			0));
}


/************************** Sum class ************************************/

struct Sum
{
	struct Combinator _;
};

/*************************** Methods *************************************/


static void Sum_switchNegated(void * _self)
{
	struct Combinator * self = cast(_self, Combinator());
	size_t i;
	for(i = 0; i < self->argc; ++i)
		switchNegated(self->argv[i]);
}

static void Sum_append(void * _self, void * _arg)
{
	struct Combinator * self = cast(_self, Combinator());
	if(isA(_arg, Sum()))
	{
		struct Combinator * arg = cast(_arg, Combinator());
		size_t i;
		for(i = 0; i < arg->argc; ++i)
			append(self, copy(arg->argv[i]));
		delete(arg);
		return;
	}
	self->argc++;
	self->argv = realloc(self->argv, self->argc * sizeof(void*));
	self->argv[self->argc - 1] = _arg;
}


static void Sum_showTex(const void * _self, OutParams * _op)
{
	const struct Combinator * self = cast(_self, Combinator());
	size_t i;
	assert(self->argc > 1);
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	for(i = 0; i < self->argc; ++i)
	{
		if(negated(self->argv[i]) &&
			!isA(self->argv[i], Pow())) 
			fprintf(yyout, "-");
		else if(i) fprintf(yyout, " + ");
		showTex(self->argv[i], _op);
	}
	if(reversed(_self)) fprintf(yyout, "}"); 
}

static void Sum_showPlot(const void * _self, 
	FILE * _fp, OutParams * _op)
{
	const struct Combinator * self = cast(_self, Combinator());
	size_t i;
	assert(self->argc > 1);
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / (");
	for(i = 0; i < self->argc; ++i)
	{
		if(i) fprintf(_fp, "+");
		showPlot(self->argv[i], _fp, _op);
	}
	if(reversed(_self)) fprintf(_fp, "))");
	fprintf(_fp, ")");
}

static void * Sum_derive(const void * _self, const char * _by)
{
	const struct Combinator * self = 
		cast(_self, Combinator());
	struct Combinator * _copy = copy(_self);
	size_t i;
	for(i = 0; i < self->argc; ++i)
		_copy->argv[i] = derive(self->argv[i], _by);
	return _copy;
}

/************************* Instance **************************************/

static const void * _Sum;
const void * Sum(void)
{
	return _Sum ? _Sum :
		(_Sum = new(MathShowPlotClass(),
			Combinator(), sizeof(struct Sum),
			derive, Sum_derive,
			showTex, Sum_showTex,
			showPlot, Sum_showPlot,
			switchNegated, Sum_switchNegated,
			append, Sum_append,
			0));
}


/************************** Product class ********************************/

struct Product
{
	struct Combinator _;
};

/*************************** Methods *************************************/

static void Product_append(void * _self, void * _arg)
{
	struct Combinator * self = cast(_self, Combinator());
	if(isA(_arg, Product()))
	{
		struct Combinator * arg = cast(_arg, Combinator());
		size_t i;
		if(negated(_self) != negated(_arg))
			switchNegated(_self);
		for(i = 0; i < arg->argc; ++i)
			append(_self, copy(arg->argv[i]));
		delete(arg);
		return;
	}
	if(negated(_arg))
	{
		switchNegated(_arg);
		switchNegated(_self);
	}
	self->argc++;
	self->argv = realloc(self->argv, self->argc * sizeof(void*));
	self->argv[self->argc - 1] = _arg;
}

static void Product_showTex(const void * _self, OutParams * _op)
{
	const struct Combinator * self = cast(_self, Combinator());
	size_t i;
	assert(self->argc > 1);
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	for(i = 0; i < self->argc; ++i)
	{
		if(i) fprintf(yyout, " * ");
		if(isA(self->argv[i], Sum()))
			fprintf(yyout, "(");
		showTex(self->argv[i], _op);
		if(isA(self->argv[i], Sum()))
			fprintf(yyout, ")");
	}
	if(reversed(_self)) fprintf(yyout, "}"); 
}

static void Product_showPlot(const void * _self, 
	FILE * _fp, OutParams * _op)
{
	const struct Combinator * self = cast(_self, Combinator());
	size_t i;
	assert(self->argc > 1);
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / (");
	for(i = 0; i < self->argc; ++i)
	{
		if(i) fprintf(_fp, "*");
		showPlot(self->argv[i], _fp, _op);
	}
	if(reversed(_self)) fprintf(_fp, "))");
	fprintf(_fp, ")");
}

static void * Product_derive(const void * _self, const char * _by)
{
	const struct Combinator * self = 
		cast(_self, Combinator());
	void * _copy = new(Sum());
	struct Combinator * el;
	size_t i;
	for(i = 0; i < self->argc; ++i)
	{
		el = copy(self);
		delete(el->argv[i]);
		el->argv[i] = derive(self->argv[i], _by);
		append(_copy, el);
	}
	return _copy;
}

/************************* Instance **************************************/

static const void * _Product;
const void * Product(void)
{
	return _Product ? _Product :
		(_Product = new(MathShowPlotClass(),
			Combinator(), sizeof(struct Product),
			derive, Product_derive,
			showTex, Product_showTex,
			showPlot, Product_showPlot,
			append, Product_append,
			0));
}


/***************************** Power class *******************************/

struct Pow
{
	struct MathObject _;
	void * base;
	void * power;
};

/***************************** Methods ***********************************/

static void * Pow_ctor(void * _self, va_list * _app)
{
	struct Pow * self = super_ctor(Pow(), _self, _app);
	self->base = NULL;
	self->power = NULL;
	return self;
}

static int Pow_isDependOn(const void * _self, const char * _by)
{
	const struct Pow * self = cast(_self, Pow());
	assert(self->base && self->power);
	return isDependOn(self->base, _by) ||
		isDependOn(self->power, _by);
}

static int Pow_equal(const void * _self, const void * _another)
{
	const struct Pow * self = cast(_self, Pow());
	const struct Pow * another = cast(_self, Pow());
	return super_equal(Pow(), _self, _another) &&
		equal(self->base, another->base) &&
		equal(self->power, another->power);
}

static void * Pow_copy(const void * _self)
{
	const struct Pow * self = cast(_self, Pow());
	struct Pow * _copy = super_copy(Pow(), _self);
	_copy->base = copy(self->base);
	_copy->power = copy(self->power);
	return _copy;
}

static void Pow_showTex(const void * _self, OutParams * _app)
{
	const struct Pow * self = cast(_self, Pow());
	if(negated(_self) || reversed(_self) ||
		isOf(self->base, Combinator()))
		fprintf(yyout, "(");
	if(negated(_self)) fprintf(yyout, "-");
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	if(negated(self->base) && !isA(self->base, Pow()))
		fprintf(yyout, "-");
	showTex(self->base, _app);
	if(reversed(_self)) fprintf(yyout, "}");
	if(negated(_self) || reversed(_self) ||
		isOf(self->base, Combinator()))
		fprintf(yyout, ")");
	fprintf(yyout, " ^ {");
	if(negated(self->power) && !isA(self->base, Pow()))
		fprintf(yyout, "-");
	showTex(self->power, _app);
	fprintf(yyout, "}");
}

static void Pow_showPlot(const void * _self, 
	FILE * _fp, OutParams * _app)
{
	const struct Pow * self = cast(_self, Pow());
	fprintf(_fp, "((");
	if(negated(_self)) fprintf(_fp, "(-");
	if(reversed(_self)) fprintf(_fp, "(1/(");
	showPlot(self->base, _fp, _app);
	if(reversed(_self)) fprintf(_fp, "))");
	if(negated(_self)) fprintf(_fp, ")");
	fprintf(_fp, ")**(");
	showPlot(self->power, _fp, _app);
	fprintf(_fp, "))");
}

static void * Pow_derive(const void * _self, const char * _by)
{
	const struct Pow * self = cast(_self, Pow());
	struct Product * left = new(Product());
	struct Product * right = new(Product());
	assert(self->base && self->power);
	if(isDependOn(self->base, _by))
	{
		void * f = copy(self->base);
		setReversed(f, 1);
		append(left, copy(self));
		append(left, derive(self->base, _by));
		append(left, copy(self->power));
		append(left, f);
	}
	else
	{
		delete(left);
		left = NULL;
	}
	if(isDependOn(self->power, _by))
	{
		void * f = new(Ln());
		setArg(f, copy(self->base));
		append(right, derive(self->power, _by));
		append(right, f);
		append(right, copy(self));
	}
	else
	{
		delete(right);
		right = NULL;
	}
	if(left && !right) return left;
	else if(right && !left) return right;
	else
	{
		struct Combinator * sum = new(Sum());
		append(sum, left);
		append(sum, right);
		return sum;
	}
}

static void * Pow_dtor(void * _self)
{
	struct Pow * self = super_dtor(Pow(), _self);
	assert(self->base && self->power);
	delete(self->base);
	delete(self->power);
	return self;
}


/*************************** Properties **********************************/

void * power(void * _self)
{
	struct Pow * self = cast(_self, Pow());
	return self->power;
}

void setPower(void * _self, void * _power)
{
	struct Pow * self = cast(_self, Pow());
	struct MathObject * power = cast(_power, MathObject());
	self->power = power;
}

void * base(void * _self)
{
	struct Pow * self = cast(_self, Pow());
	return self->base;
}

void setBase(void * _self, void * _base)
{
	struct Pow * self = cast(_self, Pow());
	struct MathObject * base = cast(_base, MathObject());
	self->base = base;
}


/************************** Instance *************************************/

static const void * _Pow;
const void * Pow(void)
{
	return _Pow ? _Pow :
		(_Pow = new(MathShowPlotClass(),
			MathObject(), sizeof(struct Pow),
			ctor, Pow_ctor,
			dtor, Pow_dtor,
			derive, Pow_derive,
			showTex, Pow_showTex,
			showPlot, Pow_showPlot,
			equal, Pow_equal,
			copy, Pow_copy,
			isDependOn, Pow_isDependOn,
			0));
}


/**************************** Sin ****************************************/

struct Sin
{
	struct Apply_1 _;
};


/************************* Methods ***************************************/

static void Sin_showTex(const void * _self, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "\\sin(");
	if(negated(self->arg) && !isA(self->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self->arg, _op);
	fprintf(yyout, ")");
	if(reversed(_self)) fprintf(yyout, "}");
}

static void Sin_showPlot(const void * _self,
	FILE * _fp, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / ");
	fprintf(_fp, "sin(");
	showPlot(self->arg, _fp, _op);
	fprintf(_fp, ")");
	if(reversed(_self)) fprintf(_fp, ")");
	fprintf(_fp, ")");
}

static void * Sin_derive(const void * _self, const char * _by)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	struct Apply_1 * _copy = cast(new(Cos()), Apply_1());
	struct Combinator * _prod = 
		cast(new(Product()), Combinator());
	_copy->arg = copy(self->arg);
	append(_prod, _copy);
	append(_prod, derive(self->arg, _by));
	return _prod;
}


/*************************** Instance ************************************/

static const void * _Sin;
const void * Sin(void)
{
	return _Sin ? _Sin :
		(_Sin = new(MathShowPlotClass(),
		Apply_1(), sizeof(struct Sin),
		showTex, Sin_showTex,
		showPlot, Sin_showPlot,
		derive, Sin_derive,
		0));
}


/**************************** Cos ****************************************/

struct Cos
{
	struct Apply_1 _;
};


/************************* Methods ***************************************/

static void Cos_showTex(const void * _self, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "\\cos(");
	if(negated(self->arg) && !isA(self->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self->arg, _op);
	fprintf(yyout, ")");
	if(reversed(_self)) fprintf(yyout, "}");
}

static void Cos_showPlot(const void * _self,
	FILE * _fp, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / ");
	fprintf(_fp, "cos(");
	showPlot(self->arg, _fp, _op);
	fprintf(_fp, ")");
	if(reversed(_self)) fprintf(_fp, ")");
	fprintf(_fp, ")");
}

static void * Cos_derive(const void * _self, const char * _by)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	struct Apply_1 * _copy = cast(new(Sin()), Apply_1());
	struct Combinator * _prod = 
		cast(new(Product()), Combinator());
	switchNegated(_copy);
	_copy->arg = copy(self->arg);
	append(_prod, _copy);
	append(_prod, derive(self->arg, _by));
	return _prod;
}


/*************************** Instance ************************************/

static const void * _Cos;
const void * Cos(void)
{
	return _Cos ? _Cos :
		(_Cos = new(MathShowPlotClass(),
		Apply_1(), sizeof(struct Cos),
		showTex, Cos_showTex,
		showPlot, Cos_showPlot,
		derive, Cos_derive,
		0));
}

/**************************** Tan ****************************************/

struct Tan
{
	struct Apply_1 _;
};


/************************* Methods ***************************************/

static void Tan_showTex(const void * _self, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "\\tan(");
	if(negated(self->arg) && !isA(self->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self->arg, _op);
	fprintf(yyout, ")");
	if(reversed(_self)) fprintf(yyout, "}");
}

static void Tan_showPlot(const void * _self,
	FILE * _fp, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / ");
	fprintf(_fp, "tan(");
	showPlot(self->arg, _fp, _op);
	fprintf(_fp, ")");
	if(reversed(_self)) fprintf(_fp, ")");
	fprintf(_fp, ")");
}

static void * Tan_derive(const void * _self, const char * _by)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	struct Apply_1 * _copy1 = cast(new(Cos()), Apply_1());
	struct Apply_1 * _copy2 = cast(new(Cos()), Apply_1());
	struct Combinator * _prod = 
		cast(new(Product()), Combinator());
	_copy1->arg = copy(self->arg);
	_copy2->arg = copy(self->arg);
	setReversed(_copy1, 1);
	setReversed(_copy2, 1);
	append(_prod, _copy1);
	append(_prod, _copy2);
	append(_prod, derive(self->arg, _by));
	return _prod;
}


/*************************** Instance ************************************/

static const void * _Tan;
const void * Tan(void)
{
	return _Tan ? _Tan :
		(_Tan = new(MathShowPlotClass(),
		Apply_1(), sizeof(struct Tan),
		showTex, Tan_showTex,
		showPlot, Tan_showPlot,
		derive, Tan_derive,
		0));
}


/***************************** Ln ****************************************/

struct Ln
{
	struct Apply_1 _;
};


/************************* Methods ***************************************/

static void Ln_showTex(const void * _self, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "\\ln(");
	if(negated(self->arg) && !isA(self->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self->arg, _op);
	fprintf(yyout, ")");
	if(reversed(_self)) fprintf(yyout, "}");
}

static void Ln_showPlot(const void * _self,
	FILE * _fp, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "(1 / ");
	fprintf(_fp, "ln(");
	showPlot(self->arg, _fp, _op);
	fprintf(_fp, ")");
	if(reversed(_self)) fprintf(_fp, ")");
	fprintf(_fp, ")");
}

static void * Ln_derive(const void * _self, 
	const char * _by __attribute__((unused)))
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	void * x = copy(self->arg);
	setReversed(x, 1);
	return x;
}


/*************************** Instance ************************************/

static const void * _Ln;
const void * Ln(void)
{
	return _Ln ? _Ln :
		(_Ln = new(MathShowPlotClass(),
		Apply_1(), sizeof(struct Ln),
		showTex, Ln_showTex,
		showPlot, Ln_showPlot,
		derive, Ln_derive,
		0));
}


/**************************** Var ****************************************/

struct Var
{
	struct MathObject _;
	char * name;
};


/************************* Methods ***************************************/

static void Var_showTex(const void * _self, 
	OutParams * _op __attribute__((unused)))
{
	const struct Var * self = cast(_self, Var());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "%s", self->name);
	if(reversed(_self)) fprintf(yyout, "}");
}

static void Var_showPlot(const void * _self __attribute__((unused)),
	FILE * _fp, OutParams * _op __attribute__((unused)))
{
	fprintf(_fp, "(");
	if(negated(_self)) fprintf(_fp, "-");
	if(reversed(_self)) fprintf(_fp, "1 / ");
	fprintf(_fp, "x");
	fprintf(_fp, ")");
}

static void * Var_derive(const void * _self __attribute__((unused)), 
	const char * _by __attribute__((unused)))
{
	void * i = new(Integer());
	setIValString(i, "1");
	return i;
}

static void * Var_ctor(void * _self, va_list * _app)
{
	struct Var * self = super_ctor(Var(), _self, _app);
	self->name = NULL;
	return self;
}

static void * Var_dtor(void * _self)
{
	struct Var * self = super_dtor(Var(), _self);
	free(self->name);
	return self;
}

static void * Var_copy(const void * _self)
{
	const struct Var * self = cast(_self, Var());
	struct Var * _copy = super_copy(Var(), _self);
	_copy->name = strdup(self->name);
	return _copy;
}

static int Var_equal(const void * _self, const void * _another)
{
	const struct Var * self = cast(_self, Var());
	const struct Var * another = cast(_another, Var());
	return super_equal(Var(), _self, _another) &&
		!strcmp(self->name, another->name);
}

static int Var_isDependOn(const void * _self, const char * _by)
{
	const struct Var * self = cast(_self, Var());
	return !strcmp(self->name, _by);
}

/***************************** Properties ********************************/

const char * name(const void * _self)
{
	const struct Var * self = cast(_self, Var());
	return self->name;
}

void setName(void * _self, char * _name)
{
	struct Var * self = cast(_self, Var());
	if(self->name) free(self->name);
	self->name = _name;
}

/*************************** Instance ************************************/

static const void * _Var;
const void * Var(void)
{
	return _Var ? _Var :
		(_Var = new(MathShowPlotClass(),
		MathObject(), sizeof(struct Var),
		ctor, Var_ctor,
		dtor, Var_dtor,
		equal, Var_equal,
		isDependOn, Var_isDependOn,
		copy, Var_copy,
		showTex, Var_showTex,
		showPlot, Var_showPlot,
		derive, Var_derive,
		0));
}


/************************* Diff class ************************************/

struct Diff
{
	struct Apply_1 _;
	int final;
	size_t diffp;
	char ** by;
};

/*************************** Methods *************************************/

static void * Diff_ctor(void * _self, va_list * _app)
{
	struct Diff * self = super_ctor(Diff(), _self, _app);
	self->diffp = 0;
	self->by = NULL;
	self->final = 0;
	return self;
}

static void * Diff_dtor(void * _self)
{
	struct Diff * self = super_dtor(Diff(), _self);
	size_t i;
	for(i = 0; i < self->diffp; ++i)
		free(self->by[i]);
	if(i) free(self->by);
	return self;
}

static int Diff_equal(const void * _self, const void * _another)
{
	const struct Diff * self = cast(_self, Diff());
	const struct Diff * another = cast(_another, Diff());
	size_t i;
	int eql = self->diffp == another->diffp &&
		self->final == another->final;
	if(!eql) return 0;
	for(i = 0; i < self->diffp; ++i)
		eql = eql && !strcmp(self->by[i], another->by[i]);
	return eql && super_equal(Diff(), _self, _another);
}

static int Diff_isDependOn(const void * _self, const char * _by)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	return isDependOn(self->arg, _by);
}

static void * Diff_copy(const void * _self)
{
	const struct Diff * self = cast(_self, Diff());
	struct Diff * _copy = super_copy(Diff(), _self);
	size_t i;
	_copy->diffp = self->diffp;
	_copy->final = self->final;
	_copy->by = calloc(_copy->diffp, sizeof(char *));
	for(i = 0; i < _copy->diffp; ++i)
		_copy->by[i] = strdup(self->by[i]);
	return _copy;
}

static void Diff_showTex(const void * _self, OutParams * _op)
{
	const struct Diff * self = cast(_self, Diff());
	const struct Apply_1 * self_1 = cast(_self, Apply_1());
	size_t i;
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	if(self->diffp == 1) fprintf(yyout, "\\frac{\\mathrm{d}");
	else fprintf(yyout, "\\frac{\\partial ^{%lu}", 
		(unsigned long) self->diffp);
	if(negated(self_1->arg) && !isA(self_1->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self_1->arg, _op);
	fprintf(yyout, "}{");
	if(self->diffp == 1) 
		fprintf(yyout, "\\mathrm{d}%s}", self->by[0]);
	else
	{
		for(i = 0; i < self->diffp; ++i)
			fprintf(yyout, "\\partial%s", self->by[i]);
		fprintf(yyout, "}");
	}
	if(reversed(_self)) fprintf(yyout, "}");
}

static void * Diff_derive(const void * _self, const char * _by)
{
	const struct Diff * self = cast(_self, Diff());
	const struct Apply_1 * self_1 = cast(_self, Apply_1());
	if(self->final) 
	{
		void * _copy = copy(self);
		diffBy(_copy, _by);
		return _copy;
	}
	else
	{
		void * _copy = copy(self_1->arg);
		size_t i;
		for(i = 0; i < self->diffp; ++i)
		{
			void * _copy_2 = derive(_copy, self->by[i]);
			delete(_copy);
			_copy = _copy_2;
		}
		return _copy;
	}
}


/*************************** Properties **********************************/

void diffBy(void * _self, const char * _by)
{
	struct Diff * self = cast(_self, Diff());
	self->diffp++;
	self->by = realloc(self->by, self->diffp * sizeof(char *));
	self->by[self->diffp - 1] = strdup(_by);
}


/************************** Instance *************************************/

static const void * _Diff;
const void * Diff(void)
{
	return _Diff ? _Diff :
		(_Diff = new(MathShowClass(),
			Apply_1(), sizeof(struct Diff),
			ctor, Diff_ctor,
			dtor, Diff_dtor,
			derive, Diff_derive,
			equal, Diff_equal,
			copy, Diff_copy,
			isDependOn, Diff_isDependOn,
			showTex, Diff_showTex,
			0));
}


/************************* Function class ********************************/

struct Function
{
	struct Apply_1 _;
	char * name;
};


/************************* Methods ***************************************/

static void * Function_ctor(void * _self, va_list * _app)
{
	struct Function * self = 
		super_ctor(Function(), _self, _app);
	self->name = NULL;
	return self;
}

static void * Function_dtor(void * _self)
{
	struct Function * self =
		super_dtor(Function(), _self);
	free(self->name);
	return self;
}

static int Function_equal(const void * _self, const void * _another)
{
	const struct Function * self = cast(_self, Function());
	const struct Function * another = cast(_another, Function());
	return super_equal(Function(), _self, _another) &&
		!strcmp(self->name, another->name);
}

static void * Function_copy(const void * _self)
{
	const struct Function * self = cast(_self, Function());
	struct Function * _copy = super_copy(Function(), _self);
	_copy->name = strdup(self->name);
	return _copy;
}

static void Function_showTex(const void * _self, OutParams * _op)
{
	const struct Apply_1 * self = cast(_self, Apply_1());
	const struct Function * selff = cast(_self, Function());
	if(reversed(_self)) fprintf(yyout, "\\frac{1}{");
	fprintf(yyout, "%s(", selff->name);
	if(negated(self->arg) && !isA(self->arg, Pow()))
		fprintf(yyout, "-");
	showTex(self->arg, _op);
	fprintf(yyout, ")");
	if(reversed(_self)) fprintf(yyout, "}");
}

static int Function_isDependOn(const void * _self, const char * _by)
{
	const struct Function * self = cast(_self, Function());
	const struct Apply_1 * self_1 = cast(_self, Apply_1());
	return !strcmp(_by, self->name) || 
		isDependOn(self_1->arg, _by);
}

static void * Function_derive(const void * _self, const char * _by)
{
	const struct Function * self = cast(_self, Function());
	if(!strcmp(self->name, _by))
	{	
		void * i = new(Integer());
		setIValString(i, "1");
		return i;
	}
	else
	{
		struct Diff * d = new(Diff());
		diffBy(d, _by);
		d->final = 1;
		setArg(d, copy(_self));
		return d;
	}
}


/***************************** Properties ********************************/

const char * fname(const void * _self)
{
	const struct Function * self = cast(_self, Function());
	return self->name;
}

void setFName(void * _self, char * _name)
{
	struct Function * self = cast(_self, Function());
	if(self->name) free(self->name);
	self->name = _name;
}

/*************************** Instance ************************************/

static const void * _Function;
const void * Function(void)
{
	return _Function ? _Function :
		(_Function = new(MathShowClass(),
		Apply_1(), sizeof(struct Function),
		ctor, Function_ctor,
		dtor, Function_dtor,
		equal, Function_equal,
		copy, Function_copy,
		showTex, Function_showTex,
		derive, Function_derive,
		isDependOn, Function_isDependOn,
		0));
}
