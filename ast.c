#include "ast.h"
#include "stack.h"
#include "symbol.h"
#include "hashmap.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#define EMPTY_TAB 0
#define TABBING_INC 4
int TopLevelTracker = 0;
int GlobalPass = 0;
int myTypePass = 0;
int isMain = 0;
int breakSearcher = 0;
char strP[10000]; // Paramter LIST
char strD[10000]; // Formals Inside
char strC[10000]; // Formals
struct Ast* program;
char* globalRet;
int rFlag = 0;

//Semantic Checking
void pShowAstImpl(struct Ast* program, int tabbing);

//Preorder
void pShowAst(struct Ast* program)
{
    pShowAstImpl(program, 0);
}

void pSemanticsAst(struct Ast* program)
{
    // Predeclared Identifiers Block, Prime the Symbol Table
    initTableC();
    definel("$void","void",0,1);
    definel("bool","bool",0,1);
    definel("int","int",0,1);
    definel("string","string",0,1);
    definel("$true","bool",1,0);
    definel("true","bool",1,0);
    definel("false","bool",1,0);
    defineFunc("getchar","FX() int", "Lgetchar", 0,0,"void","int");
    defineFunc("halt","FX() void", "Lhalt", 0,0,"void","void");
    defineFunc("len","FX(str) int", "Llen", 0,0,"string","int");
    defineFunc("printb","FX(bool) void", "Lprintb", 0,0, "bool","void");
    defineFunc("printc","FX(int) void", "Lprintc", 0,0, "int","void");
    defineFunc("printi","FX(int) void", "Lprinti", 0,0, "int","void");
    defineFunc("prints","FX(str) void", "Lprints", 0,0, "string","void");

    // Initialize Scope that Corresponds to the File Block
    openscope();

    // Pass 1
    //myTypeCheck(program);
    myTypeGet(program);

    // Pass 2
    GlobalPass = 1;
    //myTypeCheck(program);
    myTypeGet(program);

    // END
    closescope(); // Close File Block
    closescope(); // Clock Predeclared Block, End Semantic Checker
}

void myTypeCheck(struct Ast* program)
{
    switch (program->ty) {
    case X_PROGRAM: {
        // This is to deal with test cast 13, which apparently jumps far ahead
        if(GlobalPass == 1) {
            char* m3 = lookupMain("main",program->lineno)->SymbolName;
            if(strcmp(m3,"main") != 0) {
                fprintf(stderr, "WARNING MAIN FOUND AT %d IS NOT A FUNCTION EXITING\n", program->lineno);
                exit(1);
            }
        }
        for(int x = 0; x < program->payload.fr.r->length; x++) {
            myTypeCheck(&program->payload.fr.r->payload.fc.child[x]);
        }
        break;
    }
    case X_ID: {
        // I believe when dealing with this case in particular, it looks up the table to see if it exists.
        //printf("This is a ID check 1\n");
        program->sig = lookupID(program->payload.fs.ident,program->lineno)->TypeSig;
        program->symbol = lookupID(program->payload.fs.ident,program->lineno);
        //program->sig = lookup(program->payload.fs.ident)->TypeSig;
        //printf("This is a ID check 2 %s \n", xLook);
        break;
    }
    case X_NUM: {
        program->sig = "int";
        /* 
        const char* ival = program->payload.fs.ident;
        int ival2 = atoi(ival);
        //fprintf(stderr, "ERROR %d %s is too high of a value LineNo: %d \n",ival2, ival, program->lineno);
        if(ival2 > 2147483647 || ival2 == -1) {
            fprintf(stderr, "ERROR %d is too high of a value LineNo: %d \n", ival2, program->lineno);
            exit(1);
        }
        */
        //program->sig = lookup(program->payload.fs.ident)->TypeSig;
        //printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d'\n", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno);
        break;
    }
    case X_STRING_LITERAL: {
        program->sig = "string";
        break;
    }
    case X_TID:
    case X_NID:
    case X_FNID: {
        program->symbol = lookupID(program->payload.fs.ident,program->lineno);
        //program->sig = lookup(program->payload.fs.ident)->TypeSig;
        break;
    }
    case X_VAR: {
        // Checks for ID, Parser should not return anything but XID and NID
        char* childTypeL = program->payload.ff.l->payload.fs.ident;
        char* childTypeR = program->payload.ff.r->payload.fs.ident;
        /*
        if(strcmp(lookup(childTypeL)->SymbolName,lookup(childTypeR)->SymbolName) != 0){
            fprintf(stderr, "WARNING REDUNDANT LOOP BOTH NEW AND TYPE ARE SAME WITH %s\n", lookup(childTypeR)->SymbolName);
            exit(1);
        }
        */
        // IF Global Var Found
        if(strcmp(program->payload.fs.ident, "global var") == 0) {
            // FIRST PASS To Fill in Variables
            if(GlobalPass == 0) {
                definel(childTypeL,childTypeR,0,0);
                program->symbol = lookup(childTypeL);
                //printf("This is a child check 2 %d \n", lookup(childTypeL)->IsType);
                // Second Pass Type Set these variables
            } else {
                //printf("This is a child check %d \n", lookup(childTypeR)->IsType);
                if(lookup(childTypeR)->IsType != 1) {
                    fprintf(stderr, "ERROR %s is not a TYPE! LineNo: %d Check if redeclared\n", lookup(childTypeR)->SymbolName, program->lineno);
                    exit(1);
                }
                defineType(childTypeL,childTypeR);
                program->symbol = lookup(childTypeL);
            }
            //printf("This is a global var '%s' X '%s' \n", childTypeL,childTypeR);
            // SECOND PASS

        } else {
            // printf("This is a local var \n");
            if(lookup(childTypeR)->IsType != 1) {
                fprintf(stderr, "ERROR %s is not a TYPE! LineNo: %d Check if redeclared\n", lookup(childTypeR)->SymbolName, program->lineno);
                exit(1);
            }
            definel(childTypeL,childTypeR,0,0); // First the Variable Name, then the name of the right most child.
            program->symbol = lookup(childTypeL);
        }
        myTypeCheck(program->payload.ff.l);
        myTypeCheck(program->payload.ff.r);
        break;
    }
    case X_IF: {
        myTypeCheck(program->payload.ff.l);
        myTypeCheck(program->payload.ff.r);
        char* sigbelow = program->payload.ff.l->sig;
        if(strcmp(sigbelow, "bool") != 0) {
            fprintf(stderr, "CONDITION IS NOT BOOLEAN IN IF STATEMENT! COND: %s LineNo: %d\n", sigbelow, program->lineno);
            exit(1);
        }
        break;
    }
    case X_IFELSE: {
        //printf("If Else at Line Number: '%d'\n",program->lineno);
        myTypeCheck(program->payload.ft.l); // ID
        myTypeCheck(program->payload.ft.m); // Block IF
        myTypeCheck(program->payload.ft.r); // Block ELSE
        char* sigbelow = program->payload.ff.l->sig;
        if(strcmp(sigbelow, "bool") != 0) {
            fprintf(stderr, "CONDITION IS NOT BOOLEAN IN IF STATEMENT! COND: %s LineNo: %d\n", sigbelow, program->lineno);
            exit(1);
        }
        break;
    }
    case X_ELSE: {
        //printf("Else Block at Line Number: '%d'\n",program->lineno);
        //printf("\n");
        myTypeCheck(program->payload.fr.r);
        break;
    }
    case X_ELSEIF: {
        //printf("Else IF Block at Line Number: '%d'\n",program->lineno);
        myTypeCheck(program->payload.fr.r);
        break;
    }
    //OI DEAL WITH THIS
    case X_UNARY: {
        if(program->payload.fr.r->ty == X_NUM) {
            //printf("UNARY MINUS\n");
        }
        //printf("Unary at at %d: %s with attribute `%s`\n", program->lineno, pShowAstTy(program->ty), program->payload.fr.ident);
        myTypeCheck(program->payload.fr.r);
        break;
    }
    case X_UNARYNOT: {
        myTypeCheck(program->payload.fr.r);
        break;
    }
    //fl
    case X_ACTUALS: {
        //memset(strP, 0, sizeof strP);
        // THIS IS FOR ONE SINGLE PARAMETER, JUST CHECK THE ONE
        //program->sig =
        myTypeCheck(program->payload.ft.l);
        myTypeCheck(program->payload.ft.r);

        //printf("STR %s \n",strP);
        break;
    }
    case X_ACTUALS2: {
        // THIS IS FOR A PARAMATER LIST, CHECK IF IT MATCHES THE ENTIRE PARAMTER SCOPE, UNLIKE PARAMATERS< WE'D NEED TO CHECK FOR TYPES
        //char* childTypeL = program->payload.ff.l->payload.fs.ident;
        //char* childTypeR = program->payload.ff.r->payload.fs.ident;
        //strcat(strC,childTypeR);
        //char* childTypeL = program->payload.ff.l->payload.fs.ident; // Name of Function
        // Remember to add the space
        //fprintf(stderr, "Else Block at Line Number: '%s' %s \n",childTypeL,childTypeR);
            myTypeCheck(program->payload.ft.l);
            //char* typeis = lookup(childTypeL)->FuncReturn;
        for(int x = 0; x < program->payload.ft.r->length; x++) {
            //char* childTypeR = program->payload.ft.r->payload.fc.child[x];
            //strcat(strP,childTypeR);
            myTypeCheck(&program->payload.ft.r->payload.fc.child[x]);
        }
        //program->sig = typeis;
        break;
    }
    case X_ACTUALS3: {
        memset(strP, 0, sizeof strP);
        // THIS IS THE CASE FOR VOID, CHECK IF CURRENT NAME IS VOID< THATS ALL
        myTypeCheck(program->payload.ft.l);
        break;
    }
    case X_FACTUALSX: {
        //printf("ACTUALS CALL AT at Line Number: '%d'\n",program->lineno);
        break;
    }
    case X_FORMALSEMPTY: {
        //printf("Formals\n");
        break;
    }
    case X_FORMALS: {
        //printf("Formals at Line Number: '%d' and %d children\n",program->lineno,program->payload.ff.r->length);
        //char * strB = "FX() ", strC[100000] = {0};
        //strcat(strC, strB);
        //strcat(strC, childTypeL);
        if(isMain == 1) {
            fprintf(stderr, "ERROR! MAIN HAS PARAMATERS! EXITING LineNo: %d\n", program->lineno);
            exit(1);
        }

        myTypeCheck(program->payload.ff.l);

        for(int x = 0; x < program->payload.ff.r->length; x++) {
            // RIGHT HERE?? For the Array Implementation i beleive
            myTypeCheck(&program->payload.ff.r->payload.fc.child[x]);
            //printf("STR FORMALS %s \n",strC);
        }
        break;
    }
    case X_PARAMS: {
        //printf("Formal\n");
        myTypeCheck(program->payload.ff.l);
        myTypeCheck(program->payload.ff.r);
        break;
    }
    case X_PARAMS2: {
        //printf("SIG\n");
        if(program->childflag == 0) {
            myTypeCheck(program->payload.ff.l);
        }
        myTypeCheck(program->payload.ff.r);
        break;
    }
    case X_PARDECL: {
        //printf("Formal\n");
        char* childTypeL = program->payload.ff.l->payload.fs.ident;
        char* childTypeR = program->payload.ff.r->payload.fs.ident;
        strcat(strC,childTypeR);
        // Remember to add the space
        //fprintf(stderr, "Else Block at Line Number: '%s' %s \n",childTypeL,childTypeR);
        if(myTypePass == 1) { // This is so i can access pardecl twice
            int tCheck = lookup(childTypeR)->IsType;
            definel(childTypeL,childTypeR,0,0);
            if(tCheck != 1) {
                fprintf(stderr, "ERROR %s IS NOT A TYPE! X at lineno %d \n", childTypeR, program->lineno);
                exit(1);
            }
            myTypeCheck(program->payload.ff.l);
            myTypeCheck(program->payload.ff.r);
        }
        break;
    }
    case X_EXPS: {
        //printf("Expression Statement at Line Number: '%d'\n",program->lineno);
        myTypeCheck(program->payload.ff.r);
        break;
    }
    case X_RETURN: {
        if((strcmp(globalRet, "$void") == 0)) {

        } else {
            rFlag = 1;
            if(program->childflag != 0) {
                fprintf(stderr, "ERROR: TYPE MISMATCH, RETURN IS VOID WHILE FUNC IS %s LineNo: %d \n",globalRet, program->lineno);
                exit(1);
            }
        }
        //printf("FLAG Statement at Line Number: '%d'\n",program->childflag);
        //printf("RETURN Statement at Line Number: '%d'\n",program->lineno);
        if(program->childflag == 0) {
            if((strcmp(globalRet, "$void") == 0)) {
                fprintf(stderr,"A FUNCTION WITH $void HAS A RETURN VALUE! LineNo: %d \n",program->lineno);
                exit(1);
            }
            myTypeCheck(program->payload.ff.r);
            char* sysig = program->payload.ff.r->sig;
            if((sysig == NULL)){
                sysig = program->payload.ff.r->payload.ff.l->sig;
            }
            if((strcmp(sysig,"FX()") == 0)) {
                sysig = lookup(program->payload.ff.r->payload.ff.l->payload.ff.ident)->FuncReturn;
            }
            if((strcmp(globalRet, sysig) != 0)) {
                fprintf(stderr, "ERROR: RETURN VALUE TYPE MISMATCH %s and %s LineNo: %d \n",globalRet, sysig, program->lineno);
                exit(1);
            }
        }
        break;
    }
    case X_BLOCK: {
        // This is to account for the first function block counting, so openscope was called there for the first found block.
        if(TopLevelTracker == 1) {
            TopLevelTracker = 0;
        } else {
            openscope();
        }
        //printf("Block at Line Number: '%d' and %d children\n",program->lineno,program->payload.ff.r->length);
        //fprintf(stdout, "SMLA: %p\n",program->payload.ff.r);
        for(int x = 0; x < program->payload.ff.r->length; x++) {
            myTypeCheck(&program->payload.ff.r->payload.fc.child[x]);
        }
        closescope();
        break;
    }
    //fl
    case X_FUNC: {
        memset(strC, 0, sizeof strC);
        char* childTypeL = program->payload.ft.l->payload.fs.ident;
        if(GlobalPass == 1) { // Set so this only procs on the second pass

            //char* m1 = lookupMain("main",program->lineno)->SymbolName;
            char* m2 = lookup("main")->TypeSig;
            char* xl = lookup(childTypeL)->TypeSig;
            if(strstr(xl, "FX()") == NULL) {
                fprintf(stderr, "WARNING %s is not a FUNCTION! TS: %s LineNo: %d  EXITING\n",childTypeL, xl, program->lineno);
            }
            if((strcmp(m2, "FX()") != 0)) {
                fprintf(stderr, "WARNING MAIN FOUND AT %d IS NOT A FUNCTION EXITING\n", program->lineno);
                exit(1);
            } else if(strcmp(childTypeL, "main") == 0) { // If current func is main
                char* childTypeM = program->payload.ft.m->payload.ff.r->payload.fs.ident;
                if((strcmp(childTypeM, "$void") != 0)) {
                    fprintf(stderr, "WARNING MAIN HAS A NON VOID RETURN TYPE at %d EXITING\n", program->lineno);
                    exit(1);
                }
                isMain = 1;
            } else {
                myTypePass = 0;
                char* childTypeR = program->payload.ft.m->payload.ff.r->payload.fs.ident; // The return type of the function
                myTypeCheck(program->payload.ft.m); // The job of this is to grab the variables to define the first function
                char strF[10000]; // Formals
                memset(strF, 0, sizeof strF);
                char* fstring = "FX() ";
                strcat(strF,fstring);
                strcat(strF,childTypeR);
                //printf("0 %s 1 %s 2 %s 3 %s\n",childTypeL,fstring,strC,childTypeR);
                if((strcmp(strC, "") == 0)) {
                    redefineFunc(childTypeL,strF,0,0,"void",childTypeR);
                } else {
                    redefineFunc(childTypeL,strF,0,0,strC,childTypeR);
                }
            }

            openscope();
            TopLevelTracker = 1; // This is to make sure global declarations can no longer happen.
            myTypePass = 1;
            myTypeCheck(program->payload.ft.l);
            myTypeCheck(program->payload.ft.m);
            globalRet = program->payload.ft.m->payload.ff.r->payload.fs.ident;
            myTypeCheck(program->payload.ft.r);
            // Check for return type here if not void
            if((strcmp(globalRet, "$void") != 0)) {
                if(rFlag != 1) {
                    fprintf(stderr, "WARNING NO RETURN TYPE %s IN FUNCTION at LineNo: %d EXITING\n", globalRet, program->lineno);
                    exit(1);
                }
            }
            TopLevelTracker = 0;
            isMain = 0;
            // I believe scope should close once func closes
        } else {
            char* childTypeL = program->payload.ft.l->payload.fs.ident; // Name of the ID Name
            definel(childTypeL,"FX()",0,0);
            program->symbol = lookup(childTypeL);
        }
        // STOP COMBINING, WILL ASK TA LATEr
        break;
    }
    case X_FOR: {
        if(breakSearcher != 0){

        } else {
        breakSearcher = 1;
        }
        //printf("Func at Line Number: '%d'\n",program->lineno);
        // printf("ForNode: '%s' Lexeme: '%s' Line Number: '%d'\n", pShowAstTy(program->ty), program->payload.fornode.ident, program->lineno);
        myTypeCheck(program->payload.fornode.l);
        myTypeCheck(program->payload.fornode.r);
        char* sigbelow = program->payload.fornode.l->sig;
        if(strcmp(sigbelow, "bool") != 0) {
            fprintf(stderr, "CONDITION IS NOT BOOLEAN IN FOR STATEMENT! COND: %s LineNo: %d\n", sigbelow, program->lineno);
            exit(1);
        }
        /*
        if(breakSearcher != 2) {
            fprintf(stderr, "ERROR NO BREAKS FOUND IN FOR LOOP! COND: %s LineNo: %d\n", sigbelow, program->lineno);
            exit(1);
        }
        */
        breakSearcher = 0;
        break;
    }
    case X_EMPTY: {
        // printf("Empty Statement at Line Number: '%d'\n",program->lineno);
        break;
    }
    case X_BREAK: {
        if(breakSearcher == 0) {
            //fprintf(stderr, "ERROR BREAK FOUND OUTSIDE FOR LOOP LineNo: %d\n", program->lineno);
            //exit(1);
        }
        breakSearcher = 2;
        // printf("Break Statement at Line Number: '%d'\n",program->lineno);
        break;
    }
    case X_ASSIGN: {
        if(program->payload.ff.l->ty != X_VAR) {
            if(program->payload.ff.l->ty != X_ID && program->payload.ff.l->ty != X_NID){
            fprintf(stderr, "ERROR CAN ONLY ASSIGN TO A VARIABLE! LineNo %d \n", program->lineno);
            exit(1);
            }
        }
        char* childTypeL = program->payload.ff.l->payload.fs.ident;
        char* xSymbol = lookup(childTypeL)->SymbolName; // Name of Left Child Variable, check if exists
        char* xType = lookup(childTypeL)->TypeSig; // Type of Left Child Variable
        char isConst = lookup(childTypeL)->IsConst;
        if(isConst == 1) {
            fprintf(stderr, "ERROR %s ATTEMPTING TO DECLARE CONSTANT INTO ASSIGN LineNo: %d \n", xSymbol, program->lineno);
            exit(1);
        }
        if(strstr(xType, "FX()") != NULL) {
            fprintf(stderr, "ERROR %s ATTEMPTING TO DECLARE INTO FUNCTION LineNo %d \n", xType,program->lineno);
            exit(1);
        }
        // Self Check, So this should print r correctly
        int enumPullL = program->payload.ff.l->ty;
        if(enumPullL == 7 || enumPullL == 22) {
                char* typeLookup = lookup(program->payload.ff.l->payload.fs.ident)->TypeSig;
                if(lookup(typeLookup)->IsType == 0) {
                    fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo: %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                    exit(1);
                }
                if(strcmp(typeLookup, "int") == 0) {
                    enumPullL = 0;
                } else if (strcmp(typeLookup, "string") == 0) {
                    enumPullL = 9;
                } else if (strcmp(typeLookup, "bool") == 0) {
                    enumPullL = 5;
                }
        }
        int enumPullR = program->payload.ff.r->ty;
        if(enumPullR == X_MUL || enumPullR == X_ADD){
            enumPullR = 0; // The reason why its just 0 is because only integers can exist in this operation
        } else if(enumPullR == X_REL) {
            fprintf(stderr, "WARNING TYPE MISMATCH USING COMPARISON OPERATORS IN ASSIGN %s and %s EXITING LineNo: %d\n", program->payload.ff.l->payload.fs.ident, program->payload.ff.r->payload.ff.l->payload.fs.ident,program->lineno);
            exit(1);
        } else if (enumPullR == X_UNARY){
            enumPullR = 0;
        } else if (enumPullR == X_ACTUALS){
            char* lval2 = lookup(program->payload.ff.r->payload.ft.l->payload.ft.ident)->SymbolName;
            char* lval = lookup(lval2)->FuncReturn;
            fprintf(stderr, "LVAL TEST %s and %s EXITING LineNo: %d\n", lval, lval2, program->lineno);
            if(strcmp(lval, "int") == 0) {
                enumPullR = 0;
            } else if(strcmp(lval, "string") == 0) {
                enumPullR = 9;
            } else if(strcmp(lval, "bool") == 0) {
                enumPullR = 5;
            } else {
                fprintf(stderr, "ERROR %s VOID FUNCTION IN RIGHT SIDE OF ASSIGN LineNo: %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        } else {
            if(enumPullR == 7 || enumPullR == 22) {
                char* typeLookup = lookup(program->payload.ff.r->payload.fs.ident)->TypeSig;
                if(strcmp(typeLookup, "int") == 0) {
                    enumPullR = 0;
                } else if (strcmp(typeLookup, "string") == 0) {
                    enumPullR = 9;
                } else if (strcmp(typeLookup, "bool") == 0) {
                    enumPullR = 5;
                }
            }
        }
        if(enumPullL != enumPullR) { // This checks for Type Mismatch between Enum Vals
            fprintf(stderr, "WARNING TYPE MISMATCH FOR ASSIGNMENT TYPES with Enums %d and %d EXITING LineNo: %d\n", enumPullL, enumPullR, program->lineno);
            exit(1);
        }
        program->sig = "void";
        myTypeCheck(program->payload.ff.l);
        myTypeCheck(program->payload.ff.r);
        break;
    }
    // RELS Comparison
    case X_OR:
    case X_AND: {

    }
    case X_REL: {
        // WITHOUT BOOLEAN
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9; X_UNARY = 6;
        myTypeCheck(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        myTypeCheck(program->payload.ff.r); // Same here
        int enumPullL = program->payload.ff.l->ty; // This is the Enum of the left type
        int enumPullR = program->payload.ff.r->ty; // This is the Enum of the right type.

        if(enumPullL == 7 || enumPullL == 22) {
            char* typeLookup = lookup(program->payload.ff.l->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullL = 0;
                program->sig = "int";
            } else if (strcmp(typeLookup, "string") == 0) {
                enumPullL = 9;
                program->sig = "string";
            } else if (strcmp(typeLookup, "bool") == 0) {
                fprintf(stderr, "ERROR %s BOOLEAN IS NOT AN ORDERED OPERATOR! %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
        } else if (enumPullL == X_UNARY){
            enumPullL = 0;
        } else if (enumPullL == X_ACTUALS){
            char* lval = lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullL = 0;
            } else if(strcmp(lval, "string") == 0) {
                enumPullL = 9;
            } else if(strcmp(lval, "bool") == 0) {
                fprintf(stderr, "ERROR %s BOOLEAN IS NOT AN ORDERED OPERATOR! %d\n", lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            } else {
                fprintf(stderr, "ERROR %s VOID FUNCTION IN COMPARISON LineNo: %d\n", lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }
        if(enumPullR == 7 || enumPullR == 22) {
            char* typeLookup = lookup(program->payload.ff.r->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullR = 0;
            } else if (strcmp(typeLookup, "string") == 0) {
                enumPullR = 9;
            } else if (strcmp(typeLookup, "bool") == 0) {
                fprintf(stderr, "ERROR %s BOOLEAN IS NOT AN ORDERED OPERATOR! %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
        } else if (enumPullR == X_UNARY){
            enumPullR = 0;
        } else if (enumPullR == X_ACTUALS){
            char* lval = lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullR = 0;
            } else if(strcmp(lval, "string") == 0) {
                enumPullR = 9;
            } else if(strcmp(lval, "bool") == 0) {
                fprintf(stderr, "ERROR %s BOOLEAN IS NOT AN ORDERED OPERATOR! %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            } else {
                fprintf(stderr, "ERROR %s VOID FUNCTION IN COMPARISON OF ASSIGN LineNo: %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }

        if(enumPullL != enumPullR) { // This checks for Type Mismatch between Enum Vals
            fprintf(stderr, "WARNING TYPE MISMATCH FOR OPERATOR TYPES %s and %s EXITING LineNo %d\n", program->payload.ff.l->payload.fs.ident, program->payload.ff.r->payload.fs.ident,program->lineno);
            exit(1);
        }
        program->sig = "bool";

        break;
    }
    case X_RELC: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9; X_UNARY = 6;
        myTypeCheck(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        myTypeCheck(program->payload.ff.r); // Same here
        int enumPullL = program->payload.ff.l->ty; // This is the Enum of the left type
        int enumPullR = program->payload.ff.r->ty; // This is the Enum of the right type.

        if(enumPullL == 7 || enumPullL == 22) {
            char* typeLookup = lookup(program->payload.ff.l->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullL = 0;
                program->sig = "int";
            } else if (strcmp(typeLookup, "string") == 0) {
                enumPullL = 9;
                program->sig = "string";
            } else if (strcmp(typeLookup, "bool") == 0) {
                enumPullL = 5;
                program->sig = "bool";
        } 
        } else if (enumPullL == X_REL || enumPullL == X_RELC) {
            enumPullL = 5;
        } else if (enumPullL == X_AND || enumPullR == X_OR){
            enumPullL = 5;    
        }else if (enumPullL == X_ADD || enumPullL == X_MUL) {
            enumPullL = 0;
        } else if (enumPullL == X_UNARY){
            enumPullL = 0;
        } else if (enumPullL == X_UNARYNOT){
            enumPullL = 5;
        } else if (enumPullL == X_ACTUALS){
            char* lval = lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullL = 0;
            } else if(strcmp(lval, "string") == 0) {
                enumPullL = 9;
            } else if(strcmp(lval, "bool") == 0) {
                enumPullL = 5;
            } else {
                fprintf(stderr, "ERROR %s VOID FUNCTION IN COMPARISON LineNo: %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }
        if(enumPullR == 7 || enumPullR == 22) {
            char* typeLookup = lookup(program->payload.ff.r->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullR = 0;
            } else if (strcmp(typeLookup, "string") == 0) {
                enumPullR = 9;
            } else if (strcmp(typeLookup, "bool") == 0) {
                enumPullR = 5;
                program->sig = "bool";
            }
        } else if (enumPullR == X_REL || enumPullR == X_RELC) {
            enumPullR = 5;
        } else if (enumPullR == X_AND || enumPullR == X_OR){
            enumPullR = 5;    
        }else if (enumPullR == X_ADD || enumPullR == X_MUL) {
            enumPullL = 0;
        } else if (enumPullR == X_UNARY){
            enumPullR = 0;
        } else if (enumPullR == X_UNARYNOT){
            enumPullR = 5;
            }else if (enumPullR == X_ACTUALS){
            char* lval = lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullR = 0;
            } else if(strcmp(lval, "string") == 0) {
                enumPullR = 9;
            } else if(strcmp(lval, "bool") == 0) {
                enumPullR = 5;
            } else {
                fprintf(stderr, "ERROR %s VOID FUNCTION IN COMPARISON OF ASSIGN LineNo: %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }
    
        if(enumPullL != enumPullR) { // This checks for Type Mismatch between Enum Vals
            fprintf(stderr, "WARNING TYPE MISMATCH FOR OPERATOR TYPES %s and %s EXITING LineNo %d\n", program->payload.ff.l->payload.fs.ident, program->payload.ff.r->payload.fs.ident,program->lineno);
            exit(1);
        }
        program->sig = "bool";

        break;
    }
    //Binary Operators, INTEGERS ONLY
    case X_MUL:
    case X_ADD: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9;
        myTypeCheck(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        myTypeCheck(program->payload.ff.r); // Same here
        int enumPullL = program->payload.ff.l->ty; // This is the Enum of the left type
        int enumPullR = program->payload.ff.r->ty; // This is the Enum of the right type.

        // Fake conversion from unary to int.
        if(enumPullL == 6) {
            enumPullL = 0;
        } else if (enumPullR == 6) {
            enumPullR = 0;
        }

        if(enumPullL != 7 && enumPullL != 0 &&enumPullL == 6) {
            fprintf(stderr, "ERROR NON INTEGER with ENUM: '%d' FOUND FROM BINARY OPERATIONS EXITING LineNo: %d\n", enumPullL, program->lineno);
            exit(1);
        }
        if(enumPullR != 7 && enumPullR != 0 && enumPullR == 6) {
            fprintf(stderr, "ERROR NON INTEGER with ENUM: '%d' FOUND FROM BINARY OPERATIONS EXITING LineNo: %d\n", enumPullR, program->lineno);
            exit(1);
        }

        if(enumPullL == 7 || enumPullL == 22) {
            char* typeLookup = lookup(program->payload.ff.l->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullL = 0;
                program->sig = "int";
            } else {
                fprintf(stderr, "ERROR %s NON INTEGER FOUND FROM BINARY OPERATIONS LineNo: %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
        } else if (enumPullL == X_ADD || enumPullL == X_MUL) {
            enumPullL = 0;
        } else if (enumPullL == X_UNARY){
            enumPullL = 0;
        } else if (enumPullL == X_ACTUALS){
            char* lval = lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullL = 0;
                program->sig = "int";
            } else {
                fprintf(stderr, "ERROR %s NON INTEGER FUNCTION FOUND FROM BINARY OPERATIONS LineNo: %d\n", lookup(program->payload.ff.l->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }


        if(enumPullR == 7 || enumPullR == 22) {
            char* typeLookup = lookup(program->payload.ff.r->payload.fs.ident)->TypeSig;
            if(lookup(typeLookup)->IsType == 0) {
                fprintf(stderr, "ERROR %s is not a TYPE! Check if redeclared LineNo %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
            if(strcmp(typeLookup, "int") == 0) {
                enumPullR = 0;
            } else {
                fprintf(stderr, "ERROR %s NON INTEGER FOUND FROM BINARY OPERATIONS LineNo: %d\n", lookup(typeLookup)->SymbolName,program->lineno);
                exit(1);
            }
        } else if (enumPullR == X_ADD || enumPullR == X_MUL) {
            enumPullR = 0;
        } else if (enumPullR == X_UNARY){
            enumPullR = 0;
        } else if (enumPullR == X_ACTUALS){
            char* lval = lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->FuncReturn;
            if(strcmp(lval, "int") == 0) {
                enumPullR = 0;
                program->sig = "int";
            }  else {
                fprintf(stderr, "ERROR %s NON INTEGER FUNCTION FOUND FROM BINARY OPERATIONS LineNo: %d\n", lookup(program->payload.ff.r->payload.ff.l->payload.fs.ident)->SymbolName,program->lineno);
                exit(1);
            }
        }

        if(enumPullL != enumPullR) { // This checks for Type Mismatch between Enum Vals
            fprintf(stderr, "WARNING TYPE MISMATCH FOR OPERATOR TYPES %s and %s w Enums %d and %d EXITING LineNo %d\n", program->payload.ff.l->payload.fs.ident, program->payload.ff.r->payload.fs.ident,enumPullL, enumPullR, program->lineno);
            exit(1);
        }
        if(enumPullL == 0) {
            program->sig = "int";
        }

        break;
    }
    default: {
        //   printf("UNKNOWN TOKEN: '%d'\n",program->ty);
    }
    };
    //printf("TEST2\n");
    //  return STRING;
}

void myTypeGet(struct Ast* program)
{
    switch (program->ty) {
    case X_PROGRAM: {
        for(int x = 0; x < program->payload.fr.r->length; x++) {
            myTypeGet(&program->payload.fr.r->payload.fc.child[x]);
        }
        break;
    }
    case X_ID: {
        program->sig = lookupID(program->payload.fs.ident,program->lineno)->TypeSig;
        program->symbol = lookupID(program->payload.fs.ident,program->lineno);
        break;
    }
    case X_NUM: {
        program->sig = "int";
        break;
    }
    case X_STRING_LITERAL: {
        program->sig = "string";
        break;
    }
    case X_TID:
    case X_NID:
    case X_FNID: {
        program->sig = lookupID(program->payload.fs.ident,program->lineno)->TypeSig;
        program->symbol = lookupID(program->payload.fs.ident,program->lineno);
        break;
    }
    case X_VAR: {
        char* childTypeL = program->payload.ff.l->payload.fs.ident;
        char* childTypeR = program->payload.ff.r->payload.fs.ident;
        // IF Global Var Found
        if(strcmp(program->payload.fs.ident, "global var") == 0) {
            // FIRST PASS To Fill in Variables
            if(GlobalPass == 0) {
                definel(childTypeL,childTypeR,0,0);
                program->symbol = lookup(childTypeL);
            } else {
                defineType(childTypeL,childTypeR);
                program->symbol = lookup(childTypeL);
            }
        } else {
            definel(childTypeL,childTypeR,0,0); // First the Variable Name, then the name of the right most child.
            program->symbol = lookup(childTypeL);
        }
        myTypeGet(program->payload.ff.l);
        myTypeGet(program->payload.ff.r);
        break;
    }

    case X_IF: {
        myTypeGet(program->payload.ff.l);
        myTypeGet(program->payload.ff.r);
        break;
    }
    case X_IFELSE: {
        myTypeGet(program->payload.ft.l); // ID
        myTypeGet(program->payload.ft.m); // Block IF
        myTypeGet(program->payload.ft.r); // Block ELSE
        break;
    }
    case X_ELSE: {
        myTypeGet(program->payload.fr.r);
        break;
    }
    case X_ELSEIF: {
        myTypeGet(program->payload.fr.r);
        break;
    }

    case X_UNARY: {
        program->sig = "int";
        myTypeGet(program->payload.fr.r);
        break;
    }
    case X_UNARYNOT: {
        myTypeGet(program->payload.fr.r);
        break;
    }
    //fl
    case X_ACTUALS: {
        myTypeGet(program->payload.ft.l);
        program->sig = program->payload.ft.l->sig;
        myTypeGet(program->payload.ft.r);
        break;
    }
    case X_ACTUALS2: {
            myTypeGet(program->payload.ft.l);
        for(int x = 0; x < program->payload.ft.r->length; x++) {
            myTypeGet(&program->payload.ft.r->payload.fc.child[x]);
        }
        break;
    }
    case X_ACTUALS3: {
        memset(strP, 0, sizeof strP);
        myTypeGet(program->payload.ft.l);
        break;
    }
    case X_FACTUALSX: {
        break;
    }
    case X_FORMALSEMPTY: {
        break;
    }
    case X_FORMALS: {
        myTypeGet(program->payload.ff.l);
        for(int x = 0; x < program->payload.ff.r->length; x++) {
            myTypeGet(&program->payload.ff.r->payload.fc.child[x]);
        }
        break;
    }
    case X_PARAMS: {
        myTypeGet(program->payload.ff.l);
        myTypeGet(program->payload.ff.r);
        break;
    }
    case X_PARAMS2: {
        myTypeGet(program->payload.ff.l);
        myTypeGet(program->payload.ff.r);
        break;
    }
    case X_PARDECL: {
        char* childTypeL = program->payload.ff.l->payload.fs.ident;
        char* childTypeR = program->payload.ff.r->payload.fs.ident;
        strcat(strC,childTypeR);
        if(myTypePass == 1) {
            definel(childTypeL,childTypeR,0,0);
            myTypeGet(program->payload.ff.l);
            myTypeGet(program->payload.ff.r);
        }
        break;
    }
    case X_EXPS: {
        myTypeGet(program->payload.ff.r);
        break;
    }
    case X_RETURN: {
        if((strcmp(globalRet, "$void") == 0)) {

        } else {
            rFlag = 1;
        }
        if(program->childflag == 0) {
            myTypeGet(program->payload.ff.r);
        }
        break;
    }
    case X_BLOCK: {
        if(TopLevelTracker == 1) {
            TopLevelTracker = 0;
        } else {
            openscope();
        }
        for(int x = 0; x < program->payload.ff.r->length; x++) {
            myTypeGet(&program->payload.ff.r->payload.fc.child[x]);
        }
        closescope();
        break;
    }
    //fl
    case X_FUNC: {
        memset(strC, 0, sizeof strC);
        char* childTypeL = program->payload.ft.l->payload.fs.ident;
        if(GlobalPass == 1) { // Set so this only procs on the second pass
            char* m2 = lookup("main")->TypeSig;
            if((strcmp(m2, "FX()") != 0)) {
            } else if(strcmp(childTypeL, "main") == 0) { // If current func is main
                isMain = 1;
            } else {
                myTypePass = 0;
                char* childTypeR = program->payload.ft.m->payload.ff.r->payload.fs.ident; // The return type of the function
                myTypeGet(program->payload.ft.m); // The job of this is to grab the variables to define the first function
                char strF[10000]; // Formals
                memset(strF, 0, sizeof strF);
                char* fstring = "FX() ";
                strcat(strF,fstring);
                strcat(strF,childTypeR);
                if((strcmp(strC, "") == 0)) {
                    redefineFunc(childTypeL,strF,0,0,"void",childTypeR);
                } else {
                    redefineFunc(childTypeL,strF,0,0,strC,childTypeR);
                }
            }

            openscope();
            TopLevelTracker = 1; // This is to make sure global declarations can no longer happen.
            myTypePass = 1;
            myTypeGet(program->payload.ft.l);
            myTypeGet(program->payload.ft.m);
            globalRet = program->payload.ft.m->payload.ff.r->payload.fs.ident;
            myTypeGet(program->payload.ft.r);
            TopLevelTracker = 0;
            isMain = 0;
        } else {
            char* childTypeL = program->payload.ft.l->payload.fs.ident; // Name of the ID Name
            definel(childTypeL,"FX()",0,0);
            program->symbol = lookup(childTypeL);
        }
        break;
    }
    case X_FOR: {
        myTypeGet(program->payload.fornode.l);
        myTypeGet(program->payload.fornode.r);
        break;
    }
    case X_EMPTY: {
        break;
    }
    case X_BREAK: {
        break;
    }
    case X_ASSIGN: {
        // Self Check, So this should print r correctly
        program->sig = "void";
        myTypeGet(program->payload.ff.l);
        myTypeGet(program->payload.ff.r);
        break;
    }
    // RELS Comparison
    case X_OR:
    case X_AND: {

    }
    case X_REL: {
        myTypeGet(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        program->sig = program->payload.ff.l->sig;
        myTypeGet(program->payload.ff.r); // Same here
        program->sig = "bool";

        break;
    }
    case X_RELC: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9; X_UNARY = 6;
        myTypeGet(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        program->sig = program->payload.ff.l->sig;
        myTypeGet(program->payload.ff.r); // Same here
        program->sig = "bool";

        break;
    }
    //Binary Operators, INTEGERS ONLY
    case X_MUL:
    case X_ADD: {
        // ENUMS X_ID = 7, X_NUM = 0, X_STRING = 9;
        myTypeGet(program->payload.ff.l); // This runs first so it doesn't use an undeclared id.
        program->sig = program->payload.ff.l->sig;
        myTypeGet(program->payload.ff.r); // Same here
        break;
    }
    default: {
        //   printf("UNKNOWN TOKEN: '%d'\n",program->ty);
    }
    };
}


// Shows AST, heavily modified version of TAs implementation,
void pShowAstImpl(struct Ast* program, int tabbing)
{
    for (int i = 0; i < tabbing; ++i) {
        putchar(' ');
    }
    switch (program->ty) {
    //fsd
    case X_PROGRAM: {
        printf("PROGRAM at Line Number: '%d' and %d children\n",program->lineno,program->payload.fr.r->length);
        for(int x = 0; x < program->payload.fr.r->length; x++) {
            pShowAstImpl(&program->payload.fr.r->payload.fc.child[x], tabbing + TABBING_INC);
        }
        break;
    }
    case X_NUM: {
        printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d' Signature Decl: '%s'\n", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno,program->sig);
        break;
    }
    case X_ID: {
        printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d' Signature Decl: '%s' Symbol Decl: '%p'", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno,program->sig,program->symbol);
        printf("\n");
        break;
    }
    case X_STRING_LITERAL: {
        printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d' Signature Decl: '%s'\n", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno,program->sig);
        break;
    }
    case X_TID:
    case X_NID:
    case X_FNID: {
        printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d' Symbol Decl: %p\n", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno, program->symbol);
        break;
    }
    case X_VAR: {
        printf("%s at Line Number: '%d' Symbol: %p\n",pShowAstTy(program->ty), program->lineno, program->symbol);
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    case X_IF: {
        printf("If at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    case X_IFELSE: {
        printf("If Else at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.ft.l, tabbing + TABBING_INC); // ID
        pShowAstImpl(program->payload.ft.m, tabbing + TABBING_INC); // Block IF
        pShowAstImpl(program->payload.ft.r, tabbing + TABBING_INC); // Block ELSE
        break;
    }
    case X_ELSE: {
        printf("\n");
        pShowAstImpl(program->payload.fr.r, tabbing);
        break;
    }
    case X_ELSEIF: {
        printf("Else IF Block at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.fr.r, tabbing);
        break;
    }
    //fr
    case X_UNARY: {
        printf("Unary at at %d: %s with attribute `%s`\n", program->lineno, pShowAstTy(program->ty), program->payload.fr.ident);
        pShowAstImpl(program->payload.fr.r, tabbing + TABBING_INC);
        break;
    }
    case X_UNARYNOT: {
        printf("Unary at at %d: %s with attribute `%s`\n", program->lineno, pShowAstTy(program->ty), program->payload.fr.ident);
        pShowAstImpl(program->payload.fr.r, tabbing + TABBING_INC);
        break;
    }
    //fl
    /**/
    case X_ACTUALS: {
        printf("FUNC CALL + Sig: %s\n",program->sig);
        pShowAstImpl(program->payload.ft.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ft.r, tabbing + TABBING_INC);
        break;
    }
    case X_ACTUALS2: {
        printf("ACTUALS CALL AT at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.ft.l, tabbing + TABBING_INC);
        for(int x = 0; x < program->payload.ft.r->length; x++) {
            pShowAstImpl(&program->payload.ft.r->payload.fc.child[x], tabbing + TABBING_INC);
        }
        break;
    }
    case X_ACTUALS3: {
        printf("FUNC CALL AT at Line Number: '%d' \n",program->lineno);
        pShowAstImpl(program->payload.ft.l, tabbing + TABBING_INC);
        break;
    }
    case X_FACTUALSX: {
        printf("ACTUALS CALL AT at Line Number: '%d'\n",program->lineno);
        break;
    }
    case X_FORMALSEMPTY: {
        printf("Formals\n");
        break;
    }
    case X_FORMALS: {
        printf("Formals at Line Number: '%d' and %d children\n",program->lineno,program->payload.ff.r->length);
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        for(int x = 0; x < program->payload.ff.r->length; x++) {
            pShowAstImpl(&program->payload.ff.r->payload.fc.child[x], tabbing + TABBING_INC);
        }
        break;
    }
    case X_PARAMS: {
        printf("Formal\n");
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    case X_PARAMS2: {
        printf("SIG\n");
        if(program->childflag == 0) {
            pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        }
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC); // Check why this became null all of the sudden
        break;
    }
    case X_PARDECL: {
        printf("Formal\n");
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    case X_EXPS: {
        printf("Expression Statement at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    case X_RETURN: {
        printf("RETURN Statement at Line Number: '%d'\n",program->lineno);
        if(program->childflag == 0) {
            pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        }
        break;
    }
    case X_BLOCK: {
        printf("Block at Line Number: '%d' and %d children\n",program->lineno,program->payload.ff.r->length);
        for(int x = 0; x < program->payload.ff.r->length; x++) {
            pShowAstImpl(&program->payload.ff.r->payload.fc.child[x], tabbing + TABBING_INC);
        }
        break;
    }
    //fl
    case X_FUNC: {
        printf("Func at Line Number: '%d'\n",program->lineno);
        pShowAstImpl(program->payload.ft.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ft.m, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ft.r, tabbing + TABBING_INC);
        break;
    }
    case X_FOR: {
        printf("ForNode: '%s' Lexeme: '%s' Line Number: '%d'\n", pShowAstTy(program->ty), program->payload.fornode.ident, program->lineno);
        pShowAstImpl(program->payload.fornode.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.fornode.r, tabbing + TABBING_INC);
        break;
    }
    case X_EMPTY: {
        printf("Empty Statement at Line Number: '%d'\n",program->lineno);
        break;
    }
    case X_BREAK: {
        printf("Break Statement at Line Number: '%d'\n",program->lineno);
        break;
    }

    case X_MUL:
    case X_ADD:
    case X_OR:
    case X_AND:
    case X_REL:
    case X_RELC:
    case X_ASSIGN: {
        printf("TOKEN: '%s' Lexeme: '%s' Line Number: '%d' \n", pShowAstTy(program->ty), program->payload.fs.ident, program->lineno);
        pShowAstImpl(program->payload.ff.l, tabbing + TABBING_INC);
        pShowAstImpl(program->payload.ff.r, tabbing + TABBING_INC);
        break;
    }
    default: {
        printf("UNKNOWN TOKEN: '%d'\n",program->ty);
    }
    };
}

// Provided by TA to help generate ASTs
char const* pShowAstTy(enum AstTy ty)
{
    switch(ty) {
#define X(INP) case INP: return #INP;
        AST_TY_X
#undef X
    default:
        return "<bad AstTy>";
    }
}
