

%option noyywrap nodefault yylineno
%{
# include "fb3-2.h"
# include "fb3-2.tab.h"
%}
 
/* float exponent */
EXP	([Ee][-+]?[0-9]+) 
 
%%
"arvore" { return ARVORE; }
"print" { return PRINTAR; }

"+" | 
"-" |     
"*" |  
"/" |      
"^" |
"=" |
"(" | 
")"     { return yytext[0]; }




 /* nomes de variáveis */
[a-zA-Z][a-zA-Z0-9]*  { yylval.s = lookup(yytext); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }

"//".*  
[ \t]   /* ignorando espaço em branco */ 
\\\n    printf("c> "); /* ignora a continuação da linha */
"\n"    { return EOL; }

.	{ yyerror("Mystery character %c\n", *yytext); }
%%
