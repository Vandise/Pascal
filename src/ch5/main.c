#include <stdio.h>
#include <stdlib.h>
#include "ch5/misc/common.h"
#include "ch5/error/error.h"
#include "ch5/scanner/scanner.h"
#include "ch5/parser/statement.h"

extern TOKEN_CODE token;
extern int        line_number;
extern int        error_count;

char buffer[MAX_PRINT_LINE_LENGTH];

int main(int argc, char **argv) {
  init_scanner(argv[1]);

  get_token();
  statement();

  while (token != END_OF_FILE) {
    error(UNEXPECTED_TOKEN);
    get_token();
  }

  quit_scanner();

  print_line("\n");
  print_line("\n");
  sprintf(buffer, "%20d Source lines.\n", line_number);
  print_line(buffer);
  sprintf(buffer, "%20d Source errors.\n", error_count);
  print_line(buffer);

  if (error_count == 0) exit(0);
  else                  exit(-SYNTAX_ERROR);

  return 0;
}
