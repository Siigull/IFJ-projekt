/**
 * IFJ PROJEKT 2024
 * @file parser_semantics.c
 * @author David Bujzaš (xbujzad00@stud.fit.vutbr.cz)
 * @brief Implementation of IFJ24 semantic analysis
 *
 * @date 2024-11-17
 */


#include "parser_semantics.h"

extern Parser* parser;

bool is_nullable_decl(Expr_Type declaration, Expr_Type expression) {
    if (!((declaration == N_F64 && expression == R_F64) ||
          (declaration == N_I32 && expression == R_I32) ||
          (declaration == N_U8 && expression == R_U8) ||
          (is_nullable(declaration) && expression == R_NULL))) {
                    return false;
        }
    return true;
}

bool null_in_relation_operators(Expr_Type left_side, Expr_Type right_side) {
	if ((left_side == R_NULL && right_side == R_NULL) ||
		(left_side == R_NULL && right_side == N_U8) ||
		(left_side == N_U8 && right_side == R_NULL) ||
		(left_side == N_I32 && right_side == R_NULL) ||
		(left_side == R_NULL && right_side == N_I32) ||
		(left_side == N_F64 && right_side == R_NULL) ||
		(left_side == R_NULL && right_side == N_F64) ||
		(left_side == N_I32 && right_side == N_I32) ||
		(left_side == N_F64 && right_side == N_F64) ||
		(left_side == N_I32 && right_side == R_I32) ||
		(left_side == R_I32 && right_side == N_I32) ||
		(left_side == N_F64 && right_side == R_F64) ||
		(left_side == R_F64 && right_side == N_F64)) {
			return true;
		}
	return false;
}

void check_func_call_stmt(AST_Node* node) {
    if (node->type == FUNC_CALL) {
        Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);
        if(func_entry == NULL) {
            ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
        }

        if (func_entry->ret_type != R_VOID) {
            ERROR_RET(ERR_SEM_RET_TYPE_DISCARD);
        }
    }
}

bool is_nullable(Expr_Type type) {
    switch(type) {
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
        case I32: return R_I32;
        case F64: return R_F64;
        case STRING: return R_STRING;
        case ID: {
            Entry* entry = tree_find(parser->s_table, node->as.var_name);
            if(entry == NULL) {
                ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
            }

            entry->was_used = true;
            return entry->ret_type;
        }
        case NIL: return R_NULL;
        default: return -1;
    }
}

bool is_numeric_type(Expr_Type type) {
    return (type == N_F64 || type == N_I32 || type == R_F64 || type == R_I32);
}

Expr_Type sem_check_binary_expression(AST_Node* node, Sem_State* state) {
    Expr_Type left_type = check_node(node->left, state);
    Expr_Type right_type = check_node(node->right, state);
	bool has_null_operations = null_in_relation_operators(left_type, right_type); // check if any operation has null or is nullable

	// check if the types are numeric
    if (!is_numeric_type(left_type) || !is_numeric_type(right_type)) {
		// if there are no null or nullable operations, return error
		if (!has_null_operations) {
        	ERROR_RET(ERR_SEM_TYPE_CONTROL);
		}
    }

    bool has_int = (left_type == R_I32 || right_type == R_I32);
    bool has_float = (left_type == R_F64 || right_type == R_F64);
	bool has_nullable = (is_nullable(left_type) || is_nullable(right_type));

	// check if there are any operations with null
	if (has_null_operations) {
		// null operations can only be used in "==" and "!=" operations
		if (node->type == ISEQ || node->type == ISNEQ) {
			// set the expression type to boolean and return it
			node->as.expr_type = (Expr_Type){R_BOOLEAN};
			return (Expr_Type){R_BOOLEAN};
		} else {
			ERROR_RET(ERR_SEM_TYPE_CONTROL);
		}
	} else {
		switch (node->type) {
			case PLUS:
			case MINUS:
			case MUL: {
				// nullable types are not allowed in these operations
				if (has_nullable) {
					ERROR_RET(ERR_SEM_TYPE_CONTROL);
				}
				if (has_float) {
                    if(has_int) {
                        ERROR_RET(ERR_SEM_TYPE_CONTROL);
                    }
					node->as.expr_type = (Expr_Type){R_F64};
					return (Expr_Type){R_F64};

				} else if (has_int && !has_float) {
					node->as.expr_type = (Expr_Type){R_I32};
					return (Expr_Type){R_I32};
				}
			}
			case DIV: {
				// nullable types or combination of float and int are not allowed in these operations
				if (has_nullable || (has_float && has_int)) {
					ERROR_RET(ERR_SEM_TYPE_CONTROL);
				}
				// if there is a float in the operation, set the expression type to float
				else if (has_float) {
					node->as.expr_type = (Expr_Type){R_F64};
					return (Expr_Type){R_F64};
				}
				// if there is no float in the operation, set the expression type to int
				node->as.expr_type = (Expr_Type){R_I32};
				return (Expr_Type){R_I32};
			}
			case ISEQ:
			case ISNEQ:
			case ISLESS:
			case ISLESSEQ:
			case ISMORE:
			case ISMOREEQ: {
				// nullable types are not allowed in these operations
				if (has_nullable && node->type != ISEQ && node->type != ISNEQ) {
        			ERROR_RET(ERR_SEM_TYPE_CONTROL);
    			}
				// if the types are the same, set the expression type to boolean
				if (left_type == right_type) {
					node->as.expr_type = (Expr_Type){R_BOOLEAN};
					return (Expr_Type){R_BOOLEAN};

				} else{
                    ERROR_RET(ERR_SEM_TYPE_CONTROL);
                }
			}
			default: ERROR_RET(ERR_SEM_TYPE_CONTROL);
		}
	}
}

bool is_writeable(AST_Node* node) {
    if (node->type == FUNC_CALL) {
        Entry* entry = tree_find(parser->s_table, node->as.func_data->var_name);
        if (entry->ret_type == R_VOID) {
            return false;
        }
    }
    return true;
}

Expr_Type sem_func_call(AST_Node* node, Sem_State* state) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

	// if the function call is defined
    if (func_entry != NULL) {
        size_t expected_params = func_entry->as.function_args->length;
        size_t actual_params = node->as.func_data->arr->length;

		// if the parameters do not match the expected parameters, return error
        if (expected_params != actual_params) {
            ERROR_RET(ERR_SEM_PARAMS);
        }
		// if the function call is ifj.write and the parameter is not writeable, return error
        if(!strcmp(node->as.func_data->var_name, "*write") && !is_writeable((AST_Node*)node->as.func_data->arr->data[0])){
            ERROR_RET(ERR_SEM_PARAMS);
        }
	// if the function call is not defined, return error
    } else {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    if (node->as.func_data->arr) {
		// go through all the parameters
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i]; // get parameter node
            Expr_Type current_expr_type = check_node(current_node, state); // get parameter type from the node
            Function_Arg* arg_node = (Function_Arg*)func_entry->as.function_args->data[i]; // get parameter from the function entry

			// if the parameter is a string, the expression type must be a string or []u8
			if (arg_node->type == R_TERM_STRING) {
				if (current_expr_type != R_STRING && current_expr_type != R_U8) {
					ERROR_RET(ERR_SEM_PARAMS);
				}
			// if the parameter is not a string, the expression type must match the parameter type
			} else if (current_expr_type != arg_node->type && arg_node->type != R_VOID) {
				// check if the function is nullable and the expression is of the same type but not nullable
                if (!is_nullable_decl(arg_node->type, current_expr_type)) {
                    ERROR_RET(ERR_SEM_PARAMS);
                }
            }
        }
    }

    return func_entry->ret_type;
}

Expr_Type sem_function_decl(AST_Node* node, Sem_State* state) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

	// if the function is not defined, return error
	if (func_entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    if (node->as.func_data->arr) {
		// go through all statements in the function
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* stmt = (AST_Node*)node->as.func_data->arr->data[i]; // current statement
            check_node(stmt, state); // check the current statement
            check_func_call_stmt(stmt); // special check if the statement is a function call
        }
    }

	// if there is no return in a non-void function, return error
    if (!state->seen_return && func_entry->ret_type != R_VOID) {
        ERROR_RET(ERR_SEM_RET_EXP);
    }

    return -1;
}

Expr_Type sem_var_decl(AST_Node* node, Sem_State* state) {
    Entry* entry = tree_find(parser->s_table, node->as.var_name);

	// if the variable is not defined, return error
    if (entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    Expr_Type declared_type = entry->ret_type;
    Expr_Type expression_type = check_node(node->left, state);

    if (declared_type != expression_type) {
		// if the declared type is implicit and the expression type is not null or void, set the declared type to the expression type
        if (declared_type == IMPLICIT && expression_type != R_NULL && expression_type != R_VOID) {
            entry->ret_type = expression_type;
		// if the expression type is a string or the declared types is implicit and expression type is null, return error
        } else if (expression_type == R_STRING || (declared_type == IMPLICIT && expression_type == R_NULL)) {
            ERROR_RET(ERR_SEM_TYPE_INFERENCE);
        } else {
            if (!is_nullable_decl(declared_type, expression_type)) {
				if (!(declared_type == R_F64 && expression_type == R_I32)) {
                	ERROR_RET(ERR_SEM_TYPE_CONTROL);
				}
            }
        }
	// if the expression type is void, return error
    } else if (expression_type == R_VOID) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    return -1;
}

Expr_Type sem_var_assignment(AST_Node* node, Sem_State* state) {
    Expr_Type expression_type = check_node(node->left, state);

	// if the variable name is not "_"
    if (strcmp(node->as.var_name, "_")) {
        Entry* entry = tree_find(parser->s_table, node->as.var_name);

		// if the variable is not defined, return error
        if (entry == NULL) {
            ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
        }

        Expr_Type declared_type = entry->ret_type;

		// if the variable is const, it cannot be modified
        if (!entry->as.can_mut) {
            ERROR_RET(ERR_SEM_ASSIGN_NON_MODIF);
        }

        if (declared_type != expression_type || expression_type == R_STRING) {
            if (!is_nullable_decl(declared_type, expression_type)) {
				if (!(declared_type == R_F64 && expression_type == R_I32)) {
                	ERROR_RET(ERR_SEM_TYPE_CONTROL);
				}
            }
        }

		// if the variable is var, set the usage and assignment flags to true
        if (entry->as.can_mut) {
            entry->was_assigned = true;
            entry->was_used = true;
        }
    }

    return -1;
}

Expr_Type sem_else(AST_Node* node, Sem_State* state) {
	// go through all statements in the else block
    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i]; // current statement
        check_node(stmt, state); // check the current statement
        check_func_call_stmt(stmt); // special check if the statement is a function call
    }

    return -1;
}

Expr_Type sem_nnull_var_decl(AST_Node* node, Sem_State* state) {
    return -1;
}

Expr_Type sem_return(AST_Node* node, Sem_State* state) {
    Entry* entry = tree_find(parser->s_table, state->func_name);

	// if the function is not defined, return error
    if (entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

	// if the expression is not null and the function has a void return type, return error
    if (node->left != NULL && entry->ret_type == R_VOID) {
        ERROR_RET(ERR_SEM_RET_EXP);

	// if the expression is null and the function has a non-void return type, return error
    } else if (node->left == NULL && entry->ret_type != R_VOID) {
        ERROR_RET(ERR_SEM_RET_EXP);

    } else if (node->left != NULL) {
        Expr_Type expression_type = check_node(node->left, state);

		// if the expression type does not match the return type, return error
        if (expression_type != entry->ret_type) {
			// if the return type is not nullable and the expression type is not the same type but non nullable, return error
            if (!is_nullable_decl(entry->ret_type, expression_type)) {
                ERROR_RET(ERR_SEM_RET_TYPE_DISCARD);
            }
        }
    }

	// set the seen return flag to true
    state->seen_return = true;

    return -1;
}

Expr_Type sem_while(AST_Node* node, Sem_State* state) {
    Expr_Type cond_type = check_node(node->left, state);

	// if the condition type is not boolean and not nullable, return error
    if (cond_type != R_BOOLEAN && !is_nullable(cond_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

		// if the first statement is a non-nullable variable declaration
        if (first_stmt->type == NNULL_VAR_DECL) {
			// if the condition type is not nullable, return error
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL);
            }

			// non-nullable variable declaration
            Entry* nnull_entry = tree_find(parser->s_table, first_stmt->as.var_name);
            nnull_entry->ret_type = null_to_nnul(cond_type);

			// go through all statements in the while block
            for (size_t i = 1; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i]; // current statement
                check_node(stmt, state); // check the current statement
                check_func_call_stmt(stmt); // special check if the statement is a function call
            }
		// if the first statement is not a non-nullable variable declaration
        } else {
			// if the condition type is not boolean, return error
			if (cond_type != R_BOOLEAN) {
				ERROR_RET(ERR_SEM_TYPE_CONTROL)
			}
			// go through all statements in the while block
            for (size_t i = 0; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i]; // current statement
                check_node(stmt, state); // check the current statement
                check_func_call_stmt(stmt); // special check if the statement is a function call
            }
        }
    } else {
		// if the condition type is not boolean, return error
		if (cond_type != R_BOOLEAN) {
			ERROR_RET(ERR_SEM_TYPE_CONTROL);
		}
	}

    return -1;
}

Expr_Type sem_if(AST_Node* node, Sem_State* state) {
    Expr_Type cond_type = check_node(node->left, state);

	// if the condition type is not boolean and not nullable, return error
    if (cond_type != R_BOOLEAN && !is_nullable(cond_type)) {
        ERROR_RET(ERR_SEM_TYPE_CONTROL);
    }

    if (node->as.arr->length > 0) {
        AST_Node* first_stmt = (AST_Node*)node->as.arr->data[0];

		// if the first statement is a non-nullable variable declaration
        if (first_stmt->type == NNULL_VAR_DECL) {
            if (!is_nullable(cond_type)) {
                ERROR_RET(ERR_SEM_TYPE_CONTROL);
            }

			// non-nullable variable declaration
            Entry* nnull_entry = tree_find(parser->s_table, first_stmt->as.var_name);
            nnull_entry->ret_type = null_to_nnul(cond_type);

			// go through all statements in the if block
            for (size_t i = 1; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
                check_func_call_stmt(stmt);
            }
		// if the first statement is not a non-nullable variable declaration
        } else {
			// if the condition type is not boolean, return error
			if (cond_type != R_BOOLEAN) {
				ERROR_RET(ERR_SEM_TYPE_CONTROL)
			}
			// go through all statements in the if block
            for (size_t i = 0; i < node->as.arr->length; i++) {
                AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
                check_node(stmt, state);
                check_func_call_stmt(stmt);
            }
        }
    } else {
		// if the condition type is not boolean, return error
		if (cond_type != R_BOOLEAN) {
			ERROR_RET(ERR_SEM_TYPE_CONTROL);
		}
	}

	// if there is no else block in the if statement, return error
    if (node->right == NULL) {
        ERROR_RET(ERR_PARSE);
    }

	// go through all statements in the else block
    check_node(node->right, state);

    return -1;
}

Expr_Type check_node(AST_Node* node, Sem_State* state) {
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
            fprintf(stderr ,"Unknown node type\n"); return -1;
        }
    }
}

Ret val_if(AST_Node* node, Sem_State* state) {
    Ret ret = check_node_2(node->left, state);
    check_node_2(node->right, state);

    state->expr_type = ret.type;

    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
        check_node_2(stmt, state);
    }

    return (Ret){R_VOID, false, 0, 0};
}

bool can_implicit(Ret val) {
    if(val.type == R_F64 && val.is_const) {
        return (val.as.f64 - (int)val.as.f64) == 0.0;
    }

    return false;
}

Ret val_literal_type(AST_Node* node, Sem_State* state) {
    Ret t;
    t.is_const = true;
    t.type = R_VOID;
    t.is_lit = false;

    switch(node->type) {
        case I32:
        {
            t.type = R_I32;
            t.as.i32 = node->as.i32;
            t.is_lit = true;
            break;
        }
        case F64:
        {
            t.type = R_F64;
            t.as.f64 = node->as.f64;
            t.is_lit = true;
            break;
        }
        case STRING:
            break;

        case ID: {
            Entry* entry = tree_find(parser->s_table, node->as.var_name);

            if(entry == NULL) {
                ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
            }

            t.type = entry->ret_type;

            entry->was_used = true;

            // Replace with value if var contains const expression
            if(entry->is_const_val && !is_nullable(entry->ret_type)) {
                if(t.type == R_I32) {
                    node->type = I32;
                    node->as.i32 = entry->const_val.i32;
                    node->left = NULL;
                    node->right = NULL;
                    t.as.i32 = entry->const_val.i32;

                } else if(t.type == R_F64) {
                    node->type = F64;
                    node->as.f64 = entry->const_val.f64;
                    node->left = NULL;
                    node->right = NULL;
                    t.as.f64 = entry->const_val.f64;

                } else {
                    return (Ret){t.type, false, 0, 0};
                }
            } else {
                t.is_const = false;
            }
            break;
        }
        case NIL: {
            return (Ret){R_NULL, false, 0, 0};
        }
        default:
            t.is_const = false;
            break;
    }

    return t;
}

void implicit_f64_i32(Ret* l, Ret* r, AST_Node* node) {
    if(null_to_nnul(l->type) != null_to_nnul(r->type)) {
        if(can_implicit(*l)) {
            l->type = R_I32;
            l->as.i32 = (int)l->as.f64;
            node->left->type = I32;
            node->left->as.i32 = (int)node->left->as.f64;

        } else if(can_implicit(*r)) {
            r->type = R_I32;
            r->as.i32 = (int)r->as.f64;
            node->right->type = I32;
            node->right->as.i32 = (int)node->right->as.f64;
        }
    }
}

void implicit_i32_f64_lit(Ret* l, Ret* r, AST_Node* node) {
    if(null_to_nnul(l->type) != null_to_nnul(r->type)) {
        if(l->type == R_I32 && l->is_lit) {
            l->type = R_F64;
            l->as.f64 = l->as.i32;
            node->left->type = F64;
            node->left->as.f64 = (double)node->left->as.i32;

        } else if(r->type == R_I32 && r->is_lit) {
            r->type = R_F64;
            r->as.f64 = r->as.i32;
            node->right->type = F64;
            node->right->as.f64 = (double)node->right->as.i32;
        }
    }
}

void implicit_i32_f64(Ret* l, Ret* r, AST_Node* node) {
    if(null_to_nnul(l->type) != null_to_nnul(r->type)) {
        if(l->type == R_I32 && l->is_const) {
            l->type = R_F64;
            l->as.f64 = l->as.i32;
            node->left->type = F64;
            node->left->as.f64 = (double)node->left->as.i32;

        } else if(r->type == R_I32 && r->is_const) {
            r->type = R_F64;
            r->as.f64 = r->as.i32;
            node->right->type = F64;
            node->right->as.f64 = (double)node->right->as.i32;
        }
    }
}

Ret val_binary_expression(AST_Node* node, Sem_State* state) {
#define OPERATE(left, right, op)             \
    if((left).type == R_F64) {               \
        if((right).type == R_F64) {          \
            (left).as.f64 = (left).as.f64 op (right).as.f64; \
            (left).type = R_F64;             \
        } else {                             \
            ERROR_RET(ERR_SEM_TYPE_CONTROL);                    \
        }                                    \
    } else if((left).type == R_I32){         \
        if((right).type == R_I32) {          \
            (left).as.i32 = (left).as.i32 op (right).as.i32; \
            (left).type = R_I32;             \
        } else {                             \
            ERROR_RET(ERR_SEM_TYPE_CONTROL);                    \
        }                                    \
    } else {                                 \
        ERROR_RET(ERR_SEM_TYPE_CONTROL);                        \
    }                                        \

    Ret ret_left = check_node_2(node->left, state);
    Ret ret_right = check_node_2(node->right, state);

    // conversions
    switch (node->type) {
        case PLUS:
        case MINUS:
        case MUL: {
            implicit_f64_i32(&ret_left, &ret_right, node);
            implicit_i32_f64_lit(&ret_left, &ret_right, node);
            break;
        }
        case DIV: {
            implicit_f64_i32(&ret_left, &ret_right, node);
            break;
        }
        case ISEQ:
        case ISNEQ:
        case ISLESS:
        case ISLESSEQ:
        case ISMORE:
        case ISMOREEQ:
            implicit_f64_i32(&ret_left, &ret_right, node);
            implicit_i32_f64(&ret_left, &ret_right, node);

            return (Ret){R_BOOLEAN, false, 0, 0};
        default:
            break;
    }

    // relational and arithmetic operations
    if(ret_left.is_const && ret_right.is_const) {
        switch (node->type) {
            case PLUS: {
                OPERATE(ret_left, ret_right, +);
                break;
            }
            case MINUS: {
                OPERATE(ret_left, ret_right, -);
                break;
            }
            case MUL: {
                OPERATE(ret_left, ret_right, *);
                break;
            }
            case DIV: {
                Ret left_og;
                memcpy(&left_og, &ret_left, sizeof(Ret));

                if(ret_left.type != ret_right.type) {
                    return (Ret){R_VOID, false, 0, 0};
                }

                OPERATE(ret_left, ret_right, /);

                // div floor
                if(ret_left.type == R_I32 &&
                left_og.as.i32 != 0 &&
                ret_left.as.i32 <= 0 &&
                (left_og.as.i32 >= 0 != ret_right.as.i32 >= 0)) {
                    if(left_og.as.i32 % ret_right.as.i32) {
                        ret_left.as.i32--;
                    }
                }
                break;
            }
            case ISEQ:
            case ISNEQ:
            case ISLESS:
            case ISLESSEQ:
            case ISMORE:
            case ISMOREEQ:
                return (Ret){R_BOOLEAN, false, 0, 0};
            default:
                break;
        }

        if(ret_left.type == R_I32) {
            node->type = I32;
            node->as.i32 = ret_left.as.i32;
            node->left = NULL;
            node->right = NULL;

        } else {
            node->type = F64;
            node->as.f64 = ret_left.as.f64;
            node->left = NULL;
            node->right = NULL;
        }

    } else {
        ret_left.is_const = false;
    }

    return ret_left;

#undef OPERATE
}

Ret val_func_call(AST_Node* node, Sem_State* state) {
    Entry* func_entry = tree_find(parser->s_table, node->as.func_data->var_name);

    if(func_entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
        if(i >= func_entry->as.function_args->length) {
            return (Ret){R_VOID, false, 0, 0};
        }

        AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
        Function_Arg* current_arg = (Function_Arg*)func_entry->as.function_args->data[i];
        Ret ret = check_node_2(current_node, state);

        if(can_implicit(ret) && current_arg->type == R_I32) {
            current_node->type = I32;
            current_node->as.i32 = (int)current_node->as.f64;
        }
    }

    return (Ret){func_entry->ret_type, false, 0, 0};
}

Ret val_function_decl(AST_Node* node, Sem_State* state) {
    if (node->as.func_data->arr) {
        for (size_t i = 0; i < node->as.func_data->arr->length; i++) {
            AST_Node* current_node = (AST_Node*)node->as.func_data->arr->data[i];
            check_node_2(current_node, state);
        }
    }

    return (Ret){R_VOID, false, 0, 0};
}

Ret val_var_decl(AST_Node* node, Sem_State* state) {
    Entry* entry = tree_find(parser->s_table, node->as.var_name);

    Ret ret = check_node_2(node->left, state);
    if(entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    if(entry->ret_type == IMPLICIT) {
        if(ret.type == R_NULL) {
            ERROR_RET(ERR_SEM_TYPE_INFERENCE);
        }
        entry->ret_type = ret.type;
    }

    if(ret.is_const) {
        if(can_implicit(ret) && entry->ret_type == R_I32) {
            node->left->type = I32;
            node->left->as.i32 = (int)ret.as.f64;

            ret.type = R_I32;
            ret.as.i32 = (int)ret.as.f64;

        } else if(ret.type == R_I32 && entry->ret_type == R_F64) {
            node->left->type = F64;
            node->left->as.f64 = node->left->as.i32;

            ret.type = R_F64;
            ret.as.f64 = (double)ret.as.i32;
        }

        if(ret.type != R_VOID && ret.is_const && !entry->as.can_mut) {

            if(ret.type == R_I32 && entry->ret_type == R_I32) {
                entry->is_const_val = true;
                entry->const_val.i32 = ret.as.i32;

            } else if(ret.type == R_F64 && entry->ret_type == R_F64){
                entry->is_const_val = true;
                entry->const_val.f64 = ret.as.f64;
            }
        }
    }

    return (Ret){R_VOID, false, 0, 0};
}

Ret val_var_assignment(AST_Node* node, Sem_State* state) {
    Ret ret = check_node_2(node->left, state);

    return (Ret){R_VOID, false, 0, 0};
}

Ret val_else(AST_Node* node, Sem_State* state) {
    for (size_t i = 0; i < node->as.arr->length; i++) {
        AST_Node* stmt = (AST_Node*)node->as.arr->data[i];
        check_node_2(stmt, state);
    }

    return (Ret){R_VOID, false, 0, 0};
}

Ret val_nnull_var_decl(AST_Node* node, Sem_State* state) {
    Entry* entry = tree_find(parser->s_table, node->as.string);

    if(entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    entry->ret_type = null_to_nnul(state->expr_type);

    return (Ret){R_VOID, false, 0, 0};
}

Ret val_return(AST_Node* node, Sem_State* state) {
    Ret ret = check_node_2(node->left, state);
    Entry* entry = tree_find(parser->s_table, state->func_name);
    if(entry == NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }

    if(can_implicit(ret) && entry->ret_type == R_I32) {
        node->left->type = I32;
        node->left->as.i32 = (int)node->left->as.f64;
    }

    return (Ret){R_VOID, false, 0, 0};
}

Ret check_node_2(AST_Node* node, Sem_State* state) {
    if (node == NULL) {
        return (Ret){R_VOID, false, 0, 0};
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
            return val_binary_expression(node, state);
        case I32:
        case F64:
        case STRING:
        case ID:
        case NIL:
            return val_literal_type(node, state);
        // statements
        case IF:             return val_if(node, state);
        case WHILE:          return val_if(node, state);
        case FUNC_CALL:      return val_func_call(node, state);
        case FUNCTION_DECL:  return val_function_decl(node, state);
        case VAR_DECL:       return val_var_decl(node, state);
        case VAR_ASSIGNMENT: return val_var_assignment(node, state);
        case ELSE:           return val_else(node, state);
        case NNULL_VAR_DECL: return val_nnull_var_decl(node, state);
        case RETURN:         return val_return(node, state);

        default:{
            fprintf(stderr, "Unknown node type\n"); return (Ret){R_VOID, 0};
        }
    }
}

void check_semantics(AST_Node* node) {
    // First pass
    // constant value evaluation
    Sem_State state_val;
    state_val.func_name = node->as.func_data->var_name;
    check_node_2(node, &state_val);

    // Second pass
    Sem_State state;
    state.func_name = node->as.func_data->var_name;
    state.seen_return = false;

    if (!strcmp(state.func_name, "main")) {
        Entry* func_entry = tree_find(parser->s_table, state.func_name);
        Expr_Type func_type = func_entry->ret_type;

        if (func_type != R_VOID || func_entry->as.function_args->length != 0) {
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
