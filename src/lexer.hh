/**
 * @file lexer.h
 * @brief Lexer implementation header file
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 *
 *
 *
 */

#ifndef LEXER_HH
#define LEXER_HH

extern "C" {
#include <stdbool.h>
#include <stdio.h>

/// \todo ’\"’, ’\n’, ’\r’, ’\t’, ’\\’
typedef enum {
	T_ID,
	T_F64,
	T_I32,
	T_U8,
	T_DTYPE,
	T_DOT = 10,
	T_QUOTATION,
	T_QUESTMARK,
	T_EXCLEMARK,
	T_EXPONENT,
	T_STRING,
	T_COMMENT,
	T_BUILDIN,
	T_RPAR = 20, // (
	T_LPAR,      // )
	T_SQRBRACKET,
	T_SQLBRACKET,
	T_CUYRBRACKET, // {
	T_CUYLBRACKET, // }
	T_VBAR, // |
	T_CONST = 30,
	T_IF,
	T_ELSE,
	T_FN,
	T_PUB,
	T_RETURN,
	T_VAR,
	T_VOID,
	T_WHILE,
	T_IMPORT,
	T_SEMI,
	T_DDOT,
	T_COMMA,
	T_UNDER,
	T_PLUS = 50,
	T_MINUS,
	T_MUL,
	T_DIV,
	T_EQUAL,
	T_DDEQ, // ==
	T_NEQUAL,
	T_GTHAN,
	T_GETHAN,
	T_STHAN,
	T_SETHAN,
	T_OPERATOR = 70,
	T_UNDEF,
	T_NULL = 90,
	T_EOF,
	T_ERR,
	T_PROLOG,
} T_TYPE;

typedef struct {
	T_TYPE type;
	char* value;
	unsigned int length;
} Token;

typedef struct {
	char* input;
	unsigned int idr;
	unsigned int idl;
	size_t input_len;
	// int error_code?
	// size_t err_len
	// char* err_msg
} Lexer;

/**
 * @brief analyzing the input for lexems
 *
 * @param input the code that is being checked for tokens
 * @return Lexer*
 *
 */
Lexer* init_lexer(char* input);

Token* get_next_token(Lexer* lexer);

void print_token(Token* token, FILE* out);
}

#endif