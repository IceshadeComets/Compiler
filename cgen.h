#ifndef CGEN_H
#define CGEN_H
#include "ast.h"

void emit(char *printcode);
void emitnewline(char *printcode);
void emitvar(char *opcode);
void pCodeGenC(struct Ast* program);
void cGen(struct Ast* program);


#endif