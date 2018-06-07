#ifndef statement_h
#define statement_h

#include <stdio.h>
#include <stdlib.h>
#include "ch5/misc/common.h"
#include "ch5/error/error.h"
#include "ch5/scanner/scanner.h"
#include "ch5/symbol_table/symbol_table.h"
#include "ch5/parser/parser.h"
#include "ch5/parser/expression.h"

extern TOKEN_CODE token;
extern LITERAL    literal;
extern TOKEN_CODE statement_start_list[], statement_end_list[];

void statement();
void assignment_statement();
void repeat_statement();
void while_statement();
void if_statement();
void for_statement();
void case_statement();
void case_branch();
void case_label();
void compound_statement();

#endif