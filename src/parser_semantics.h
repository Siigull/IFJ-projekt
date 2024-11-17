#ifndef PARSER_SEMANTICS_H
#define PARSER_SEMANTICS_H
#include "ast.h"
#include <stdio.h>

void parse_check(AST_Node* node);
Ret_Type check_node(AST_Node* node);

#endif