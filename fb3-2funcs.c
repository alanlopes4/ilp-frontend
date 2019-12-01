
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <math.h>
#  include "fb3-2.h"

FILE *yyin;

int position = 0;
int hasErro = 0;

int listTypes[NHASH];
int positionListTypes = 0;

int line = 0;
int column = 0;

//Insere o tipo da variável na tabela de tipos
void insertListTypes(struct symref *a, struct symbol *s){
  int p = lookupAST(a->s->name);
  if(p != -1)
    listTypes[positionListTypes++] = asttab[p].valuetype;
  else{
    showErro("A variavel %s nao foi declarada\n");
    hasErro = 1;
  }
}
//usado para atulizar a linha e coluna do erro
void updateInfoErro(int linha, int coluna) {
    line = linha;
    column = coluna;
}
//Retorna a posicao da arvore da variavel na tabela de arvores
int lookupAST(char* ref){
    for(int i = 0; i < position; i++ )
        if(strcmp(asttab[i].name, ref) == 0)
          return i;
 return -1;
}

struct ast * searchAST(char* ref){
    for(int i = 0; i < position; i++ )
        if(strcmp(asttab[i].name, ref) == 0)
            return asttab[i].a;
 return ;
}

void showErro(char *erro){
  yyerror(erro);
}

int isInteger(double num){
  if(floor(num)==ceil(num)) return 1;
  return 0;
}

//Verifica se todos os tipos da lista de tipos são iguais
int checklistTypes(){
  int check = listTypes[0];
  for(int i = 1; i < positionListTypes; i++)
    if(listTypes[i] != check) return -1;
  return 1;
}

//Valida se a operação pode ser feita
int checkOperation(double num1, double num2)
{
     if(floor(num1)==ceil(num1) && floor(num2)==ceil(num2)) return 1;
     else if(floor(num1)!=ceil(num1) && floor(num2)!=ceil(num2)) return 1;
     return 0;
}

void updateASTTab(struct symasgn *a, struct ast *v, struct symbol *s){
  //Atribuindo a arvore na tabela de arvore
  if(position != 0){
    int p = lookupAST(s->name);
    if(p != -1){ //Elemento econtrado na tabela
      //Se n tiverem o mesmo tipo (uma for inteiro  e a outra real)
      if(asttab[p].valuetype != isInteger(eval(v))){
        showErro("Variável de tipos diferentes\n");
        a->v = asttab[p].a;
      }
      else{ //Caso tenha o mesmo tipo, precisamos verificar se ocorreu algum erro na operação. ex: b = a + 1.2
        if(hasErro == 0){//Caso não tenha erro
          a->v = v; //atribuindo o valor
          asttab[p].name = s->name; 
          asttab[p].a = malloc(sizeof(struct symasgn));
          asttab[p].a = a;
        }else{ //Caso tenha erro
          a->v = asttab[p].a;
        }
        
      }
    }
    else{ //Elemento não encontrada na tabela
        if(hasErro == 0){//Caso não tenha erro
          if(positionListTypes > 1){
              if(checklistTypes()== 1){
                 insertASTTab(a, v, s);
              }
              else{
                a->v = v;
              }
          }else{
                 insertASTTab(a, v, s);
          }
         
        }else{ //Caso tenha erro
          a->v = v;
        }
    }
  }
  //Caso seja o primeiro elemento da tabela
  else{
    insertASTTab(a, v, s);
  }
  //zerando o contador da lista de Tipos
  positionListTypes = 0;
}

void insertASTTab(struct symasgn *a, struct ast *v, struct symbol *s){
  a->v = v; 
  asttab[position].name = s->name; 
  asttab[position].a = malloc(sizeof(struct symasgn));
  asttab[position].a = a;
  asttab[position].valuetype = isInteger(eval(a->v));
  position++;
}

void showAST(){
   printf("ARVORE\n");
   for(int i = 0; i < position; i++){
      if(strlen(asttab[i].name)>0)
        dumpast(asttab[i].a, 0);
    }
}


/* symbol table */
/* hash a symbol */
static unsigned
symhash(char *sym)
{
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
      return sp; 
      }

    if(!sp->name) {		/* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
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
  //Armazenando o tipo do número
  listTypes[positionListTypes++] = isInteger(d);

  return (struct ast *)a;
}

struct ast *
newref(struct symbol *s)
{
  
  struct symref *a = malloc(sizeof(struct symref));

  if(!a) {
    yyerror("out of space");
    exit(0);
  }
    a->nodetype = 'N';
    a->s = s;

  insertListTypes(a, s);

  return (struct ast *)a;
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

 updateASTTab(a, v, s);
  

  return (struct ast *)a;

}

double
eval(struct ast *a)
{
  double v;

  if(!a) {
    yyerror("Erro interno, EVAL recebendo arvore NULA");
    return 0.0;
  }

  switch(a->nodetype) {
    /* constante */
    case 'K': v = ((struct numval *)a)->number; break;
    /* nome de variavel */
    case 'N': v = ((struct symref *)a)->s->value; break;
    /* declaração */
    case '=': 
    v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); break;
    /* expressões */
    case '+': 
        if(checkOperation(eval(a->l), eval(a->r))==0){ 
          showErro("Operação com tipos incompatíveis\n");
          hasErro = 1;
        }
        else v = eval(a->l) + eval(a->r); 
      break;
    case '-':
      if(checkOperation(eval(a->l), eval(a->r))==0){
          showErro("Operação com tipos incompatíveis\n");
          hasErro = 1;
        }else v = eval(a->l) - eval(a->r); break;
    case '*':
      if(checkOperation(eval(a->l), eval(a->r))==0){
          showErro("Operação com tipos incompatíveis\n");
          hasErro = 1;
        }else v = eval(a->l) * eval(a->r); break;
    case '/': 
      if(checkOperation(eval(a->l), eval(a->r))==0){
          showErro("Operação com tipos incompatíveis\n");
          hasErro = 1;
        }else v = eval(a->l) / eval(a->r); break;
    case '^': 
      if(checkOperation(eval(a->l), eval(a->r))==0){
          showErro("Operação com tipos incompatíveis\n");
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

  fprintf(stderr, "(%d, %d): ", line, column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int
main()
{

  FILE *file;
  char name_file[100];

  printf("#####################################################################\n");
  printf("Digite o nome do arquivo:");
  scanf("%s", name_file);
  file = fopen(name_file, "r");  
  if(file == NULL)
  {
      printf("Não foi possível abrir o arquivo!\n Por favor verifique se o arquivo existe. \n");
      return 1;
  }
  yyin = file;
  
  yyparse(); 
  showAST();

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
  case 'K': printf("%4.4g\n", ((struct numval *)a)->number); break;

    /* nome de variável */
  case 'N': //printf("ref %s\n", ((struct symref *)a)->s->name); 
        
        dumpast(searchAST(((struct symref *)a)->s->name), level);
        break;

    /* declaração */
  case '=': printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast( ((struct symasgn *)a)->v, level); return;

    /* expressões */
  case '+': case '-': case '*': case '/': case '^': case 'L':
  case '1': case '2': case '3':
  case '4': case '5': case '6': 
    printf("op %c\n", a->nodetype);
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
