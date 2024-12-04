/**
 * IFJ PROJEKT 2024
 * @file lexer.h
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Header file of lexer for IFJ 2024
 * 
 * @date 2024-09-21
 * 
 */

#ifndef LEXER_H
#define LEXER_H
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "ast.h"
#include "error.h"


#define BACKSLASH 92


typedef enum {
	
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
	T_BAR, // |
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
	T_ID = 50,
	T_PLUS,
	T_MINUS,
	T_MUL,
	T_DIV,
	T_DOLLARLIST,
	T_LEFTSHIFTLIST,
	T_RIGHTSHIFTLIST,
	T_DDEQ, // ==
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
	T_ERR,
	T_PROLOG,
} T_Type;

typedef struct {
	T_Type type;
	char* value;
	unsigned int length;
	bool isProcessed; 
	AST_Node* node;
} Token;

typedef struct {
	char* input;
	unsigned int idr;
	unsigned int idl;
	size_t input_len;
} Lexer;

/**
 * @brief analyzing the input for lexems
 *
 * @param input the code that is being checked for tokens
 * @return Lexer*
 *
 */
Lexer* init_lexer(char* input);
/**
 * @brief Get the next token from the input
 * 
 * @param lexer 
 * @return Token* 
 */
Token* get_next_token(Lexer* lexer);
/**
 * @brief initializes token
 * 
 * @param value, value of token
 * @param type, type of token
 * @param length, length of token
 * @return Token* 
 */
Token* init_token(const char* value, T_Type type, unsigned int length);
/**
 * @brief prints token
 * 
 * @param token 
 * @param out 
 * @param string 
 * @return char* 
 */
char* print_token(Token* token, FILE* out, bool string);
#endif