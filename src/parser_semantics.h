#ifndef PARSER_SEMANTICS_H
#define PARSER_SEMANTICS_H
#include "ast.h"
#include "parser.h"
#include <stdio.h>

void parse_check(AST_Node* node);
Ret_Type check_node(AST_Node* node);
bool is_nullable(Ret_Type type);
bool is_numeric_type(Ret_Type type);
Ret_Type sem_check_binary_expression(AST_Node* node);
Ret_Type sem_func_call(AST_Node* node);
Ret_Type sem_function_decl(AST_Node* node);
Ret_Type sem_var_decl(AST_Node* node);
Ret_Type sem_var_assignment(AST_Node* node);
Ret_Type sem_else(AST_Node* node);
Ret_Type sem_nnull_var_decl(AST_Node* node);
Ret_Type sem_return(AST_Node* node);
Ret_Type sem_if(AST_Node* node);

#endif