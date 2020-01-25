
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
int typeOfLastVariable = -1;
int isLoadLastOperation = 0;

int line = 0;
int column = 0;
int lastNumberRegister = 0;
int availableRegister = 1;
int countRegisters = 0;

int reg_var = 0;
int num_register = 0;
char percent[] = "%";
int declarations[NHASH];
int countDeclarations = 0;

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
  asttab[position].value = s->value;
  asttab[position].a = malloc(sizeof(struct symasgn));
  asttab[position].a = (struct ast *)a;
  asttab[position].valuetype = isInteger(eval(a->v));
  position++;
}

void insertPrintASTTab(struct ast *arv)
{

  struct symasgn *a = malloc(sizeof(struct symasgn));
  struct symbol *s = malloc(sizeof(struct symbol));

  if (!a)
  {
    yyerror("Fora do espaço");
    exit(0);
  }

  //Caracterizando um print
  a->nodetype = 'p';
  s->name = "print";
  s->valuetype = 2;
  a->s = s;

  //Alocando o print
  a->v = arv;
  a->s->value = eval(a->v);
  asttab[position].name = s->name;
  asttab[position].a = malloc(sizeof(struct ast));
  asttab[position].a = (struct ast *)a;
  asttab[position].valuetype = isInteger(eval(a->v));
  position++;

  /*a->nodetype = 'p';
  s->name = "print";
  s->valuetype = 2;
  a->s = s;

  asttab[position].name = "print";
  asttab[position].a = malloc(sizeof(struct ast));
  asttab[position].a = (struct ast *)arv;
  asttab[position].a->nodetype = 'p';
  asttab[position].valuetype = isInteger(eval(a->v));
  position++;*/
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
          asttab[p].value = s->value;
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
      printf("%s %d %c %lf\n", asttab[i].name, asttab[i].valuetype, asttab[i].a->nodetype, asttab[i].value);
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

void newprint(struct ast *arv)
{

  if (!arv)
  {
    yyerror("erro on print");
    exit(0);
  }
  insertPrintASTTab(arv);
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
  a->s->value = eval(a->v);

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
    //yyerror("Erro interno, EVAL recebendo arvore NULA");
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
  case 'p':
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
    printf("%4.4g\n", ((struct numval *)a)->number);
    break;

    /* nome de variável */
  case 'N':
    printf("ref %s\n", ((struct symref *)a)->s->name);
    dumpast(searchAST(((struct symref *)a)->s->name), level);
    break;

    /* declaração */
  case '=':
    printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast(((struct symasgn *)a)->v, level);
    return;

    /* expressões */
  case '+':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '-':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '*':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '/':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '^':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case 'L':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '1':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '2':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '3':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '4':
    printf("op %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;
  case '5':
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

char *replaceWord(const char *s, const char *oldW,
                  const char *newW)
{
  char *result;
  int i, cnt = 0;
  int newWlen = strlen(newW);
  int oldWlen = strlen(oldW);
  for (i = 0; s[i] != '\0'; i++)
  {
    if (strstr(&s[i], oldW) == &s[i])
    {
      cnt++;
      i += oldWlen - 1;
    }
  }
  result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
  i = 0;
  while (*s)
  {
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

void writeHeader(FILE *resultFile)
{
  fputs("target triple = \"x86_64-pc-linux-gnu\"\n", resultFile);
  fputs("define i32 @main() #0 {\n", resultFile);
}

void footer(FILE *resultFile)
{
  fputs("ret i32 0\n }", resultFile);
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
  }
  return count;
}

int searchAllocaTab(char *name)
{
  for (int i = 0; i < lastNumberRegister; i++)
  {
    if (strcmp(allocatab[i].nome_variavel, name) == 0)
      return i;
  }
  return -1;
}

void writeAlloca(FILE *resultFile)
{
  //Realiza a contagem de alocações de acordo com a quantidade de declarações
  int count = countDECL();
  for (int i = 1; i <= count; i++)
    fprintf(resultFile, "%s%d = alloca %s, align 4\n", percent, i, tipos[allocatab[i - 1].tipo]);
}

char *writeOpFloatingPoint(char op[])
{

  char *newOp = malloc(4);
  if (strcmp(op, "add") == 0)
  {
    strcpy(newOp, "fadd");
  }
  else if (strcmp(op, "sub") == 0)
  {
    strcpy(newOp, "fsub");
  }
  else if (strcmp(op, "mul") == 0)
  {
    strcpy(newOp, "fmul");
  }
  else if (strcmp(op, "sdiv") == 0)
  {
    strcpy(newOp, "fdiv");
  }
  return newOp;
}

void writeOp(FILE *resultFile, char op[], char type[], struct ast *a)
{

  printf("lastOperation:%d\n", isLoadLastOperation);
  type = (isLoadLastOperation == 1) ? tipos[typeOfLastVariable] : type;
  if (strcmp(type, "i32") == 0)
  {
    if (strcmp(op, "sdiv") == 0)
      fprintf(resultFile, "\n%s%d = %s %s %s%d, %s%d\n", percent, num_register, op, (type), percent, converter(a->l, resultFile), percent, converter(a->r, resultFile));
    else
      fprintf(resultFile, "\n%s%d = %s nsw %s %s%d, %s%d\n", percent, num_register, op, type, percent, converter(a->l, resultFile), percent, converter(a->r, resultFile));
  }
  else
  {
    char *newOp = writeOpFloatingPoint(op);
    fprintf(resultFile, "\n%s%d = %s  %s %s%d, %s%d\n", percent, num_register, newOp, type, percent, converter(a->l, resultFile), percent, converter(a->r, resultFile));
  }
}

int declarationsContains(int num)
{
  for (int i = 0; i < countDeclarations; i++)
  {
    if (declarations[i] == num)
      return 1;
  }
  return 0;
}

void writePrint(FILE *resultFile, char type[])
{
  if (declarationsContains(4) == 0)
    declarations[countDeclarations++] = 4;
  if (typeOfLastVariable == 1)
  {
    fprintf(resultFile, "%s%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %s%d)\n", percent, num_register, percent, num_register - 1);
    if (declarationsContains(2) == 0)
      declarations[countDeclarations++] = 2;
  }
  else
  {
    fprintf(resultFile, "%s%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %s%d)\n", percent, num_register, percent, num_register - 1);
    if (declarationsContains(3) == 0)
      declarations[countDeclarations++] = 3;
  }
}

int converter(struct ast *a, FILE *resultFile)
{
  int reg;
  if (!a)
  {
    yyerror("Erro interno, CONVERTER recebendo arvore NULA");
    return -1;
  }
  char type[6];
  switch (a->nodetype)
  {
  /* constante */
  case 'K':
    isLoadLastOperation = 0;
    printf("K");
    strcpy(type, tipos[isInteger(((struct numval *)a)->number)]);
    fprintf(resultFile, "\n%s%d = alloca %s, align 4\n", percent, num_register, type);
    if (strcmp(type, "i32") == 0)
      fprintf(resultFile, "store %s %.0lf, %s* %s%d, align 4\n", type, ((struct numval *)a)->number, type, percent, num_register);
    else
      fprintf(resultFile, "store %s %lf, %s* %s%d, align 4\n", type, ((struct numval *)a)->number, type, percent, num_register);
    num_register++;
    fprintf(resultFile, "%s%d = load %s, %s*  %s%d, align 4\n", percent, num_register, type, type, percent, num_register - 1);
    reg = num_register;
    break;
  /* nome de variavel */
  case 'N':
    printf("LOAD\n");
    typeOfLastVariable = allocatab[searchAllocaTab(getNameVariable(a))].tipo;
    isLoadLastOperation = 1;
    strcpy(type, tipos[typeOfLastVariable]);
    int num_registrador = allocatab[searchAllocaTab(getNameVariable(a))].num_registrado;
    fprintf(resultFile, "\n%s%d = load %s, %s*  %s%d, align 4\n", percent, num_register, type, type, percent, num_registrador);
    //reg = allocatab[searchAllocaTab(((struct symref *)a)->s->name)].num_registrado;
    reg = num_register;
    break;
  case 'p':
    printf("P");

    isLoadLastOperation = 1;
    converter(((struct symasgn *)a)->v, resultFile);
    //strcpy(type, tipos[isInteger(eval(((struct symasgn *)a)->v))]);
    writePrint(resultFile, type);
    reg = num_register;
    break;
  /* declaração */
  case '=':
    printf("=");
    isLoadLastOperation = 0;
    reg = converter(((struct symasgn *)a)->v, resultFile);
    strcpy(type, tipos[isInteger(eval(((struct symasgn *)a)->v))]);
    if (strcmp(type, "double") == 0)
      num_register--;
    fprintf(resultFile, "store %s %s%d , %s* %s%d, align 4\n", type, percent, num_register, type, percent, ++reg_var);
    break;
  /* expressões */
  case '+':
    strcpy(type, tipos[isInteger(eval(a->l))]);
    writeOp(resultFile, "add", type, a);
    reg = num_register;
    break;
  case '-':
    strcpy(type, tipos[isInteger(eval(a->l))]);
    writeOp(resultFile, "sub", type, a);
    num_register--;
    reg = num_register;
    break;
  case '*':
    strcpy(type, tipos[isInteger(eval(a->l))]);
    writeOp(resultFile, "mul", type, a);
    reg = num_register;
    break;
  case '/':
    strcpy(type, tipos[isInteger(eval(a->l))]);
    writeOp(resultFile, "sdiv", type, a);
    reg = num_register;
    break;
  case '^':
    strcpy(type, tipos[isInteger(eval(a->l))]);

    int regLeft = converter(a->l, resultFile);
    int regRight = converter(a->r, resultFile);

    if (strcmp(type, "i32") == 0)
    {
      fprintf(resultFile, "%s%d = sitofp i32 %s%d to double\n", percent, num_register, percent, regRight);
      num_register++;
      fprintf(resultFile, "%s%d = sitofp i32 %s%d to double\n", percent, num_register, percent, regLeft);
      num_register++;
      fprintf(resultFile, "%s%d = call double @pow(double %s%d, double %s%d)\n", percent, num_register, percent, num_register - 1, percent, num_register);
    }
    else
      fprintf(resultFile, "%s%d = call double @pow(double %s%d, double %s%d)\n", percent, num_register, percent, converter(a->l, resultFile), percent, converter(a->r, resultFile));

    if (isInteger(pow(eval(a->l), eval(a->r))))
      fprintf(resultFile, "%s%d = fptosi double %s%d to i32\n", percent, num_register + 1, percent, num_register);

    if (declarationsContains(1) == 0)
      declarations[num_register] = 1;
    reg = num_register;
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

void writeIR(FILE *resultFile)
{
  for (int i = 0; i < position; i++)
  {
    if (strlen(asttab[i].name) > 0)
    {
      converter(asttab[i].a, resultFile);
    }
  }
}

void writeDeclares(FILE *resultFile)
{
  for (int i = 0; i < NHASH; i++)
  {
    if (declarations[i] == 1)
      fprintf(resultFile, "declare double @pow(double, double)\n");
    if (declarations[i] == 4)
      fprintf(resultFile, "declare i32 @printf(i8*, ...) #1\n");
    if (declarations[i] == 2)
      fprintf(resultFile, "@.str = private unnamed_addr constant [3 x i8] c\"%s%s\\00\", align 1\n", "%", "d");

    if (declarations[i] == 3)
      fprintf(resultFile, "@.str.1 = private unnamed_addr constant [3 x i8] c\"%s%s\\00\", align 1\n", "%", "f");
  }
}

void createFile(FILE *resultFile)
{
  writeHeader(resultFile);
  writeAlloca(resultFile);
  num_register = countDECL() + 1;
  writeIR(resultFile);
  footer(resultFile);
  writeDeclares(resultFile);
}

int main(int argc, char **argv)
{

  //FILE *file = fopen(argv[1], "r");
  FILE *file = fopen("programa.txt", "r");
  FILE *resultFile = fopen("./resultado.ll", "w+");
  if (file == NULL)
  {
    printf("Não foi possível abrir o arquivo!\n Por favor verifique se o arquivo existe. \n");
    return 1;
  }
  yyin = file;

  yyparse();
  createFile(resultFile);
  fclose(resultFile);
  system("clang resultado.ll -lm");
}