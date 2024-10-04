#include "parser_semantics.h"

//TODO(Sigull) Once expr parser is fleshed out add
//             functions for expr, literals, binary...

void _if(node) {
    //TODO(Sigull) todo
}

void func_call(node) {
    //TODO(Sigull) todo
}

void function_decl(node) {
    //TODO(Sigull) todo
}

void var_decl(node) {
    //TODO(Sigull) todo
}

void var_assignment(node) {
    //TODO(Sigull) todo
}

void _else(node) {
    //TODO(Sigull) todo
}

void nnull_var_decl(node) {
    //TODO(Sigull) todo
}

void _return(node) {
    //TODO(Sigull) todo
}


void _if (AST_Node* node) {
    check_node(node->left);
 
    if (node->as.arr->length) {
        AST_Node* node = (AST_Node*)node->as.arr->data[0];

        if (node->type == NNULL_VAR_DECL) {
            if(is_nullable(node->left->type)) {
                exit(10);
            }
        }
    }

    check_node(node->right);
}

void check_node(AST_Node* node) {
    if (node == NULL) {
        // TODO(Sigull) Decide if it should throw error here
        return;
    }

    switch(node->type) {
        case IF:             _if(node); break;
        case FUNC_CALL:      func_call(node); break;
        case FUNCTION_DECL:  function_decl(node); break;
        case VAR_DECL:       var_decl(node); break;
        case VAR_ASSIGNMENT: var_assignment(node); break;
        case ELSE:           _else(node); break;
        case NNULL_VAR_DECL: nnull_var_decl(node); break;
        case RETURN:         _return(node); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

// all errors are exits my brotha
void parse_check(AST_Node* node) {
    check_node(node);
}