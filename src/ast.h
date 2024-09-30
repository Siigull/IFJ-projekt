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
    FUNC_CALL,
    FUNCTION_DECL,
    VAR_DECL,
    VAR_ASSIGNMENT,
    IF,
    ELSE,
    NNULL_VAR_DECL,
    RETURN,
} AST_Type;

typedef enum {
    F64,
    I32,
    STRING,
    NIL,
} Literal_Type;

typedef struct {
    Literal_Type type;

} Literal_value;

typedef enum {
    PLUS, MINUS, MUL, DIV,
    EQUAL, NOT_EQUAL,
    SMALLER, LARGER,
    SMALLER_EQUAL, LARGER_EQUAL
} Operator_Type;

// typedef struct {
//     Arr* 
//     AST_Node* expr;
//     AST_Node* without_null;
// } If_Data;

/**
 *  - If statement
 *      - left child is expr.
 *      - right child is else.
 *      - as is arr, first el in arr can be NNUL_VAR_DECL
 *        which should be initialized from expr and has 
 *        the same type as expr but without null
 *  
 *  - Var declaration
 *      - left child is init expr
 *      - as is var_name, var details in bvs
 */
typedef struct AST_Node {
    AST_Type type;
    struct AST_Node* left;
    struct AST_Node* right;

    union {
        const char* var_name;
        Arr* arr;
        // size_t data;
    } as;

} AST_Node;

AST_Node* node_init(AST_Type type);

#endif