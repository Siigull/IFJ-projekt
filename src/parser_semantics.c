/**
 * @file parser_semantics.c
 * @author David Bujzaš (xbujzad00@stud.fit.vutbr.cz)
 * @brief Implementation of IFJ24 semantic analysis
 * 
 * @date 2024-11-17
 * @todo implicit float conversion, must have return
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
            entry->was_used = true;
            return entry->ret_type;
        }
        case NIL: return (Expr_Type){R_NULL, true};
        default: return (Expr_Type){-1, false};
    }
}

bool is_numeric_type(Expr_Type type) {
    return (type.type == N_F64 || type.type == N_I32 || type.type == R_F64 || type.type == R_I32);
}

Expr_Type sem_check_binary_expression(AST_Node* node, sem_state* state) {
    Expr_Type left_type = check_node(node->left, state);
    Expr_Type right_type = check_node(node->right, state);

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
                    node->left->type = F64;
                    node->left->as.f64 = (double)node->left->as.i32;
                } else if (right_type.type == R_I32) {
                    node->right->type = F64;
                    node->right->as.f64 = (double)node->right->as.i32;
                }
                node->as.expr_type = (Expr_Type){R_BOOLEAN, is_node_const};
                return (Expr_Type){R_BOOLEAN, is_node_const};
            }
        default: ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }
}

Expr_Type sem_func_call(AST_Node* node, sem_state* state) {
    
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if (func_entry != NULL) {
        size_t expected_params = func_entry->as.function_args->length;
        size_t actual_params = node->as.func_data->arr->length;
        if (expected_params != actual_params) {
            ERROR_RET(ERR_SEM_PARAMS);
        }
    }

    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
            Expr_Type current_expr_type = check_node(current_node, state);
            Function_Arg* arg_node = (Function_Arg*)func_entry->as.function_args->data[i];

            if (current_expr_type.type != arg_node->type && arg_node->type != R_VOID) {
                ERROR_RET(ERR_SEM_PARAMS);    
            }
        }
    }

    return func_entry->ret_type;
}

Expr_Type sem_function_decl(AST_Node* node, sem_state* state) {
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
            check_node(current_node, state);
        }
    }

    if (!state->seen_return && func_entry->ret_type.type != R_VOID) {
        ERROR_RET(ERR_SEM_RET_EXP);
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_var_decl(AST_Node* node, sem_state* state) {
    Entry* entry = tree_find(parser->s_table, node->as.var_name);

    Expr_Type declared_type = entry->ret_type;
    Expr_Type expression_type = check_node(node->left, state);

    if (declared_type.type != expression_type.type) {
        if (declared_type.type == IMPLICIT) {
            entry->ret_type = expression_type;
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

Expr_Type sem_var_assignment(AST_Node* node, sem_state* state) {
    Expr_Type expression_type = check_node(node->left, state);

    if (strcmp(node->as.var_name, "_")) {
        Entry* entry = tree_find(parser->s_table, node->as.var_name);
        Expr_Type declared_type = entry->ret_type;

        if (!entry->as.can_mut) {
            ERROR_RET(ERR_SEM_ASSIGN_NON_MODIF);
        }

        if (declared_type.type != expression_type.type || expression_type.type == R_STRING) {
            if (!is_nullable(declared_type) && expression_type.type == R_NULL) ERROR_RET(ERR_SEM_TYPE_CONTROL);
        }

        if (entry->as.can_mut) {
            entry->was_assigned = true;
            entry->was_used = true;
        }
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_else(AST_Node* node, sem_state* state) {
    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
        check_node(stmt, state);
    }
    
    return (Expr_Type){-1, false};
}

Expr_Type null_to_nnul(Expr_Type type) {
    return (Expr_Type){type.type - 1, type.is_const_literal};
}

Expr_Type sem_nnull_var_decl(AST_Node* node, sem_state* state) {
    //TODO(Sigull) This has to be from if node not this one

    // if (!is_nullable(entry->ret_type)) {
    //     ERROR_RET(ERR_SEM_TYPE_CONTROL);
    // }

    // Entry* new_entry = entry_init(node->as.var_name, E_VAR, entry->ret_type, false);

    // entry->ret_type = (Expr_Type){R_I32, false};

    return (Expr_Type){-1, false}; 
}

Expr_Type sem_return(AST_Node* node, sem_state* state) {
    Entry* entry = tree_find(parser->s_table, state->func_name);

    if (node->left != NULL && entry->ret_type.type == R_VOID) {
        ERROR_RET(ERR_SEM_RET_EXP);
    }

    if (node->left != NULL) {
        Expr_Type expression_type = check_node(node->left, state);

        if (expression_type.type != entry->ret_type.type) {
            ERROR_RET(ERR_SEM_RET_EXP);
        }
    }

    state->seen_return = true;
}

Expr_Type sem_while(AST_Node* node, sem_state* state) {
    Expr_Type cond_type = check_node(node->left, state);
 
    if (cond_type.type != R_BOOLEAN && !is_nullable(cond_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

        if (first_stmt->type == NNULL_VAR_DECL) {
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL); 
            }

            Entry* nnull_entry = tree_find(parser->s_table, first_stmt->as.var_name);
            nnull_entry->ret_type = null_to_nnul(cond_type);

            for (size_t i = 1; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
            }
        } else {
            for (size_t i = 0; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
            }
        }
    }

    return (Expr_Type){-1, false};
}

Expr_Type sem_if(AST_Node* node, sem_state* state) {
    Expr_Type cond_type = check_node(node->left, state);
 
    if (cond_type.type != R_BOOLEAN && !is_nullable(cond_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

        if (first_stmt->type == NNULL_VAR_DECL) {
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL); 
            }

            Entry* nnull_entry = tree_find(parser->s_table, first_stmt->as.var_name);
            nnull_entry->ret_type = null_to_nnul(cond_type);


            for (size_t i = 1; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
            }
        } else {
            for (size_t i = 0; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
            }
        }
    }

    if (node->right == NULL) {
        ERROR_RET(ERR_PARSE);
    }
    
    check_node(node->right, state);

    return (Expr_Type){-1, false};
}

Expr_Type check_node(AST_Node* node, sem_state* state) {
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
            return sem_check_binary_expression(node, state);
        case I32:
        case F64:
        case STRING:
        case ID:
        case NIL:
            return get_literal_type(node);
        // statements
        case IF:             return sem_if(node, state);
        case WHILE:          return sem_while(node, state);
        case FUNC_CALL:      return sem_func_call(node, state);
        case FUNCTION_DECL:  return sem_function_decl(node, state);
        case VAR_DECL:       return sem_var_decl(node, state);
        case VAR_ASSIGNMENT: return sem_var_assignment(node, state);
        case ELSE:           return sem_else(node, state);
        case NNULL_VAR_DECL: return sem_nnull_var_decl(node, state);
        case RETURN:         return sem_return(node, state);
        
        default:{
            printf("Unknown node type\n"); return (Expr_Type){-1, false};
        }
    }
}

void check_semantics(AST_Node* node) {
    sem_state state;
    state.func_name = node->as.func_data->var_name;
    state.seen_return = false;

    if (!strcmp(state.func_name, "main")) {
        Entry* func_entry = tree_find(parser->s_table, state.func_name);
        Expr_Type func_type = func_entry->ret_type;

        if (func_type.type != R_VOID || func_entry->as.function_args->length != 0) {
            ERROR_RET(ERR_SEM_PARAMS);
        }
    }

    check_node(node, &state);
}

void check_var_usage_traverse(Node* node) {
    if (node == NULL) return;
    Entry* entry = node->entry;

    if (entry->type == E_VAR && (!entry->was_used || (entry->as.can_mut && !entry->was_assigned))) {
        ERROR_RET(ERR_SEM_MODIF_VAR);
    }

    check_var_usage_traverse(node->left);
    check_var_usage_traverse(node->right);
}

void check_var_usage(Tree* table) {
    if (table == NULL) return;
    check_var_usage_traverse(table->root);
}

// all errors are exits my brotha
void parse_check(AST_Node* node, sem_state* state) {
    check_node(node, state);
}