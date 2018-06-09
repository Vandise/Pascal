#include "ch12/parser/statement.h"

TOKEN_CODE follow_expr_list[] = {
  OF,
  SEMICOLON,
  0
};

TOKEN_CODE case_label_start_list[] = {
  IDENTIFIER,
  NUMBER,
  PLUS,
  MINUS,
  STRING,
  0
};

TOKEN_CODE follow_case_label_list[] = {
  COLON,
  SEMICOLON,
  0
};

void statement() {
    /*
        --	Call the appropriate routine based on the first
        --	token of the statement.
        */
    switch (token) {

    case IDENTIFIER:
      {
        SYMTAB_NODE_PTR idp;

        /*
        --  Assignment statement or procedure call?
        */
        search_and_find_all_symtab(idp);

        if (idp->defn.key == PROC_DEFN) {
          get_token();
          routine_call(idp, TRUE);
        } else assignment_statement(idp);

        break;
      }

    case BEGIN:
      compound_statement();
      break;

    case WHILE:
    case REPEAT:
    case IF:
    case FOR:
    case CASE:
      {
        error(UNIMPLEMENTED_FEATURE);
        exit(-UNIMPLEMENTED_FEATURE);
      }
    }

    /*
    --  Error synchronization:  Only a semicolon, END, ELSE, or
    --                          UNTIL may follow a statement.
    --                          Check for a missing semicolon.
    */
    synchronize(statement_end_list, statement_start_list, NULL);
    if (token_in(statement_start_list)) error(MISSING_SEMICOLON);
}

void assignment_statement(SYMTAB_NODE_PTR var_idp) {
    TYPE_STRUCT_PTR var_tp, expr_tp; /* types of var and expr */
    BOOLEAN stacked_flag;
    /* TRUE iff target address
					   was pushed on stack */

    var_tp = variable(var_idp, TARGET_USE);
    stacked_flag = (var_idp->defn.key == VARPARM_DEFN) ||
      (var_idp->defn.key == FUNC_DEFN) ||
      (var_idp->typep->form == ARRAY_FORM) ||
      (var_idp->typep->form == RECORD_FORM) ||
      ((var_idp->level > 1) && (var_idp->level < level));

    if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);
    expr_tp = expression();

    if (!is_assign_type_compatible(var_tp, expr_tp))
      error(INCOMPATIBLE_ASSIGNMENT);

    var_tp = base_type(var_tp);
    expr_tp = base_type(expr_tp);

    /*
    --  Emit code to do the assignment.
    */
    if (var_tp == char_typep) {
      /*
      --  char := char
      */
      if (stacked_flag) {
        emit_1(POP, reg(BX));
        emit_2(MOVE, byte_indirect(BX), reg(AL));
      } else emit_2(MOVE, byte(var_idp), reg(AL));
    } else if (var_tp == real_typep) {
      /*
      --  real := ...
      */
      if (expr_tp == integer_typep) {
        /*
        --  ... integer
        */
        emit_1(PUSH, reg(AX));
        emit_1(CALL, name_lit(FLOAT_CONVERT));
        emit_2(ADD, reg(SP), integer_lit(2));
      }
      /*
      --  ... real
      */
      if (stacked_flag) {
        emit_1(POP, reg(BX));
        emit_2(MOVE, word_indirect(BX), reg(AX));
        emit_2(MOVE, high_dword_indirect(BX), reg(DX));
      } else {
        emit_2(MOVE, word(var_idp), reg(AX));
        emit_2(MOVE, high_dword(var_idp), reg(DX));
      }
    } else if ((var_tp->form == ARRAY_FORM) ||
      (var_tp->form == RECORD_FORM)) {
      /*
      --  array  := array
      --  record := record
      */
      emit_2(MOVE, reg(CX), integer_lit(var_tp->size));
      emit_1(POP, reg(SI));
      emit_1(POP, reg(DI));
      emit_2(MOVE, reg(AX), reg(DS));
      emit_2(MOVE, reg(ES), reg(AX));
      emit(CLEAR_DIRECTION);
      emit(MOVE_BLOCK);
    } else {
      /*
      --  integer := integer
      --  enum    := enum
      */
      if (stacked_flag) {
        emit_1(POP, reg(BX));
        emit_2(MOVE, word_indirect(BX), reg(AX));
      } else emit_2(MOVE, word(var_idp), reg(AX));
    }
}

void repeat_statement() {
  TYPE_STRUCT_PTR expr_tp;

  /*
  --  <stmt-list>
  */
  get_token();
  do {
    statement();
    while (token == SEMICOLON) get_token();
  } while (token_in(statement_start_list));

  if_token_get_else_error(UNTIL, MISSING_UNTIL);

  expr_tp = expression();
  if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);
}

void while_statement() {
  TYPE_STRUCT_PTR expr_tp;

  get_token();

  expr_tp = expression();
  if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

  if_token_get_else_error(DO, MISSING_DO);
  statement();
}

void if_statement() {
  TYPE_STRUCT_PTR expr_tp;

  get_token();

  expr_tp = expression();
  if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

  if_token_get_else_error(THEN, MISSING_THEN);
  statement();

  /*
  --  ELSE branch?
  */
  if (token == ELSE) {
    get_token();
    statement();
  }
}

void for_statement() {
  SYMTAB_NODE_PTR for_idp;
  TYPE_STRUCT_PTR for_tp, expr_tp;

  get_token();

  if (token == IDENTIFIER) {
    search_and_find_all_symtab(for_idp);

    for_tp = base_type(for_idp-> typep);
    get_token();

    if ((for_tp != integer_typep) &&
      (for_tp != char_typep) &&
      (for_tp-> form != ENUM_FORM)) error(INCOMPATIBLE_TYPES);
  } else {
    error(MISSING_IDENTIFIER);
    for_tp = & dummy_type;
  }

  if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);

  expr_tp = expression();
  if (!is_assign_type_compatible(for_tp, expr_tp))
    error(INCOMPATIBLE_TYPES);

  if ((token == TO) || (token == DOWNTO)) get_token();
  else error(MISSING_TO_OR_DOWNTO);

  expr_tp = expression();
  if (!is_assign_type_compatible(for_tp, expr_tp))
    error(INCOMPATIBLE_TYPES);

  if_token_get_else_error(DO, MISSING_DO);
  statement();
}

void case_statement() {
  BOOLEAN another_branch;
  TYPE_STRUCT_PTR expr_tp;
  TYPE_STRUCT_PTR case_label();

  get_token();
  expr_tp = expression();

  if (((expr_tp-> form != SCALAR_FORM) &&
      (expr_tp-> form != ENUM_FORM) &&
      (expr_tp-> form != SUBRANGE_FORM)) || (expr_tp == real_typep)) error(INCOMPATIBLE_TYPES);

  /*
  --  Error synchronization:  Should be OF
  */
  synchronize(follow_expr_list, case_label_start_list, NULL);
  if_token_get_else_error(OF, MISSING_OF);

  /*
  --  Loop to process CASE branches.
  */
  another_branch = token_in(case_label_start_list);
  while (another_branch) {
    if (token_in(case_label_start_list)) case_branch(expr_tp);

    if (token == SEMICOLON) {
      get_token();
      another_branch = TRUE;
    } else if (token_in(case_label_start_list)) {
      error(MISSING_SEMICOLON);
      another_branch = TRUE;
    } else another_branch = FALSE;
  }

  if_token_get_else_error(END, MISSING_END);
}

void case_branch(TYPE_STRUCT_PTR expr_tp) {
  BOOLEAN another_label;
  TYPE_STRUCT_PTR label_tp;
  TYPE_STRUCT_PTR case_label();

  /*
  --  <case-label-list>
  */
  do {
    label_tp = case_label();
    if (expr_tp != label_tp) error(INCOMPATIBLE_TYPES);

    get_token();
    if (token == COMMA) {
      get_token();
      if (token_in(case_label_start_list)) another_label = TRUE;
      else {
        error(MISSING_CONSTANT);
        another_label = FALSE;
      }
    } else another_label = FALSE;
  } while (another_label);

  /*
  --  Error synchronization:  Should be :
  */
  synchronize(follow_case_label_list, statement_start_list, NULL);
  if_token_get_else_error(COLON, MISSING_COLON);

  statement();
}

TYPE_STRUCT_PTR case_label() {
  TOKEN_CODE sign = PLUS; /* unary + or - sign */
  BOOLEAN saw_sign = FALSE; /* TRUE iff unary sign */
  TYPE_STRUCT_PTR label_tp;

  /*
  --  Unary + or - sign.
  */
  if ((token == PLUS) || (token == MINUS)) {
    sign = token;
    saw_sign = TRUE;
    get_token();
  }

  /*
  --  Numeric constant:  Integer type only.
  */
  if (token == NUMBER) {
    if (literal.type == REAL_LIT) error(INVALID_CONSTANT);
    return (integer_typep);
  }

  /*
  --  Identifier constant:  Integer, character, or enumeration
  --                        types only.
  */
  else if (token == IDENTIFIER) {
    SYMTAB_NODE_PTR idp;

    search_all_symtab(idp);

    if (idp == NULL) {
      error(UNDEFINED_IDENTIFIER);
      return ( & dummy_type);
    } else if (idp-> defn.key != CONST_DEFN) {
      error(NOT_A_CONSTANT_IDENTIFIER);
      return ( & dummy_type);
    } else if (idp-> typep == integer_typep)
      return (integer_typep);

    else if (idp-> typep == char_typep) {
      if (saw_sign) error(INVALID_CONSTANT);
      return (char_typep);
    } else if (idp-> typep-> form == ENUM_FORM) {
      if (saw_sign) error(INVALID_CONSTANT);
      return (idp-> typep);
    } else return ( & dummy_type);
  }

  /*
  --  String constant:  Character type only.
  */
  else if (token == STRING) {
    if (saw_sign) error(INVALID_CONSTANT);

    if (strlen(literal.value.string) == 1) return (char_typep);
    else {
      error(INVALID_CONSTANT);
      return ( & dummy_type);
    }
  } else {
    error(INVALID_CONSTANT);
    return ( & dummy_type);
  }
}

void compound_statement() {
    /*
        --  <stmt-list>
        */
    get_token();
    do {
      statement();
      while (token == SEMICOLON) get_token();
      if (token == END) break;

      /*
      --  Error synchronization:  Should be at the start of the
      --                          next statement.
      */
      synchronize(statement_start_list, NULL, NULL);
    } while (token_in(statement_start_list));

    if_token_get_else_error(END, MISSING_END);
}