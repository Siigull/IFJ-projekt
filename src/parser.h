#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    LITERAL,
    BINARY,
    GROUPING,
    VAR,
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
    } data;

} AST_Node;

typedef struct Parser {
    Token* next;
    Token* prev;
} Parser;

void parse(const char* input);

#endif