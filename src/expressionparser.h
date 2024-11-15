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
#include "error.h"
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
		E → E == E
		E  → (E)
		E  → id
		
*/

typedef enum{
	E_PLUS_E,
	E_MINUS_E,
	E_TIMES_E,
	E_DIV_E,
	E_EQ_E,
	E_E,
	E_ID,
}rules;

void init_stack(List* Stack);
AST_Node* parse_expression(List* list);
void handle_rule(int rule, List* Stack);
void handle_precedence(int precedence, List* Stack, List* input);
void insert_dollar(List* input);
int find_reduction_rule(List* Stack);
int find_precedence_index(List* list);
bool is_nonTerm(T_Type type, bool isProcessed);
bool is_id(T_Type type);
#endif
