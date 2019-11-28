

%option noyywrap nodefault yylineno
%{
# include "fb3-2.h"
# include "fb3-2.tab.h"

static void update_loc(){
  static int curr_line = 1;
  static int curr_col  = 1;

  yylloc.first_line   = curr_line;
  yylloc.first_column = curr_col;

  {char * s; for(s = yytext; *s != '\0'; s++){
    if(*s == '\n'){
      curr_line++;
      curr_col = 1;
    }else{
      curr_col++;
    }
  }}

  yylloc.last_line   = curr_line;
  yylloc.last_column = curr_col-1;
}

#define YY_USER_ACTION update_loc();


%}
 
/* float exponent */
EXP	([Ee][-+]?[0-9]+) 
 
%%
"arvore" { atualizarLinhaColunaErro(yylloc.first_line, yylloc.first_column); return ARVORE; }
"print" { atualizarLinhaColunaErro(yylloc.first_line, yylloc.first_column); return PRINTAR; }

"+" | 
"-" |     
"*" |  
"/" |      
"^" |
"=" |
"(" | 
")"     { atualizarLinhaColunaErro(yylloc.first_line, yylloc.first_column); return yytext[0]; }




 /* nomes de variáveis */
[a-zA-Z][a-zA-Z0-9]*  { yylval.s = lookup(yytext); atualizarLinhaColunaErro(yylloc.first_line, yylloc.first_column); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); atualizarLinhaColunaErro(yylloc.first_line, yylloc.first_column); return NUMBER; }

"//".*  
[ \t]   /* ignorando espaço em branco */ 
\\\n    printf("c> "); /* ignora a continuação da linha */
"\n"    { return EOL; }

.	{ yyerror("O caracter não foi reconhecido"); }
%%
