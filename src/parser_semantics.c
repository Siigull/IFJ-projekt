#include "parser_semantics.h"
#include "parser.h"

extern Parser* parser;

//TODO(Sigull) Once expr parser is fleshed out add
//             functions for expr, literals, binary...
bool is_nullable(Ret_Type type) {
    return false;
}

void check_node(AST_Node* node);

void sem_func_call(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_function_decl(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_var_decl(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_var_assignment(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_else(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_nnull_var_decl(AST_Node* node) {
    //TODO(Sigull) todo
}

void sem_return(AST_Node* node) {
    check_node(node->left);
}


void sem_if (AST_Node* node) {
    check_node(node->left);
 
    if (node->as.arr->length) {
        AST_Node* node = (AST_Node*)node->as.arr->data[0];

        if (node->type == NNULL_VAR_DECL) {
            if(is_nullable(node->left->as.expr_type)) {
                exit(ERR_SEM_OTHER);
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
        case IF:             sem_if(node); break;
        case FUNC_CALL:      sem_func_call(node); break;
        case FUNCTION_DECL:  sem_function_decl(node); break;
        case VAR_DECL:       sem_var_decl(node); break;
        case VAR_ASSIGNMENT: sem_var_assignment(node); break;
        case ELSE:           sem_else(node); break;
        case NNULL_VAR_DECL: sem_nnull_var_decl(node); break;
        case RETURN:         sem_return(node); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

// all errors are exits my brotha
void parse_check(AST_Node* node) {
    check_node(node);
}