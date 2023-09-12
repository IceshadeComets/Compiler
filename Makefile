.PHONY: clean

CC=gcc
CFLAGS=-g -Wall
OBJ=main.o parse.tab.o lex.yy.o ast.o globals.o stack.o hashmap.o symbol.o cgen.o
YACC=bison
YFLAGS=-Wall -d
LEX=flex
LFLAGS=

golf: $(OBJ)
	$(CC) $(CFLAGS) -o golf $^

main.o: main.c parse.tab.h ast.h globals.h hashmap.h stack.h symbol.h cgen.h

ast.o: ast.c ast.h

globals.o: globals.c globals.h

symbol.o: symbol.c symbol.h ast.h stack.h hashmap.h

stack.o: stack.c stack.h

hashmap.o: hashmap.c hashmap.h

cgen.o: cgen.c cgen.h

lex.yy.c: lex.l parse.tab.h
	$(LEX) $(LFLAGS) $<

parse.tab.c parse.tab.h: parse.y ast.h globals.h
	$(YACC) $(YFLAGS) $<

clean:
	-rm *.yy.c
	-rm *.tab.h
	-rm *.tab.c
	-rm *.o
	-rm golf
