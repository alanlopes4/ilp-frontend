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

%type <a> exp attr 

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
    PRINTAR exp ENDL 
;

exp: exp '+' exp          {  $$ = newast('+', $1,$3);}
   | exp '-' exp          { $$ = newast('-', $1,$3);}
   | exp '*' exp          { $$ = newast('*', $1,$3); }
   | exp '/' exp          { $$ = newast('/', $1,$3); }
   | exp '^' exp          { $$ = newast('^', $1, $3); }  
   | '(' exp ')'          { $$ = $2; }  
   | '-' exp %prec UMINUS { $$ = newast('M', $2, NULL); } 
   | NUMBER               { $$ = newnum($1); }
   | NAME                 { $$ = newref($1); }

;



%%
