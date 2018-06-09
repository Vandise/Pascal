#include "ch12/parser/standard.h"
#include "ch12/parser/routine.h"
#include "ch12/code/code.h"
#include "ch12/code/emitcode.h"

TYPE_STRUCT_PTR standard_routine_call(SYMTAB_NODE_PTR rtn_idp) {
    switch (rtn_idp->defn.info.routine.key) {

    case WRITE:
    case WRITELN:
      write_writeln(rtn_idp);
      return (NULL);

    default:
      error(UNIMPLEMENTED_FEATURE);
      exit(-UNIMPLEMENTED_FEATURE);
    }
}

void read_readln(SYMTAB_NODE_PTR rtn_idp) {
  TYPE_STRUCT_PTR actual_parm_tp; /* actual parm type */

  /*
  --  Parameters are optional for readln.
  */
  if (token == LPAREN) {
    /*
    --  <id-list>
    */
    do {
      get_token();

      /*
      --  Actual parms must be variables (but parse
      --  an expression anyway for error recovery).
      */
      if (token == IDENTIFIER) {
        SYMTAB_NODE_PTR idp;

        search_and_find_all_symtab(idp);
        actual_parm_tp = base_type(variable(idp,
          VARPARM_USE));

        if (actual_parm_tp->form != SCALAR_FORM)
          error(INCOMPATIBLE_TYPES);
      } else {
        actual_parm_tp = expression();
        error(INVALID_VAR_PARM);
      }

      /*
      --  Error synchronization:  Should be , or )
      */
      synchronize(follow_parm_list, statement_end_list, NULL);

    } while (token == COMMA);

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else if (rtn_idp->defn.info.routine.key == READ)
    error(WRONG_NUMBER_OF_PARMS);
}

void write_writeln(SYMTAB_NODE_PTR rtn_idp) {
    TYPE_STRUCT_PTR actual_parm_tp; /* actual parm type */
    TYPE_STRUCT_PTR field_width_tp, precision_tp;

    /*
    --  Parameters are optional for writeln.
    */
    if (token == LPAREN) {
      do {
        /*
        --  Value <expr>
        */
        get_token();
        actual_parm_tp = base_type(expression());

        /*
        --  Push the scalar value to be written onto the stack.
        --  A string value is already on the stack.
        */
        if (actual_parm_tp->form != ARRAY_FORM)
          emit_push_operand(actual_parm_tp);

        if ((actual_parm_tp->form != SCALAR_FORM) &&
          (actual_parm_tp != boolean_typep) &&
          ((actual_parm_tp->form != ARRAY_FORM) ||
            (actual_parm_tp->info.array.elmt_typep !=
              char_typep)))
          error(INVALID_EXPRESSION);

        /*
        --  Optional field width <expr>
        --  Push onto the stack.
        */
        if (token == COLON) {
          get_token();
          field_width_tp = base_type(expression());
          emit_1(PUSH, reg(AX));

          if (field_width_tp != integer_typep)
            error(INCOMPATIBLE_TYPES);

          /*
          --  Optional precision <expr>
          --  Push onto the stack if the value to be printed
          --  is of type real.
          */
          if (token == COLON) {
            get_token();
            precision_tp = base_type(expression());

            if (actual_parm_tp == real_typep)
              emit_1(PUSH, reg(AX));

            if (precision_tp != integer_typep)
              error(INCOMPATIBLE_TYPES);
          } else if (actual_parm_tp == real_typep) {
            emit_2(MOVE, reg(AX),
              integer_lit(DEFAULT_PRECISION));
            emit_1(PUSH, reg(AX));
          }
        } else {
          if (actual_parm_tp == integer_typep) {
            emit_2(MOVE, reg(AX),
              integer_lit(DEFAULT_NUMERIC_FIELD_WIDTH));
            emit_1(PUSH, reg(AX));
          } else if (actual_parm_tp == real_typep) {
            emit_2(MOVE, reg(AX),
              integer_lit(DEFAULT_NUMERIC_FIELD_WIDTH));
            emit_1(PUSH, reg(AX));
            emit_2(MOVE, reg(AX),
              integer_lit(DEFAULT_PRECISION));
            emit_1(PUSH, reg(AX));
          } else {
            emit_2(MOVE, reg(AX), integer_lit(0));
            emit_1(PUSH, reg(AX));
          }
        }

        if (actual_parm_tp == integer_typep) {
          emit_1(CALL, name_lit(WRITE_INTEGER));
          emit_2(ADD, reg(SP), integer_lit(4));
        } else if (actual_parm_tp == real_typep) {
          emit_1(CALL, name_lit(WRITE_REAL));
          emit_2(ADD, reg(SP), integer_lit(8));
        } else if (actual_parm_tp == boolean_typep) {
          emit_1(CALL, name_lit(WRITE_BOOLEAN));
          emit_2(ADD, reg(SP), integer_lit(4));
        } else if (actual_parm_tp == char_typep) {
          emit_1(CALL, name_lit(WRITE_CHAR));
          emit_2(ADD, reg(SP), integer_lit(4));
        } else /* string */ {
          /*
          --  Push the string length onto the stack.
          */
          emit_2(MOVE, reg(AX),
            integer_lit(actual_parm_tp->info.array
              .elmt_count));

          emit_1(PUSH, reg(AX));
          emit_1(CALL, name_lit(WRITE_STRING));
          emit_2(ADD, reg(SP), integer_lit(6));
        }

        /*
        --  Error synchronization:  Should be , or )
        */
        synchronize(follow_parm_list, statement_end_list, NULL);

      } while (token == COMMA);

      if_token_get_else_error(RPAREN, MISSING_RPAREN);
    } else if (rtn_idp->defn.info.routine.key == WRITE)
      error(WRONG_NUMBER_OF_PARMS);

    if (rtn_idp->defn.info.routine.key == WRITELN)
      emit_1(CALL, name_lit(WRITE_LINE));
}

TYPE_STRUCT_PTR eof_eoln(SYMTAB_NODE_PTR rtn_idp) {
  if (token == LPAREN) {
    error(WRONG_NUMBER_OF_PARMS);
    actual_parm_list(rtn_idp, FALSE);
  }

  return (boolean_typep);
}

TYPE_STRUCT_PTR abs_sqr() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */
  TYPE_STRUCT_PTR result_tp; /* result type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if ((parm_tp != integer_typep) && (parm_tp != real_typep)) {
      error(INCOMPATIBLE_TYPES);
      result_tp = real_typep;
    } else result_tp = parm_tp;

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (result_tp);
}

TYPE_STRUCT_PTR arctan_cos_exp_ln_sin_sqrt() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if ((parm_tp != integer_typep) && (parm_tp != real_typep))
      error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (real_typep);
}

TYPE_STRUCT_PTR pred_succ() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */
  TYPE_STRUCT_PTR result_tp; /* result type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if ((parm_tp != integer_typep) &&
      (parm_tp->form != ENUM_FORM)) {
      error(INCOMPATIBLE_TYPES);
      result_tp = integer_typep;
    } else result_tp = parm_tp;

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (result_tp);
}

TYPE_STRUCT_PTR chr() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if (parm_tp != integer_typep) error(INCOMPATIBLE_TYPES);
    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (char_typep);
}

TYPE_STRUCT_PTR odd() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if (parm_tp != integer_typep) error(INCOMPATIBLE_TYPES);
    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (boolean_typep);
}

TYPE_STRUCT_PTR ord() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if ((parm_tp->form != ENUM_FORM) && /* 2/9/91 */
      (parm_tp != char_typep))
      error(INCOMPATIBLE_TYPES);
    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (integer_typep);
}

TYPE_STRUCT_PTR round_trunc() {
  TYPE_STRUCT_PTR parm_tp; /* actual parameter type */

  if (token == LPAREN) {
    get_token();
    parm_tp = base_type(expression());

    if (parm_tp != real_typep) error(INCOMPATIBLE_TYPES);
    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else error(WRONG_NUMBER_OF_PARMS);

  return (integer_typep);
}