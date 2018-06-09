#include <stdio.h>
#include <stdlib.h>
#include "ch12/misc/common.h"
#include "ch12/error/error.h"
#include "ch12/scanner/scanner.h"
#include "ch12/parser/parser.h"
#include "ch12/parser/routine.h"

FILE *code_file;

int main(int argc, char * * argv) {
  /*
  --  Open the code file.  If no code file name was given,
  --  use the standard output file.
  */
  code_file = (argc == 3) ? fopen(argv[2], "w")
		    : stdout;

  /*
  --  Initialize the scanner.
  */
  init_scanner(argv[1]);

  /*
  --  Process a program.
  */
  get_token();
  program();
}