/* symbol table */
struct symbol {		/* a variable name */
  char *name;
  double value;
  int valuetype;
  struct ast *func;	/* stmt for the function */
};

struct reftab{
    char *name;
    int valuetype;
    struct ast *a;
};

/* simple symtab of fixed size */
#define NHASH 9997
struct symbol symtab[NHASH];
struct reftab asttab[NHASH];

struct symbol *lookup(char*);

struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};



struct numval {
  int nodetype;			/* type K */
  double number;
};

struct symref {
  int nodetype;			/* type N */
  struct symbol *s;
};

struct symasgn {
  int nodetype;			/* type = */
  int valuetype;
  struct symbol *s;
  struct ast *v;		/* value */
};

/* Construção de uma AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);


double eval(struct ast *);
void treefree(struct ast *);
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
extern int debug;
extern int position;
extern void showAST();
void dumpast(struct ast *a, int level);
void updateInfoErro(int linha, int coluna);


