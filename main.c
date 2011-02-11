#include "parser_param.h"
#include "parser.h"
#include "lexer.h"
#include "actions.h"

int yyparse(void * param);

static int initParserParam(ParserParam * param)
{
	int ret = 0;

	ret = yylex_init(&param->scanner);
	param->ast = NULL;
	param->lasta = NULL;
	return ret;
}

static int destroyParserParam(ParserParam * param)
{
	return yylex_destroy(param->scanner);
}

void runParser(FILE * fp)
{
	ParserParam p;
	YY_BUFFER_STATE state;
	initParserParam(&p);
	state = yy_create_buffer(fp, YY_BUF_SIZE, p.scanner);
	yy_switch_to_buffer(state, p.scanner);
	yyparse(&p);
	destroyParserParam(&p);
	printf("\n=================\n");
}

int main(int argc, char ** argv)
{
	FILE * fp = fopen("test.xtex", "r");
	runParser(fp);
	fclose(fp);
	return 0;
}
