#ifndef standard_h
#define standard_h

#include <stdio.h>
#include <stdlib.h>
#include "ch12/misc/common.h"
#include "ch12/error/error.h"
#include "ch12/scanner/scanner.h"
#include "ch12/symbol_table/symbol_table.h"
#include "ch12/parser/parser.h"

#define DEFAULT_NUMERIC_FIELD_WIDTH     10
#define DEFAULT_PRECISION               2

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;
extern TYPE_STRUCT      dummy_type;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

extern TOKEN_CODE       follow_parm_list[];
extern TOKEN_CODE       statement_end_list[];

TYPE_STRUCT_PTR standard_routine_call(SYMTAB_NODE_PTR rtn_idp);
void read_readln(SYMTAB_NODE_PTR rtn_idp);
void write_writeln(SYMTAB_NODE_PTR rtn_idp);
TYPE_STRUCT_PTR eof_eoln(SYMTAB_NODE_PTR rtn_idp);
TYPE_STRUCT_PTR abs_sqr();
TYPE_STRUCT_PTR arctan_cos_exp_ln_sin_sqrt();
TYPE_STRUCT_PTR pred_succ();
TYPE_STRUCT_PTR chr();
TYPE_STRUCT_PTR odd();
TYPE_STRUCT_PTR ord();
TYPE_STRUCT_PTR round_trunc();

#endif