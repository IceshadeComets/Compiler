%{
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include"ast.h"
#include<string.h>
#include"globals.h"

// Global Declarations for Bison
void yyerror(char * err);
int yylex(void);
char allocationadd;
int *myPointer;
int statementFlag = 0;
int globaldec = 0;
int formalvoid = 0;

%}

%locations
%define parse.error custom

%union {
    struct Ast *myAst;
    char *myString;
    int ival;
}
%start start

// this indicates the return type of the *terminals* to be the type of the
// field `myString` in the `%union` declaration

%token <myString> 
    T_DOT "."
    T_IDENTIFIER
    T_SUB "-"
    T_DIV "/"
    T_MODULO "%"
    T_SBRACKET "("
    T_EBRACKET ")"
    T_SBRACE "{"
    T_EBRACE "}"
    T_SEMICOL ";"
    T_COMMA ","
    T_NOT "!"
    T_NE "!="
    T_EQ "=="
    T_ASSIGN "="
    T_LT "<"
    T_LE "<="
    T_GT ">"
    T_GE ">="
    T_AND "&&"
    T_OR "||"
    T_BREAK "break"
    T_ELSE "else"
    T_FOR "for"
    T_FUNC "func"
    T_IF "if"
    T_RETURN "return"
    T_VAR "var"
    T_PLUS "+" 
    T_MULT "*"
    T_NUM
    T_STRING_LITERAL

%type <myAst>
    Expression
    ExpressionList
    unary_op
    bOp1
    bOp2
    relOp
    addOp 
    mulOp
    factor
    identifier
    Declaration
    Type
    TypeName
    BreakStmt
    Block
    SimpleStmt
    ReturnStmt
    ExpressionStmt
    Assignment
    ForStmt
    IfStmt
    ElseStatement
    Condition
    TopLevelDecl
    FunctionDecl
    FunctionName
    Signature
    FunctionBody
    Parameters
    Result
    SourceFile
    StatementList
    Statement
    EmptyStmt
    NewID
    GDeclaration
    starttile
    ParameterList
    ParameterDecl
    astExpr
%%

start : starttile {program = $1;}
      ;

starttile : SourceFile[ex] {                $$ = malloc(sizeof(struct Ast));
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_PROGRAM;
                                            $$->payload.fr.ident = "program";
                                            $$->payload.fr.r = $ex;
                   }
    ;

SourceFile : %empty {   $$ = malloc(sizeof(struct Ast));
                        $$->length = 0;
                        $$->capacity = 1;
                        $$->payload.fc.child = malloc(sizeof(struct Ast)*1);}
    | SourceFile[SL] TopLevelDecl[ex] ";" {
        if($SL->length == $SL->capacity){$SL->capacity = $SL->capacity + 1; $SL-> payload.fc.child = realloc($SL->payload.fc.child,$SL->capacity*sizeof(struct Ast));};
        $SL->payload.fc.child[$SL->length] = *$ex;
        $SL->length += 1;
        }
    ;

/* Function Declarations */
FunctionDecl : "func" FunctionName[l] Signature[m] FunctionBody[r] {
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_FUNC;
                                            $$->payload.ft.l = $l;
                                            $$->payload.ft.m = $m;
                                            $$->payload.ft.r = $r;
                                            }
    ;

FunctionName : T_IDENTIFIER[id]{            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_FNID; 
                                            $$->payload.fs.ident = $id; 
                                            }
     ;

FunctionBody : Block {}
    ;

Signature : Parameters[ex]  {               $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_PARAMS2;
                                            $$->payload.ff.ident = "formals";
                                            $$->payload.ff.l = $ex;
                                            $$->payload.ff.r = &IVOID; // Fake Void Insertion
                                            }

    | Parameters[l] Result[r] {             $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_PARAMS2;
                                            $$->payload.ff.ident = "formals";
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    ;

Result : TypeName
    ;

Parameters : "(" ")" {                                     $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;
                                                           $$->ty = X_FORMALSEMPTY;
                                                           $$->payload.ff.ident = "formal";
                                                           }
    | "(" ParameterDecl[l] ParameterList[ex] ")" {         $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;
                                                           $$->ty = X_FORMALS;
                                                           $$->payload.ff.ident = "formal";
                                                           $$->payload.ff.l = $l;
                                                           $$->payload.ff.r = $ex;}
    | "(" ParameterDecl[l] ParameterList[ex] "," ")" {     $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;    
                                                           $$->ty = X_FORMALS;
                                                           $$->payload.ff.ident = "formal";
                                                           $$->payload.ff.l = $l;
                                                           $$->payload.ff.r = $ex;}
    ;

ParameterList : %empty {$$ = malloc(sizeof(struct Ast));
                        $$->length = 0;
                        $$->capacity = 1;
                        $$->payload.fc.child = malloc(sizeof(struct Ast)*1);}
    | ParameterList[SL] "," ParameterDecl[ex] {
            if($SL->length == $SL->capacity){$SL->capacity = $SL->capacity + 1; $SL-> payload.fc.child = realloc($SL->payload.fc.child,$SL->capacity*sizeof(struct Ast));};
            $SL->payload.fc.child[$SL->length] = *$ex;
            $SL->length += 1;
        }
    ; 

ParameterDecl : NewID[l] Type[r] {
                            $$ = malloc( sizeof(struct Ast) );
                            $$->lineno = @$.first_line;
                            $$->ty = X_PARDECL;
                            $$->payload.ff.ident = "pardecl";
                            $$->payload.ff.l = $l;
                            $$->payload.ff.r = $r;
                           }
    ;

Type: TypeName
    ;

TopLevelDecl : GDeclaration {}
    |   FunctionDecl
    ;

Statement : Declaration {}
      | SimpleStmt
      | ReturnStmt
      | BreakStmt
      | Block
      | IfStmt
      | ForStmt
    ;

/* Variable Declarations */

GDeclaration : "var" NewID[l] TypeName[r]   {$$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_VAR;
                                            $$->payload.ff.ident = "global var";
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    ;

Declaration : "var" NewID[l] TypeName[r]   {$$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_VAR;
                                            $$->payload.ff.ident = "var";
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    ;

NewID : T_IDENTIFIER[id]{                $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_NID; 
                                            $$->payload.fs.ident = $id; 
                                            }
 
TypeName : T_IDENTIFIER[id]{                $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_TID; 
                                            $$->payload.fs.ident = $id; 
                                            }
     ;

/* SimpleStmt Block */
SimpleStmt : EmptyStmt
    | ExpressionStmt
    | Assignment
    ;

EmptyStmt : %empty {                        
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_EMPTY;
    }
    ;

ExpressionStmt : Expression[ex] {           
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_EXPS;
                                            $$->payload.ff.ident = "expression";
                                            $$->payload.ff.r = $ex;
                                            }
    ;
    
Assignment : NewID[l] "="[op] Expression[r]      {
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_ASSIGN;
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    ;

ReturnStmt : "return" {$$ = malloc( sizeof(struct Ast) );
                       $$->childflag = 1;
                       $$->lineno = @$.first_line;          
                       $$->ty = X_RETURN;
                       $$->payload.ff.ident = "return";
                        }
    | "return" Expression[ex] {$$ = malloc( sizeof(struct Ast) );
                           $$->lineno = @$.first_line;
                           $$->childflag = 0;
                           $$->ty = X_RETURN;
                           $$->payload.ff.ident = "return";
                           $$->payload.ff.r = $ex;
                        }
    ;

BreakStmt : "break" {                       $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_BREAK;
                                            $$->payload.ff.ident = "break";
                                            }
    ;

// Block Node for Braces
Block : "{" StatementList[ex] "}"   {
                                            $$ = malloc(sizeof(struct Ast));
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_BLOCK;
                                            $$->payload.ff.ident = "block";
                                            $$->payload.ff.r = $ex;
                                            }
    ;

StatementList : %empty {$$ = malloc(sizeof(struct Ast));
                        $$->length = 0;
                        $$->capacity = 1;
                        $$->payload.fc.child = malloc(sizeof(struct Ast)*1);
}
    | StatementList[SL] Statement[ex] ";" {
        if($SL->length == $SL->capacity){$SL->capacity = $SL->capacity + 1; $SL-> payload.fc.child = realloc($SL->payload.fc.child,$SL->capacity*sizeof(struct Ast));};
        $SL->payload.fc.child[$SL->length] = *$ex;
        $SL->length += 1;
    }
    ;   


/* If/Else Statement */
IfStmt : "if" Expression[l] Block[r] { // IF
                        $$ = malloc( sizeof(struct Ast) );
                        $$->lineno = @$.first_line;
                        $$->ty = X_IF;
                        $$->payload.ff.ident = "if"; 
                        $$->payload.ff.l = $l;
                        $$->payload.ff.r = $r;
                        }
    | "if" Expression[l] Block[m] ElseStatement[r] { // IF ELSE
                        $$ = malloc( sizeof(struct Ast) );
                        $$->lineno = @$.first_line;
                        $$->ty = X_IFELSE;
                        $$->payload.ft.ident = "ifelse"; 
                        $$->payload.ft.l = $l;
                        $$->payload.ft.m = $m;  
                        $$->payload.ft.r = $r;
                        }
    ;

ElseStatement : "else" IfStmt[r] { 
                        $$ = malloc( sizeof(struct Ast) ); // ELSE
                        $$->lineno = @$.first_line;
                        $$->ty = X_ELSEIF;
                        $$->payload.fr.ident = "else"; 
                        $$->payload.fr.r = $r;
    } 
    | "else" Block[r] { $$ = malloc( sizeof(struct Ast) ); // ELSE
                        $$->lineno = @$.first_line;
                        $$->ty = X_ELSE;
                        $$->payload.fr.ident = "else"; 
                        $$->payload.fr.r = $r;
                        }
    ;

/* For Statement, remember to look up later */
ForStmt : "for" Block[ex] { // for {}
                        $$ = malloc( sizeof(struct Ast) );
                        $$->lineno = @$.first_line;
                        $$->ty = X_FOR;
                        $$->payload.fornode.ident = "for"; 
                        $$->payload.fornode.l = &TRUE;
                        $$->payload.fornode.r = $ex;
                        }
    | "for" Condition[l] Block[ex] { // for x > 5 {}
                        $$ = malloc( sizeof(struct Ast) );
                        $$->lineno = @$.first_line;
                        $$->ty = X_FOR;
                        $$->payload.fornode.ident = "for"; 
                        $$->payload.fornode.l = $l;
                        $$->payload.fornode.r = $ex;
    }
    ;

Condition : Expression {}
    ;



Expression : bOp1[op]
    ;

bOp1 : bOp1[l] "||"[op] bOp2[r]{
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_OR;
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                            }
     | bOp2 {$$ = $1;}
     ;

bOp2 : bOp2[l] "&&"[op] relOp[r]{
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_AND; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;

                            }
     | relOp {$$ = $1;}
     ;

relOp : relOp[l] "=="[op] addOp[r]{         $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_RELC; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | relOp[l] "!="[op] addOp[r]{         $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_RELC; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | relOp[l] "<"[op] addOp[r]{         $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_REL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | relOp[l] "<="[op] addOp[r]{         $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_REL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | relOp[l] ">"[op] addOp[r]{         $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_REL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | relOp[l] ">="[op] addOp[r]{        $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_REL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                            }
    | addOp {$$ = $1;}
    ;

addOp  : addOp[l] "+"[op]  mulOp[r]  { 
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_ADD; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                          }
          | addOp[l]  "-"[op] mulOp[r] {
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_ADD; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                          }
         | mulOp                     { $$ = $1; }

mulOp  : mulOp[l] "*"[op]  unary_op[r]   { 
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_MUL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                       }
        | mulOp[l] "/"[op]  unary_op[r]   { 
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_MUL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                       }
        | mulOp[l] "%"[op]  unary_op[r]   { 
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_MUL; 
                                            $$->payload.ff.ident = $op; 
                                            $$->payload.ff.l = $l;
                                            $$->payload.ff.r = $r;
                                       }
         | unary_op                    { $$ = $1; 
                                       }

unary_op : "-"[op] unary_op[r] {            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_UNARY;
                                            $$->payload.fr.ident = $op;
                                            $$->payload.fr.r = $r;
                                            }
        | "!"[op] unary_op[r] {            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_UNARYNOT;
                                            $$->payload.fr.ident = $op;
                                            $$->payload.fr.r = $r;
                                            }
         | factor                     { $$ = $1; 
                                      }

factor : T_NUM[num]  { 
                                            $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;
                                            $$->ty = X_NUM; 
                                            $$->payload.fs.ident = $num;
                                            }
        | T_STRING_LITERAL[sl]{             $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_STRING_LITERAL; 
                                            $$->payload.fs.ident = $sl; 
                                            }

        | identifier
        | "(" Expression[ex] ")" {$$ = $ex;}
        | factor[a] "(" ")" {                              $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;    
                                                           $$->ty = X_ACTUALS;
                                                           $$->payload.ft.l = $a;
                                                           $$->payload.ft.r = &FACTUALS;} // Insert fake empty braces inside
        | factor[a] "(" astExpr[ex] ")" {                  $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;    
                                                           $$->ty = X_ACTUALS;
                                                           $$->payload.ft.l = $a;
                                                           $$->payload.ft.r = $ex;
            }
        ;

        astExpr : Expression[a] ExpressionList[ex] optExpr { 
                                                           $$ = malloc( sizeof(struct Ast) );
                                                           $$->lineno = @$.first_line;    
                                                           $$->ty = X_ACTUALS2;
                                                           $$->payload.ft.l = $a;
                                                           $$->payload.ft.r = $ex;}
            ;

        optExpr : %empty
            | "," 
            ;

// Implements a List of Expressions for Function Calls, Similar Approach for Function Params
ExpressionList : %empty {$$ = malloc(sizeof(struct Ast));
                        $$->length = 0;
                        $$->capacity = 1;
                        $$->payload.fc.child = malloc(sizeof(struct Ast)*1);
                        }
    |   ExpressionList[SL] "," Expression[ex] {
        if($SL->length == $SL->capacity){$SL->capacity = $SL->capacity + 1; $SL-> payload.fc.child = realloc($SL->payload.fc.child,$SL->capacity*sizeof(struct Ast));};
        $SL->payload.fc.child[$SL->length] = *$ex;
        $SL->length += 1;
        }
    ;

identifier :  T_IDENTIFIER[id]{             $$ = malloc( sizeof(struct Ast) );
                                            $$->lineno = @$.first_line;          
                                            $$->ty = X_ID; 
                                            $$->payload.fs.ident = $id; 
                                            }

%%

// Syntax Error Checking, Modified Version of the one provided in the Tutorial Section, code is similar to due to how robust the original was in detecting Syntax Errrors.
static int yyreport_syntax_error (const yypcontext_t *yyctx) {
    // Gets the unexpected token and linenumbers and prints them out.
    char const *unexpectedToken =
        yypcontext_token(yyctx) == YYSYMBOL_YYEMPTY
            ? ""
            : yysymbol_name (yypcontext_token(yyctx));
    YYLTYPE unexpectedTokenLoc = *yypcontext_location(yyctx);
    syntaxError(unexpectedToken, unexpectedTokenLoc.first_line);
    // This grabs every token that we expected when the parser throws a Syntax Error.
    int const MAX_EXPECTED_TOKENS = 10;
    yysymbol_kind_t expectedSymbolKinds[MAX_EXPECTED_TOKENS];
    int actualNumberOfExpectedTokens = 
        yypcontext_expected_tokens
            ( yyctx
            , expectedSymbolKinds
            , MAX_EXPECTED_TOKENS
            );
    if (actualNumberOfExpectedTokens < 0) {
        fprintf(stderr , "\n");
        return actualNumberOfExpectedTokens;
    }
    if (actualNumberOfExpectedTokens == 0) {
        if (expectedSymbolKinds[0] != YYSYMBOL_YYEMPTY) {
            actualNumberOfExpectedTokens = MAX_EXPECTED_TOKENS;
        }
    }
    // Prints token list based on whether there is one or more tokens in the warning message.
    if (actualNumberOfExpectedTokens == 1) {
        fprintf(stderr, "Needed this Token:  `%s`.\n", yysymbol_name(expectedSymbolKinds[0]));
    } else if (actualNumberOfExpectedTokens > 1) {
        fprintf(stderr, "Needed these Tokens: ");
        for (int i = 0; i + 1 < actualNumberOfExpectedTokens; ++i)
            fprintf(stderr, "`%s`, ", yysymbol_name(expectedSymbolKinds[i]));

        fprintf(stderr, "or `%s`.\n", yysymbol_name(expectedSymbolKinds[actualNumberOfExpectedTokens - 1]));
    }
    return 0; 
}

void yyerror(char* err) {
    fprintf(stderr, "%s", err);
}