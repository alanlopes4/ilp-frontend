%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "fb3-2.h"
%}

%union {
  struct ast *a;  
  double d;
  struct symbol *s;	
}




%token <d> NUMBER
%token <s> NAME
%token <s> PRINTAR
%token <s> ARVORE
%token EOL



%right '='
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <a> exp stmt

%start calclist

%%

stmt: 
    exp  
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
   | NAME '=' exp         { $$ = newasgn($1, $3); }
;


calclist: /* nothing */
  | calclist stmt EOL {
     //dumpast($2, 0);
     printf("> ");
     eval($2);
     //treefree($2);
    }
  | calclist PRINTAR stmt EOL {
    if(debug) dumpast($3, 0);
     
     printf("= %4.4g\n> ", eval($3));
     //treefree($3);
    }
  | calclist ARVORE EOL {
    for(int i = 0; i < posicao; i++){
      if(strlen(asttab[i].name)>0)
        dumpast(asttab[i].a, 0);
    }
    printf("> ");
  }

  | calclist error EOL { yyerrok; printf("> "); }
 ;
%%
