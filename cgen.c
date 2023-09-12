#include "cgen.h"
#include "ast.h"
#include "stack.h"
#include "symbol.h"
#include "hashmap.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// Code Generation Phase, all of the C Code Generated is purely within this block, and pulls from the AST when needed.

// Global Variables and Other Information, similar to Semantic AST, including all needed flags for checking
#define EMPTY_TAB 0
#define TABBING_INC 4
int TopLevelC = 0;
int GPass = 0;
int mPass = 0;
int isMainX = 0;
int bSearch = 0;
int elseStmt = 0;
int elseIfStmt = 0;
int worldEmission = 0;
char strA[10000]; // Paramter LIST
char strB[10000]; // Formals
struct Ast* astnode;
char* globalFuncName;
char* gR;
int yFlag = 0;
int printEmit = 0;
int funcFlagA = 0;
int funcFlagB = 0;
int divzeroflag = 0;
int noreturnflag = 0;
int haltflag = 0;
int getcharflag = 0;
int lenflag = 0;
int printbflag = 0;
int printcflag = 0;
int printiflag = 0;
int printsflag = 0;
int trueflag = 0;
int falseflag = 0;

// Emit Functions, takes all of the code and prints it to standard output.
void emit(char *printcode){
    fprintf(stdout,"%s",printcode);
}

void emitnewline(char *printcode){
    fprintf(stdout,"%s\n",printcode);
}

// Special Version that adds a X in front of a variable, deals with variable syntax as well.
void emitvar(char *opcode) {
    char tcode[10000];
    memset(tcode, 0, sizeof tcode);
    strcat(tcode,"X");
    strcat(tcode,opcode);
    fprintf(stdout,"%s",tcode);
}

// Code Gen
void pCodeGenC(struct Ast* astnode)
{
    // Prelude (needed declarations)
    emit("// C CODE GENERATION BY NOUMAN SYED 30024513\n");
    emit("#include <stdio.h>\n");
    emit("#include <stdlib.h>\n");
    emit("#include <string.h>\n");
    emit("#include <sys/types.h>\n");
    emit("typedef int32_t golf_t;\n");
    emit("golf_t Ltrue = 1;\n");
    emit("golf_t Lfalse = 0;\n");
    emit("golf_t Ldivzero(golf_t);\n");
    emit("_Noreturn void Lnoreturn(const char *);\n");
    emit("golf_t Lgetchar();\n");
    emit("_Noreturn void Lhalt();\n");
    emit("golf_t Llen(const char *);\n");
    emit("void Lprintb(golf_t);\n");
    emit("void Lprintc(golf_t);\n");
    emit("void Lprinti(golf_t);\n");
    emit("void Lprints(const char *);\n");

    // Predeclared Identifiers Block, Prime the Symbol Table, Keep Temp Symbol table here for now
    initTableC();
    // Pass 1
    cGen(astnode);
    // Pass 2
    GPass = 1;
    cGen(astnode);
    closescope();

    //Epilogue (Needed functions for program, taken from reference)
    emitnewline("golf_t Ldivzero(golf_t n) {");
    emitnewline("    if (n == 0) {");
    emitnewline("        fprintf(stderr, \"error: division by zero\\n\");");
    emitnewline("        exit(!0);");
    emitnewline("    }");
    emitnewline("    return n;");
    emitnewline("}");

    emitnewline("_Noreturn");
    emitnewline("void Lnoreturn(const char *fn) {");
    emitnewline("    fprintf(stderr, \"error: function \\\"%s\\\" must return a value\\n\", fn);");
    emitnewline("    exit(!0);");
    emitnewline("}");

    emitnewline("_Noreturn");
    emitnewline("void Lhalt() {");
    emitnewline("    exit(0);");
    emitnewline("}");

    emitnewline("golf_t Lgetchar() {");
    emitnewline("    return getchar();");
    emitnewline("}");

    emitnewline("golf_t Llen(const char *s) {");
    emitnewline("    return strlen(s);");
    emitnewline("}");

    emitnewline("void Lprintb(golf_t b) {");
    emitnewline("    printf(b ? \"true\" : \"false\");");
    emitnewline("}");

    emitnewline("void Lprintc(golf_t c) {");
    emitnewline("    putchar(c);");
    emitnewline("}");

    emitnewline("void Lprinti(golf_t i) {");
    emitnewline("    printf(\"%d\", i);");
    emitnewline("}");

    emitnewline("void Lprints(const char *s) {");
    emitnewline("    printf(\"%s\", s);");
    emitnewline("}");

    emitnewline("int main() {");
    emitnewline("    Xmain();");
    emitnewline("    exit(EXIT_SUCCESS);");
    emitnewline("}");

}

void cGen(struct Ast* astnode)
{
    switch (astnode->ty) {
    case X_PROGRAM: {
        for(int x = 0; x < astnode->payload.fr.r->length; x++) {
            cGen(&astnode->payload.fr.r->payload.fc.child[x]);
        }
        break;
    }
    case X_ID: {
        // PrintEmit is the way I print any ID, basically its a global flag and if true it prints the lexeme onto the printf, making the program segfault free when printing to emit.
        char* ival = astnode->payload.fs.ident;
        if(printEmit == 1){
            // Check for booleans
            if(strcmp(ival,"true") == 0){
            if(trueflag == 0){
            emit("(");
            emit("Ltrue");
            emit(")");
            } else {
            emit("(");
            emit("Xtrue");
            emit(")");   
            }
            } else if(strcmp(ival,"$true") == 0){
            if(trueflag == 0){
            emit("(");
            emit("Ltrue");
            emit(")");
            } else {
            emit("(");
            emit("Xtrue");
            emit(")");   
            }
            } else if(strcmp(ival,"false") == 0){
            if(falseflag == 0){
            emit("(");
            emit("Lfalse");
            emit(")");
            } else {
            emit("(");
            emit("Xfalse");
            emit(")");
            }
            } else {
            emitvar(ival);
            }
        } else {
            // STRING LITERAL CHECKING
            if(strstr(ival,"\"") == 0){
                astnode->sig = "string";
            }
        }
        astnode->idtransfer = ival;
        break;
    }
    case X_NUM: {
        astnode->sig = "int";
        char* ival = astnode->payload.fs.ident;
        astnode->idtransfer = ival;
        if(printEmit == 1){
            emit("(");
            emit(ival);
            emit(")");
        }
        break;
    }
    case X_STRING_LITERAL: {
        astnode->sig = "string";
        char* ival = astnode->payload.fs.ident;
        astnode->idtransfer = ival;
        if(printEmit == 1){
            emit("(");
            emit(ival);
            emit(")");
        } else {
            // STRING LITERAL CHECKING
            astnode->sig = "string";
        }
        break;
    }
    case X_NID:
    case X_FNID: { // Similar to X_ID
        char* ival = astnode->payload.fs.ident;
        if(printEmit == 1){
            if(strcmp(ival,"true") == 0){
            if(trueflag == 0){
            emit("(");
            emit("Ltrue");
            emit(")");
            } else {
            emit("(");
            emit("Xtrue");
            emit(")");   
            }
            } else if(strcmp(ival,"$true") == 0){
            if(trueflag == 0){
            emit("(");
            emit("Ltrue");
            emit(")");
            } else {
            emit("(");
            emit("Xtrue");
            emit(")");   
            }
            } else if(strcmp(ival,"false") == 0){
            if(falseflag == 0){
            emit("(");
            emit("Lfalse");
            emit(")");
            } else {
            emit("(");
            emit("Xfalse");
            emit(")");
            }
            } else {
            emitvar(ival);
            }
        }
        astnode->idtransfer = ival;
        break;
    }
    case X_TID: {
        char* typeDef = astnode->payload.fs.ident;
        if(strcmp(typeDef,"int") == 0 || strcmp(typeDef,"bool") == 0){
            // Rewrite to int32_t
        astnode->payload.fs.ident = "golf_t";
        }
        if(strcmp(typeDef,"string") == 0){
            // Rewrite to int32_t
        astnode->payload.fs.ident = "const char *";
        }
        break;
    }
    case X_VAR: {
        // This code post prints the global and local variables together based on what is found
        // IF Global Var Found
        char* varType = astnode->payload.ff.ident;
        if(strcmp(varType,"global var") == 0 && GPass == 0){
        cGen(astnode->payload.ff.l);
        cGen(astnode->payload.ff.r);
        char* childTypeL = astnode->payload.ff.l->payload.fs.ident;
        char* childTypeR = astnode->payload.ff.r->payload.fs.ident;
        char t2code[10000];
        memset(t2code, 0, sizeof t2code);
        strcat(t2code,childTypeR);
        strcat(t2code," ");
        strcat(t2code,"X");
        strcat(t2code,childTypeL);

        //This here is to determine, whether the current id is one of the predeclared ids, if so, redef the flag.
        if (strcmp(childTypeL, "divzero") == 0 && divzeroflag == 0) {
            divzeroflag = 1;
        } else if (strcmp(childTypeL, "noreturn") == 0 && noreturnflag == 0) {
            noreturnflag = 1;
        } else if (strcmp(childTypeL, "halt") == 0 && haltflag == 0) {
            haltflag = 1;
        } else if (strcmp(childTypeL, "getchar") == 0 && getcharflag == 0) {
            getcharflag = 1;
        } else if (strcmp(childTypeL, "len") == 0 && lenflag == 0) {
            lenflag = 1;
        } else if (strcmp(childTypeL, "printb") == 0 && printbflag == 0) {
            printbflag = 1;
        } else if (strcmp(childTypeL, "printc") == 0 && printcflag == 0) {
            printcflag = 1;
        } else if (strcmp(childTypeL, "printi") == 0 && printiflag == 0) {
            printiflag = 1;
        } else if (strcmp(childTypeL, "prints") == 0 && printsflag == 0) {
            printsflag = 1;
        } else if (strcmp(childTypeL, "true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "&true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "false") == 0 && falseflag == 0) {
            falseflag = 1;
        }

        // Checks for var type
        if(strcmp(childTypeR,"golf_t") == 0){
        strcat(t2code,"=0");

        } else if(strcmp(childTypeR,"const char *") == 0){
        strcat(t2code,"= \"\"");
        }
        strcat(t2code,";");
        emitnewline(t2code);
        // Local Vars Now
        } else if(strcmp(varType,"var") == 0){
        cGen(astnode->payload.ff.l);
        cGen(astnode->payload.ff.r);
        char* childTypeL = astnode->payload.ff.l->payload.fs.ident;
        char* childTypeR = astnode->payload.ff.r->payload.fs.ident;
        char t2code[10000];
        memset(t2code, 0, sizeof t2code);
        strcat(t2code,childTypeR);
        strcat(t2code," ");
        strcat(t2code,"X");
        strcat(t2code,childTypeL);

        if (strcmp(childTypeL, "divzero") == 0 && divzeroflag == 0) {
            divzeroflag = 1;
        } else if (strcmp(childTypeL, "noreturn") == 0 && noreturnflag == 0) {
            noreturnflag = 1;
        } else if (strcmp(childTypeL, "halt") == 0 && haltflag == 0) {
            haltflag = 1;
        } else if (strcmp(childTypeL, "getchar") == 0 && getcharflag == 0) {
            getcharflag = 1;
        } else if (strcmp(childTypeL, "len") == 0 && lenflag == 0) {
            lenflag = 1;
        } else if (strcmp(childTypeL, "printb") == 0 && printbflag == 0) {
            printbflag = 1;
        } else if (strcmp(childTypeL, "printc") == 0 && printcflag == 0) {
            printcflag = 1;
        } else if (strcmp(childTypeL, "printi") == 0 && printiflag == 0) {
            printiflag = 1;
        } else if (strcmp(childTypeL, "prints") == 0 && printsflag == 0) {
            printsflag = 1;
        } else if (strcmp(childTypeL, "true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "&true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "false") == 0 && falseflag == 0) {
            falseflag = 1;
        }

        if(strcmp(childTypeR,"golf_t") == 0){
        strcat(t2code,"=0");
        } else if(strcmp(childTypeR,"const char *") == 0){
        strcat(t2code,"= \"\"");
        }
        strcat(t2code,";");
        emitnewline(t2code);
        }
        break;
    }
    // IF
    case X_IF: {
        emit("if(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // Cond
        printEmit = 0;
        emit(")");
        astnode->payload.ff.r->parentty = X_IF;
        cGen(astnode->payload.ff.r); // Block
        break;
    }
    // IFELSE
    case X_IFELSE: {
        emit("if(");
        printEmit = 1;
        cGen(astnode->payload.ft.l); // ID
        printEmit = 0;
        emit(")");
        elseStmt = 1;
        astnode->payload.ft.m->parentty = X_IFELSE;
        cGen(astnode->payload.ft.m); // Block IF
        elseStmt = 0;
        //elseIfStmt = 1;
        astnode->payload.ft.r->parentty = X_IFELSE;
        cGen(astnode->payload.ft.r); // Block ELSE
        elseIfStmt = 0;
        break;
    }
    // ELSE
    case X_ELSE: {
        //printf("Else Block at Line Number: '%d'\n",astnode->lineno);
        //printf("\n");
        // TEST 20 DONE
        emit(" else ");
        elseIfStmt = 0;
        astnode->payload.fr.r->parentty = X_ELSE;
        cGen(astnode->payload.fr.r);
        break;
    }
    // ELSE IFS
    case X_ELSEIF: {
        //printf("Else IF Block at Line Number: '%d'\n",astnode->lineno);
        emit(" else ");
        elseIfStmt = 0;
        astnode->payload.fr.r->parentty = X_ELSEIF;
        cGen(astnode->payload.fr.r);
        break;
    }
    //UNARY -
    case X_UNARY: {
        emit("-(");
        cGen(astnode->payload.fr.r);
        emit(")");
        break;
    }
    // UNARY !
    case X_UNARYNOT: {
        emit("!(");
        cGen(astnode->payload.fr.r);
        emit(")");
        break;
    }
case X_ACTUALS: {
    memset(strA, 0, sizeof strA);
    printEmit = 0;
    cGen(astnode->payload.ft.l);
    printEmit = 0;
    char* jumpGen = astnode->payload.ft.l->idtransfer;
    // Similar to above, check for declarations and insert L into predeclared if not redeclared.
    if (strcmp(jumpGen, "divzero") == 0 && divzeroflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "noreturn") == 0 && noreturnflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "halt") == 0 && haltflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "getchar") == 0 && getcharflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "len") == 0 && lenflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "printb") == 0 && printbflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "printc") == 0 && printcflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "printi") == 0 && printiflag == 0) {
        strcat(strA, "L");
    } else if (strcmp(jumpGen, "prints") == 0 && printsflag == 0) {
        strcat(strA, "L");
    } else {
        strcat(strA, "X"); // Program Function
    }

    strcat(strA,jumpGen);
    emit(strA);
    emit("(");
    // Init left to right.

    astnode->payload.ft.r->parentty = X_ACTUALS;
    cGen(astnode->payload.ft.r);
    emit(")");
    
    // This is to check for the end of a Function Call.
    if(astnode->parentty == X_EXPS || astnode->parentty == X_BLOCK){
    emitnewline(";");
    }
    break;
}

case X_ACTUALS2: {
    // PAR LIST inside FCall
    printEmit = 1;
    worldEmission = 1;
    astnode->payload.ft.l->parentty = X_ACTUALS2;
    cGen(astnode->payload.ft.l);
    printEmit = 0;
    for(int x = 0; x < astnode->payload.ft.r->length; x++) {
        emit(",");
        printEmit = 1;
        worldEmission = 1;
        astnode->payload.ft.r->parentty = X_ACTUALS2;
        cGen(&astnode->payload.ft.r->payload.fc.child[x]);
        printEmit = 0;
    }
    break;
}
    case X_FACTUALSX: {
        break;
    }
    case X_FORMALSEMPTY: {
        break;
    }
    case X_FORMALS: {
        cGen(astnode->payload.ff.l);
        for(int x = 0; x < astnode->payload.ff.r->length; x++) {
            // Array Implementation
            strcat(strB,",");
            cGen(&astnode->payload.ff.r->payload.fc.child[x]);
        }
        break;
    }
    case X_PARAMS: {
        cGen(astnode->payload.ff.l);
        cGen(astnode->payload.ff.r);
        break;
    }
    case X_PARAMS2: {
        cGen(astnode->payload.ff.l);
        cGen(astnode->payload.ff.r);
        break;
    }
    case X_PARDECL: {
        cGen(astnode->payload.ff.l);
        cGen(astnode->payload.ff.r);
        char* childTypeL = astnode->payload.ff.l->payload.fs.ident;
        char* childTypeR = astnode->payload.ff.r->payload.fs.ident;
        strcat(strB,childTypeR);
        strcat(strB," X");
        strcat(strB,childTypeL);

        // This is for the inside of the paramaters
        if (strcmp(childTypeL, "divzero") == 0 && divzeroflag == 0) {
            divzeroflag = 1;
        } else if (strcmp(childTypeL, "noreturn") == 0 && noreturnflag == 0) {
            noreturnflag = 1;
        } else if (strcmp(childTypeL, "halt") == 0 && haltflag == 0) {
            haltflag = 1;
        } else if (strcmp(childTypeL, "getchar") == 0 && getcharflag == 0) {
            getcharflag = 1;
        } else if (strcmp(childTypeL, "len") == 0 && lenflag == 0) {
            lenflag = 1;
        } else if (strcmp(childTypeL, "printb") == 0 && printbflag == 0) {
            printbflag = 1;
        } else if (strcmp(childTypeL, "printc") == 0 && printcflag == 0) {
            printcflag = 1;
        } else if (strcmp(childTypeL, "printi") == 0 && printiflag == 0) {
            printiflag = 1;
        } else if (strcmp(childTypeL, "prints") == 0 && printsflag == 0) {
            printsflag = 1;
        } else if (strcmp(childTypeL, "true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "&true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "false") == 0 && falseflag == 0) {
            falseflag = 1;
        }

        break;
    }
    case X_EXPS: {
        astnode->payload.ff.r->parentty = X_EXPS;
        cGen(astnode->payload.ff.r);
        break;
    }
    case X_RETURN: {
        funcFlagA = 1;
        emit("return");
        if((strcmp(gR, "$void") == 0)) {

        } else if (astnode->childflag == 0) {
            emit(" ");
            printEmit = 1;
            cGen(astnode->payload.ff.r);
            printEmit = 0;
        }
        emitnewline(";");
        break;
    }
    case X_BLOCK: {
        // This is to account for the first function block counting, so openscope was called there for the first found block.
        if(TopLevelC == 1) {
            TopLevelC = 0;
        } else {
        }
        funcFlagA = 0;
        emitnewline("{");
        for(int x = 0; x < astnode->payload.ff.r->length; x++) {
            astnode->payload.ff.r->payload.fc.child->parentty = X_BLOCK;
            cGen(&astnode->payload.ff.r->payload.fc.child[x]);
        }

        if(funcFlagA == 0){
            if(astnode->parentty == X_FUNC){
            if(strcmp(gR,"$void")!= 0){
                    emit("Lnoreturn(\"");
                    emit(globalFuncName);
                    emitnewline("\");");
            }
            }
        }
        if(elseStmt == 1){
        emit(";}");
        } else {
        emitnewline(";}");
        }
        funcFlagA = 0;
        break;
    }
    //fl
    case X_FUNC: {
        memset(strB, 0, sizeof strB);
        if(GPass == 1) {
        char* childTypeL = astnode->payload.ft.l->payload.fs.ident;
        char* childTypeR = astnode->payload.ft.m->payload.ff.r->payload.fs.ident;
            if(strcmp(childTypeR,"$void") == 0){
                childTypeR = "void";
            }
            emit(childTypeR);
            emit(" ");
            emitvar(childTypeL);
            emit("(");
            cGen(astnode->payload.ft.m);
            emit(strB);
            emit(")");
            TopLevelC = 1; // This is to make sure global declarations can no longer happen.
            mPass = 1;
            gR = astnode->payload.ft.m->payload.ff.r->payload.fs.ident; // Part of return check, come back later
            funcFlagA = 0; // Dynamic Semantic Check
            funcFlagB = 0; // Dynamic Semantic Check
            globalFuncName = childTypeL;
            astnode->payload.ft.r->parentty = X_FUNC;
            cGen(astnode->payload.ft.r);
            // Check for return type here if not void
            TopLevelC = 0;
            isMainX = 0;
            // I believe scope should close once func closes
        } else {
            cGen(astnode->payload.ft.m);
            char* childTypeL = astnode->payload.ft.l->payload.fs.ident;
            char* childTypeR = astnode->payload.ft.m->payload.ff.r->payload.fs.ident;
            // This part of code generation generates the function header names.
            if(strcmp(childTypeR,"$void") == 0){
                childTypeR = "void";
            }
            emit(childTypeR);
            emit(" ");
            emitvar(childTypeL);
            emit("(");
            emit(strB);
            emitnewline(");");
            define(childTypeL,"XchildTypeL"); // This means that during function checks, if this shows up, its not a func call anymore.
        
        // This is for the declaration of a function
        if (strcmp(childTypeL, "divzero") == 0 && divzeroflag == 0) {
            divzeroflag = 1;
        } else if (strcmp(childTypeL, "noreturn") == 0 && noreturnflag == 0) {
            noreturnflag = 1;
        } else if (strcmp(childTypeL, "halt") == 0 && haltflag == 0) {
            haltflag = 1;
        } else if (strcmp(childTypeL, "getchar") == 0 && getcharflag == 0) {
            getcharflag = 1;
        } else if (strcmp(childTypeL, "len") == 0 && lenflag == 0) {
            lenflag = 1;
        } else if (strcmp(childTypeL, "printb") == 0 && printbflag == 0) {
            printbflag = 1;
        } else if (strcmp(childTypeL, "printc") == 0 && printcflag == 0) {
            printcflag = 1;
        } else if (strcmp(childTypeL, "printi") == 0 && printiflag == 0) {
            printiflag = 1;
        } else if (strcmp(childTypeL, "prints") == 0 && printsflag == 0) {
            printsflag = 1;
        } else if (strcmp(childTypeL, "true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "&true") == 0 && trueflag == 0) {
            trueflag = 1;
        } else if (strcmp(childTypeL, "false") == 0 && falseflag == 0) {
            falseflag = 1;
        }
        }
        break;
    }
    case X_FOR: {
        emit("while(");
        printEmit = 1;
        cGen(astnode->payload.fornode.l);
        printEmit = 0;
        emit(")");
        astnode->payload.fornode.r->parentty = X_FOR;
        elseIfStmt = 0;
        cGen(astnode->payload.fornode.r);
        break;
    }
    case X_EMPTY: {
        break;
    }
    case X_BREAK: {
        emitnewline("");
        emitnewline("break;");
        break;
    }
    case X_ASSIGN: {
        cGen(astnode->payload.ff.l);
        char* childTypeL = astnode->payload.ff.l->payload.fs.ident;
        emitvar(childTypeL);
        emit("=");
        printEmit = 1;
        cGen(astnode->payload.ff.r);
        printEmit = 0;
        emitnewline(";");
        astnode->sig = "void";
        break;
    }
    case X_OR:
    case X_AND: {
        emit("(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        char* xType = astnode->payload.ff.ident;
        emit(xType);
        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        emit(")");
        break;
    }
    case X_REL: case X_RELC: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9; X_UNARY = 6;
        if(strstr(astnode->payload.ff.l->sig,"string") != NULL){
        // ok we have to check for the rels types here, > < <= and >= all run the below

       char* xType = astnode->payload.ff.ident;
       if(strcmp(xType,"==") == 0){
        emit("!strcmp(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        emit(",");
        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        emit(")");
        /* == runs
        == Prints like this
        if (!strcmp("Hello",Xbye()))
        */

       } else if (strcmp(xType,"!=") == 0){
        emit("strcmp(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        emit(",");
        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        emit(")");
        
        /* != runs
        != Prints like this
        if (strcmp("       ","\t")
        */

       } else {
        // Rest of the cases

        emit("strcmp(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        emit(",");
        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        emit(") ");
        char* xType = astnode->payload.ff.ident;
        emit(xType);
        emit(" 0");
        }

        } else {
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        

        char* xType = astnode->payload.ff.ident;
        emit(xType);

        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        }
        astnode->sig = "bool";
        break;
    }
    //Binary Operators, INTEGERS ONLY
    case X_MUL:
    case X_ADD: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9;
        emit("(");
        printEmit = 1;
        cGen(astnode->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        printEmit = 0;
        char* xType = astnode->payload.ff.ident;
        if(strcmp(xType,"/") == 0){
        emit(xType);
        printEmit = 1;
        emit("Ldivzero(");
        cGen(astnode->payload.ff.r); // Same here
        emit(")");
        printEmit = 0;
        } else {
        emit(xType);
        printEmit = 1;
        cGen(astnode->payload.ff.r); // Same here
        printEmit = 0;
        }
        emit(")");
        astnode->sig = "int";

        break;
    }
    default: {
    }
    };
}

