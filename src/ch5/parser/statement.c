#include "ch5/parser/statement.h"

TOKEN_CODE follow_expr_list[]      = {OF, SEMICOLON, 0};

TOKEN_CODE case_label_start_list[] = {IDENTIFIER, NUMBER, PLUS,
				      MINUS, STRING, 0};

TOKEN_CODE follow_case_label_list[] = {COLON, SEMICOLON, 0};

void statement() {
  switch (token) {
    case IDENTIFIER:    assignment_statement(); break;
    case REPEAT:        repeat_statement();     break;
    case WHILE:         while_statement();      break;
    case IF:            if_statement();         break;
    case FOR:           for_statement();        break;
    case CASE:          case_statement();       break;
    case BEGIN:         compound_statement();   break;
  }

  synchronize(statement_end_list, statement_start_list, NULL);

  if (token_in(statement_start_list)) {
    error(MISSING_SEMICOLON);
  }
}

void assignment_statement() {
  get_token();
  if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);
  expression();
}

void repeat_statement() {
  get_token();

  do {
    statement();
    while (token == SEMICOLON) {
      get_token();
    }
  } while (token_in(statement_start_list));

  if_token_get_else_error(UNTIL, MISSING_UNTIL);
  expression();
}

void while_statement() {
  get_token();
  expression();

  if_token_get_else_error(DO, MISSING_DO);
  statement();
}

void if_statement() {
  get_token();
  expression();

  if_token_get_else_error(THEN, MISSING_THEN);
  statement();

  if (token == ELSE) {
    get_token();
    statement();
  }
}

void for_statement() {
  get_token();
  if_token_get_else_error(IDENTIFIER, MISSING_IDENTIFIER);

  if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);
  expression();

  if ((token == TO) || (token == DOWNTO)) get_token();
  else error(MISSING_TO_OR_DOWNTO);

  expression();
  if_token_get_else_error(DO, MISSING_DO);

  statement();
}

void case_statement() {
  BOOLEAN another_branch;

  get_token();
  expression();

  synchronize(follow_expr_list, case_label_start_list, NULL);
  if_token_get_else_error(OF, MISSING_OF);

  another_branch = token_in(case_label_start_list);
  while (another_branch) {
    if (token_in(case_label_start_list)) {
      case_branch();
    }

    if (token == SEMICOLON) {
      get_token();
      another_branch = TRUE;
    } else if (token_in(case_label_start_list)) {
      error(MISSING_SEMICOLON);
      another_branch = TRUE;
    } else {
      another_branch = FALSE;
    }
  }

  if_token_get_else_error(END, MISSING_END);
}

void case_branch() {
  BOOLEAN another_label;

  do {
    case_label();

    get_token();
    if (token == COMMA) {
      get_token();
      if (token_in(case_label_start_list)) {
        another_label = TRUE;
      } else {
        error(MISSING_CONSTANT);
        another_label = FALSE;
      }
    } else {
      another_label = FALSE;
    }
  } while (another_label);

  synchronize(follow_case_label_list, statement_start_list, NULL);
  if_token_get_else_error(COLON, MISSING_COLON);

  statement();
}

void case_label() {
  TOKEN_CODE sign     = PLUS;         /* unary + or - sign */
  BOOLEAN    saw_sign = FALSE;        /* TRUE iff unary sign */

  if ((token == PLUS) || (token == MINUS)) {
    sign     = token;
    saw_sign = TRUE;
    get_token();
  }


  if ((token == NUMBER) || (token == IDENTIFIER)) {
    return;
  } else if (token == STRING) {
    if (saw_sign) {
      error(INVALID_CONSTANT);
    }
    if (strlen(literal.value.string) != 1) {
      error(INVALID_CONSTANT);
    }
  }
}

void compound_statement() {
  get_token();
  do {
    statement();

    while (token == SEMICOLON) {
      get_token();
    }

    if (token == END) {
      break;
    }
    synchronize(statement_start_list, NULL, NULL);
  } while (token_in(statement_start_list));

  if_token_get_else_error(END, MISSING_END);
}