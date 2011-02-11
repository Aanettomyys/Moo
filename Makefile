CFLAGS= -Wall -std=gnu99 -g -Wextra
CC=gcc
OBJS=lexer.o parser.tab.o show.o lists.o \
	main.o ast.o utils.o reduce.o
LDFLAGS= -lgmp -lmpfr

all: $(OBJS)
	$(CC) -o test $(OBJS) $(LDFLAGS)
utils.o:
	$(CC) -c utils.c $(CFLAGS)

reduce.o:
	$(CC) -c reduce.c $(CFLAGS)

lists.o:
	$(CC) -c lists.c $(CFLAGS)

parser:
	bison --defines=parser.h parser.y
	flex --outfile=lexer.c --header-file=lexer.h lexer.l

lexer.o: parser
	$(CC) -c lexer.c $(CFLAGS)

parser.tab.o:
	$(CC) -c parser.tab.c $(CFLAGS)

show.o:
	$(CC) -c show.c $(CFLAGS)

main.o:
	$(CC) -c main.c $(CFLAGS)

ast.o:
	$(CC) -c ast.c $(CFLAGS)
clean:
	rm -rf *.o parser.h parser.tab.c lexer.h lexer.c test

test: all
	./test && echo "Done."
