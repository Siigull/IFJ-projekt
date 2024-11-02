/**
 * @file ast.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#include "ast.h"

AST_Node* node_init(AST_Type type) {
    AST_Node* node = malloc(sizeof(AST_Node));

    node->type = type;

    switch(type) {
        case FUNCTION_DECL:
        case IF:
        case WHILE:
        case ELSE:
            node->as.arr = arr_init();
            break;
        case VAR_DECL:
        case FUNC_CALL:
            node->as.var_name = NULL;
            break;
    }

    return node;
}
