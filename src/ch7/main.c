
#include <stdio.h>
#include <stdlib.h>
#include "ch7/misc/common.h"
#include "ch7/error/error.h"
#include "ch7/scanner/scanner.h"
#include "ch7/parser/parser.h"

int main(int argc, char * * argv) {
  /*
  --  Initialize the scanner.
  */
  init_scanner(argv[1]);

  /*
  --  Process a program.
  */
  get_token();
  program();
  return 0;
}