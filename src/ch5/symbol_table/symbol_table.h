#ifndef symtab_h
#define symtab_h

#include <string.h>
#include <stdlib.h>
#include "ch5/misc/common.h"

/*--------------------------------------------------------------*/
/*  Value structure                                             */
/*--------------------------------------------------------------*/

typedef union {
  int   integer;
  float real;
  char  character;
  char  *stringp;
} VALUE;

/*--------------------------------------------------------------*/
/*  Definition structure                                        */
/*--------------------------------------------------------------*/

typedef enum {
  UNDEFINED,
  CONST_DEFN, TYPE_DEFN, VAR_DEFN, FIELD_DEFN,
  VALPARM_DEFN, VARPARM_DEFN,
  PROG_DEFN, PROC_DEFN, FUNC_DEFN,
} DEFN_KEY;

typedef enum {
  DECLARED, FORWARD,
  READ, READLN, WRITE, WRITELN,
  ABS, ARCTAN, CHR, COS, EOFF, EOLN, EXP, LN, ODD, ORD,
  PRED, ROUND, SIN, SQR, SQRT, SUCC, TRUNC,
} ROUTINE_KEY;

typedef struct {
  DEFN_KEY key;
  union {
    struct {
      VALUE value;
    } constant;

    struct {
      ROUTINE_KEY        key;
      int                parm_count;
      int                total_parm_size;
      int                total_local_size;
      struct symtab_node *parms;
      struct symtab_node *locals;
      struct symtab_node *local_symtab;
      char               *code_segment;
    } routine;

    struct {
      int                offset;
      struct symtab_node *record_idp;
    } data;
  } info;
} DEFN_STRUCT;

/*--------------------------------------------------------------*/
/*  Symbol table node						*/
/*--------------------------------------------------------------*/

typedef struct symtab_node {
  struct symtab_node *left, *right;	/* ptrs to subtrees */
  struct symtab_node *next;		/* for chaining nodes */
  char               *name;		/* name string */
  char               *info;           /* ptr to generic info */
  DEFN_STRUCT        defn;            /* definition struct */
  int                level;           /* nesting level */
  int                label_index;     /* index for code label */
} SYMTAB_NODE, *SYMTAB_NODE_PTR;

/*--------------------------------------------------------------*/
/*  Functions                                                   */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR search_symtab(char *name, SYMTAB_NODE_PTR np);
SYMTAB_NODE_PTR enter_symtab(char *name, SYMTAB_NODE_PTR *npp);

#endif
