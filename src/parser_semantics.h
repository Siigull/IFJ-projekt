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

typedef struct sem_state {
    const char* func_name;
    bool seen_return;
    bool assigned_func_call;
} sem_state;

void check_func_call_stmt(AST_Node* node);
void check_semantics(AST_Node* node);
void check_var_usage_traverse(Node* node);
void check_var_usage(Tree* table);
bool is_nullable(Expr_Type type);
bool is_numeric_type(Expr_Type type);
bool check_nullable_condition(Expr_Type left_side, Expr_Type right_side);
bool is_nullable_decl(Expr_Type declaration, Expr_Type expression);
Expr_Type check_node(AST_Node* node, sem_state* state);
Expr_Type sem_check_binary_expression(AST_Node* node, sem_state* state);
Expr_Type sem_func_call(AST_Node* node, sem_state* state);
Expr_Type sem_function_decl(AST_Node* node, sem_state* state);
Expr_Type sem_var_decl(AST_Node* node, sem_state* state);
Expr_Type sem_var_assignment(AST_Node* node, sem_state* state);
Expr_Type sem_else(AST_Node* node, sem_state* state);
Expr_Type sem_return(AST_Node* node, sem_state* state);
Expr_Type sem_if(AST_Node* node, sem_state* state);
Expr_Type sem_while(AST_Node* node, sem_state* state);
Expr_Type get_literal_type(AST_Node* node);

#endif
