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

void sem_validate_return(AST_Node* node, Ret_Type expected_type) {
    if (node->type == RETURN) {
        // has return value
        if (node->left) {
            check_node(node->left);

            if (node->left->as.expr_type != expected_type) {
                exit(ERR_SEM_RET_TYPE_DISCARD); // return type missmatch
            }
        // doesn't have return value    
        } else if (expected_type != R_VOID) {
            exit(ERR_SEM_RET_TYPE_DISCARD); // non-void func must return a value
        }
    }

    // check statements in nested blocks
    if (node->as.arr) {
        for (int i = 0; i < node->as.arr->length; i++) {
            AST_Node* child = (AST_Node*)node->as.arr->data[i];
            sem_validate_return(child, expected_type);
        }
    }

    // check both branches for if statements
    if (node->left) sem_validate_return(node->left, expected_type);
    if (node->right) sem_validate_return(node->right, expected_type);
}

void sem_function_decl(AST_Node* node) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if (func_entry != NULL) {
        exit(ERR_SEM_REDEF);
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            Function_Arg* current = (Function_Arg*)node->as.func_data->arr->data[i];

            for (size_t j = i + 1; j < node->as.func_data->arr->length; j++) {
                Function_Arg* next = (Function_Arg*)node->as.func_data->arr->data[j];

                if (!strcmp(current->arg_name, next->arg_name)) {
                    exit(ERR_SEM_PARAMS);
                }
            }
        }
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            check_node((AST_Node*)node->as.func_data->arr->data[i]);
        }
    }

    sem_validate_return(node, node->as.expr_type);
}

void sem_var_decl(AST_Node* node) {
    check_node(node->left);

    Ret_Type declared_type = node->as.expr_type;
    Ret_Type expression_type = node->left->as.expr_type;

    if (declared_type != expression_type) {
        if (declared_type == IMPLICIT) {
            declared_type = expression_type;
        } else {
            exit(ERR_SEM_TYPE_CONTROL);
        }
    }
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
        case RETURN:         sem_validate_return(node, node->as.expr_type); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

// all errors are exits my brotha
void parse_check(AST_Node* node) {
    check_node(node);
}