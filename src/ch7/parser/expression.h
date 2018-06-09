#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <stdlib.h>
#include "ch7/misc/common.h"
#include "ch7/error/error.h"
#include "ch7/scanner/scanner.h"
#include "ch7/parser/parser.h"
#include "ch7/symbol_table/symbol_table.h"

extern TOKEN_CODE token;
extern char       word_string[];
extern LITERAL    literal;

extern SYMTAB_NODE_PTR  symtab_root;
extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;


TYPE_STRUCT_PTR expression(), simple_expression(), term(), factor(), function_call();
TYPE_STRUCT_PTR base_type(TYPE_STRUCT_PTR tp);
BOOLEAN is_assign_type_compatible(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2);
void check_rel_op_types(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2);
TYPE_STRUCT_PTR record_field(TYPE_STRUCT_PTR tp);
TYPE_STRUCT_PTR array_subscript_list(TYPE_STRUCT_PTR tp);
TYPE_STRUCT_PTR variable(SYMTAB_NODE_PTR var_idp, USE use);


#define integer_operands(tp1, tp2)  ((tp1 == integer_typep) &&  \
				     (tp2 == integer_typep))


#define real_operands(tp1, tp2) (((tp1 == real_typep) &&        \
				    ((tp2 == real_typep) ||     \
				     (tp2 == integer_typep)))   \
					||                      \
				 ((tp2 == real_typep) &&        \
				    ((tp1 == real_typep) ||     \
				     (tp1 == integer_typep))))

#define boolean_operands(tp1, pt2)  ((tp1 == boolean_typep) &&  \
				     (tp2 == boolean_typep))

#endif