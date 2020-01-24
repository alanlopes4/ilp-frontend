
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
int availableRegister = 1;
int countRegisters = 0;

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

void insertAllocaTab(int tipo, double valor, int numRegistrador)
{
  printf("insert reg: %d, tipo: %d, valor: %lf\n", numRegistrador, tipo, valor);
  allocatab[numRegistrador - 1].num_registrado = numRegistrador;
  allocatab[numRegistrador - 1].tipo = tipo;
  allocatab[numRegistrador - 1].valor_variavel = valor;
}

void insertNewAllocaTab(int tipo, double valor, int numRegistrador)
{
  printf("insert reg: %d, tipo: %d, valor: %lf\n", numRegistrador, tipo, valor);
  allocatab[countRegisters].num_registrado = numRegistrador;
  allocatab[countRegisters].tipo = tipo;
  allocatab[countRegisters].valor_variavel = valor;
  countRegisters++;
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
    {
      int reg = converter(asttab[i].a);
      //dumpast(asttab[i].a, 0);
    }
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

int isVariableOrNumber(struct ast *a)
{
  if (!a)
  {
    return -1;
  }
  switch (a->nodetype)
  {
  case 'K':
    return 1;
    break;
  case 'N':
    return 2;
    break;
  default:
    return -1;
    break;
  }
}

char *getNameVariable(struct ast *a)
{
  return ((struct symref *)a)->s->name;
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
int countDECL()
{
  int count = 0;
  for (int i = 0; i < NHASH; i++)
  {
    if (asttab[i].name != NULL)
    {
      count++;
      allocatab[lastNumberRegister].num_registrado = count;
      allocatab[lastNumberRegister].tipo = asttab[i].valuetype;
      allocatab[lastNumberRegister].nome_variavel = asttab[i].name;
      lastNumberRegister++;
    }
    //printf("name: %s valuetype: %d\n", symtab[i].name, symtab[i].valuetype);
  }

  return count;
}

int reg_var = 0;
int num_register = 0; 
char percent[] = "%";
int converter(struct ast *a)
{
  int reg;
  if (!a)
  {
    yyerror("Erro interno, EVAL recebendo arvore NULA");
    return;
  }

  //printf("NodeType:%c\n", a->nodetype);
  switch (a->nodetype)
  {
  /* constante */
  case 'K':
    //printf("store %d\n", num_register, ((struct numval *)a)->number);
    printf("%s%d = alloca i32, align 4\n", percent, num_register);
    printf("store i32 %.0lf , i32* %s%d, align\n", ((struct numval *)a)->number, percent, num_register);
    reg = num_register;
    break;
  /* nome de variavel */
  case 'N':
    //v = ((struct symref *)a)->s->value;     v = ((struct symref *)a)->s->value;
    printf("%s%d = load i32, i32*  %s%d, align 4\n", percent, num_register, percent, allocatab[searchAllocaTab(getNameVariable(a))].num_registrado);
    reg = allocatab[searchAllocaTab(((struct symref *)a)->s->name)].num_registrado;
    break;
  /* declaração */
  case '=':
    //v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v);
    reg = converter(((struct symasgn *)a)->v);
    break;
  /* expressões */
  case '+':
    printf("%d = add %d, %d\n", num_register, converter(a->l), converter(a->r));
    printf("store i32 %s%d , i32* %s%d, align\n", percent, num_register, percent, ++reg_var);

    //printf("store %d -> %d\n", num_register, ++reg_var);
    reg = num_register;
    //v = eval(a->l) + eval(a->r);
    break;
  case '-':
    printf("%s%d = sub nsw i32 %s%d, %s%d\n", percent, num_register, percent, converter(a->l), percent, converter(a->r));
    printf("store i32 %s%d , i32* %s%d, align\n", percent, num_register, percent, ++reg_var);

    //printf("store %d -> %d\n", num_register, ++reg_var);
    reg = num_register;
    //v = eval(a->l) - eval(a->r);
    break;
  case '*':
    //v = eval(a->l) * eval(a->r);
    break;
  case '/':
    //v = eval(a->l) / eval(a->r);
    break;
  case '^':

    //v = pow(eval(a->l), eval(a->r));
    break;
  case 'M':
    //v = -eval(a->l);
    break;
  default:
    printf("Erro interno: no desconhecido %c\n", a->nodetype);
    return 0;
  }
  num_register++;
  return reg;
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
      ;
    //printf("symtab name: %s valuetype: %d\n", symtab[i].name, symtab[i].valuetype);
    if (asttab[i].name != NULL)
      ;
    //printf("astab name: %s valuetype: %d nodetype:%d\n", asttab[i].name, asttab[i].valuetype, asttab[i].a->nodetype);
  }

  createFile();
  num_register = countDECL() + 1;
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
    //printf("Constante:");
    //printf("%4.4g\n", ((struct numval *)a)->number);
    break;

    /* nome de variável */
  case 'N': //printf("ref %s\n", ((struct symref *)a)->s->name);
    //printf("Nome variável\n");
    //writeLoad(((struct symref *)a)->s->name, ((struct symref *)a)->s->valuetype);

    //dumpast(searchAST(((struct symref *)a)->s->name), level);
    break;

    /* declaração */
  case '=':
    //printf("Declaracao\n");
    //printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast(((struct symasgn *)a)->v, level);
    return;

    /* expressões */
  case '+':
    printf("SOMA\n");
    writeSum(a);
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '-':
    //printf("SUB\n");
    writeSub(a);
    //printf("op %c\n", a->nodetype);
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
  countRegisters = count;
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

void writeSub(struct ast *a)
{
  int tipo1 = isVariableOrNumber(a->l);
  int tipo2 = isVariableOrNumber(a->r);
  //printf("L:%d R:%d\n", tipo1, tipo2);

  char sub[] = "%REGISTRADOR0 = sub nsw TIPO0 %REGISTRADOR1, REGISTRADOR2\n";
  char oldW0[] = "REGISTRADOR0";
  char oldW1[] = "REGISTRADOR1";
  char oldW2[] = "REGISTRADOR2";
  char oldType1[] = "TIPO1";
  char oldType0[] = "TIPO0";
  char number_register[10];
  char value[10];
  char *final_line;

  //Se os dos lados da operação forem constantes
  if (tipo1 == 1 && tipo2 == 1)
  {
    writeStore(a, 2, 1);
  }
  else if (tipo1 == 2 && tipo2 == 1) //Variavel lado esquerdo
  {
    writeLoad(a, 1);
    sprintf(number_register, "%d", ++lastNumberRegister);
    final_line = replaceWord(sub, oldW0, number_register);
    final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(getNameVariable(a->l))].tipo]);
    sprintf(number_register, "%d", lastNumberRegister - 1);
    final_line = replaceWord(final_line, oldW1, number_register);
    sprintf(value, "%.2lf", eval(a->r));
    final_line = replaceWord(final_line, oldW2, value);
    printf(final_line);
    //isertAllocaTab(allocatab[searchAllocaTab(getNameVariable(a->l))].tipo, allocatab[searchAllocaTab(getNameVariable(a->l))].valor_variavel, lastNumberRegister);
    writeStore(a, 2, 4);
  }
  else if (tipo1 == 1 && tipo2 == 2) //Variavel lado direito
  {
    writeLoad(a, 2);
    sprintf(number_register, "%d", ++lastNumberRegister);
    final_line = replaceWord(sub, oldW0, number_register);
    final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(getNameVariable(a->r))].tipo]);
    sprintf(number_register, "%d", lastNumberRegister - 1);
    final_line = replaceWord(final_line, oldW1, number_register);
    sprintf(value, "%.2lf", eval(a->l));
    final_line = replaceWord(final_line, oldW2, value);
    printf(final_line);
    //isertAllocaTab(allocatab[searchAllocaTab(getNameVariable(a->r))].tipo, allocatab[searchAllocaTab(getNameVariable(a->r))].valor_variavel, lastNumberRegister);
    writeStore(a, 2, 3);
  }

  showAllocaTab();

  //final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
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

//side - 1 = esquerdo, 2 = direito, 3 = ambos
void writeLoad(struct ast *a, int side)
{

  //printf("REGISTRADOR0 = load TIPO0 REGISTRADO1, TIPO1* REGISTRADOR2, align 4\n");
  char load[] = "%REGISTRADOR0 = load TIPO0, TIPO1* %REGISTRADOR1, align 4\n";
  char oldW[] = "REGISTRADOR1";
  char oldW0[] = "REGISTRADOR0";
  char oldType1[] = "TIPO1";
  char oldType0[] = "TIPO0";
  char number_register[10];
  char *final_line;
  int tipo = -1;
  double valor = 0.0;

  if (side == 1)
  {
    char *variable_name = getNameVariable(a->l);
    sprintf(number_register, "%d", allocatab[searchAllocaTab(variable_name)].num_registrado + 1);
    final_line = replaceWord(load, oldW, number_register);
    sprintf(number_register, "%d", ++lastNumberRegister);
    final_line = replaceWord(final_line, oldW0, number_register);
    final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
    final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
    //insertAllocaTab(a, lastNumberRegister, (((struct symasgn *)a)->s->name != NULL ? ((struct symasgn *)a)->s->name : "K") );
    //isertAllocaTab(allocatab[searchAllocaTab(variable_name)].tipo, allocatab[searchAllocaTab(variable_name)].valor_variavel, lastNumberRegister);
  }
  if (side == 2)
  {
    char *variable_name = getNameVariable(a->r);
    sprintf(number_register, "%d", allocatab[searchAllocaTab(variable_name)].num_registrado + 1);
    final_line = replaceWord(load, oldW, number_register);
    sprintf(number_register, "%d", ++lastNumberRegister);
    final_line = replaceWord(final_line, oldW0, number_register);
    final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
    final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
    //isertAllocaTab(allocatab[searchAllocaTab(variable_name)].tipo, allocatab[searchAllocaTab(variable_name)].valor_variavel, lastNumberRegister);
  }

  printf(final_line);
  showAllocaTab();
}

double executarOperacao(struct ast *a, int operacao)
{
  double v;
  switch (operacao)
  {
  case 1: //SOMA
    v = eval(a->l) + eval(a->r);
    break;
  case 2:
    v = (eval(a->l) - eval(a->r));
    break;
  default:
    v = 0.0;
    break;
  }
  return v;
}
//situacao: 1 - ambas constantes, 2 - ambas variaveis; 3-L:constante e R:variavel, 4-L:variavel e R:constante
void writeStore(struct ast *a, int operacao, int situacao)
{
  //printf("REGISTRADOR0 = load TIPO0 REGISTRADO1, TIPO1* REGISTRADOR2, align 4\n");
  char store[] = "store TIPO0 %REGISTRADOR0, TIPO1* %REGISTRADOR1, align 4\n";
  char oldW[] = "REGISTRADOR1";
  char oldW0[] = "REGISTRADOR0";
  char oldType1[] = "TIPO1";
  char oldType0[] = "TIPO0";
  char number_register[10];
  char number_result[10];
  char *final_line;

  if (situacao == 1) //Ambas são constantes
  {
    double result = executarOperacao(a, operacao);
    final_line = replaceWord(store, oldType0, tipos[isInteger(result)]);
    final_line = replaceWord(final_line, oldType1, tipos[isInteger(result)]);
    sprintf(number_register, "%d", availableRegister++);
    final_line = replaceWord(final_line, oldW, number_register);
    sprintf(number_result, "%.2lf", result);
    final_line = replaceWord(final_line, oldW0, number_result);
    printf(final_line);
    //isertAllocaTab(isInteger(result), result, availableRegister - 1);
  }
  else if (situacao == 3)
  { //variavel do lado direito

    final_line = replaceWord(store, oldType0, tipos[allocatab[lastNumberRegister].tipo]);
    final_line = replaceWord(final_line, oldType1, tipos[allocatab[lastNumberRegister].tipo]);
    sprintf(number_register, "%d", lastNumberRegister);
    final_line = replaceWord(final_line, oldW0, number_register);
    sprintf(number_register, "%d", availableRegister++);
    final_line = replaceWord(final_line, oldW, number_register);
    printf(final_line);
  }
  else if (situacao == 4)
  { //variavel do lado esquerdo

    final_line = replaceWord(store, oldType0, tipos[allocatab[lastNumberRegister].tipo]);
    final_line = replaceWord(final_line, oldType1, tipos[allocatab[lastNumberRegister].tipo]);
    sprintf(number_register, "%d", lastNumberRegister);
    final_line = replaceWord(final_line, oldW0, number_register);
    sprintf(number_register, "%d", availableRegister++);
    final_line = replaceWord(final_line, oldW, number_register);
    printf(final_line);
  }

  /*sprintf(number_register, "%d", allocatab[searchAllocaTab(variable_name)].num_registrado + 1);
  final_line = replaceWord(store, oldW, number_register);
  sprintf(number_register, "%d", ++lastNumberRegister);
  final_line = replaceWord(final_line, oldW0, number_register);
  final_line = replaceWord(final_line, oldType0, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  final_line = replaceWord(final_line, oldType1, tipos[allocatab[searchAllocaTab(variable_name)].tipo]);
  printf(final_line);*/
}

void showAllocaTab()
{
  for (int i = 0; i < lastNumberRegister; i++)
  {
    printf("reg: %d nome:%s  tipo:%d valor:%.2lf\n", allocatab[i].num_registrado, allocatab[i].nome_variavel == NULL ? " " : allocatab[i].nome_variavel, allocatab[i].tipo, allocatab[i].valor_variavel);
  }
}

void writeRet()
{
  char ret[] = "ret TIPO0 RESULT0";
}

void writeFooter()
{
  char footer[] = "}";
}
