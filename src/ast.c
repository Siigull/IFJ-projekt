/**
 * @file ast.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#include "ast.h"

AST_Node* node_init(AST_Type type) {
    AST_Node* node = malloc(sizeof(AST_Node*));

    node->type = type;

    if (type == FUNCTION_DECLARATION) {
        node->as.arr = arr_init();
    }

    return node;
}
