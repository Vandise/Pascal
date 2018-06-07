#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <stdlib.h>
#include "ch5/misc/common.h"
#include "ch5/error/error.h"
#include "ch5/scanner/scanner.h"
#include "ch5/parser/parser.h"

extern TOKEN_CODE token;
extern char       word_string[];

void expression();
void simple_expression();
void term();
void factor();

#endif