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
bool is_nullable(Expr_Type type) {
    switch(type.type) {
        case N_F64: 
        case N_I32:
        case N_U8:
            return true;
        default:
            return false;
    }
}

Expr_Type get_literal_type(AST_Node* node) {
    switch(node->type) {
        case I32: return (Expr_Type){R_I32, true};
        case F64: return (Expr_Type){R_F64, true};
        case STRING: return (Expr_Type){R_STRING, true};
        case ID: {
            Entry* entry = tree_find(parser->s_table, node->as.var_name);
            return entry->ret_type;
        }
        default: return (Expr_Type){-1, false};
    }
}

bool is_numeric_type(Expr_Type type) {
    return (type.type == N_F64 || type.type == N_I32 || type.type == R_F64 || type.type == R_I32);
}

Expr_Type sem_check_binary_expression(AST_Node* node) {
    Expr_Type left_type = check_node(node->left);
    Expr_Type right_type = check_node(node->right);

    if (!is_numeric_type(left_type) || !is_numeric_type(right_type) || is_nullable(left_type) || is_nullable(right_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    bool has_int = (left_type.type == R_I32 || right_type.type == R_I32);
    bool has_float = (left_type.type == R_F64 || right_type.type == R_F64);

    if (left_type.is_const_literal == true && left_type.is_const_literal == true) {
        node->as.expr_type.is_const_literal = true;    
    }

    bool is_node_const = node->as.expr_type.is_const_literal;

    switch (node->type) {
        case PLUS:
        case MINUS:
        case MUL: {
            if (has_float) {
                if (left_type.type == R_I32) {
                    node->left->type = F64;
                    node->left->as.f64 = (double)node->left->as.i32;
                } else if (right_type.type == R_I32) {
                    node->right->type = F64;
                    node->right->as.f64 = (double)node->right->as.i32;
                }
                node->as.expr_type = (Expr_Type){R_F64, is_node_const};
                return (Expr_Type){R_F64, is_node_const};
            } else if (has_int && !has_float) {
                node->as.expr_type = (Expr_Type){R_I32, is_node_const};
                return (Expr_Type){R_I32, is_node_const};
            }
        }
        case DIV: {
            if (has_float && has_int) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL);
            }
            else if (has_float) {
                node->as.expr_type = (Expr_Type){R_F64, is_node_const};
                return (Expr_Type){R_F64, is_node_const};
            }
            node->as.expr_type = (Expr_Type){R_I32, is_node_const};
            return (Expr_Type){R_I32, is_node_const};
        }
        case ISEQ:
        case ISNEQ:
        case ISLESS:
        case ISLESSEQ:
        case ISMORE:
        case ISMOREEQ:
            if (left_type.type == right_type.type) {
                node->as.expr_type = (Expr_Type){R_BOOLEAN, is_node_const};
                return (Expr_Type){R_BOOLEAN, is_node_const};
            } else if ((left_type.type == R_I32 && right_type.type == R_F64) || (left_type.type == R_F64 && right_type.type == R_I32)) {
                if (left_type.type == R_I32) {
                    left_type.type = R_BOOLEAN;
                } else if (right_type.type == R_I32) {
                    right_type.type = R_BOOLEAN;
                }
                node->as.expr_type = (Expr_Type){R_BOOLEAN, is_node_const};
                return (Expr_Type){R_BOOLEAN, is_node_const};
            }
        default: ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }
}

Expr_Type sem_func_call(AST_Node* node) {
    
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
            check_node(current_node);
        }
    }

    if (func_entry != NULL) {
        size_t expected_params = func_entry->as.function_args->length;
        size_t actual_params = node->as.func_data->arr->length;
        if (expected_params != actual_params) {
            ERROR_RET(ERR_SEM_PARAMS);
        }
    }

    if (!strcmp(node->as.func_data->var_name, "*string") || !strcmp(node->as.func_data->var_name, "*write")) {
        if (((AST_Node*)(node->as.func_data->arr->data[0]))->type == STRING) {
            return (Expr_Type){R_U8, false};
        }
    }

    return func_entry->ret_type;
}

Expr_Type sem_function_decl(AST_Node* node) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);
    
    if (func_entry->as.function_args) {
        for (size_t i = 0; i < func_entry->as.function_args->length; i++) {
            Function_Arg* current = (Function_Arg*)func_entry->as.function_args->data[i];

            for (size_t j = i + 1; j < func_entry->as.function_args->length; j++) {
                Function_Arg* next = (Function_Arg*)func_entry->as.function_args->data[j];

                if (!strcmp(current->arg_name, next->arg_name)) {
                    ERROR_RET(ERR_SEM_PARAMS);
                }
            }
        }
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
            check_node(current_node);
        }
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_var_decl(AST_Node* node) {
    Entry* entry = tree_find(parser->s_table, node->as.var_name);

    Expr_Type declared_type = entry->ret_type;
    Expr_Type expression_type = check_node(node->left);

    if (declared_type.type != expression_type.type) {
        if (declared_type.type == IMPLICIT) {
            declared_type = expression_type;
        } else if (expression_type.type == R_STRING) {
            ERROR_RET(ERR_SEM_TYPE_INFERENCE);
        } else if (declared_type.type == N_F64 && expression_type.type == R_F64) {
            expression_type.type = N_F64;
        } else {
            if (!((declared_type.type == N_F64 && expression_type.type == R_F64) || 
                (declared_type.type == N_I32 && expression_type.type == R_I32) || 
                (declared_type.type == N_U8 && expression_type.type == R_U8))) {
                    ERROR_RET(ERR_SEM_TYPE_CONTROL);   
            }
        }
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_var_assignment(AST_Node* node) {
    Entry* entry = tree_find(parser->s_table, node->as.var_name);

    Expr_Type declared_type = entry->ret_type;
    Expr_Type expression_type = check_node(node->left);

    if (declared_type.type != expression_type.type || expression_type.type == R_STRING) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_else(AST_Node* node) {
    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
        check_node(stmt);
    }
    
    return (Expr_Type){-1, false};
}

Expr_Type sem_nnull_var_decl(AST_Node* node) {
    if (!node->left) {
        ERROR_RET(ERR_SEM_OTHER);
    }

    Expr_Type type = check_node(node->left);

    if (!is_nullable(type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    Entry* new_entry = entry_init(node->as.var_name, E_VAR, type, false);
    tree_insert(parser->s_table, new_entry);

    node->as.expr_type = type;
    return (Expr_Type){-1, false}; 
}

Expr_Type sem_return(AST_Node* node) {
    return check_node(node->left);
}

Expr_Type sem_if(AST_Node* node) {
    Expr_Type cond_type = check_node(node->left);
 
    if (cond_type.type != R_BOOLEAN && !is_nullable(cond_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

        if (first_stmt->type == NNULL_VAR_DECL) {
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL); 
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

    if (node->right == NULL) {
        ERROR_RET(ERR_PARSE);
    }
    
    check_node(node->right);

    return (Expr_Type){-1, false};
}

Expr_Type check_node(AST_Node* node) {
    if (node == NULL) {
        return (Expr_Type){-1, false};
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
        case ISLESSEQ:
        case ISMORE:
        case ISMOREEQ:
            return sem_check_binary_expression(node);
        case I32:
        case F64:
        case STRING:
        case ID:
            return get_literal_type(node);
        // statements
        case IF:             return sem_if(node);
        case FUNC_CALL:      return sem_func_call(node);
        case FUNCTION_DECL:  return sem_function_decl(node);
        case VAR_DECL:       return sem_var_decl(node);
        case VAR_ASSIGNMENT: return sem_var_assignment(node);
        case ELSE:           return sem_else(node);
        case NNULL_VAR_DECL: return sem_nnull_var_decl(node);
        case RETURN:         return sem_return(node);
        
        default:{
            printf("Unknown node type\n"); return (Expr_Type){-1, false};
        }
    }
}

// all errors are exits my brotha
void parse_check(AST_Node* node) {
    check_node(node);
}