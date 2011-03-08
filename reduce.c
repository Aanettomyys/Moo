#include <assert.h>

#include "ast.h"

void * reduce_phase1(const void * _exp)
{
	if(isA(_exp, Integer()) || isA(_exp, Real()) || isA(_exp, Var()))
		return copy(_exp);
	else if(isA(_exp, Sum()) || isA(_exp, Product()))
	{
		void * s = copy(_exp);
		size_t i;
		for(i = 0; i < size(s); ++i)
		{
			delete(argv(s, i));
			setArgv(s, i, reduce_phase1(argv(_exp, i)));
		}
		return s;
	}
	else if(isA(_exp, Diff()))
	{
		return diffExec(_exp);
	}
	else if(isA(_exp, Pow()))
	{
		void * p = copy(_exp);
		delete(base(p));
		delete(power(p));
		setBase(p, reduce_phase1(base(_exp)));
		setPower(p, reduce_phase1(power(_exp)));
		return p;
	}
	else if(isOf(_exp, Apply_1()))
	{
		void * f = copy(_exp);
		delete(arg(f));
		setArg(f, reduce_phase1(arg(_exp)));
		return f;
	}
	assert(0);
}

void * reduce_phase2(const void * _exp)
{
	if(isA(_exp, Integer()))
		return domainCast(_exp, Real());
	else if(isA(_exp, Real()) || isA(_exp, Var()))
		return copy(_exp);
	else if(isA(_exp, Sum()) || isA(_exp, Product()))
	{
		void * s = copy(_exp);
		size_t i;
		for(i = 0; i < size(s); ++i)
		{
			delete(argv(s, i));
			setArgv(s, i, reduce_phase2(argv(_exp, i)));
		}
		return s;
	}
	else if(isA(_exp, Pow()))
	{
		void * p = copy(_exp);
		delete(base(p));
		delete(power(p));
		setBase(p, reduce_phase2(base(_exp)));
		setPower(p, reduce_phase2(power(_exp)));
		return p;
	}
	else if(isOf(_exp, Apply_1()))
	{
		void * f = copy(_exp);
		delete(arg(f));
		setArg(f, reduce_phase2(arg(_exp)));
		return f;
	}
	assert(0);
}
