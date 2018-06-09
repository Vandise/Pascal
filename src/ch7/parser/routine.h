#ifndef routine_h
#define routine_h

#include <stdio.h>
#include <stdlib.h>
#include "ch7/misc/common.h"
#include "ch7/error/error.h"
#include "ch7/scanner/scanner.h"
#include "ch7/symbol_table/symbol_table.h"
#include "ch7/parser/parser.h"

extern int              line_number;
extern int              error_count;

extern TOKEN_CODE       token;
extern char             word_string[];
extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT      dummy_type;

extern TOKEN_CODE       statement_start_list[],
			statement_end_list[],
			declaration_start_list[];

void program();
SYMTAB_NODE_PTR program_header();
void routine();
SYMTAB_NODE_PTR procedure_header();
SYMTAB_NODE_PTR function_header();
SYMTAB_NODE_PTR formal_parm_list(int * countp, int * total_sizep);
TYPE_STRUCT_PTR routine_call(SYMTAB_NODE_PTR rtn_idp, BOOLEAN parm_check_flag);
TYPE_STRUCT_PTR declared_routine_call(SYMTAB_NODE_PTR rtn_idp, BOOLEAN parm_check_flag);
void actual_parm_list(SYMTAB_NODE_PTR rtn_idp, BOOLEAN parm_check_flag);
void block(SYMTAB_NODE_PTR rtn_idp);


#endif