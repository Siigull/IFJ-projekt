/**
 * @file expressionparser.h
 * @author Martin Vaculik (xvaculm00)
 * @brief
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H
#include "ast.h"
#include "lexer.h"
#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#define TABLE_SIZE 8

typedef enum {
	N, // no precedence
	L, // left precedence
	R, // right precedence
	M, // equal precedence
} precedence;

/*
Rules: E  → E + E
	   E  → E - E
	   E  → E * E
		E  → E / E
		E  → (E)
		E  → id
		E → E == E
*/

void init_stack(List* Stack);
AST_Node* parse_expression(List* list);
void handle_rule(int rule, List* Stack, List* input);
int find_reduction_rule(List* Stack);
#endif