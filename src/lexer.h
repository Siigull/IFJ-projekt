/**
 * @file lexer.h
 * @brief Lexer implementation header file
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 *
 *
 *
 */

#ifndef LEXER_H
#define LEXER_H
#include <stdbool.h>
#include <stdio.h>

/// \todo ’\"’, ’\n’, ’\r’, ’\t’, ’\\’
typedef enum {
	T_ID,
	T_F64,
	T_I32,
	T_U8,
	T_DOT = 10,
	T_QUOTATION,
	T_QUESTMARK,
	T_EXCLEMARK,
	T_EXPONENT,
	T_STRING,
	T_COMMENT,
	T_RPAR = 20, // (
	T_LPAR,      // )
	T_SQRBRACKET,
	T_SQLBRACKET,
	T_CUYRBRACKET, // {
	T_CUYLBRACKET, // }
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
	T_PLUS = 50,
	T_MINUS,
	T_MUL,
	T_DIV,
	T_EQUAL,
	T_NEQUAL,
	T_GTHAN,
	T_GETHAN,
	T_STHAN,
	T_SETHAN,
	T_OPERATOR = 70,
	T_UNDEF,
	T_NULL = 90,
	T_EOF,
	T_ERR
} T_TYPE;

typedef struct {
	T_TYPE type;
	char* value;
	unsigned int length;
} TOKEN;

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
 * @brief takes an array, type that was checked before, and length of token
 *
 * @param value array of symbols that constructs a value of the token(32, int, firstNumber)
 * @param type  enum of T_TYPE
 * @param length
 * @return TOKEN* pointer
 * 
 */
TOKEN* init_token(char* value, T_TYPE type, unsigned int length);

/**
 * @brief analyzing the input for lexems
 *
 * @param input the code that is being checked for tokens
 * @return Lexer*
 * 
 */
Lexer* init_lexer(char* input);


/**
 * @brief moves main index of lexer to scan next character, but makes sure we are still in range
 * 
 * @param lexer Lexer
 * 
 */
void lexer_advance(Lexer* lexer);


/**
 * @brief skips whole line in input - used for skipping comments in code
 * 
 * @param lexer Lexer
 * 
 */
void lexer_skip_line(Lexer* lexer);

/**
 * @brief skips all whitespace characters to get to the next token
 * 
 * @param c character being skipped
 * @return true if whitespace
 * @return false if not whitespace
 * 
 */
bool is_whitespace(char c);


/**
 * @brief detects all whitespace characters
 * 
 * @param c char to be evaluated
 * @return true if whitespace
 * @return false if not whitespace
 * 
 */
void lexer_skip_whitespace(Lexer* lexer);


/**
 * @brief evaluates if character is one of arithmetic operators used in the language
 * 
 * @param c character to be evaluated
 * @return true if operator
 * @return false if not operator
 * 
 */
bool is_operator(char c);

/**
 * @brief evaluates if char is a number (digit)
 * 
 * @param c character to be evaluated
 * @return true if number
 * @return false if not a number
 */
bool is_num(char c);

TOKEN* get_next_token(Lexer* lexer);
TOKEN* find_token_value(Lexer* lexer, T_TYPE type);

#endif