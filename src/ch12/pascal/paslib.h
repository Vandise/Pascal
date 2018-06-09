#ifndef paslib_h
#define paslib_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ch12/misc/common.h"

union {
    float real;
    long  dword;
} value;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

BOOLEAN eof_flag  = FALSE;
BOOLEAN eoln_flag = FALSE;

int read_integer();
long
read_real();
char
read_char();
void read_line();
void write_integer(int field_width, int i);
void write_real(int precision, int field_width, long i);
void write_boolean(int field_width, int b);
void write_char(int field_width, int ch);
void write_string(int length, int field_width, char *value);
void write_line();
BOOLEAN
std_end_of_file();
BOOLEAN
std_end_of_line();
long
float_negate(long i);
long
float_add(long j, long i);
long
float_subtract(long j, long i);
long
float_multiply(long j, long i);
long
float_divide(long j, long i);
long
float_convert(int i);
int float_compare(long j, long i);
long
std_abs(long i);
long
std_arctan(long i);
long
std_cos(long i);
long
std_exp(long i);
long
std_ln(long i);
long
std_sin(long i);
long
std_sqrt(long i);
int
std_round(long i);
int
std_trunc(long i);

#endif
