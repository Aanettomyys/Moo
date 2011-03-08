CFLAGS	= -O0 -g -Wall -Wextra -W -pedantic
SOURCES	=lexer.c parser.c main.c ast.c reduce.c
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

