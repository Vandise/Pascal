
#include "ch7/parser/standard.h"
#include "ch7/parser/routine.h"

TYPE_STRUCT_PTR standard_routine_call(SYMTAB_NODE_PTR rtn_idp) {
  switch (rtn_idp->defn.info.routine.key) {

  case READ:
  case READLN:
    read_readln(rtn_idp);
    return (NULL);

  case WRITE:
  case WRITELN:
    write_writeln(rtn_idp);
    return (NULL);

  case EOFF:
  case EOLN:
    return (eof_eoln(rtn_idp));

  case ABS:
  case SQR:
    return (abs_sqr());

  case ARCTAN:
  case COS:
  case EXP:
  case LN:
  case SIN:
  case SQRT:
    return (arctan_cos_exp_ln_sin_sqrt());

  case PRED:
  case SUCC:
    return (pred_succ());

  case CHR:
    return (chr());
  case ODD:
    return (odd());
  case ORD:
    return (ord());

  case ROUND:
  case TRUNC:
    return (round_trunc());
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

      if ((actual_parm_tp->form != SCALAR_FORM) &&
        (actual_parm_tp != boolean_typep) &&
        ((actual_parm_tp->form != ARRAY_FORM) ||
          (actual_parm_tp->info.array.elmt_typep !=
            char_typep)))
        error(INVALID_EXPRESSION);

      /*
      --  Optional field width <expr>
      */
      if (token == COLON) {
        get_token();
        field_width_tp = base_type(expression());

        if (field_width_tp != integer_typep)
          error(INCOMPATIBLE_TYPES);

        /*
        --  Optional precision <expr>
        */
        if (token == COLON) {
          get_token();
          precision_tp = base_type(expression());

          if (precision_tp != integer_typep)
            error(INCOMPATIBLE_TYPES);
        }
      }

      /*
      --  Error synchronization:  Should be , or )
      */
      synchronize(follow_parm_list, statement_end_list, NULL);

    } while (token == COMMA);

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
  } else if (rtn_idp->defn.info.routine.key == WRITE)
    error(WRONG_NUMBER_OF_PARMS);
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