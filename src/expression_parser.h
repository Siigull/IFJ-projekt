/**
 * IFJ PROJEKT 2024
 * @file expressionparser.h
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Header file of expression parser for IFJ 2024
 * 
 * @date 2024-10-15
 * 
 */

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#define TABLE_SIZE 14

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
        E  → E relacni operator E

*/

typedef enum {
    E_PLUS_E,
    E_MINUS_E,
    E_TIMES_E,
    E_DIV_E,
    E_EQ_E,
    E_E,
    E_ID,
    E_NEQ_E,
    E_GET_E,
    E_GT_E,
    E_ST_E,
    E_SET_E,
    E_ZAVAROKA_DEADLY_E
} rules;

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