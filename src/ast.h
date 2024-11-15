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
#include "bvs.h"

typedef enum {
    FUNC_CALL,
    FUNCTION_DECL,
    VAR_DECL,
    VAR_ASSIGNMENT,
    IF,
    WHILE,
    ELSE,
    NNULL_VAR_DECL,
    RETURN,
    NIL,
    I32,
    F64, 
    ID,
    STRING,
    PLUS,
    ISEQ,
    ISNEQ,
    ISLESS,
    ISMORE,
    ISLESSEQ,
    ISMOREEQ,
    MINUS,
    MUL,
    DIV,
} AST_Type;

// typedef enum {
//     F64,
//     I32,
//     STRING,
//     NIL,
// } Literal_Type;

// typedef struct {
//     Literal_Type type;

// } Literal_value;

// typedef enum {
//     PLUS, MINUS, MUL, DIV,
//     EQUAL, NOT_EQUAL,
//     SMALLER, LARGER,
//     SMALLER_EQUAL, LARGER_EQUAL
// } Operator_Type;

// typedef struct {
//     Arr* 
//     AST_Node* expr;
//     AST_Node* without_null;
// } If_Data;

typedef struct {
    const char* var_name;
    Arr* arr;
} Func_Data;

/**
 *  ------- Normal Parser -------
 *  - If statement
 *      - left child is expression.
 *      - right child is else.
 *      - as is arr, first el in arr can be NNUL_VAR_DECL
 *        which should be initialized from expr and has 
 *        the same type as expr but without null
 *  
 *  - Else statement
 *      - no children
 *      - as is arr
 * 
 *  - Assignment statement
 *      - left child is expression
 *      - as is var_name
 *      - If var_name is '_'. only expression is returned.
 * 
 *  - Return statement
 *      - left child is expression
 *      - nothing in as
 * 
 *  - While statement (almost the same as If)
 *      - left child is expression
 *      - right child is empty, but if we add extension its else
 *      - as is arr first el can be NNUL_VAR_DECL
 * 
 *  - Var declaration
 *      - left child is expression
 *      - as is var_name, var details in bvs
 * 
 *  - Func declaration
 *      - no children
 *      - as is func_data, arr is staments in block
 *      - entry has function args in as
 * 
 *  - Function call
 *      - no children
 *      - as is func_data, params are an array of expressions
 * 
 *  ------- Expression parser -------
 * 
 */
typedef struct AST_Node {
    AST_Type type;
    struct AST_Node* left;
    struct AST_Node* right;

    union {
        const char* string;
        const char* var_name;
        Arr* arr;
        Ret_Type expr_type;
        Func_Data* func_data;
        int i32;
        double f64;
    } as;

} AST_Node;

AST_Node* node_init(AST_Type type);

#endif