
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <math.h>
#  include "fb3-2.h"

//armazena a posicao no array da arvore
int posicao = 0;
int hasErro = 0;

//para expresoes como: a = b + c + d + 1.2 + 2
//armazena todos os tipos da expressão 
int listaTipos[NHASH];
int posicaoListaTipos = 0;


/* symbol table */
/* hash a symbol */
static unsigned
symhash(char *sym)
{
  //printf("HASH\n");
  unsigned int hash = 0;
  unsigned c;

  while(c = *sym++) hash = hash*9 ^ c;

  return hash;
}

struct symbol *
lookup(char* sym)
{
  
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
 
  int scount = NHASH;		
  hasErro = 0; //Para cada nova expressão, zera os erros


  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) { 
      //printf("LOOKUP: %s\n", sp->name);
      return sp; 
      }

    if(!sp->name) {		/* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
      //printf("LOOKUP: %s\n", sp->name);
      return sp;
    }

    if(++sp >= symtab+NHASH) sp = symtab; /* cria uma nova entrada */

  }
  yyerror("Tabela hash está cheia\n");
  abort(); /* Se a tabela estiver cheia, aborta a operação */

}



struct ast *
newast(int nodetype, struct ast *l, struct ast *r)
{
  //printf("NEWAST: %d - %d\n", l->nodetype, r->nodetype);
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast *
newnum(double d)
{

  struct numval *a = malloc(sizeof(struct numval));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'K';
  a->number = d;
  //printf("NEWNUM: %lf\n", d);

  //Armazenando o tipo do número
  listaTipos[posicaoListaTipos++] = isInteiro(d);

  return (struct ast *)a;
}

int lookupAST(char* ref){
    for(int i = 0; i < posicao; i++ )
        if(strcmp(asttab[i].name, ref) == 0)
          return i;
 return -1;
}

struct ast * buscaArvoreRef(char* ref){
    for(int i = 0; i < posicao; i++ )
        if(strcmp(asttab[i].name, ref) == 0)
            return asttab[i].a;
 return ;
}

struct ast *
newref(struct symbol *s)
{
  
  struct symref *a = malloc(sizeof(struct symref));
  //printf("NEWREF\n");
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
    a->nodetype = 'N';
    a->s = s;

  int p = lookupAST(a->s->name);
  if(p != -1)
    listaTipos[posicaoListaTipos++] = asttab[p].valuetype;
  else{
    printf("Erro de atribuição: a variavel %s nao foi declarada\n", a->s->name);
    hasErro = 1;
  }



  return (struct ast *)a;
}

//Descobre o tipo
int isInteiro(double num){
  if(floor(num)==ceil(num)) return 1;
  return 0;
}



struct ast *
newasgn(struct symbol *s, struct ast *v)
{
  struct symasgn *a = malloc(sizeof(struct symasgn));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;

  //printf("NEWASGN: %s\n", a->s->name);

 //Atribuindo a arvore na tabela de arvore
  if(posicao != 0){
    int p = lookupAST(s->name);
    if(p != -1){ //Elemento econtrado na tabela
       //Se forem as mesmas variáveis e tiverem tipos diferentes
      //if(strcmp(s->name, current_symbol) == 0 && validaTipos(current_number, s->value)==0)
        //printf("Erro de atribuição, variável de tipo diferente!\n");
      
      //Se n tiverem o mesmo tipo (uma for inteiro  e a outra real)
      if(asttab[p].valuetype != isInteiro(eval(v))){
        printf("Erro de atribuição, variável de tipo diferente!\n");
        a->v = asttab[p].a;
      }
      else{ //Caso tenha o mesmo tipo, precisamos verificar se ocorreu algum erro na operação. ex: b = a + 1.2
        if(hasErro == 0){//Caso não tenha erro
          //printf("1\n");
          a->v = v; //atribuindo o valor
          asttab[p].name = s->name; 
          asttab[p].a = malloc(sizeof(struct symasgn));
          asttab[p].a = a;
        }else{ //Caso tenha erro
          //printf("2\n");
          a->v = asttab[p].a;
        }
        
      }
    }
    else{ //Elemento não encontrada na tabela
      //a->v = v; //atribuindo o valor
      //asttab[posicao].name = s->name; 
      //asttab[p].a = malloc(sizeof(struct symasgn));
      //asttab[posicao].a = a;
      //asttab[posicao].valuetype = isInteiro(eval(a->v));

        if(hasErro == 0){//Caso não tenha erro
          //printf("3   posicaoListaTipos: %d\n", posicaoListaTipos);

          if(posicaoListaTipos > 1){
              //printf("check: %d\n", checkListaTipos());
              if(checkListaTipos()== 1){
                  a->v = v; //atribuindo o valor
                  asttab[posicao].name = s->name; 
                  asttab[posicao].a = malloc(sizeof(struct symasgn));
                  asttab[posicao].a = a;
                  asttab[posicao].valuetype = isInteiro(eval(a->v));
                  posicao++;
                  printf(asttab[posicao].name);
              }
              else{
                a->v = v;
              }
          }else{
                  a->v = v; //atribuindo o valor
                  asttab[posicao].name = s->name; 
                  asttab[posicao].a = malloc(sizeof(struct symasgn));
                  asttab[posicao].a = a;
                  asttab[posicao].valuetype = isInteiro(eval(a->v));
                  posicao++;
          }
         
        }else{ //Caso tenha erro
          //printf("4\n");
          a->v = v;
        }
        
      
      
    }
  }//Caso seja o primeiro elemento da tabela
  else{
    //printf("5\n");
    a->v = v; //atribuindo o valor
    asttab[posicao].name = s->name; 
    asttab[posicao].a = malloc(sizeof(struct symasgn));
    asttab[posicao].a = a;
    asttab[posicao].valuetype = isInteiro(eval(a->v));
    posicao++;
  }


  //zerando o contador da lista de Tipos
  posicaoListaTipos = 0;

  return (struct ast *)a;

}

//Verifica se todos os tipos da lista de tipos são iguais
int checkListaTipos(){
  //captura o primeiro tipo da lista
  /*for(int i = 0; i < posicaoListaTipos; i++)
    printf("ListaTipos[%d] = %d\n", i, listaTipos[i]);*/

  int check = listaTipos[0];
  for(int i = 1; i < posicaoListaTipos; i++)
    if(listaTipos[i] != check) return -1;

  return 1;
}

//Valida se os dois números possuem o mesmo tipo
int validaTipos(double num1, double num2)
{
     if(floor(num1)==ceil(num1) && floor(num2)==ceil(num2)) return 1;
     else if(floor(num1)!=ceil(num1) && floor(num2)!=ceil(num2)) return 1;
     return 0;
}

double
eval(struct ast *a)
{
  //printf("EVAL\n");
  double v;

  if(!a) {
    yyerror("Erro inteirno, EVAL recebendo arvore NULA");
    return 0.0;
  }

  switch(a->nodetype) {
    /* constante */
  case 'K': v = ((struct numval *)a)->number; break;

    /* nome de variavel */
  case 'N': v = ((struct symref *)a)->s->value; break;

    /* declaração */
  case '=': 
  //printf("TIPO: "+((struct symasgn *)a)->valuetype);
  v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); 
        
      
      break;

    /* expressões */
  case '+': 
      if(validaTipos(eval(a->l), eval(a->r))==0){ 
        yyerror("Operação com tipos incompatíveis"); 
        hasErro = 1;
      }
      else v = eval(a->l) + eval(a->r); 
    break;
  case '-':
     if(validaTipos(eval(a->l), eval(a->r))==0){
        yyerror("Operação com tipos incompatíveis"); 
        hasErro = 1;
      }else v = eval(a->l) - eval(a->r); break;
  case '*':
     if(validaTipos(eval(a->l), eval(a->r))==0){
        yyerror("Operação com tipos incompatíveis"); 
        hasErro = 1;
      }else v = eval(a->l) * eval(a->r); break;
  case '/': 
    if(validaTipos(eval(a->l), eval(a->r))==0){
        yyerror("Operação com tipos incompatíveis"); 
        hasErro = 1;
      }else v = eval(a->l) / eval(a->r); break;
  case '^': 
    if(validaTipos(eval(a->l), eval(a->r))==0){
        yyerror("Operação com tipos incompatíveis"); 
        hasErro = 1;
      }else v = pow(eval(a->l), eval(a->r)); break;
  case 'M': v = -eval(a->l); break;

  default: printf("Erro interno: no desconhecido %c\n", a->nodetype);
  }
  return v;
}



void
treefree(struct ast *a)
{
  switch(a->nodetype) {

    /* two subtrees */
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
  case '1':  case '2':  case '3':  case '4':  case '5':  case '6':
  case 'M': 
    treefree(a->l);

    /* no subtree */
  case 'K': case 'N':
    break;

  case '=':
    free( ((struct symasgn *)a)->v);
    break;

  default: printf("Erro interno: no libre desconhecido %c\n", a->nodetype);
  }	  
  
  free(a); 

}

void
yyerror(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: Erro: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int
main()
{
  printf("> "); 
  return yyparse();
}

/* debugging: dump out an AST */
int debug = 0;
void
dumpast(struct ast *a, int level)
{

  printf("%*s", 2*level, "");	/* indent to this level */
  level++;

  if(!a) {
    printf("NULL\n");
    return;
  }

  switch(a->nodetype) {
    /* constante */
  case 'K': printf("Numero %4.4g\n", ((struct numval *)a)->number); break;

    /* nome de variável */
  case 'N': //printf("ref %s\n", ((struct symref *)a)->s->name); 
        
        dumpast(buscaArvoreRef(((struct symref *)a)->s->name), level);
        break;

    /* declaração */
  case '=': printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast( ((struct symasgn *)a)->v, level); return;

    /* expressões */
  case '+': case '-': case '*': case '/': case '^': case 'L':
  case '1': case '2': case '3':
  case '4': case '5': case '6': 
    printf("binop %c\n", a->nodetype);
    dumpast(a->l, level); 
    dumpast(a->r, level);
    return;
  case 'M': 
    printf("unop %c\n", a->nodetype);
    dumpast(a->l, level);
    return;

  default: printf("ruim %c\n", a->nodetype);
    return;
  }
}
