#include "ch5/parser/expression.h"

TOKEN_CODE rel_op_list[] = {LT, LE, EQUAL, NE, GE, GT, 0};
TOKEN_CODE add_op_list[] = {PLUS, MINUS, OR, 0};
TOKEN_CODE mult_op_list[] = {STAR, SLASH, DIV, MOD, AND, 0};

void expression() {
  TOKEN_CODE op;              /* an operator token */

  simple_expression();        /* first simple expr */

  if (token_in(rel_op_list)) {
    op = token;             /* remember operator */
    get_token();
    simple_expression();    /* 2nd simple expr */
  }
}

void simple_expression() {
  TOKEN_CODE op;                      /* an operator token */
  TOKEN_CODE unary_op = PLUS;		/* a unary operator token */

  if ((token == PLUS) || (token == MINUS)) {
    unary_op = token;
    get_token();
  }

  term();             /* first term */

  while (token_in(add_op_list)) {
    op = token;     /* remember operator */
    get_token();
    term();         /* subsequent term */
  }
}

void term() {
  TOKEN_CODE op;      /* an operator token */

  factor();           /* first factor */

  while (token_in(mult_op_list)) {
    op = token;     /* remember operator */
    get_token();
    factor();       /* subsequent factor */
  }
}

void factor() {
  switch (token) {
    case IDENTIFIER:
      get_token();
      break;
    case NUMBER:
      get_token();
      break;
    case STRING:
      get_token();
      break;
    case NOT:
      get_token();
      factor();
      break;
    case LPAREN:
      get_token();
      expression();
      if_token_get_else_error(RPAREN, MISSING_RPAREN);
      break;
    default:
      error(INVALID_EXPRESSION);
      break;
  }
}