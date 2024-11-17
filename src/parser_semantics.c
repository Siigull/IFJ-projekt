#include "parser_semantics.h"
#include "parser.h"

extern Parser* parser;

//TODO(Sigull) Once expr parser is fleshed out add
//             functions for expr, literals, binary...
bool is_nullable(Ret_Type type) {
    switch(type) {
        case N_F64: 
        case N_I32:
        case N_U8:
            return true;
        default:
            return false;
    }
}

Ret_Type sem_func_call(AST_Node* node) {
    //TODO(Sigull) todo
}

Ret_Type sem_function_decl(AST_Node* node) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if (func_entry != NULL) {
        ERROR_RET(ERR_SEM_REDEF);
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            Function_Arg* current = (Function_Arg*)node->as.func_data->arr->data[i];

            for (size_t j = i + 1; j < node->as.func_data->arr->length; j++) {
                Function_Arg* next = (Function_Arg*)node->as.func_data->arr->data[j];

                if (!strcmp(current->arg_name, next->arg_name)) {
                    ERROR_RET(ERR_SEM_PARAMS);
                }
            }
        }
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
            Ret_Type return_result = check_node(current_node);
            if (return_result != node->as.expr_type && node->type == RETURN) {
                ERROR_RET(ERR_SEM_RET_TYPE_DISCARD);
            }
        }
    }

    return -1;
}

Ret_Type sem_var_decl(AST_Node* node) {

    Ret_Type declared_type = node->as.expr_type;
    Ret_Type expression_type = check_node(node->left);

    if (declared_type != expression_type) {
        if (declared_type == IMPLICIT) {
            declared_type = expression_type;
        } else {
            ERROR_RET(ERR_SEM_TYPE_CONTROL);
        }
    }

    return -1;
}

Ret_Type sem_var_assignment(AST_Node* node) {
    //TODO(Sigull) todo
}

Ret_Type sem_else(AST_Node* node) {
    //TODO(Sigull) todo
}

Ret_Type sem_nnull_var_decl(AST_Node* node) {
    //TODO(Sigull) todo
}

Ret_Type sem_return(AST_Node* node) {
    return check_node(node->left);
}

Ret_Type sem_if (AST_Node* node) {
    check_node(node->left);
 
    if (node->as.arr->length) {
        AST_Node* node = (AST_Node*)node->as.arr->data[0];

        if (node->type == NNULL_VAR_DECL) {
            if(is_nullable(node->left->as.expr_type)) {
                ERROR_RET(ERR_SEM_OTHER);
            }
        }
    }

    check_node(node->right);
}

Ret_Type check_node(AST_Node* node) {
    if (node == NULL) {
        return -1;
    }

    switch(node->type) {
        case IF:             return sem_if(node);
        case FUNC_CALL:      return sem_func_call(node);
        case FUNCTION_DECL:  return sem_function_decl(node);
        case VAR_DECL:       return sem_var_decl(node);
        case VAR_ASSIGNMENT: return sem_var_assignment(node);
        case ELSE:           return sem_else(node);
        case NNULL_VAR_DECL: return sem_nnull_var_decl(node);
        case RETURN:         return sem_return(node);
        
        default:             printf("Unknown node type!\n"); return -1;
    }
}

// all errors are exits my brotha
void parse_check(AST_Node* node) {
    check_node(node);
}