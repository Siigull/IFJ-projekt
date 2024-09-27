/**
 * @file ast.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#ifndef AST_H
#define AST_H
#include "helper.h"

typedef enum {
    LITERAL,
    BINARY,
    GROUPING,
    VAR,
    FUNCTION_DECLARATION,
} AST_Type;

typedef enum {
    F64,
    I32,
    STRING,
    NIL,
} Literal_Type;

typedef enum {
    PLUS, MINUS, MUL, DIV,
    EQUAL, NOT_EQUAL,
    SMALLER, LARGER,
    SMALLER_EQUAL, LARGER_EQUAL
} Operator_Type;

typedef struct AST_Node {
    AST_Type type;
    struct AST_Node* left;
    struct AST_Node* right;

    union {
        Literal_Type literal;
        Operator_Type operator;
        Arr* arr;
    } as;

} AST_Node;

AST_Node* node_init(AST_Type type);

#endif