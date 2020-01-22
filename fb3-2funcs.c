
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "fb3-2.h"

FILE *yyin;

int position = 0;
int hasErro = 0;

int listTypes[NHASH];
int positionListTypes = 0;
int positionAlloca = 0;

int line = 0;
int column = 0;
int lastNumberRegister = 0;

int isInteger(double num)
{
  if (floor(num) == ceil(num))
    return 1;
  return 0;
}

//Retorna a posicao da arvore da variavel na tabela de arvores
int lookupAST(char *ref)
{
  for (int i = 0; i < position; i++)
    if (strcmp(asttab[i].name, ref) == 0)
      return i;
  return -1;
}

void insertASTTab(struct symasgn *a, struct ast *v, struct symbol *s)
{
  a->v = v;
  asttab[position].name = s->name;
  asttab[position].a = malloc(sizeof(struct symasgn));
  asttab[position].a = (struct ast *)a;
  asttab[position].valuetype = isInteger(eval(a->v));
  position++;
}

void isertAllocaTab()
{
  allocatab[positionAlloca].num_registrado = positionAlloca;
  allocatab[positionAlloca].tipo = 1;
}

void showErro(char *erro)
{
  yyerror(erro);
}

//Insere o tipo da variável na tabela de tipos
void insertListTypes(struct symref *a, struct symbol *s)
{
  int p = lookupAST(a->s->name);
  if (p != -1)
    listTypes[positionListTypes++] = asttab[p].valuetype;
  else
  {
    showErro("A variavel %s nao foi declarada\n");
    hasErro = 1;
  }
}
//usado para atulizar a linha e coluna do erro
void updateInfoErro(int linha, int coluna)
{
  line = linha;
  column = coluna;
}

struct ast *searchAST(char *ref)
{
  for (int i = 0; i < position; i++)
    if (strcmp(asttab[i].name, ref) == 0)
      return asttab[i].a;
  return NULL;
}

//Verifica se todos os tipos da lista de tipos são iguais
int checklistTypes()
{
  int check = listTypes[0];
  for (int i = 1; i < positionListTypes; i++)
    if (listTypes[i] != check)
      return -1;
  return 1;
}

//Valida se a operação pode ser feita
int checkOperation(double num1, double num2)
{
  if (floor(num1) == ceil(num1) && floor(num2) == ceil(num2))
    return 1;
  else if (floor(num1) != ceil(num1) && floor(num2) != ceil(num2))
    return 1;
  return 0;
}

void updateASTTab(struct symasgn *a, struct ast *v, struct symbol *s)
{
  //Atribuindo a arvore na tabela de arvore
  if (position != 0)
  {
    int p = lookupAST(s->name);
    if (p != -1)
    { //Elemento econtrado na tabela
      //Se n tiverem o mesmo tipo (uma for inteiro  e a outra real)
      if (asttab[p].valuetype != isInteger(eval(v)))
      {
        showErro("Variável de tipos diferentes\n");
        a->v = asttab[p].a;
      }
      else
      { //Caso tenha o mesmo tipo, precisamos verificar se ocorreu algum erro na operação. ex: b = a + 1.2
        if (hasErro == 0)
        {           //Caso não tenha erro
          a->v = v; //atribuindo o valor
          asttab[p].name = s->name;
          asttab[p].a = malloc(sizeof(struct symasgn));
          asttab[p].a = (struct ast *)a;
        }
        else
        { //Caso tenha erro
          a->v = asttab[p].a;
        }
      }
    }
    else
    { //Elemento não encontrada na tabela
      if (hasErro == 0)
      { //Caso não tenha erro
        if (positionListTypes > 1)
        {
          if (checklistTypes() == 1)
          {
            insertASTTab(a, v, s);
          }
          else
          {
            a->v = v;
          }
        }
        else
        {
          insertASTTab(a, v, s);
        }
      }
      else
      { //Caso tenha erro
        a->v = v;
      }
    }
  }
  //Caso seja o primeiro elemento da tabela
  else
  {
    insertASTTab(a, v, s);
  }
  //zerando o contador da lista de Tipos
  positionListTypes = 0;
}

void showAST()
{
  for (int i = 0; i < position; i++)
  {
    if (strlen(asttab[i].name) > 0)
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

  while ((c = *sym++))
    hash = hash * 9 ^ c;

  return hash;
}

struct symbol *
lookup(char *sym)
{

  struct symbol *sp = &symtab[symhash(sym) % NHASH];

  int scount = NHASH;
  hasErro = 0; //Para cada nova expressão, zera os erros

  while (--scount >= 0)
  {
    if (sp->name && !strcmp(sp->name, sym))
    {
      return sp;
    }

    if (!sp->name)
    { /* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
      return sp;
    }

    if (++sp >= symtab + NHASH)
      sp = symtab; /* cria uma nova entrada */
  }
  yyerror("Tabela hash está cheia\n");
  abort(); /* Se a tabela estiver cheia, aborta a operação */
}

struct ast *
newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a = malloc(sizeof(struct ast));

  if (!a)
  {
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

  if (!a)
  {
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

  if (!a)
  {
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

  if (!a)
  {
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

  if (!a)
  {
    yyerror("Erro interno, EVAL recebendo arvore NULA");
    return 0.0;
  }

  switch (a->nodetype)
  {
  /* constante */
  case 'K':
    v = ((struct numval *)a)->number;
    break;
  /* nome de variavel */
  case 'N':
    v = ((struct symref *)a)->s->value;
    break;
  /* declaração */
  case '=':
    v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v);
    break;
  /* expressões */
  case '+':
    if (checkOperation(eval(a->l), eval(a->r)) == 0)
    {
      showErro("Operação com tipos incompatíveis\n");
      hasErro = 1;
    }
    else
      v = eval(a->l) + eval(a->r);
    break;
  case '-':
    if (checkOperation(eval(a->l), eval(a->r)) == 0)
    {
      showErro("Operação com tipos incompatíveis\n");
      hasErro = 1;
    }
    else
      v = eval(a->l) - eval(a->r);
    break;
  case '*':
    if (checkOperation(eval(a->l), eval(a->r)) == 0)
    {
      showErro("Operação com tipos incompatíveis\n");
      hasErro = 1;
    }
    else
      v = eval(a->l) * eval(a->r);
    break;
  case '/':
    if (checkOperation(eval(a->l), eval(a->r)) == 0)
    {
      showErro("Operação com tipos incompatíveis\n");
      hasErro = 1;
    }
    else
      v = eval(a->l) / eval(a->r);
    break;
  case '^':
    if (checkOperation(eval(a->l), eval(a->r)) == 0)
    {
      showErro("Operação com tipos incompatíveis\n");
      hasErro = 1;
    }
    else
      v = pow(eval(a->l), eval(a->r));
    break;
  case 'M':
    v = -eval(a->l);
    break;

  default:
    printf("Erro interno: no desconhecido %c\n", a->nodetype);
  }
  return v;
}

void treefree(struct ast *a)
{
  switch (a->nodetype)
  {

    /* two subtrees */
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case 'M':
    treefree(a->l);
    /* no subtree */
  case 'K':
  case 'N':
    break;

  case '=':
    free(((struct symasgn *)a)->v);
    break;

  default:
    printf("Erro interno: no libre desconhecido %c\n", a->nodetype);
  }

  free(a);
}

void yyerror(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "(%d, %d): ", line, column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int main()
{

  FILE *file;
  char name_file[100];

  printf("#####################################################################\n");
  printf("Digite o nome do arquivo:");
  scanf("%s", name_file);
  file = fopen(name_file, "r");
  if (file == NULL)
  {
    printf("Não foi possível abrir o arquivo!\n Por favor verifique se o arquivo existe. \n");
    return 1;
  }
  yyin = file;

  yyparse();

  for (int i = 0; i < NHASH; i++)
  {
    if (symtab[i].name != NULL)
      printf("symtab name: %s valuetype: %d\n", symtab[i].name, symtab[i].valuetype);
    if (asttab[i].name != NULL)
      printf("astab name: %s valuetype: %d nodetype:%d\n", asttab[i].name, asttab[i].valuetype, asttab[i].a->nodetype);
  }

  createFile();
  showAST();
}

struct alocacao
{
  int num_registrador;
  char tipo[];
};

/* debugging: dump out an AST */
int debug = 0;
void dumpast(struct ast *a, int level)
{

  printf("%*s", 2 * level, ""); /* indent to this level */
  level++;

  if (!a)
  {
    printf("NULL\n");
    return;
  }

  switch (a->nodetype)
  {
    /* constante */
  case 'K':
    printf("Constante:");
    printf("%4.4g\n", ((struct numval *)a)->number);
    break;

    /* nome de variável */
  case 'N': //printf("ref %s\n", ((struct symref *)a)->s->name);
    printf("Nome variável\n");
    writeLoad(((struct symref *)a)->s->name, ((struct symref *)a)->s->valuetype);

    //dumpast(searchAST(((struct symref *)a)->s->name), level);
    break;

    /* declaração */
  case '=':
    printf("Declaracao\n");
    printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast(((struct symasgn *)a)->v, level);
    return;

    /* expressões */
  case '+':
    printf("SOMA\n");
    writeSum();
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '-':
    printf("SUB\n");
    writeSub();
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '*':
    printf("MULT\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '/':
    printf("DIV\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '^':
    printf("ELE\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case 'L':
    printf("L\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '1':
    printf("11111\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '2':
    printf("22222\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '3':
    printf("3333\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '4':
    printf("444\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '5':
    printf("5555\n");
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '6':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case 'M':
    printf("unop %c\n", a->nodetype);
    dumpast(a->l, level);
    return;

  default:
    printf("ruim %c\n", a->nodetype);
    return;
  }
}

//#######################FUNÇÕES ESCRITA ######################
char tipos[2][10] = {
    "double",
    "i32"};

void createFile()
{
  header();
  createAlloc();
}

char *replaceWord(const char *s, const char *oldW,
                  const char *newW)
{
  char *result;
  int i, cnt = 0;
  int newWlen = strlen(newW);
  int oldWlen = strlen(oldW);

  // Counting the number of times old word
  // occur in the string
  for (i = 0; s[i] != '\0'; i++)
  {
    if (strstr(&s[i], oldW) == &s[i])
    {
      cnt++;

      // Jumping to index after the old word.
      i += oldWlen - 1;
    }
  }

  // Making new string of enough length
  result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

  i = 0;
  while (*s)
  {
    // compare the substring with the result
    if (strstr(s, oldW) == s)
    {
      strcpy(&result[i], newW);
      i += newWlen;
      s += oldWlen;
    }
    else
      result[i++] = *s++;
  }

  result[i] = '\0';
  return result;
}

void header()
{
  char header[] = "target triple = 'x86_64-pc-linux-gnu'";
  char header2[] = "define i32 @main() #0 {";
  printf("%s\n", header);
  printf("%s\n", header2);
}
//Quantidade de alocação = quantidade de declarações
int countDECL()
{
  int count = 0;
  for (int i = 0; i < NHASH; i++)
  {
    if (asttab[i].name != NULL)
    {
      allocatab[lastNumberRegister].num_registrado = count;
      allocatab[lastNumberRegister].tipo = asttab[i].valuetype;
      allocatab[lastNumberRegister].nome_variavel = asttab[i].name;
      count++;
      lastNumberRegister++;
    }
    //printf("name: %s valuetype: %d\n", symtab[i].name, symtab[i].valuetype);
  }

  return count;
}

char *writeAlloc()
{
  char line[] = "REGISTRADOR0 = alloca TIPO0, align 4";
  return line;
}
int count = 0;
void createAlloc()
{
  //Realiza a contagem de alocações de acordo com a quantidade de declarações
  count = countDECL();
  for (int i = 1; i <= count; i++)
  {

    char s1[] = "\%REGISTRADOR0 = alloca TIPO0, align 4\n";
    char s2[] = "REGISTRADOR0";
    char type0[] = "TIPO0";
    char s3[10];
    sprintf(s3, "%d", i);
    char *final_line = replaceWord(s1, s2, s3);
    final_line = replaceWord(final_line, type0, tipos[allocatab[i - 1].tipo]);
    printf(final_line);
  }
}

void writeSum()
{
  char sum[] = "REGISTRADOR0 = add nsw TIPO0 REGISTRADOR1, REGISTRADOR2";
}

void writeSub()
{
  char sub[] = "REGISTRADOR0 = sub nsw TIPO0 REGISTRADOR1, REGISTRADOR2";
}

void writeMul()
{
  char mul[] = "REGISTRADOR0 = mul nsw TIPO0 REGISTRADOR1, REGISTRADOR2";
}

void writeDiv()
{
  char div[] = "REGISTRADOR0 = sdiv TIPO0 REGISTRADOR1, REGISTRADOR2";
}

void writePow()
{
  char pow[] = "REGISTRADOR0 = xor TIPO0 REGISTRADOR1, REGISTRADOR2";
}

int searchAllocaTab(char *name)
{
  for (int i = 0; i < count; i++)
  {
    if (strcmp(allocatab[i].nome_variavel, name) == 0)
      return i;
  }
  return -1;
}

void writeLoad(char variable_name[], int type)
{
  //printf("REGISTRADOR0 = load TIPO0 REGISTRADO1, TIPO1* REGISTRADOR2, align 4\n");
  char load[] = "%REGISTRADOR0 = load TIPO0, TIPO1* %REGISTRADOR1, align 4";
  char oldW[] = "REGISTRADOR1";
  char oldW0[] = "REGISTRADOR0";
  char oldType1[] = "TIPO1";
  char oldType0[] = "TIPO0";
  char number_register[10];
  char *final_line;
  sprintf(number_register, "%d", allocatab[searchAllocaTab(variable_name)].num_registrado+1);
  final_line = replaceWord(load, oldW, number_register);
  sprintf(number_register, "%d", ++lastNumberRegister);
  final_line = replaceWord(final_line, oldW0, number_register);
  final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  printf(final_line);
}


void writeStore(char * variable_name)
{
  //printf("REGISTRADOR0 = load TIPO0 REGISTRADO1, TIPO1* REGISTRADOR2, align 4\n");
  char store[] = "store TIPO0 REGISTRADOR0, TIPO1* REGISTRADOR1, align 4";
  char oldW[] = "REGISTRADOR1";
  char oldW0[] = "REGISTRADOR0";
  char oldType1[] = "TIPO1";
  char oldType0[] = "TIPO0";
  char number_register[10];
  char *final_line;


  sprintf(number_register, "%d", allocatab[searchAllocaTab(variable_name)].num_registrado+1);
  final_line = replaceWord(store, oldW, number_register);
  sprintf(number_register, "%d", ++lastNumberRegister);
  final_line = replaceWord(final_line, oldW0, number_register);
  final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  printf(final_line);
}

void writeRet()
{
  char ret[] = "ret TIPO0 RESULT0";
}

void writeFooter()
{
  char footer[] = "}";
}
