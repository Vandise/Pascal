#ifndef statement_h
#define statement_h

#include <stdio.h>
#include <stdlib.h>
#include "ch12/misc/common.h"
#include "ch12/error/error.h"
#include "ch12/scanner/scanner.h"
#include "ch12/symbol_table/symbol_table.h"
#include "ch12/parser/parser.h"
#include "ch12/parser/expression.h"
#include "ch12/code/code.h"
#include "ch12/code/emitcode.h"

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;
extern TOKEN_CODE       statement_start_list[], statement_end_list[];

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

void statement();
void assignment_statement(SYMTAB_NODE_PTR var_idp);
void repeat_statement();
void while_statement();
void if_statement();
void for_statement();
void case_statement();
void case_branch(TYPE_STRUCT_PTR expr_tp);
TYPE_STRUCT_PTR case_label();
void compound_statement();

#endif