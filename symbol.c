#include "hashmap.h"
#include "stack.h"
#include "ast.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Symbol Table Implementation, used for Milestone 3 and 4, Most of this code uses the Stack and Hashmap Functions, which are esssential towards storing data in the Symbol Table.

static struct Stack SymbolStack;
static int StackCounter = 0;
#define DEFAULT_STACK_CAPACITY 2
#define DEFAULT_HASH_MAP_CAPACITY 1

// Init Stack
void initTableC(){
  stackInit(&SymbolStack,DEFAULT_STACK_CAPACITY);
  openscope();
}

// First Define Function for the First Definition of Vars.
void definel(char* name, char* TS, int isconst, int isty){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->IsConst = isconst;
symbolRecord->IsType = isty;
if(hashMapFind((struct HashMap *)stackTop(&SymbolStack), name) != NULL){
  fprintf(stderr, "WARNING %s exists IN CURRENT SCOPE, EXITING\n", name);
  exit(1);
}
if(strcmp(name, "break") == 0 || strcmp(name, "else") == 0 || strcmp(name, "for") == 0 || strcmp(name, "func") == 0 || strcmp(name, "if") == 0 || strcmp(name, "return") == 0 || strcmp(name, "var") == 0){
  fprintf(stderr, "WARNING %s is a reserved word, tried to redefine it, EXITING\n", name);
  exit(1);
}
hashMapInsert((struct HashMap *)stackTop(&SymbolStack), name, symbolRecord);
}

// This was created to deal with multiple passes, as a 2nd pass could go through the same records, so this was created for that pass.
void redefinel(char* name, char* TS, int isconst, int isty){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->IsConst = isconst;
symbolRecord->IsType = isty;
if(strcmp(name, "break") == 0 || strcmp(name, "else") == 0 || strcmp(name, "for") == 0 || strcmp(name, "func") == 0 || strcmp(name, "if") == 0 || strcmp(name, "return") == 0 || strcmp(name, "var") == 0){
  fprintf(stderr, "WARNING %s is a reserved word, tried to redefine it, EXITING\n", name);
  exit(1);
}
hashMapInsert((struct HashMap *)stackTop(&SymbolStack), name, symbolRecord);
}

// Additional define functions for niche purposes etc.
void define(char* name,char* TS){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->IsConst = 0;
symbolRecord->IsType = 0;
if(hashMapFind((struct HashMap *)stackTop(&SymbolStack), name) != NULL){
  fprintf(stderr, "WARNING %s exists IN CURRENT SCOPE, EXITING\n", name);
  exit(1);
}

hashMapInsert((struct HashMap *)stackTop(&SymbolStack), name, symbolRecord);
}

void defineName(char* name){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->IsConst = 0;
symbolRecord->IsType = 0;
if(hashMapFind((struct HashMap *)stackTop(&SymbolStack), name) != NULL){
  fprintf(stderr, "WARNING %s exists IN CURRENT SCOPE, EXITING\n", name);
  exit(1);
}
}

void defineType(char* name, char* TS){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->IsConst = 0;
symbolRecord->IsType = 0;
}

// Function Define, This is a specical version that works only for functions, used to store data for functions
void defineFunc(char* name, char* TS, char* rtName, int isconst, int isty, char* funcPR, char* funcR){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->rtname = rtName;
symbolRecord->IsConst = isconst;
symbolRecord->IsType = isty;
symbolRecord->FuncParams = funcPR;
symbolRecord->FuncReturn = funcR;
if(hashMapFind((struct HashMap *)stackTop(&SymbolStack), name) != NULL){
  fprintf(stderr, "WARNING %s exists IN CURRENT SCOPE, EXITING\n", name);
  exit(1);
}
if(strcmp(name, "break") == 0 || strcmp(name, "else") == 0 || strcmp(name, "for") == 0 || strcmp(name, "func") == 0 || strcmp(name, "if") == 0 || strcmp(name, "return") == 0 || strcmp(name, "var") == 0){
  fprintf(stderr, "WARNING %s is a reserved word, tried to redefine it, EXITING\n", name);
  exit(1);
}
hashMapInsert((struct HashMap *)stackTop(&SymbolStack), name, symbolRecord);
}

void redefineFunc(char* name, char* TS, int isconst, int isty, char* funcPR, char* funcR){
struct Record * symbolRecord = malloc(sizeof(struct Record));
symbolRecord->SymbolName = name;
symbolRecord->TypeSig = TS;
symbolRecord->IsConst = isconst;
symbolRecord->IsType = isty;
symbolRecord->FuncParams = funcPR;
symbolRecord->FuncReturn = funcR;
if(strcmp(name, "break") == 0 || strcmp(name, "else") == 0 || strcmp(name, "for") == 0 || strcmp(name, "func") == 0 || strcmp(name, "if") == 0 || strcmp(name, "return") == 0 || strcmp(name, "var") == 0){
  fprintf(stderr, "WARNING %s is a reserved word, tried to redefine it, EXITING\n", name);
  exit(1);
}
hashMapInsert((struct HashMap *)stackTop(&SymbolStack), name, symbolRecord);
}

// Lookup the record in the symbol table and return it, main way to access is like record->SymbolName for example.
struct Record* lookup(char* name){
if(StackCounter == 0){
  fprintf(stderr, "WARNING ATTEMPTED TO LOOK UP WITH NO STACKS %s EXITING\n", name);
  exit(1);
}
int tempCounter = StackCounter;
int arrayIndex = 0;
for(tempCounter = StackCounter; tempCounter > 0; tempCounter--){
  if(hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name) != NULL){
    return hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name);
  }
  arrayIndex++;
}

  fprintf(stderr, "WARNING NULL FOUND WITH LOOKUP AT CHAR NAME %s AT EXITING\n", name);
  exit(1);
return hashMapFind((struct HashMap *)stackTop(&SymbolStack), name);
}

// Lookup the record in the symbol table and return it, main way to access is like record->SymbolName for example.
struct Record* lookupID(char* name, int lineno){
if(StackCounter == 0){
  fprintf(stderr, "WARNING ATTEMPTED TO LOOK UP WITH NO STACKS %s EXITING\n", name);
  exit(1);
}
int tempCounter = StackCounter;
int arrayIndex = 0;
int shouldReturn = 0;
for(tempCounter = StackCounter; tempCounter > 0; tempCounter--){
  if(hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name) != NULL){
    shouldReturn = 1;
    return hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name);
  }
  arrayIndex++;
}
if(shouldReturn == 0){
}
return hashMapFind((struct HashMap *)stackTop(&SymbolStack), name);
}

// This was there to check for the main function.
struct Record* lookupMain(char* name, int lineno){
if(StackCounter == 0){
  fprintf(stderr, "WARNING ATTEMPTED TO LOOK UP WITH NO STACKS %s EXITING\n", name);
  exit(1);
}
int tempCounter = StackCounter;
int arrayIndex = 0;
int shouldReturn = 0;
for(tempCounter = StackCounter; tempCounter > 0; tempCounter--){
  if(hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name) != NULL){
    shouldReturn = 1;
    return hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name);
  }
  arrayIndex++;
}
if(shouldReturn == 0){
  fprintf(stderr, "WARNING NO MAIN FUCNCTION FOUND AT: '%s' at lineno '%d' EXITING\n", name,lineno);
  exit(1);
}
return hashMapFind((struct HashMap *)stackTop(&SymbolStack), name);
}
// Lookup the record in the symbol table and return it, main way to access is like record->SymbolName for example.
void lookupType(char* name, int lineno){
if(StackCounter == 0){
  fprintf(stderr, "WARNING ATTEMPTED TO LOOK UP WITH NO STACKS %s EXITING\n", name);
  exit(1);
}
int tempCounter = StackCounter;
int arrayIndex = 0;
int shouldReturn = 0;
for(tempCounter = StackCounter; tempCounter > 0; tempCounter--){
  if(hashMapFind((struct HashMap *)stackRead(&SymbolStack,arrayIndex), name) != NULL){
    shouldReturn = 1;
  }
  arrayIndex++;
}
if(shouldReturn == 0){
  fprintf(stderr, "WARNING COULDN'T FIND TYPE %s IN STACK at lineno '%d' EXITING\n", name,lineno);
  exit(1);
}
}

// Opens a Scope on the stack for new variable declarations.
void openscope(){
    struct HashMap* symbolTable = malloc(sizeof(struct HashMap));
    hashMapInit(symbolTable, 1);
    stackPush(&SymbolStack, symbolTable);
    StackCounter++;
};

// Closes the scope and removes all variable declarations inside of that scope.
void closescope(){
  if(StackCounter == 0){
    fprintf(stderr, "WARNING ATTEMPTED TO CLOSE STACKS WITH NO STACKS EXITING\n");
    exit(1);
  }
    stackPop(&SymbolStack);
    StackCounter--;
};