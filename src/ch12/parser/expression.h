#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <stdlib.h>
#include "ch12/misc/common.h"
#include "ch12/error/error.h"
#include "ch12/scanner/scanner.h"
#include "ch12/parser/parser.h"
#include "ch12/symbol_table/symbol_table.h"

extern TOKEN_CODE token;
extern char       token_string[];
extern char       word_string[];
extern LITERAL    literal;

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern SYMTAB_NODE_PTR  float_literal_list;
extern SYMTAB_NODE_PTR  string_literal_list;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

extern TOKEN_CODE statement_end_list[];

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR expression();
TYPE_STRUCT_PTR simple_expression();
TYPE_STRUCT_PTR term();
TYPE_STRUCT_PTR factor();
TYPE_STRUCT_PTR float_literal(char string[], float value);
TYPE_STRUCT_PTR string_literal(char string[], int length);
TYPE_STRUCT_PTR constant_identifier(SYMTAB_NODE_PTR idp);
TYPE_STRUCT_PTR variable(SYMTAB_NODE_PTR var_idp, USE use);
TYPE_STRUCT_PTR array_subscript_list(TYPE_STRUCT_PTR tp);
TYPE_STRUCT_PTR record_field(TYPE_STRUCT_PTR tp);
void check_rel_op_types(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2);
BOOLEAN is_assign_type_compatible(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2);
TYPE_STRUCT_PTR base_type(TYPE_STRUCT_PTR tp);

/*--------------------------------------------------------------*/
/*  integer_operands    TRUE if both operands are integer,      */
/*                      else FALSE.                             */
/*--------------------------------------------------------------*/

#define integer_operands(tp1, tp2)  ((tp1 == integer_typep) &&  \
				     (tp2 == integer_typep))

/*--------------------------------------------------------------*/
/*  real_operands       TRUE if at least one or both operands   */
/*                      operands are real (and the other        */
/*                      integer), else FALSE.                   */
/*--------------------------------------------------------------*/

#define real_operands(tp1, tp2) (((tp1 == real_typep) &&        \
				    ((tp2 == real_typep) ||     \
				     (tp2 == integer_typep)))   \
					||                      \
				 ((tp2 == real_typep) &&        \
				    ((tp1 == real_typep) ||     \
				     (tp1 == integer_typep))))

/*--------------------------------------------------------------*/
/*  boolean_operands    TRUE if both operands are boolean       */
/*                      else FALSE.                             */
/*--------------------------------------------------------------*/

#define boolean_operands(tp1, pt2)  ((tp1 == boolean_typep) &&  \
				     (tp2 == boolean_typep))

#endif