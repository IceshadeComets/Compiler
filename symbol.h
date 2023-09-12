void initTableC();

// This is my symbol table structure, 7 types used for specific cases.
struct Record{
    // Every symbol will map to one of these records
        char* SymbolName; // The name of the symbol
        char* TypeSig; // Type Signature of the program is String
        char* rtname; // Return Type
        int IsConst; // 0 for false, 1 for true
        int IsType; // 0 for false, 1 for true
        char* FuncParams; // The inside paramaters of a function
        char* FuncReturn; // The function return type
};


void define(char* name,char* TS);

void defineName(char* name);

void defineType(char* name, char* TS);

struct Record* lookup(char* name);

struct Record* lookupID(char* name, int lineno);

struct Record* lookupMain(char* name, int lineno);

void lookupType(char* name, int lineno);

void definel(char* name, char* TS, int isconst, int isty);

void defineFunc(char* name, char* TS, char* rtName, int isconst, int isty, char* funcPR, char* funcR);

void redefineFunc(char* name, char* TS, int isconst, int isty, char* funcPR, char* funcR);

void redefinel(char* name, char* TS, int isconst, int isty);

void openscope();

void closescope();