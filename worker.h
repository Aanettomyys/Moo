#ifndef __WORKER_H__
#define __WORKER_H__

#include "parser_param.h"

typedef struct
{
	FILE * fin;
	ParserParam p;
	ParserRList * plr;
} Worker;

void worker_clear(Worker * w);
int worker_run(Worker * w);
void worker_init(Worker * w, FILE * in);
void worker_flush(Worker * w, FILE * out);

#endif // __WORKER_H__
