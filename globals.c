#include "ast.h"
#include "lex.h"
#include "globals.h"
#include<stdio.h>

struct Ast TRUE;
struct Ast IVOID;
struct Ast FACTUALS;
//FILE* assemblyOut = NULL;

// This is to insert fake variables into things that are autoinserted, for example, true and void.
void gimmeGlobals(){
    TRUE.ty = X_ID;
    TRUE.lineno = 0;
    TRUE.capacity = 0;
    TRUE.childflag = 0;
    TRUE.sig = "true";
    TRUE.payload.fs.ident = "$true";
    IVOID.ty = X_TID;
    IVOID.lineno = 0;
    IVOID.capacity = 0;
    IVOID.childflag = 0;
    IVOID.payload.fs.ident = "$void";
    FACTUALS.ty = X_FACTUALSX;
    FACTUALS.lineno = 0;
    FACTUALS.capacity = 0;
    FACTUALS.childflag = 0;
    FACTUALS.payload.fs.ident = "ACTUALS";
};

void syntaxError(const char * c, int d){
fprintf(stderr, "Syntax Error: Unexpected Token at `%s` at line %d. ", c, d);
}

void lexerError(char * yytex, int lineno, int recount){
fprintf(stderr, "Lexer Warning: Skipping Bad Char `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, recount);
            if(recount > 10){
              fprintf(stderr, "LEXER ERROR Bad Characters 10 or more times, EXITING\n");
            }   
}

void lexerError2(char * yytex, int lineno, int recount){
  fprintf(stderr, "BITWISE 'OR' WAS LOCATED IN GOLF PROGRAM, TERMINATING `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, recount);
}

void lexerError3(char * yytex, int lineno, int recount){
  fprintf(stderr, "BITWISE 'AND' WAS LOCATED IN GOLF PROGRAM, TERMINATING `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, recount);
}

char * TRUEIDENTIFIER = "true";
