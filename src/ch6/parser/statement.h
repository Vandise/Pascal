#ifndef statement_h
#define statement_h

#include <stdio.h>
#include <stdlib.h>
#include "ch6/misc/common.h"
#include "ch6/error/error.h"
#include "ch6/scanner/scanner.h"
#include "ch6/symbol_table/symbol_table.h"
#include "ch6/parser/parser.h"
#include "ch6/parser/expression.h"

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;
extern TOKEN_CODE       statement_start_list[], statement_end_list[];

extern SYMTAB_NODE_PTR  symtab_root;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

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