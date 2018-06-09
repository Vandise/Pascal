#ifndef scanner_h
#define scanner_h

#include "ch12/misc/common.h"
#include "ch12/error/error.h"

/*--------------------------------------------------------------*/
/*  Token codes							*/
/*--------------------------------------------------------------*/

typedef enum {
    NO_TOKEN, IDENTIFIER, NUMBER, STRING,
    UPARROW, STAR, LPAREN, RPAREN, MINUS, PLUS, EQUAL,
    LBRACKET, RBRACKET, COLON, SEMICOLON, LT, GT, COMMA, PERIOD,
    SLASH, COLONEQUAL, LE, GE, NE, DOTDOT, END_OF_FILE, ERROR,
    AND, ARRAY, BEGIN, CASE, CONST, DIV, DO, DOWNTO, ELSE, END,
    FFILE, FOR, FUNCTION, GOTO, IF, IN, LABEL, MOD, NIL, NOT,
    OF, OR, PACKED, PROCEDURE, PROGRAM, RECORD, REPEAT, SET,
    THEN, TO, TYPE, UNTIL, VAR, WHILE, WITH,
} TOKEN_CODE;

/*--------------------------------------------------------------*/
/*  Literal structure						*/
/*--------------------------------------------------------------*/

typedef enum {
    INTEGER_LIT, REAL_LIT, STRING_LIT,
} LITERAL_TYPE;

typedef struct {
    LITERAL_TYPE type;
    union {
	int   integer;
	float real;
	char  string[MAX_SOURCE_LINE_LENGTH];
    } value;
} LITERAL;

extern FILE * code_file;

void init_scanner(char *name);
void quit_scanner();
void get_char();
void skip_comment();
void skip_blanks();
void get_token();
void get_word();
void get_number();
void get_string();
void get_special();
void downshift_word();
void accumulate_value(float *valuep, ERROR_CODE error_code);
BOOLEAN is_reserved_word();
void open_source_file(char *name);
void close_source_file();
BOOLEAN get_source_line();
void print_line(char line[]);
void init_page_header(char *name);
void print_page_header();
BOOLEAN token_in(TOKEN_CODE token_list[]);
void synchronize(TOKEN_CODE token_list1[], TOKEN_CODE token_list2[], TOKEN_CODE token_list3[]);

#endif





