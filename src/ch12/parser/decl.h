#ifndef decl_h
#define decl_h

#include <stdio.h>
#include "ch12/misc/common.h"
#include "ch12/error/error.h"
#include "ch12/scanner/scanner.h"
#include "ch12/symbol_table/symbol_table.h"
#include "ch12/parser/parser.h"

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern TOKEN_CODE       declaration_start_list[],
			statement_start_list[];

TYPE_STRUCT_PTR do_type(), identifier_type(), enumeration_type(), subrange_type(), array_type(), record_type();

void declarations(SYMTAB_NODE_PTR rtn_idp);
void const_definitions();
void do_const(SYMTAB_NODE_PTR const_idp);
void type_definitions();
TYPE_STRUCT_PTR do_type();
TYPE_STRUCT_PTR identifier_type(SYMTAB_NODE_PTR idp);
TYPE_STRUCT_PTR enumeration_type();
TYPE_STRUCT_PTR subrange_type(SYMTAB_NODE_PTR min_idp);
void get_subrange_limit(SYMTAB_NODE_PTR minmax_idp, int *minmaxp, TYPE_STRUCT_PTR *typepp);
TYPE_STRUCT_PTR array_type();
TYPE_STRUCT_PTR record_type();
TYPE_STRUCT_PTR make_string_typep(int length);
int array_size(TYPE_STRUCT_PTR tp);
void var_declarations(SYMTAB_NODE_PTR rtn_idp);
void var_or_field_declarations(SYMTAB_NODE_PTR rtn_idp, TYPE_STRUCT_PTR record_tp, int offset);

#endif

