#ifndef parser_h
#define parser_h

#include "ch5/misc/common.h"
#include "ch5/symbol_table/symbol_table.h"

/*--------------------------------------------------------------*/
/*  if_token_get                If token equals token_code, get */
/*                              the next token.                 */
/*--------------------------------------------------------------*/

#define if_token_get(token_code)		\
    if (token == token_code) get_token()

/*--------------------------------------------------------------*/
/*  if_token_get_else_error     If token equals token_code, get */
/*                              the next token, else error.     */
/*--------------------------------------------------------------*/

#define if_token_get_else_error(token_code, error_code)	\
    if (token == token_code) get_token(); 		\
    else                     error(error_code)

#endif

