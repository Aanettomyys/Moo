CFLAGS	= -std=gnu99 -O0 -g -Wall -Wextra -W 
SOURCES	=lexer.c parser.c show.c main.c ast.c utils.c reduce.c worker.c
OBJECTS	= $(SOURCES:.c=.o)
LDFLAGS	= -lmpfr -lgmp

.PHONY: all clean

all: moo

moo: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJECTS): parser

parser:
	bison parser.y
	flex lexer.l

clean:
	rm -rf $(OBJECTS) parser.h parser.c lexer.h lexer.c moo

