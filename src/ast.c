/**
 * IFJ PROJEKT 2024
 * @file ast.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief AST node implementation
 * 
 * @date 2024-09-27
 * 
 */

#include "ast.h"

AST_Node* node_init(AST_Type type) {
    AST_Node* node = malloc(sizeof(AST_Node));

    node->type = type;
    node->left = NULL;
    node->right = NULL;

    // init union based on AST_Type
    switch(type) {
        case FUNCTION_DECL:
        case FUNC_CALL:
            node->as.func_data = malloc(sizeof(Func_Data));
            node->as.func_data->arr = arr_init();
            break;
        case IF:
        case WHILE:
        case ELSE:
            node->as.arr = arr_init();
            break;
        case VAR_DECL:
            break;
        default:
            node->as.expr_type = R_VOID;
            break;
    }

    return node;
}
