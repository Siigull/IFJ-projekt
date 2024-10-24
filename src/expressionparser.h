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
#include <stdbool.h>
#include <stdio.h>
#include "lexer.h"
#include "stack.h"
#include "ast.h"

#define TABLE_SIZE 8

typedef enum {
    N, // no precedence
    L, // left precedence
    R, // right precedence
    M, // equal precedence
} precedence;

int precedence_table[TABLE_SIZE][TABLE_SIZE] = {
//   id +  -  *  /  (  )  $ 
    {N, L, L, L, L, N, L, L}, // id
    {R, L, L, R, R, R, L, L}, // +
    {R, L, L, R, R, R, L, L}, // -
    {R, L, L, L, L, R, L, L}, // *
    {R, L, L, L, L, R, L, L}, // /
    {R, R, R, R, R, R, M, N}, // (
    {N, L, L, L, L, N, L, L}, // )
    {R, R, R, R, R, R, N, R}  // $

};

AST_Node* parse_expression(List* list);

#endif