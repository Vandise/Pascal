
#include "ch12/code/emitcode.h"

SYMTAB_NODE_PTR float_literal_list = NULL;
SYMTAB_NODE_PTR string_literal_list = NULL;

void emit_program_prologue() {
  fprintf(code_file, "\tDOSSEG\n");
  fprintf(code_file, "\t.MODEL  small\n");
  fprintf(code_file, "\t.STACK  1024\n");
  fprintf(code_file, "\n");
  fprintf(code_file, "\t.CODE\n");
  fprintf(code_file, "\n");
  fprintf(code_file, "\tPUBLIC\t_pascal_main\n");
  fprintf(code_file, "\tINCLUDE\tpasextrn.inc\n");
  fprintf(code_file, "\n");

  /*
  --  Equates for stack frame components.
  */
  fprintf(code_file, "%s\t\tEQU\t<WORD PTR [bp+4]>\n",
    STATIC_LINK);
  fprintf(code_file, "%s\t\tEQU\t<WORD PTR [bp-4]>\n",
    RETURN_VALUE);
  fprintf(code_file, "%s\tEQU\t<WORD PTR [bp-2]>\n",
    HIGH_RETURN_VALUE);
  fprintf(code_file, "\n");
}

void emit_program_epilogue(SYMTAB_NODE_PTR prog_idp) {
  SYMTAB_NODE_PTR np;
  int i, length;

  fprintf(code_file, "\n");
  fprintf(code_file, "\t.DATA\n");
  fprintf(code_file, "\n");

  /*
  --  Emit declarations for the program's global variables.
  */
  for (np = prog_idp->defn.info.routine.locals; np != NULL; np = np->next) {
    fprintf(code_file, "%s_%03d\t", np->name, np->label_index);
    if (np->typep == char_typep)
      fprintf(code_file, "DB\t0\n");
    else if (np->typep == real_typep)
      fprintf(code_file, "DD\t0.0\n");
    else if (np->typep->form == ARRAY_FORM)
      fprintf(code_file, "DB\t%d DUP(0)\n", np->typep->size);
    else if (np->typep->form == RECORD_FORM)
      fprintf(code_file, "DB\t%d DUP(0)\n", np->typep->size);
    else
      fprintf(code_file, "DW\t0\n");
  }

  /*
  --  Emit declarations for the program's floating point literals.
  */
  for (np = float_literal_list; np != NULL; np = np->next)
    fprintf(code_file, "%s_%03d\tDD\t%e\n", FLOAT_LABEL_PREFIX,
      np->label_index,
      np->defn.info.constant.value.real);

  /*
  --  Emit declarations for the program's string literals.
  */
  for (np = string_literal_list; np != NULL; np = np->next) {
    fprintf(code_file, "%s_%03d\tDB\t\"", STRING_LABEL_PREFIX,
      np->label_index);

    length = strlen(np->name) - 2;
    for (i = 1; i <= length; ++i) fputc(np->name[i], code_file);

    fprintf(code_file, "\"\n");
  }

  fprintf(code_file, "\n");
  fprintf(code_file, "\tEND\n");
}

void emit_main_prologue() {
  fprintf(code_file, "\n");
  fprintf(code_file, "_pascal_main\tPROC\n");
  fprintf(code_file, "\n");

  emit_1(PUSH, reg(BP)); /* dynamic link */
  emit_2(MOVE, reg(BP), reg(SP)); /* new stack frame base */
}

void emit_main_epilogue() {
  fprintf(code_file, "\n");

  emit_1(POP, reg(BP)); /* restore caller's stack frame */
  emit(RETURN); /* return */

  fprintf(code_file, "\n");
  fprintf(code_file, "_pascal_main\tENDP\n");
}

void emit_routine_prologue(SYMTAB_NODE_PTR rtn_idp) {
  fprintf(code_file, "\n");
  fprintf(code_file, "%s_%03d\tPROC\n",
    rtn_idp->name, rtn_idp->label_index);
  fprintf(code_file, "\n");

  emit_1(PUSH, reg(BP)); /* dynamic link */
  emit_2(MOVE, reg(BP), reg(SP)); /* new stack frame base */

  /*
  --  Allocate stack space for a function's return value.
  */
  if (rtn_idp->defn.key == FUNC_DEFN) emit_2(SUBTRACT, reg(SP),
    integer_lit(4));

  /*
  --  Allocate stack space for the local variables.
  */
  if (rtn_idp->defn.info.routine.total_local_size > 0)
    emit_2(SUBTRACT, reg(SP),
      integer_lit(rtn_idp->defn.info.routine
        .total_local_size));
}

void emit_routine_epilogue(SYMTAB_NODE_PTR rtn_idp) {
  /*
  --  Load a function's return value into the ax or dx:ax registers.
  */
  if (rtn_idp->defn.key == FUNC_DEFN) {
    emit_2(MOVE, reg(AX), name_lit(RETURN_VALUE));
    if (rtn_idp->typep == real_typep)
      emit_2(MOVE, reg(DX), name_lit(HIGH_RETURN_VALUE));
  }

  emit_2(MOVE, reg(SP), reg(BP)); /* cut back to caller's stack */
  emit_1(POP, reg(BP)); /* restore caller's stack frame */

  emit_1(RETURN, integer_lit(rtn_idp->defn.info.routine
    .total_parm_size + 2));
  /* return and cut back stack */

  fprintf(code_file, "\n");
  fprintf(code_file, "%s_%03d\tENDP\n",
    rtn_idp->name, rtn_idp->label_index);
}

void emit_declarations(SYMTAB_NODE_PTR rtn_idp) {
  SYMTAB_NODE_PTR parm_idp = rtn_idp->defn.info.routine.parms;
  SYMTAB_NODE_PTR var_idp = rtn_idp->defn.info.routine.locals;

  fprintf(code_file, "\n");

  /*
  --  Parameters.
  */
  while (parm_idp != NULL) {
    emit_text_equate(parm_idp);
    parm_idp = parm_idp->next;
  }

  /*
  --  Local variables.
  */
  while (var_idp != NULL) {
    emit_text_equate(var_idp);
    var_idp = var_idp->next;
  }
}

void emit_numeric_equate(SYMTAB_NODE_PTR idp) {
  fprintf(code_file, "%s_%03d\tEQU\t%d\n",
    idp->name, idp->label_index,
    idp->defn.info.data.offset);
}

void emit_text_equate(SYMTAB_NODE_PTR idp) {
  char * name = idp->name;
  int label_index = idp->label_index;
  int offset = idp->defn.info.data.offset;

  if (idp->typep == char_typep)
    fprintf(code_file, "%s_%03d\tEQU\t<BYTE PTR [bp%+d]>\n",
      name, label_index, offset);
  else if (idp->typep == real_typep)
    fprintf(code_file, "%s_%03d\tEQU\t<WORD PTR [bp%+d]>\n",
      name, label_index, offset);
  else
    fprintf(code_file, "%s_%03d\tEQU\t<WORD PTR [bp%+d]>\n",
      name, label_index, offset);
}

void emit_load_value(SYMTAB_NODE_PTR var_idp, TYPE_STRUCT_PTR var_tp) {
  int var_level = var_idp->level;
  BOOLEAN varparm_flag = var_idp->defn.key == VARPARM_DEFN;

  if (varparm_flag) {
    /*
    --  VAR formal parameter.
    --  AX or DX:AX = value the address points to
    */
    emit_2(MOVE, reg(BX), word(var_idp));
    if (var_tp == char_typep) {
      emit_2(SUBTRACT, reg(AX), reg(AX));
      emit_2(MOVE, reg(AL), byte_indirect(BX));
    } else if (var_tp == real_typep) {
      emit_2(MOVE, reg(AX), word_indirect(BX));
      emit_2(MOVE, reg(AX), high_dword_indirect(BX));
    } else emit_2(MOVE, reg(AX), word_indirect(BX));
  } else if ((var_level == level) || (var_level == 1)) {
    /*
    --  Global or local parameter or variable:
    --  AX or DX:AX = value
    */
    if (var_tp == char_typep) {
      emit_2(SUBTRACT, reg(AX), reg(AX));
      emit_2(MOVE, reg(AL), byte(var_idp));
    } else if (var_tp == real_typep) {
      emit_2(MOVE, reg(AX), word(var_idp));
      emit_2(MOVE, reg(DX), high_dword(var_idp));
    } else emit_2(MOVE, reg(AX), word(var_idp));
  } else /* var_level < level */ {
    /*
    --  Nonlocal parameter or variable.
    --  First locate the appropriate stack frame, then:
    --  AX or DX:AX = value
    */
    int lev = var_level;

    emit_2(MOVE, reg(BX), reg(BP));
    do {
      emit_2(MOVE, reg(BP), name_lit(STATIC_LINK));
    } while (++lev < level);

    if (var_tp == char_typep) {
      emit_2(SUBTRACT, reg(AX), reg(AX));
      emit_2(MOVE, reg(AL), byte(var_idp));
    } else if (var_tp == real_typep) {
      emit_2(MOVE, reg(AX), word(var_idp));
      emit_2(MOVE, reg(DX), high_dword(var_idp));
    } else emit_2(MOVE, reg(AX), word(var_idp));

    emit_2(MOVE, reg(BP), reg(BX));
  }
}

void emit_push_operand(TYPE_STRUCT_PTR tp) {
  if ((tp->form == ARRAY_FORM) || (tp->form == RECORD_FORM)) return;

  if (tp == real_typep) emit_1(PUSH, reg(DX));
  emit_1(PUSH, reg(AX));
}

void emit_push_address(SYMTAB_NODE_PTR var_idp) {
  int var_level = var_idp->level;
  BOOLEAN varparm_flag = var_idp->defn.key == VARPARM_DEFN;

  if ((var_level == level) || (var_level == 1))
    emit_2(varparm_flag ? MOVE : LOAD_ADDRESS,
      reg(AX), word(var_idp))

  else /* var_level < level */ {
    int lev = var_level;

    emit_2(MOVE, reg(BX), reg(BP));
    do {
      emit_2(MOVE, reg(BP), name_lit(STATIC_LINK));
    } while (++lev < level);
    emit_2(varparm_flag ? MOVE : LOAD_ADDRESS,
      reg(AX), word(var_idp));
    emit_2(MOVE, reg(BP), reg(BX));
  }

  emit_1(PUSH, reg(AX));
}

void emit_push_return_value_address(SYMTAB_NODE_PTR var_idp) {
  int lev = var_idp->level + 1;

  if (lev < level) {
    /*
    --  Find the appropriate stack frame.
    */
    emit_2(MOVE, reg(BX), reg(BP));
    do {
      emit_2(MOVE, reg(BP), name_lit(STATIC_LINK));
    } while (++lev < level);
    emit_2(LOAD_ADDRESS, reg(AX), name_lit(RETURN_VALUE));
    emit_2(MOVE, reg(BP), reg(BX));
  } else emit_2(LOAD_ADDRESS, reg(AX), name_lit(RETURN_VALUE));

  emit_1(PUSH, reg(AX));
}

void emit_promote_to_real(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2) {
  if (tp2 == integer_typep) {
    emit_1(CALL, name_lit(FLOAT_CONVERT));
    emit_2(ADD, reg(SP), integer_lit(2));
    emit_1(PUSH, reg(DX));
    emit_1(PUSH, reg(AX)); /* ???_1 real_2 */
  }

  if (tp1 == integer_typep) {
    emit_1(POP, reg(AX));
    emit_1(POP, reg(DX));
    emit_1(POP, reg(BX));
    emit_1(PUSH, reg(DX));
    emit_1(PUSH, reg(AX));
    emit_1(PUSH, reg(BX)); /* real_2 integer_1 */

    emit_1(CALL, name_lit(FLOAT_CONVERT));
    emit_2(ADD, reg(SP), integer_lit(2)); /* real_2 real_1 */

    emit_1(POP, reg(BX));
    emit_1(POP, reg(CX));
    emit_1(PUSH, reg(DX));
    emit_1(PUSH, reg(AX));
    emit_1(PUSH, reg(CX));
    emit_1(PUSH, reg(BX)); /* real_1 real_2 */
  }
}