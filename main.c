#include"lex.h"
#include "stack.h"
#include "hashmap.h"
#include<stdio.h>
#include "parse.tab.h"
#include<stdlib.h>
#include"ast.h"
#include"globals.h"
#include"cgen.h"

int main(int argc, char** argv) {
    //inputFileName = argv[1];
    
    if(argc < 2){
      fprintf(stderr, "Error: Too Few Arguments in Program, The correct way to run the program is:\n ./golf gen.t1\n Exiting Program\n");
      exit(EXIT_FAILURE);
    }
    if(argc > 2){
      fprintf(stderr, "Error: Too Many Arguments in Program, The correct way to run the program is:\n ./golf gen.t1\n Exiting Program\n");
      exit(EXIT_FAILURE);
    }
    yyin = fopen(argv[1], "r");

  if (yyin == NULL) {
    fprintf(stderr, "Error: Bad File Name '%s', The correct way to run the program is: \n ./golf gen.t1\n Exiting Program\n",argv[1]);
      exit(EXIT_FAILURE);
  }

  gimmeGlobals();
    if(yyparse() != 0) {
        return EXIT_FAILURE;
    }

    // Semantic Checker
     pSemanticsAst(program);

     // This here is the AST Printing, disabled for Final Submission.
     //pShowAst(program);

    // Code Generation
    pCodeGenC(program);
    exit(EXIT_SUCCESS);
}