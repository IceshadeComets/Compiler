#ifndef AST_H
#define AST_H

#define AST_TY_X                                 \
    X(X_NUM)                                     \
    X(X_MUL)                                     \
    X(X_ADD)                                     \
    X(X_REL)                                     \
    X(X_AND)                                     \
    X(X_OR)                                      \
    X(X_UNARY)                                   \
    X(X_ID)                                      \
    X(X_ASSIGN)                                  \
    X(X_STRING_LITERAL)                          \
    X(X_ACTUALS)                                 \
    X(X_EXPS)                                    \
    X(X_BLOCK)                                   \
    X(X_EMPTY)                                   \
    X(X_BREAK)                                   \
    X(X_FUNC)                                    \
    X(X_RETURN)                                  \
    X(X_FOR)                                     \
    X(X_IF)                                      \
    X(X_IFELSE)                                  \
    X(X_ELSE)                                    \
    X(X_TID)                                     \
    X(X_NID)                                     \
    X(X_VAR)                                     \
    X(X_PROGRAM)                                 \
    X(X_FNID)                                    \
    X(X_ELSEIF)                                  \
    X(X_PARAMS)                                  \
    X(X_PARAMS2)                                 \
    X(X_FORMALS)                                 \
    X(X_FORMALSEMPTY)                            \
    X(X_PARDECL)                                 \
    X(X_ACTUALS2)                                \
    X(X_ACTUALS3)                                \
    X(X_FACTUALSX)                               \
    X(X_UNARYNOT)                                \
    X(X_RELC)                                    \
    


enum AstTy {
    #define X(INP) INP,
    AST_TY_X   
    #undef X
};

// R Rou
struct fR {
    char* ident;
    struct Ast* r;
};

struct fL {
    char* ident;
    struct Ast* r;
};

// Full Routine, Used for Binary
struct fF {
    char* ident;
    struct Ast* l;
    struct Ast* r;
};

// Single Routine, used for single factors
struct fS {
    char* ident;
};

// Single Routine, Used for Indiviudal AST NODES WITH NO CHILDREN
struct singleN{
    struct Ast* s;
};

// Triple Routine, Includes Middle Child
struct fT {
        char* ident;
    struct Ast* l;
    struct Ast* m;
    struct Ast* r;
};

struct forNode {
    char* ident;
    char* l2; 
    struct Ast* l;
    struct Ast* r;
};

struct extraChild{
    struct Ast* child;
};

   union astOpt {
        struct fR fr; // Right
        struct fL fl; // Left
        struct fF ff; // Full
        struct fS fs; // Single
        struct singleN sn;
        struct fT ft; // Middle
        struct forNode fornode; // For
        struct extraChild fc;
    };

struct Ast {
    enum AstTy ty;
    int lineno;
    int length;
    int capacity;
    int childflag;
    char* idtransfer;
    char* sig;
    enum AstTy parentty;
    struct Record* symbol;
    union astOpt payload;
};

// Global Variable for YYParse
extern struct Ast* program;

// pretty printing functions
void pShowAst(struct Ast* program);
void pSemanticsAst(struct Ast* program);
char const* pShowAstTy(enum AstTy ty);
void myTypeCheck(struct Ast* program);
void myTypeGet(struct Ast* program);

#endif