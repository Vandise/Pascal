#ifndef emitcode_h
#define emitcode_h

#include <stdio.h>
#include "ch12/misc/common.h"
#include "ch12/symbol_table/symbol_table.h"
#include "ch12/code/code.h"

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern int level;

extern char     asm_buffer[];
extern char     *asm_bufferp;
extern FILE     *code_file;

void emit_program_prologue();
void emit_program_epilogue(SYMTAB_NODE_PTR prog_idp);
void emit_main_prologue();
void emit_main_epilogue();
void emit_routine_prologue(SYMTAB_NODE_PTR rtn_idp);
void emit_routine_epilogue(SYMTAB_NODE_PTR rtn_idp);
void emit_declarations(SYMTAB_NODE_PTR rtn_idp);
void emit_numeric_equate(SYMTAB_NODE_PTR idp);
void emit_load_value(SYMTAB_NODE_PTR var_idp, TYPE_STRUCT_PTR var_tp);
void emit_push_operand(TYPE_STRUCT_PTR tp);
void emit_push_address(SYMTAB_NODE_PTR var_idp);
void emit_push_return_value_address(SYMTAB_NODE_PTR var_idp);
void emit_promote_to_real(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2);
void emit_text_equate(SYMTAB_NODE_PTR idp);

#endif
