#ifndef __WORKER_H__
#define __WORKER_H__

#include "parser_param.h"

typedef struct
{
	char * in;
	char * out;
	ParserParam p;
	ParserRList * plr;
} Worker;

void worker_clear(Worker * w);
void worker_run(Worker * w);
void worker_init(Worker * w, const char * in);

#endif // __WORKER_H__
