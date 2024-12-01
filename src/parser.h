/**
 * IFJ PROJEKT 2024
 * @file parser.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * 
 */

#ifndef PARSER_H
#define PARSER_H

#include "expression_parser.h"
#include "helper.h"
#include "ast.h"
#include "error.h"
#include "string.h"
#include "codegen.h"
#include "parser_semantics.h"
#include "stack.h"
// #include "test_generate_graph.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

/**
 * @brief Parser state
 *        Parser and lexer is a global var declared in parser.c at the top
 * 
 */
typedef struct Parser {
    Token* next;
    Token* prev;

    Tree* s_table;
    C_Stack c_stack;
} Parser;

/**
 * @brief allocates and initializes parser
 * 
 * @return Parser* 
 */
Parser* init_parser();

/**
 * @brief keeps s-table, removes tokens
 * 
 */
void parser_reset();

/**
 * @brief entry call to parser
 *        I feel like the code is self explanatory but the way to put 
 *          data into AST is described in ast.h
 * 
 * @param input 
 * @return Arr* root nodes of functions
 */
Arr* parse(char* input);

/**
 * @brief Calls parse and then sends Arr to codegen
 * 
 * @param input 
 * @return int 0 if no error
 */
int compile(char* input);

/**
 * @brief used in codegen. Converts unprintable characters to \xyz
 * 
 * @param string 
 * @return char* 
 */
char* string_to_assembly(const char* string);

/////// helper parse functions ///////
//////////////////////////////////////
// individual function calls for var_decl, function_call...
//   are only in parser.c

/**
 * @brief Type that has null in it to equivalent type without it
 * 
 * @param type 
 * @return Expr_Type 
 */
Expr_Type null_to_nnul(Expr_Type type);

/**
 * @brief Moves parser to next token
 * 
 */
void advance();

/**
 * @brief Checks if next token is of correct type otherwise errors
 * 
 * @param type 
 */
void consume(T_Type type);

/**
 * @brief checks the type of next token without error
 * 
 * @param type 
 * @return true if next token type matches sent type
 * @return false otherwise 
 */
bool check(T_Type type);

/**
 * @brief parses type -> optional ? and type and returns it as Expr_Type
 *        Advances parser correctly
 * 
 * @return Expr_Type 
 */
Expr_Type type();

/**
 * @brief parses statements inside function, if, else... block into arr
 * 
 * @param stmts 
 */
void block(Arr* stmts);

#endif