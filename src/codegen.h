/**
 * @file codegen.h
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Header file for IFJ24 code generator
 * @date 2024-11-15
 */

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "parser.h"
#include "ast.h"
#include "bvs.h"

#define PROG_START ".IFJcode24\nDEFVAR GF@*expression*result\nDEFVAR GF@*return*val\nJUMP main\n\n"
#define CONDITION_VARS "DEFVAR GF@*rhs\n DEFVAR GF@*lhs\n DEFVAR GF@*tmp*res\n\n"
#define PROG_END "\tPOPFRAME\n\tEXIT int@0\n\n"


/**
 * @brief generates prologue of a program in IFJcode24
 * defines variables used as return value and expression computation result
 * and jumps into main
 */
void generate_prolog();

/**
 * @brief generates start of a builtin function based on its name
 * 
 * @param func_name name of builtin function
 */
void builtin_start(char* func_name);

/**
 * @brief generates end of a builtin function
 * 
 */
void builtin_end();

/**
 * @brief generates function that reads from stdin
 * it can be string, integer or a float value
 * 
 * @param func_name name of the function - readstr, readi32, readf64
 * @param type string, int or float
 */
void builtin_reads(char* func_name, char* type);

/**
 * @brief Generates list of builtins functions in IFJ24
 * @todo make it simpler
 */
void generate_builtins();



/**
 * @brief checks the type of an ast node
 * and returns truth value that says if its an arithmetic operator
 * 
 * @param type AST type of the node to check
 * @return true if its an operator
 * @return false not an operator
 */
bool is_operator_arithmetic(AST_Type type);



/**
 * @brief checks the type of an ast node
 * and returns truth value that says if its an data type in IFJ24
 * 
 * @param type AST type of the node to check
 * @return true if its a data type
 * @return false not a data type
 */
bool is_data_type(AST_Type type);



/**
 * @brief Evaluates condition of if statement or while loop condition,
 * uses temporary GF@*lhs and GF@*rhs to evaluate equality of left and right sides
 * these are computed using generate_expression for left and right child of the root
 * 
 * root can be relation operator or a string
 * in case of a string, it compares it will null
 * 
 * @param curr expression to evaluate
 */
void eval_condition(AST_Node* curr, Tree* symtable);



/**
 * @brief recursive calling of the post-order computation of an expression
 * considers all possible cases - data types, operators, identifiers, calling of a function
 * 
 * @param curr node currently called - recursively changes
 * @param symtable symbolic table
 */
void eval_exp(AST_Node* curr, Tree* symtable);



/**
 * @brief Based on Recursive post-order traversal of a binary tree, which
 * simulates postfix evaluation of an expression, returns in GF@*expression*result
 * the value of the given expression
 * mostly uses stack instructions
 * 
 * @param curr root node of the expression
 * @param symtable symbolic table
 */
void generate_expression(AST_Node* curr, Tree* symtable);



/**
 * @brief Generation of if statement, which includes evaluation of
 * the condition, which is either zero (false) or non-zero (true), makes the jump accordingly
 * also generates all statements inside of the if
 * 
 * else block is in left child node
 * 
 * @param curr if node with statement array
 * @param symtable symbolic table 
 * @param func_name function we are currently in
 * @param stmt_index number of statement the if is (index in the function statement array)
 * @param nest level of nesting of if statement
 */
void generate_if(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest);



/**
 * @brief Generation of else block (left child of if statement)
 * which just generates all statements from array of statements
 * 
 * @param curr Else block node
 * @param symtable symbolic table
 * @param func_name function we are currently in
 * @param stmt_index number of statement the if-else is (index in the function statement array)
 * @param nest level of nesting of if statement
 */
void generate_else(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest);



/**
 * @brief Generates return from function, which can also be an expression
 * value returned from function is in GF@*return*val
 * 
 * @param curr return node with expression array
 * @param symtable symbolic table
 */
void generate_return(AST_Node* curr, Tree* symtable);



/**
 * @brief Generates while loop block
 * which evaluates the condition in each iteration and jumps to the end
 * if its not satisfied
 * 
 * generates all statements in the array
 * 
 * @param curr While block node
 * @param symtable symbolic table
 * @param func_name function we are currently in
 * @param stmt_index number of statement the loop is (index in the function statement array)
 * @param nest level of nesting of if statement
 */
void generate_while(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest);



/**
 * @brief Generates calling of a function, if it has a return value,
 * stores it into GF@*return*val
 * also used to call builtin functions
 * creates a new frame, pushes all arguments
 * 
 * @param curr node of a function to be called
 * @param symtable symbolic table
 */
void generate_func_call(AST_Node* curr, Tree* symtable);



/**
 * @brief Generates whole function, including all its statements
 * which are stored in the array of statements, label of user-defined
 * functions doesnt start with '*' symbol
 * 
 * at the start, it pushes the frame so the arguments are in local scope, 
 * at the end it pops the frame
 * 
 * @param curr Function to generate
 * @param symtable symbolic table with arguments information
 */
void generate_function_decl(AST_Node* curr, Tree* symtable);



/**
 * @brief Generates declaration of a varible, which includes
 * evaluating an expression as the assignment
 * 
 * @param curr node with var information - left child is the expression
 * @param symtable symbolic table
 */
void generate_var_decl(AST_Node* curr, Tree* symtable);



/**
 * @brief Based on generate_expression evaluates value of the given expression
 * and assigns it into the variable
 * 
 * @param curr node to assign the value to - it has information about name of the var
 * and an array of expression the value is computed from
 * @param symtable symtable
 */
void generate_var_assignment(AST_Node* curr, Tree* symtable);



/**
 * @brief Function chooses which type of statement to generate based on the current AST node
 * for while loops and if statements, there needs to be some way to distinguish between nesting levels,
 * which in a row label it will be and in which function we are currently
 * this ensures no collision can happen
 * 
 * @param curr current statement to be generated
 * @param symtable symtable with useful information 
 * @param func_name name of function in which we currently are
 * @param stmt_index number of statement from array of statements in function data
 * @param nest level of nested loop or if statement
 */
void generate_statement(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest);



/**
 * @brief Main function of code generator, which generates prolog, all builtin function
 * and user defined functions
 * 
 * @param input array of AST nodes which represents functions of parsed program
 * @param symtable symtable where information about func parameters (etc.) are stored
 */
void generate_code(Arr* input, Tree* symtable);

#endif