/**
 * @file parser_semantics.c
 * @author David Bujzaš (xbujzad00@stud.fit.vutbr.cz)
 * @brief Implementation of IFJ24 semantic analysis
 * 
 * @date 2024-11-17
 * 
 */


#include "parser_semantics.h"

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

bool is_numeric_type(Ret_Type type) {
    return (type == N_F64 || type == N_I32 || type == R_F64 || type == R_I32);
}

Ret_Type sem_check_binary_expression(AST_Node* node) {
    Ret_Type left_type = check_node(node->left);
    Ret_Type right_type = check_node(node->right);

    if (is_numeric_type(left_type) && is_numeric_type(right_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    switch (node->type) {
        case PLUS:
        case MINUS:
        case MUL:
        case DIV: {
            if (left_type == R_F64 || right_type == R_F64) {
                node->as.expr_type = R_F64;
                return R_F64;
            } else if (left_type == N_F64 || right_type == N_F64) {
                if (!is_nullable(node->as.expr_type)) {
                    ERROR_RET(ERR_SEM_TYPE_CONTROL);
                }
                node->as.expr_type = N_F64;
                return N_F64;
            }
            node->as.expr_type = R_I32;
            return R_I32;
        }

        case ISEQ:
        case ISNEQ:
        case ISLESS:
        case ISMORE:
        case ISLESSEQ:
        case ISMOREEQ: {
            node->as.expr_type = R_I32;
            return R_I32;
        }
        default:
            ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }
}

Ret_Type sem_func_call(AST_Node* node) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if (!func_entry || func_entry->type != E_FUNC) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    size_t expected_args = func_entry->as.function_args->length;
    size_t provided_args = node->as.func_data->arr->length;

    if (expected_args != provided_args) {
        ERROR_RET(ERR_SEM_PARAMS);
    }

    // todo check arg types
    return -1;
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
        } else if ((declared_type == R_I32 || declared_type == N_I32) && (expression_type == R_F64 || expression_type == N_F64)) {
            double value = node->left->as.f64;
            if (value != (int)value) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL);
            }
            node->as.expr_type = R_I32;
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
    if (node->left != NULL || node->right != NULL) {
        ERROR_RET(ERR_SEM_OTHER); 
    }

    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
        check_node(stmt);
    }
    
    return -1;
}

Ret_Type sem_nnull_var_decl(AST_Node* node) {
    if (!node->left) {
        ERROR_RET(ERR_SEM_OTHER);
    }

    Ret_Type type = check_node(node->left);

    if (!is_nullable(type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    Entry* new_entry = entry_init(node->as.var_name, E_VAR, type, false);
    tree_insert(parser->s_table, new_entry);

    node->as.expr_type = type;
    return -1; 
}

Ret_Type sem_return(AST_Node* node) {
    return check_node(node->left);
}

Ret_Type sem_if(AST_Node* node) {
    Ret_Type cond_type = check_node(node->left);
 
    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

        if (first_stmt->type == NNULL_VAR_DECL) {
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_OTHER); 
            }

            for (size_t i = 1; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt);
            }
        } else {
            for (size_t i = 0; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt);
            }
        }
    }

    if (node->right != NULL) {
        check_node(node->right);
    }

    return -1;
}

Ret_Type check_node(AST_Node* node) {
    if (node == NULL) {
        return -1;
    }

    switch(node->type) {
        // binary expressions
        case PLUS:
        case MINUS:
        case MUL:
        case DIV:
        case ISEQ:
        case ISNEQ:
        case ISLESS:
        case ISMORE:
        case ISLESSEQ:
        case ISMOREEQ: 
            return sem_check_binary_expression(node);
        // statements
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