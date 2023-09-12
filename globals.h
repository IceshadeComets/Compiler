#ifndef GLOBALS_H
#define GLOBALS_H
#include<stdio.h>

extern struct Ast TRUE;
extern struct Ast IVOID;
extern struct Ast FACTUALS;
extern char* inputFileName;
//extern FILE* assemblyOut;

void gimmeGlobals();
void symbolTableStart();
void syntaxError(const char * c, int d);
void lexerError(char * yytex, int lineno, int recount);
void lexerError2(char * yytex, int lineno, int recount);
void lexerError3(char * yytex, int lineno, int recount);

#endif