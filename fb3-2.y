%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "fb3-2.h"

%}

%locations

%union {
  struct ast *a;  
  double d;
  struct symbol *s;	
}




%token <d> NUMBER
%token <s> NAME
%token <s> PRINTAR
%token <s> ARVORE
%token <s> ENDL
%token EOL



%right '='
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <a> exp attr exp1 exp2 exp3

%start list

%%

list:
    item | item list 
;
item:
    attr | print 
;
attr:
    NAME '=' exp ENDL   { $$ = newasgn($1, $3); }
;
print:
    PRINTAR exp ENDL    { newprint($2); }
;

exp: exp1                 { $$ = $1; }
   | exp '+' exp1          {  $$ = newast('+', $1,$3);}
   | exp '-' exp1         { $$ = newast('-', $1,$3);}
;
exp1: exp2                 { $$ = $1; }
   | exp1 '*' exp2          { $$ = newast('*', $1,$3); }
   | exp1 '/' exp2          { $$ = newast('/', $1,$3); }
;
exp2: exp3                 { $$ = $1; }
   | exp2 '^' exp3          { $$ = newast('^', $1, $3); }  
   | '(' exp ')'          { $$ = $2; }  
   | '-' exp3 %prec UMINUS { $$ = newast('M', $2, NULL); } 
;
exp3:
    NUMBER               { $$ = newnum($1); }
   | NAME                 { $$ = newref($1); }
;



%%
