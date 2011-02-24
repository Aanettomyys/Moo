#ifndef __WORKER_H__
#define __WORKER_H__

#include <stdio.h>
#include "parser_param.h"

typedef struct
{
	FILE * fin;
	p_param_t p;
} worker_t;

int w_run(worker_t *);
void w_init(worker_t *, FILE *);
void w_flush(worker_t *, FILE *);

#endif // __WORKER_H__
