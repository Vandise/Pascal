#include <stdio.h>
#include <stdlib.h>
#include "ch7/misc/common.h"
#include "ch7/error/error.h"
#include "ch7/scanner/scanner.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern char	*tokenp;
extern BOOLEAN	print_flag;
extern char	source_buffer[];
extern char     *bufferp;

/*--------------------------------------------------------------*/
/*  Error messages	Keyed to enumeration type ERROR_CODE	*/
/*			in file error.h.			*/
/*--------------------------------------------------------------*/

char *error_messages[] = {
    "No error",
    "Syntax error",
    "Too many syntax errors",
    "Failed to open source file",
    "Unexpected end of file",
    "Invalid number",
    "Invalid fraction",
    "Invalid exponent",
    "Too many digits",
    "Real literal out of range",
    "Integer literal out of range",
    "Missing right parenthesis",
    "Invalid expression",
    "Invalid assignment statement",
    "Missing identifier",
    "Missing := ",
    "Undefined identifier",
    "Stack overflow",
    "Invalid statement",
    "Unexpected token",
    "Missing ; ",
    "Missing DO",
    "Missing UNTIL",
    "Missing THEN",
    "Invalid FOR control variable",
    "Missing OF",
    "Invalid constant",
    "Missing constant",
    "Missing : ",
    "Missing END",
    "Missing TO or DOWNTO",
    "Redefined identifier",
    "Missing = ",
    "Invalid type",
    "Not a type identifier",
    "Invalid subrangetype",
    "Not a constant identifier",
    "Missing .. ",
    "Incompatible types",
    "Invalid assignment target",
    "Invalid identifier usage",
    "Incompatible assignment",
    "Min limit greater than max limit",
    "Missing [ ",
    "Missing ] ",
    "Invalid index type",
    "Missing BEGIN",
    "Missing period",
    "Too many subscripts",
    "Invalid field",
    "Nesting too deep",
    "Missing PROGRAM",
    "Already specified in FORWARD",
    "Wrong number of actual parameters",
    "Invalid VAR parameter",
    "Not a record variable",
    "Missing variable",
    "Code segment overflow",
    "Unimplemented feature",
};

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

int error_count = 0;    /* number of syntax errors */

void error(ERROR_CODE code) {
  extern int buffer_offset;
  char message_buffer[MAX_PRINT_LINE_LENGTH];
  char *message = error_messages[code];
  int  offset   = buffer_offset - 2;


  if (print_flag) {
    offset += 8;
  }

  sprintf(message_buffer, "%*s^\n", offset, " ");

  if (print_flag) {
    print_line(message_buffer);
  } else {
    printf("%s", message_buffer);
  }

  sprintf(message_buffer, " *** ERROR: %s.\n", message);

  if (print_flag) {
    print_line(message_buffer);
  } else {
    printf("%s", message_buffer);
  }

  *tokenp = '\0';
  ++error_count;

  if (error_count > MAX_SYNTAX_ERRORS) {
    sprintf(message_buffer, "Too many syntax errors.  Aborted.\n");
    if (print_flag) {
      print_line(message_buffer);
    } else {
      printf("%s", message_buffer);
    }
    exit(-TOO_MANY_SYNTAX_ERRORS);
  }
}

