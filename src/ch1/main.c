#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FORM_FEED_CHAR		'\f'

#define MAX_FILE_NAME_LENGTH    32
#define MAX_SOURCE_LINE_LENGTH  256
#define MAX_PRINT_LINE_LENGTH   80
#define MAX_LINES_PER_PAGE      50
#define DATE_STRING_LENGTH      26

typedef enum {
    FALSE, TRUE,
} BOOLEAN;

int line_number = 0;                    /* current line number */
int page_number = 0;                    /* current page number */
int level       = 0;                    /* current nesting level */
int line_count  = MAX_LINES_PER_PAGE;   /* no. lines on current pg */

char source_buffer[MAX_SOURCE_LINE_LENGTH]; /* source file buffer */

char source_name[MAX_FILE_NAME_LENGTH]; /* name of source file */
char date[DATE_STRING_LENGTH];          /* current date and time */

FILE *source_file;

void init_lister(char *name);
BOOLEAN get_source_line();
void print_line(char line[]);
void print_page_header();

void init_lister(char *name) {
  time_t timer;

  strcpy(source_name, name);
  source_file = fopen(source_name, "r");

  time(&timer);
  strcpy(date, asctime(localtime(&timer)));
}

BOOLEAN get_source_line() {
  char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];
  if ( fgets(source_buffer, MAX_SOURCE_LINE_LENGTH, source_file) != NULL ) {
    ++line_number;
    sprintf(print_buffer, "%4d %d: %s", line_number, level, source_buffer);
    print_line(print_buffer);
    return TRUE;
  }
  return FALSE;
}

void print_line(char line[]) {
  char save_ch;
  char *save_chp = NULL;

  if ( ++line_count > MAX_LINES_PER_PAGE ) {
    print_page_header();
    line_count = 1;
  }

  if ( strlen(line) > MAX_PRINT_LINE_LENGTH ) {
    save_chp = &line[MAX_PRINT_LINE_LENGTH];
  }

  if ( save_chp ) {
    save_ch = *save_chp;
    *save_chp = '\0';
  }

  printf("%s", line);

  if (save_chp) {
    *save_chp = save_ch;
  }
}

void print_page_header() {
  putchar(FORM_FEED_CHAR);
  printf("Page %d    %s    %s\n\n", ++page_number, source_name, date);
}

int main(int argc, char **argv) {
  BOOLEAN get_source_line();
  init_lister(argv[1]);

  while(get_source_line());

  return 0;
}


