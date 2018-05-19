#include <stdio.h>
#include "ch3/misc/common.h"
#include "ch3/error/error.h"
#include "ch3/symbol_table/symbol_table.h"

/*--------------------------------------------------------------*/
/*  Globals							*/
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR symtab_root = NULL;     /* symbol table root */

/*--------------------------------------------------------------*/
/*  search_symtab       Search for a name in the symbol table.  */
/*                      Return a pointer of the entry if found, */
/*                      or NULL if not.                         */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR search_symtab(char *name, SYMTAB_NODE_PTR np) {
  int cmp;

  while (np != NULL) {
    cmp = strcmp(name, np->name);
      if (cmp == 0) {
        return(np);
      }
      np = cmp < 0 ? np->left : np->right;
  }

  return NULL;
}

/*--------------------------------------------------------------*/
/*  enter_symtab        Enter a name into the symbol table,     */
/*                      and return a pointer to the new entry.  */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR enter_symtab(char *name, SYMTAB_NODE_PTR *npp) {

  int             cmp;	      /* result of strcmp */
  SYMTAB_NODE_PTR new_nodep;	/* ptr to new entry */
  SYMTAB_NODE_PTR np;         /* ptr to node to test */

  new_nodep = alloc_struct(SYMTAB_NODE);
  new_nodep->name = alloc_bytes(strlen(name) + 1);
  strcpy(new_nodep->name, name);
  new_nodep->left = new_nodep->right = new_nodep->next = NULL;
  new_nodep->info = NULL;
  new_nodep->defn.key = UNDEFINED;
  new_nodep->level = new_nodep->label_index = 0;

  while ((np = *npp) != NULL) {
    cmp = strcmp(name, np->name);
    npp = cmp < 0 ? &(np->left) : &(np->right);
  }

  *npp = new_nodep;
  return new_nodep;
}
