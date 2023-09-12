%option noyywrap

%option noinput

%{
// Lexer Function, originally modifed from Tutorial Section to work with the GoLF Grammer Specification.

#include<stdio.h>
#include"lex.h"
#include "parse.tab.h"
#include<string.h>

%}
  /* Main Routines */
%{
int lineno = 1;
char lasttoken = 'x';
int yyrecount = 0;
int dualmatch = 0;
int matchflag = 0;
%}


alph    [a-zA-Z_]
alphNum [a-zA-Z_0-9]
num [0-9]
quote ["]
ASCII [\x00-\x7F]

  // start conditions
%x CMNT
%x STRING

%%
[ \r\t]+   {}
\n       { ++lineno; if(dualmatch > 2){yyrecount = 0; dualmatch = 0;}else{dualmatch++;}
        // Semi Colon Insert Function, Global Matchflag var to determine if last token is a ;
        if(matchflag == 1){
          unput(';');
          matchflag = 0;
        }
        }

<CMNT>{
  . {}
  \n {BEGIN(INITIAL); unput('\n');}
}

; {
  yylloc.first_line = lineno;
  yylloc.last_line = lineno;
  yylval.myString = strdup(yytext);
  lasttoken = ';';
  matchflag = 0;
  return T_SEMICOL;
  }

"//" {BEGIN(CMNT);}

{quote}  {BEGIN(STRING); yymore();}
<STRING>{
"\\b"   { BEGIN(STRING); yymore();}
"\\f"   { BEGIN(STRING); yymore();}
"\\r"   { BEGIN(STRING); yymore();}
"\\t"   { BEGIN(STRING); yymore();}
"\\\\"   { BEGIN(STRING); yymore();}
"\\n" {BEGIN(STRING); yymore();}
"\\\""   { BEGIN(STRING); yymore();}
"\\"(.|\n)  {fprintf(stderr, "bad literal found `%s` at lineno '%d' terminating program \n", yytext, lineno); exit(1);}
{quote}  { BEGIN(INITIAL); yymore(); matchflag = 1; 
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext); return T_STRING_LITERAL;}
\n {fprintf(stderr, "ERROR: END OF LINE FOUND BEFORE STRING END, `%s` + '%d' TERMINATING PROGRAM\n", yytext, lineno); exit(1);}
<<EOF>> {fprintf(stderr, "ERROR: END OF FILE FOUND BEFORE STRING END, TERMINATING PROGRAM\n"); exit(1);}
{ASCII} { BEGIN(STRING); yymore();}
}

"("       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            lasttoken = '(';
            matchflag = 0; return T_SBRACKET;
          }
")"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 1; return T_EBRACKET;
          }
"{"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            lasttoken = '{';
            matchflag = 0;
            return T_SBRACE;
          }
"}"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            if(lasttoken != ';'){
                yyless(0);
                lasttoken = ';';
                return T_SEMICOL;
            } else {
              lasttoken = '}';
              matchflag = 1; return T_EBRACE;
            }
          }
"*"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_MULT;
          }
"/"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_DIV;
          }
"%"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_MODULO;
          }
"+"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_PLUS;
          }
"-"       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_SUB;
          }
"=="      {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_EQ;
          }
"="       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_ASSIGN;
          }
"!="       {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_NE;
          }
"<="      {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 0;
            return T_LE;
          }
"<"     { 
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_LT;
        }
">="      {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_GE;
        }
">"     {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_GT;
        }
"&&"     {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_AND;
        }
"||"     {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_OR;
        }
","     {
        yylloc.first_line = lineno;
        yylloc.last_line = lineno;
        yylval.myString = strdup(yytext);
        matchflag = 0;
        return T_COMMA;
}
"!"     {
        yylloc.first_line = lineno;
        yylloc.last_line = lineno;
        yylval.myString = strdup(yytext);
        matchflag = 0;
        return T_NOT;
}
"break"   {
            yylloc.first_line = lineno;
            yylloc.last_line = lineno;
            yylval.myString = strdup(yytext);
            matchflag = 1; return T_BREAK;
          }
"else"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_ELSE;
          }
"for"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_FOR;
          }
"func"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_FUNC;
          }
"if"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_IF;
          }
"return"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 1; return T_RETURN;
          }
"var"   {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
          matchflag = 0;
          return T_VAR;
          }

{alph}{alphNum}* {
                  matchflag = 1;
                  yylloc.first_line = lineno;
                  yylloc.last_line = lineno;
                  yylval.myString = strdup(yytext);
                  return T_IDENTIFIER;
                 }

{num}*  {
          yylloc.first_line = lineno;
          yylloc.last_line = lineno;
          yylval.myString = strdup(yytext);
         matchflag = 1;
         return T_NUM;
        }

"&" {
   // Error Routine Bitwise
  fprintf(stderr, "LEXICAL ERROR: BITWISE 'AND' WAS LOCATED IN GOLF PROGRAM, TERMINATING `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, yyrecount);
  exit(1);
}

"|" {
   // Error Routine SOR
  fprintf(stderr, "LEXICAL ERROR: BITWISE 'OR' WAS LOCATED IN GOLF PROGRAM, TERMINATING `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, yyrecount);
  exit(1);
}

<*>.|\n      { 
          // Error Routine Normal
            dualmatch = 0;
            yyrecount++;
            fprintf(stderr, "Lexical Warning: Skipping Bad Char `%s` at lineno '%d' Error Seq '%d' \n", yytext, lineno, yyrecount);
            if(yyrecount > 10){
              fprintf(stderr, "ERROR 10 or more times, terminating\n");
              exit(1);
            }
          }
%% 