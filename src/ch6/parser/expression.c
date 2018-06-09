#include "ch6/parser/expression.h"

TOKEN_CODE rel_op_list[] = {LT, LE, EQUAL, NE, GE, GT, 0};
TOKEN_CODE add_op_list[] = {PLUS, MINUS, OR, 0};
TOKEN_CODE mult_op_list[] = {STAR, SLASH, DIV, MOD, AND, 0};

TYPE_STRUCT_PTR expression() {
  TOKEN_CODE op; /* an operator token */
  TYPE_STRUCT_PTR result_tp, tp2;

  result_tp = simple_expression(); /* first simple expr */

  /*
  --	If there is a relational operator, remember it and
  --	process the second simple expression.
  */
  if (token_in(rel_op_list)) {
    op = token; /* remember operator */
    result_tp = base_type(result_tp);

    get_token();
    tp2 = base_type(simple_expression()); /* 2nd simple expr */

    check_rel_op_types(result_tp, tp2);
    result_tp = boolean_typep;
  }

  return (result_tp);
}

TYPE_STRUCT_PTR simple_expression() {
  TOKEN_CODE op; /* an operator token */
  BOOLEAN saw_unary_op = FALSE; /* TRUE iff unary operator */
  TOKEN_CODE unary_op = PLUS; /* a unary operator token */
  TYPE_STRUCT_PTR result_tp, tp2;

  /*
  --	If there is a unary + or -, remember it.
  */
  if ((token == PLUS) || (token == MINUS)) {
    unary_op = token;
    saw_unary_op = TRUE;
    get_token();
  }

  result_tp = term(); /* first term */

  /*
  --  If there was a unary operator, check that the term
  --  is integer or real.  Negate the top of stack if it
  --  was a unary - either with the NEG instruction or by
  --  calling FLOAT_NEGATE.
  */
  if (saw_unary_op &&
    (base_type(result_tp) != integer_typep) &&
    (result_tp != real_typep)) error(INCOMPATIBLE_TYPES);

  /*
  --  Loop to process subsequent terms separated by operators.
  */
  while (token_in(add_op_list)) {
    op = token; /* remember operator */
    result_tp = base_type(result_tp);

    get_token();
    tp2 = base_type(term()); /* subsequent term */

    switch (op) {

    case PLUS:
    case MINUS:
      {
        /*
        --  integer <op> integer => integer
        */
        if (integer_operands(result_tp, tp2))
          result_tp = integer_typep;

        /*
        --  Both operands are real, or one is real and the
        --  other is integer.  The result is real.
        */
        else if (real_operands(result_tp, tp2))
          result_tp = real_typep;

        else {
          error(INCOMPATIBLE_TYPES);
          result_tp = & dummy_type;
        }

        break;
      }

    case OR:
      {
        /*
        --  boolean OR boolean => boolean
        */
        if (!boolean_operands(result_tp, tp2))
          error(INCOMPATIBLE_TYPES);

        result_tp = boolean_typep;
        break;
      }
    }
  }

  return (result_tp);
}

TYPE_STRUCT_PTR term() {
  TOKEN_CODE op; /* an operator token */
  TYPE_STRUCT_PTR result_tp, tp2;

  result_tp = factor(); /* first factor */

  /*
  --	Loop to process subsequent factors
  --	separated by operators.
  */
  while (token_in(mult_op_list)) {
    op = token; /* remember operator */
    result_tp = base_type(result_tp);

    get_token();
    tp2 = base_type(factor()); /* subsequent factor */

    switch (op) {

    case STAR:
      {
        /*
        --  Both operands are integer.
        */
        if (integer_operands(result_tp, tp2))
          result_tp = integer_typep;

        /*
        --  Both operands are real, or one is real and the
        --  other is integer.  The result is real.
        */
        else if (real_operands(result_tp, tp2))
          result_tp = real_typep;

        else {
          error(INCOMPATIBLE_TYPES);
          result_tp = & dummy_type;
        }

        break;
      }

    case SLASH:
      {
        /*
        --  Both operands are real, or both are integer, or
        --  one is real and the other is integer.  The result
        --  is real.
        */
        if ((!real_operands(result_tp, tp2)) &&
          (!integer_operands(result_tp, tp2)))
          error(INCOMPATIBLE_TYPES);

        result_tp = real_typep;
        break;
      }

    case DIV:
    case MOD:
      {
        /*
        --  integer <op> integer => integer
        */
        if (!integer_operands(result_tp, tp2))
          error(INCOMPATIBLE_TYPES);

        result_tp = integer_typep;
        break;
      }

    case AND:
      {
        /*
        --  boolean AND boolean => boolean
        */
        if (!boolean_operands(result_tp, tp2))
          error(INCOMPATIBLE_TYPES);

        result_tp = boolean_typep;
        break;
      }
    }
  }

  return (result_tp);
}

TYPE_STRUCT_PTR factor() {
  TYPE_STRUCT_PTR tp;

  switch (token) {

  case IDENTIFIER:
    {
      SYMTAB_NODE_PTR idp;

      search_and_find_all_symtab(idp);

      if (idp->defn.key == CONST_DEFN) {
        get_token();
        tp = idp->typep;
      } else tp = variable(idp, EXPR_USE);

      break;
    }

  case NUMBER:
    tp = literal.type == INTEGER_LIT ? integer_typep : real_typep;
    get_token();
    break;

  case STRING:
    {
      int length = strlen(literal.value.string);

      tp = length == 1 ? char_typep : make_string_typep(length);
      get_token();
      break;
    }

  case NOT:
    get_token();
    tp = factor();
    break;

  case LPAREN:
    get_token();
    tp = expression();

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
    break;

  default:
    error(INVALID_EXPRESSION);
    tp = & dummy_type;
    break;
  }

  return (tp);
}

TYPE_STRUCT_PTR variable(SYMTAB_NODE_PTR var_idp, USE use) {
  TYPE_STRUCT_PTR tp = var_idp->typep;
  DEFN_KEY defn_key = var_idp->defn.key;
  TYPE_STRUCT_PTR array_subscript_list();
  TYPE_STRUCT_PTR record_field();

  /*
  --  Check the variable's definition.
  */
  switch (defn_key) {
  case VAR_DEFN:
  case VALPARM_DEFN:
  case VARPARM_DEFN:
  case FUNC_DEFN:
  case UNDEFINED:
    break; /* OK */

  default:
    { /* error */
      tp = & dummy_type;
      error(INVALID_IDENTIFIER_USAGE);
    }
  }

  get_token();

  /*
  --  Subscripts and/or field designators?
  */
  while ((token == LBRACKET) || (token == PERIOD)) {
    tp = token == LBRACKET ? array_subscript_list(tp) : record_field(tp);
  }

  return (tp);
}

TYPE_STRUCT_PTR array_subscript_list(TYPE_STRUCT_PTR tp) {
  TYPE_STRUCT_PTR index_tp, elmt_tp, ss_tp;
  extern TOKEN_CODE statement_end_list[];

  /*
  --  Loop to process a subscript list.
  */
  do {
    if (tp->form == ARRAY_FORM) {
      index_tp = tp->info.array.index_typep;
      elmt_tp = tp->info.array.elmt_typep;

      get_token();
      ss_tp = expression();

      /*
      --  The subscript expression must be assignment type
      --  compatible with the corresponding subscript type.
      */
      if (!is_assign_type_compatible(index_tp, ss_tp))
        error(INCOMPATIBLE_TYPES);

      tp = elmt_tp;
    } else {
      error(TOO_MANY_SUBSCRIPTS);
      while ((token != RBRACKET) &&
        (!token_in(statement_end_list)))
        get_token();
    }
  } while (token == COMMA);

  if_token_get_else_error(RBRACKET, MISSING_RBRACKET);
  return (tp);
}

TYPE_STRUCT_PTR record_field(TYPE_STRUCT_PTR tp) {
  SYMTAB_NODE_PTR field_idp;

  get_token();

  if ((token == IDENTIFIER) && (tp->form == RECORD_FORM)) {
    search_this_symtab(field_idp,
      tp->info.record.field_symtab);

    get_token();

    if (field_idp != NULL) return (field_idp->typep);
    else {
      error(INVALID_FIELD);
      return ( & dummy_type);
    }
  } else {
    get_token();
    error(INVALID_FIELD);
    return ( & dummy_type);
  }
}

void check_rel_op_types(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2) {
  /*
  --  Two identical scalar or enumeration types.
  */
  if ((tp1 == tp2) && ((tp1->form == SCALAR_FORM) || (tp1->form == ENUM_FORM)))
    return;

  /*
  --  One integer and one real.
  */
  if (((tp1 == integer_typep) && (tp2 == real_typep)) || ((tp2 == integer_typep) && (tp1 == real_typep))) return;

  /*
  --  Two strings of the same length.
  */
  if ((tp1->form == ARRAY_FORM) &&
    (tp2->form == ARRAY_FORM) &&
    (tp1->info.array.elmt_typep == char_typep) &&
    (tp2->info.array.elmt_typep == char_typep) &&
    (tp1->info.array.elmt_count ==
      tp2->info.array.elmt_count)) return;

  error(INCOMPATIBLE_TYPES);
}

BOOLEAN is_assign_type_compatible(TYPE_STRUCT_PTR tp1, TYPE_STRUCT_PTR tp2) {
  tp1 = base_type(tp1);
  tp2 = base_type(tp2);

  if (tp1 == tp2) return (TRUE);

  /*
  --  real := integer
  */
  if ((tp1 == real_typep) && (tp2 == integer_typep)) return (TRUE);

  /*
  --  string1 := string2 of the same length
  */
  if ((tp1->form == ARRAY_FORM) &&
    (tp2->form == ARRAY_FORM) &&
    (tp1->info.array.elmt_typep == char_typep) &&
    (tp2->info.array.elmt_typep == char_typep) &&
    (tp1->info.array.elmt_count ==
      tp2->info.array.elmt_count)) return (TRUE);

  return (FALSE);
}

TYPE_STRUCT_PTR base_type(TYPE_STRUCT_PTR tp) {
  return((tp->form == SUBRANGE_FORM)
       ? tp->info.subrange.range_typep
       : tp);
}