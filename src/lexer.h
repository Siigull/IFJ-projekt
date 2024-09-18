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


/// \todo Add all tokens that will be recognized by lexer
typedef enum {
    T_ID,
    T_NUM,
    T_PLUS,
    T_MINUS,   
    T_MUL,
    T_DIV,
    T_LPAR,
    T_RPAR,
} T_TYPE;

typedef struct{
    T_TYPE type;
    char* value;
    unsigned int length; 
} TOKEN;

typedef struct{
    char* input;
    char current;
    unsigned int idr;
    unsigned int idl;
    size_t input_len;
    // int error_code? 
    // size_t err_len
    // char* err_msg
} Lexer;

TOKEN* init_token(char* value, T_TYPE type, unsigned int length);
Lexer* init_lexer(char* input);
void lexer_advance(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
TOKEN* get_next_token(Lexer* lexer);
TOKEN* find_token_value(Lexer* lexer, T_TYPE type);
bool isNum(char c);
bool isOperator(char c);

#endif