#include "parser_param.h"
#include "parser.h"
#include "lexer.h"
#include "actions.h"
#include "worker.h"

void runParser(char * msg)
{
	Worker w;
	worker_init(&w, msg);
	worker_run(&w);
	printf("======= Парсинг закончен =======\n");
	ParserRList * it = w.p.head;
	while(it != NULL)
	{
		if(it->is_ast)
		{
			printf("Получено выражение: ");
			ast_action_show(it->p.a.ast, stdout);
			ast_action_reduce(it->p.a.ast);
			printf("\nРедукция: ");
			ast_action_show(it->p.a.ast, stdout);
			printf("\n");
		}
		else
		{
			printf("Получен текст : `%s'\n", it->p.s);
		}
		it = it->next;
	}
	printf("================================\n");
}

int main(int argc, char ** argv)
{
	FILE * fp = fopen("test.xtex", "r");
	char buff[100] = {0};
	while(fgets(buff, 99, fp) != NULL)
	{
		runParser(buff);
		memset(buff, 0, 100 * sizeof(char));
	}
	fclose(fp);
	return 0;
}
