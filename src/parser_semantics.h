/**
 * IFJ PROJEKT 2024
 * @file parser_semantics.h
 * @author David Bujzaš (xbujzad00@stud.fit.vutbr.cz)
 * @brief Header file of IFJ24 semantic analysis
 *
 * @date 2024-11-21
 *
 */

#ifndef PARSER_SEMANTICS_H
#define PARSER_SEMANTICS_H
#include "ast.h"
#include "parser.h"
#include <stdio.h>

/**
 * @brief Stores vital information for semantic analysis
 * function name, if return was seen, if function call was assigned and type of the expression
 */
typedef struct Sem_State {
    const char* func_name;
    bool seen_return;
    bool assigned_func_call;
    Expr_Type expr_type;
} Sem_State;

/**
 * @brief Holds information about values for constant value evaluation
 *
 */
typedef struct {
    Expr_Type type;
    bool is_const;

    union {
        int i32;
        double f64;
    }as;

    bool is_lit;
} Ret;

/**
 * @brief If possible retype i32 to f64
 *        Only when decimal val is zero
 *
 * @param l
 * @param r
 * @param node
 */
void implicit_f64_i32(Ret* l, Ret* r, AST_Node* node);

/**
 * @brief If possible retype i32 to f64
 *        Only with literal numbers, not constant values
 *
 * @param l
 * @param r
 * @param node
 */
void implicit_i32_f64_lit(Ret* l, Ret* r, AST_Node* node);

/**
 * @brief If possible retype constant value i32 to f64
 *
 * @param l
 * @param r
 * @param node
 */
void implicit_i32_f64(Ret* l, Ret* r, AST_Node* node);

/**
 * @brief Check if the function call is not void
 *
 * @param node
 */
void check_func_call_stmt(AST_Node* node);

/**
 * @brief Main enter into semantic analysis
 *
 * @param node
 */
void check_semantics(AST_Node* node);

/**
 * @brief Recursive traverse for check_var_usage
 *
 * @param node
 */
void check_var_usage_traverse(Node* node);

/**
 * @brief Called after parsing is finished.
 *        Checks used and assignment
 *
 * @param node
 */
void check_var_usage(Tree* table);

/**
 * @brief Checks if the function is writeable with ifj.write, void cannot be writeable
 *
 * @param node
 * @return true if is writeable
 * @return false if not
 */
bool is_writeable(AST_Node* node);

/**
 * @brief Checks if value has zero decimal part
 *
 * @param val
 * @return true
 * @return false
 */
bool can_implicit(Ret val);

/**
 * @brief Check if the expression type is nullable
 *
 * @param type
 * @return true
 * @return false
 */
bool is_nullable(Expr_Type type);

/**
 * @brief Check if the expression type is numeric
 *
 * @param type
 * @return true
 * @return false
 */
bool is_numeric_type(Expr_Type type);

/**
 * @brief Function which has all null or nullable variables options in relation operators
 *
 * @param left_side
 * @param right_side
 * @return true
 * @return false
 */
bool null_in_relation_operators(Expr_Type left_side, Expr_Type right_side);

/**
 * @brief Function which decides if nullable declaration has a non nullable expression assigned to it
 *
 * @param declaration
 * @param expression
 * @return true
 * @return false
 */
bool is_nullable_decl(Expr_Type declaration, Expr_Type expression);

/**
 * @brief First pass of the semantic analysis, checks the type of the node
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type check_node(AST_Node* node, Sem_State* state);

/**
 * @brief Conducts checks for all binary operations and returns the type of the expression
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_check_binary_expression(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for function calls
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_func_call(AST_Node* node, Sem_State* state);

/**
 * @brief Function which returns -1 when node type is NNULL_VAR_DECL,
 * non-nullable var declarations are properly processed in while and if statement semantic analysis
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_nnull_var_decl(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for function declarations
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_function_decl(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for variable declarations
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_var_decl(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for variable assignments
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_var_assignment(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for else statements
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_else(AST_Node* node, Sem_State* state);


/**
 * @brief Semantic analysis for return statements
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_return(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for if statements
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_if(AST_Node* node, Sem_State* state);

/**
 * @brief Semantic analysis for while statements
 *
 * @param node
 * @param state
 * @return Expr_Type
 */
Expr_Type sem_while(AST_Node* node, Sem_State* state);

/**
 * @brief Return the return type of each literal type
 *
 * @param node
 * @return Expr_Type
 */
Expr_Type get_literal_type(AST_Node* node);

/**
 * @brief Main function for constant value evaluation
 *        Calls correct function, all traversing is done through here
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret check_node_2(AST_Node* node, Sem_State* state);

/**
 * @brief traverse if statement
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_if(AST_Node* node, Sem_State* state);

/**
 * @brief does retypes for constant values in parameters
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_func_call(AST_Node* node, Sem_State* state);

/**
 * @brief Just traverses
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_function_decl(AST_Node* node, Sem_State* state);

/**
 * @brief Does retypes for expression in var decl if it is constant value
 *        It also loads constant value into var entry in symtable
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_var_decl(AST_Node* node, Sem_State* state);

/**
 * @brief gets return type of expression
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_var_assignment(AST_Node* node, Sem_State* state);

/**
 * @brief traverses else statement
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_else(AST_Node* node, Sem_State* state);

/**
 * @brief sets non nullable return type to nullable variable
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_nnull_var_decl(AST_Node* node, Sem_State* state);

/**
 * @brief Retypes return value if possible and needed
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_return(AST_Node* node, Sem_State* state);

/**
 * @brief Evaluates binary expression at compile time
 *        More complex than IFJ24.
 *        Equivalent (when IFJ24 allows it) to comptime values in zig.
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_binary_expression(AST_Node* node, Sem_State* state);

/**
 * @brief Returns Ret value based on literal type
 *
 * @param node
 * @param state
 * @return Ret
 */
Ret val_literal_type(AST_Node* node, Sem_State* state);

#endif
