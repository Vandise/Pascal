#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "ch6/scanner/scanner.h"

#define EOF_CHAR        '\x7f'
#define TAB_SIZE        8

#define MAX_INTEGER     32767
#define MAX_DIGIT_COUNT 20
#define MAX_EXPONENT    37

#define MIN_RESERVED_WORD_LENGTH    2
#define MAX_RESERVED_WORD_LENGTH    9

/*--------------------------------------------------------------*/
/*  Character codes						*/
/*--------------------------------------------------------------*/

typedef enum {
    LETTER, DIGIT, QUOTE, SPECIAL, EOF_CODE,
} CHAR_CODE;

/*--------------------------------------------------------------*/
/*  Reserved word tables					*/
/*--------------------------------------------------------------*/

typedef struct {
    char       *string;
    TOKEN_CODE token_code;
} RW_STRUCT;

RW_STRUCT rw_2[] = {
    {"do", DO}, {"if", IF}, {"in", IN}, {"of", OF}, {"or", OR},
    {"to", TO}, {NULL, 0 },
};

RW_STRUCT rw_3[] = {
    {"and", AND}, {"div", DIV}, {"end", END}, {"for", FOR},
    {"mod", MOD}, {"nil", NIL}, {"not", NOT}, {"set", SET},
    {"var", VAR}, {NULL , 0  },
};

RW_STRUCT rw_4[] = {
    {"case", CASE}, {"else", ELSE}, {"file", FFILE},
    {"goto", GOTO}, {"then", THEN}, {"type", TYPE},
    {"with", WITH}, {NULL  , 0   },
};

RW_STRUCT rw_5[] = {
    {"array", ARRAY}, {"begin", BEGIN}, {"const", CONST},
    {"label", LABEL}, {"until", UNTIL}, {"while", WHILE},
    {NULL   , 0    },
};

RW_STRUCT rw_6[] = {
    {"downto", DOWNTO}, {"packed", PACKED}, {"record", RECORD},
    {"repeat", REPEAT}, {NULL    , 0     },
};

RW_STRUCT rw_7[] = {
    {"program", PROGRAM}, {NULL, 0},
};

RW_STRUCT rw_8[] = {
    {"function", FUNCTION}, {NULL, 0},
};

RW_STRUCT rw_9[] = {
    {"procedure", PROCEDURE}, {NULL, 0},
};

RW_STRUCT *rw_table[] = {
    NULL, NULL, rw_2, rw_3, rw_4, rw_5, rw_6, rw_7, rw_8, rw_9,
};

/*--------------------------------------------------------------*/
/*  Token lists                                                 */
/*--------------------------------------------------------------*/

TOKEN_CODE statement_start_list[]   = {BEGIN, CASE, FOR, IF, REPEAT,
				       WHILE, IDENTIFIER, 0};

TOKEN_CODE statement_end_list[]     = {SEMICOLON, END, ELSE, UNTIL,
				       END_OF_FILE, 0};

TOKEN_CODE declaration_start_list[] = {CONST, TYPE, VAR, PROCEDURE,
				       FUNCTION, 0};

/*--------------------------------------------------------------*/
/*  Globals							*/
/*--------------------------------------------------------------*/


char	    ch;			/* current input character */
TOKEN_CODE  token;		/* code of current token */
LITERAL     literal;		/* value of literal */
int         buffer_offset;      /* char offset into source buffer */
int         level = 0;          /* current nesting level */
int	    line_number = 0;	/* current line number */
BOOLEAN     print_flag = TRUE;  /* TRUE to print source lines */

char source_buffer[MAX_SOURCE_LINE_LENGTH]; /* source file buffer */
char token_string[MAX_TOKEN_STRING_LENGTH]; /* token string */
char word_string[MAX_TOKEN_STRING_LENGTH];  /* downshifted */
char *bufferp = source_buffer;              /* source buffer ptr */
char *tokenp  = token_string;               /* token string ptr */

int     digit_count;            /* total no. of digits in number */
BOOLEAN count_error;            /* too many digits in number? */

int page_number = 0;
int line_count  = MAX_LINES_PER_PAGE;   /* no. lines on current pg */

char source_name[MAX_FILE_NAME_LENGTH]; /* name of source file */
char date[DATE_STRING_LENGTH];          /* current date and time */

FILE *source_file;

CHAR_CODE char_table[256];

/*--------------------------------------------------------------*/
/*  char_code           Return the character code of ch.        */
/*--------------------------------------------------------------*/

#define char_code(ch)   char_table[ch]

void init_scanner(char *name) {
  int ch;

  for (ch = 0;   ch < 256;  ++ch) char_table[ch] = SPECIAL;
  for (ch = '0'; ch <= '9'; ++ch) char_table[ch] = DIGIT;
  for (ch = 'A'; ch <= 'Z'; ++ch) char_table[ch] = LETTER;
  for (ch = 'a'; ch <= 'z'; ++ch) char_table[ch] = LETTER;
  char_table['\''] = QUOTE;
  char_table[EOF_CHAR] = EOF_CODE;

  init_page_header(name);
  open_source_file(name);
}

void quit_scanner() {
  close_source_file();
}

void get_char() {
  BOOLEAN get_source_line();

  if (*bufferp == '\0') {
    if (! get_source_line()) {
      ch = EOF_CHAR;
      return;
    }

    bufferp = source_buffer;
    buffer_offset = 0;
  }

  ch = *bufferp++;

  switch (ch) {
    case '\t':
      buffer_offset += TAB_SIZE - buffer_offset % TAB_SIZE;
      ch = ' ';
      break;
    case '\n':
      ++buffer_offset;
      ch = ' ';
      break;
    case '{':
      ++buffer_offset;
      skip_comment();
      ch = ' ';
      break;
      default:
        ++buffer_offset;
  }
}

void skip_comment() {
  do {
    get_char();
  } while ((ch != '}') && (ch != EOF_CHAR));
}

void skip_blanks() {
  while (ch == ' ') get_char();
}

void get_token() {
  skip_blanks();
  tokenp = token_string;

  switch (char_code(ch)) {
    case LETTER:    get_word();             break;
    case DIGIT:     get_number();           break;
    case QUOTE:     get_string();           break;
    case EOF_CODE:  token = END_OF_FILE;    break;
    default:        get_special();          break;
  }
}

void get_word() {
  BOOLEAN is_reserved_word();

  while ((char_code(ch) == LETTER) || (char_code(ch) == DIGIT)) {
    *tokenp++ = ch;
    get_char();
  }

  *tokenp = '\0';
  downshift_word();

  if (!is_reserved_word()) {
    token = IDENTIFIER;
  }
}

void get_number() {
  int     whole_count    = 0;     /* no. digits in whole part */
  int     decimal_offset = 0;     /* no. digits to move decimal */
  char    exponent_sign  = '+';
  int     exponent       = 0;     /* value of exponent */
  float   nvalue         = 0.0;   /* value of number */
  float   evalue         = 0.0;   /* value of exponent */
  BOOLEAN saw_dotdot     = FALSE; /* TRUE if encounter .. */

  digit_count  = 0;
  count_error  = FALSE;
  token        = NO_TOKEN;

  literal.type = INTEGER_LIT;     /* assume it's an integer */

  /*
  --  Extract the whole part of the number by accumulating
  --  the values of its digits into nvalue.  whole_count keeps
  --  track of the number of digits in this part.
  */
  accumulate_value(&nvalue, INVALID_NUMBER);
  if (token == ERROR) return;
  whole_count = digit_count;

  /*
  --  If the current character is a dot, then either we have a
  --  fraction part or we are seeing the first character of a ..
  --  token.  To find out, we must fetch the next character.
  */
  if (ch == '.') {
    get_char();

    if (ch == '.') {
      /*
      --  We have a .. token.  Back up bufferp so that the
      --  token can be extracted next.
      */
      saw_dotdot = TRUE;
      --bufferp;
    } else {
      literal.type = REAL_LIT;
      *tokenp++ = '.';

      /*
      --  We have a fraction part.  Accumulate it into nvalue.
      --  decimal_offset keeps track of how many digits to move
      --  the decimal point back.
      */
      accumulate_value(&nvalue, INVALID_FRACTION);
      if (token == ERROR) {
        return;
      }

      decimal_offset = whole_count - digit_count;
    }
  }

  /*
  --  Extract the exponent part, if any. There cannot be an
  --  exponent part if the .. token has been seen.
  */
  if (!saw_dotdot && ((ch == 'E') || (ch == 'e'))) {
    literal.type = REAL_LIT;
    *tokenp++ = ch;
    get_char();

    /*
    --  Fetch the exponent's sign, if any.
    */
    if ((ch == '+') || (ch == '-')) {
      *tokenp++ = exponent_sign = ch;
      get_char();
    }

    /*
    --  Extract the exponent.  Accumulate it into evalue.
    */
    accumulate_value(&evalue, INVALID_EXPONENT);
    if (token == ERROR) {
      return;
    }

    if (exponent_sign == '-') {
      evalue = -evalue;
    }
  }

  /*
  --  Were there too many digits?
  */
  if (count_error) {
    error(TOO_MANY_DIGITS);
    token = ERROR;
    return;
  }

  /*
  --  Adjust the number's value using
  --  decimal_offset and the exponent.
  */
  exponent = evalue + decimal_offset;
  if ((exponent + whole_count < -MAX_EXPONENT) || (exponent + whole_count >  MAX_EXPONENT)) {
    error(REAL_OUT_OF_RANGE);
    token = ERROR;
    return;
  }

  if (exponent != 0) {
    nvalue *= pow(10, exponent);
  }

  /*
  --  Set the literal's value.
  */
  if (literal.type == INTEGER_LIT) {
    if ((nvalue < -MAX_INTEGER) || (nvalue > MAX_INTEGER)) {
      error(INTEGER_OUT_OF_RANGE);
      token = ERROR;
      return;
    }
    literal.value.integer = nvalue;
  } else {
    literal.value.real = nvalue;
  }

  *tokenp = '\0';
  token   = NUMBER;
}

void get_string() {
  char *sp = literal.value.string;

  *tokenp++ = '\'';
  get_char();

  /*
  --   Extract the string.
  */
  while (ch != EOF_CHAR) {
    /*
    --  Two consecutive single quotes represent
    --  a single quote in the string.
    */
    if (ch == '\'') {
      *tokenp++ = ch;
      get_char();
      if (ch != '\'') break;
    }

    *tokenp++ = ch;
    *sp++     = ch;
    get_char();
  }

  *tokenp      = '\0';
  *sp          = '\0';
  token        = STRING;
  literal.type = STRING_LIT;
}

void get_special() {
  *tokenp++ = ch;
  switch (ch) {
    case '^':   token = UPARROW;    get_char();  break;
    case '*':   token = STAR;       get_char();  break;
    case '(':   token = LPAREN;     get_char();  break;
    case ')':   token = RPAREN;     get_char();  break;
    case '-':   token = MINUS;      get_char();  break;
    case '+':   token = PLUS;       get_char();  break;
    case '=':   token = EQUAL;      get_char();  break;
    case '[':   token = LBRACKET;	get_char();  break;
    case ']':   token = RBRACKET;	get_char();  break;
    case ';':   token = SEMICOLON;  get_char();  break;
    case ',':   token = COMMA;      get_char();  break;
    case '/':   token = SLASH;      get_char();  break;

    case ':':
      get_char();         /* : or := */
      if (ch == '=') {
        *tokenp++ = '=';
        token     = COLONEQUAL;
        get_char();
      } else {
        token = COLON;
      }
      break;
    case '<':
      get_char();         /* < or <= or <> */
      if (ch == '=') {
        *tokenp++ = '=';
        token     = LE;
        get_char();
      } else if (ch == '>') {
        *tokenp++ = '>';
        token     = NE;
        get_char();
      } else {
        token = LT;
      }
      break;
    case '>':
      get_char();         /* > or >= */
      if (ch == '=') {
        *tokenp++ = '=';
        token     = GE;
        get_char();
      } else {
        token = GT;
      }
      break;
    case '.':
      get_char();         /* . or .. */
      if (ch == '.') {
        *tokenp++ = '.';
        token     = DOTDOT;
        get_char();
      } else {
        token = PERIOD;
      }
      break;
    default:
      token = ERROR;
      get_char();
      break;
  }
  *tokenp = '\0';
}

void downshift_word() {
  int  offset = 'a' - 'A';
  char *wp    = word_string;
  char *tp    = token_string;

  do {
    *wp++ = (*tp >= 'A') && (*tp <= 'Z') ? *tp + offset : *tp;
    ++tp;
  } while (*tp != '\0');

  *wp = '\0';
}

void accumulate_value(float *valuep, ERROR_CODE error_code) {
  float value = *valuep;

  if (char_code(ch) != DIGIT) {
    error(error_code);
    token = ERROR;
    return;
  }

  do {
    *tokenp++ = ch;

    if (++digit_count <= MAX_DIGIT_COUNT) {
      value = 10*value + (ch - '0');
    } else {
      count_error = TRUE;
    }

    get_char();
  } while (char_code(ch) == DIGIT);

  *valuep = value;
}

BOOLEAN is_reserved_word() {
  int       word_length = strlen(word_string);
  RW_STRUCT *rwp;

  if ((word_length >= MIN_RESERVED_WORD_LENGTH) && (word_length <= MAX_RESERVED_WORD_LENGTH)) {
    for (rwp = rw_table[word_length]; rwp->string != NULL; ++rwp) {
      if (strcmp(word_string, rwp->string) == 0) {
        token = rwp->token_code;
        return(TRUE);
      }
    }
  }
  return(FALSE);
}

void open_source_file(char *name) {
  if ((name == NULL) || ((source_file = fopen(name, "r")) == NULL)) {
    error(FAILED_SOURCE_FILE_OPEN);
    exit(-FAILED_SOURCE_FILE_OPEN);
  }

  bufferp = "";
  get_char();
}

void close_source_file() {
  fclose(source_file);
}

BOOLEAN get_source_line() {
  char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];

  if ((fgets(source_buffer, MAX_SOURCE_LINE_LENGTH,source_file)) != NULL) {
    ++line_number;

    if (print_flag) {
      sprintf(print_buffer, "%4d %d: %s",
      line_number, level, source_buffer);
      print_line(print_buffer);
    }
    return(TRUE);
  }
  return(FALSE);
}

void print_line(char line[]) {
  char save_ch;
  char *save_chp = NULL;

  if (++line_count > MAX_LINES_PER_PAGE) {
    print_page_header();
    line_count = 1;
  }

  if (strlen(line) > MAX_PRINT_LINE_LENGTH) {
    save_chp  = &line[MAX_PRINT_LINE_LENGTH];
    save_ch   = *save_chp;
    *save_chp = '\0';
  }

  printf("%s", line);

  if (save_chp) {
    *save_chp = save_ch;
  }
}

void init_page_header(char *name) {
  time_t timer;

  strncpy(source_name, name, MAX_FILE_NAME_LENGTH - 1);

  time(&timer);
  strcpy(date, asctime(localtime(&timer)));
}

void print_page_header() {
  putchar(FORM_FEED_CHAR);
  printf("Page %d   %s   %s\n\n", ++page_number, source_name, date);
}


BOOLEAN token_in(TOKEN_CODE token_list[]) {
  TOKEN_CODE *tokenp;

  if (token_list == NULL) {
    return(FALSE);
  }

  for (tokenp = &token_list[0]; *tokenp; ++tokenp) {
    if (token == *tokenp) {
      return(TRUE);
    }
  }

  return(FALSE);
}


void synchronize(TOKEN_CODE token_list1[], TOKEN_CODE token_list2[], TOKEN_CODE token_list3[]) {
  BOOLEAN error_flag = (! token_in(token_list1)) && (! token_in(token_list2)) && (! token_in(token_list3));

  if (error_flag) {
    error(token == END_OF_FILE ? UNEXPECTED_END_OF_FILE : UNEXPECTED_TOKEN);

    while ((! token_in(token_list1)) && (! token_in(token_list2)) && (! token_in(token_list3)) && (token != END_OF_FILE)) {
      get_token();
    }
  }
}