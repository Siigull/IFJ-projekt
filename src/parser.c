/**
 * IFJ PROJEKT 2024
 * @file parser.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief
 *
 */

#include "parser.h"

#if DEBUG
	#include "test_generate_graph.h"
#endif

Lexer* lexer;
Parser* parser;
Token* debug_token_array;
int token_index = 0;

AST_Node* binary_load(List* tl);
AST_Node* binary();
AST_Node* func_call();
AST_Node* stmt();
Ret_Type_ get_ret_type();

Expr_Type null_to_nnul(Expr_Type type) {
    if(type.type == N_I32 || type.type == N_F64 || type.type == N_U8) {
        return (Expr_Type){type.type - 1, type.is_const_literal};
    }

    return type;
}

void parser_reset() {
    parser->next = NULL;
    parser->prev = NULL;
}

Parser* init_parser() {
	Parser* parser = malloc(sizeof(Parser));

	parser->next = NULL;
	parser->prev = NULL;

	parser->s_table = tree_init();
    parser->c_stack = init_c_stack(parser->s_table);
    parser->c_stack.global_table = parser->s_table;

	return parser;
}

void advance() {
	free(parser->prev); // TODO(Sigull): Is this really how this is freed ?
	parser->prev = parser->next;
	parser->next = get_next_token(lexer);

	// parser->prev = parser->next;
	// parser->next = &(debug_token_array[token_index++]);
}

void consume(T_Type type) {
	if (parser->next->type == type) {
		advance();

    } else {
        ERROR_RET(ERR_PARSE);
    }
}

bool check(T_Type type) {
	return parser->next->type == type;
}

bool is_hexa(char x) {
	return isdigit(x) || (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f');
}

int parse_escape(const char* in, char* out) {
	int x;
	if (is_hexa(in[2]) && is_hexa(in[3])) {
		sscanf(in + 2, "%2x", &x);

    } else {
        ERROR_RET(ERR_LEX);
    }

	return x;
}

char* parse_string_value(const char* string) {
	int len = strlen(string);
	int new_len = len;

    for(int i=0; i < len; i++) {
        if(string[i] == '\\') {
            switch(string[i+1]) {
                case 'x':
                    if(i+3 >= len) {
                        ERROR_RET(ERR_LEX);
                    }
                    break;
                case 'n':
                case 'r':
                case 't':
                case '"':
                    new_len--;
                    break;
                case '\\':
                    new_len--;
                    i++;
                    break;
                default:
                    ERROR_RET(ERR_LEX);
                    break;
            }
        }
    }

	char* new_string = malloc(sizeof(char) * new_len + 1);
	char* out = new_string;

	for (int i = 0; i < len; i++) {
		if (string[i] == '\\') {
			switch (string[i + 1]) {
			case 'x': {
				int x = parse_escape(string + i, new_string);
				new_string[0] = (char) (x);
				i += 2;
				break;
			}
			case 'n':
				new_string[0] = '\n';
				break;
			case '\\':
				new_string[0] = '\\';
				break;
			case '"':
				new_string[0] = '"';
				break;
			case 'r':
				new_string[0] = '\r';
				break;
			case 't':
				new_string[0] = '\t';
				break;
			}
			i++;
			new_string++;

		} else {
			new_string[0] = string[i];
			new_string += 1;
		}
	}
	new_string[0] = '\0';

	return out;
}

char* string_to_assembly(const char* string) {
	int len = strlen(string);
	int new_len = len;

	for (int i = 0; i < len; i++) {
		if (string[i] < 32 || string[i] == ' ' || string[i] == '#' || string[i] == '\\') {
			new_len += 3;
		}
	}

	char* new_string = malloc(sizeof(char) * new_len + 1);
	char* out = new_string;

	for (int i = 0; i < len; i++) {
		if (string[i] < 32 || string[i] == ' ' || string[i] == '#' || string[i] == '\\') {
			sprintf(new_string, "\\%03d", string[i]);
			new_string += 4;

		} else {
			new_string[0] = string[i];
			new_string += 1;
		}
	}
	new_string[0] = '\0';

	return out;
}

AST_Node* string() {
	AST_Node* node = node_init(STRING);
	node->as.string = parse_string_value(parser->prev->value);

	return node;
}

 AST_Node* literal_load(List* tl) {
     Token* token = malloc(sizeof(Token));
     memcpy(token, parser->next, sizeof(Token));
     List_insertF(tl, token);

     if (check(T_ID)) {
		advance();
		if(check(T_RPAR)) {
			AST_Node* node = func_call();
			token->node = node;
			token->type = T_BUILDIN;

		} else {
			Entry* entry = context_find(&(parser->c_stack), token->value, false);
			if(entry != NULL) {
				token->value = (char*)entry->key;
			} else {
				ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
			}
		}

     } else if (check(T_STRING)) {
         advance();
         AST_Node* node = string();
         return node;

     } else if (check(T_I32) || check(T_F64) ||
                check(T_U8)  || check(T_NULL)) {
        advance();

	} else if (check(T_RPAR)) {
		advance();
		binary_load(tl);
		consume(T_LPAR);
		Token* token = malloc(sizeof(Token));
		memcpy(token, parser->prev, sizeof(Token));
		List_insertF(tl, token);

	} else if(check(T_BUILDIN)){
		advance();
		AST_Node* node = func_call();
		token->node = node;

     } else {
         ERROR_RET(ERR_PARSE);
     }

     return NULL;
 }

bool check_operator() {
    T_Type operator_types[10] = {T_PLUS, T_MINUS, T_MUL, T_DIV, T_DDEQ, T_NEQUAL, T_STHAN, T_GTHAN, T_SETHAN, T_GETHAN};
    int len = sizeof(operator_types) / sizeof(T_Type);
     for (int i=0; i < len; i++) {
         if (check(operator_types[i])){
             return true;
         }
     }

     return false;
 }

 AST_Node* binary_load(List* tl) {
     AST_Node* node = literal_load(tl);
     if (node != NULL) {
        return node;
     }

     while(check_operator()) {
         advance();
         Token* token = malloc(sizeof(Token));
         memcpy(token, parser->prev, sizeof(Token));
         List_insertF(tl, token);
         literal_load(tl);
     }

     return NULL;
}

AST_Node* literal() {
	if (check(T_ID)) {
		advance();
		if (check(T_RPAR)) {
			AST_Node* node = func_call();
			return node;
		}
		AST_Node* node = node_init(ID);
		node->as.var_name = parser->prev->value;
		return node;

	} else if (check(T_STRING)) {
		advance();
		return string();

    } else if (check(T_I32)) {
        advance();
        errno = 0;
        char* end;
        AST_Node* node = node_init(I32);
        node->as.i32 = strtol(parser->prev->value, &end, 10);
        if(errno == ERANGE || *end != '\0' ||
           node->as.i32 > INT_MAX || node->as.i32 < INT_MIN) {
            ERROR_RET(ERR_SEM_OTHER);
        }
        return node;

    } else if (check(T_F64)) {
        advance();
        errno = 0;
        char* end;
        AST_Node* node = node_init(F64);
        node->as.f64 = strtod(parser->prev->value, &end);
        if(errno == ERANGE || *end != '\0') {
            ERROR_RET(ERR_SEM_OTHER);
        }
        return node;

    } else if (check(T_NULL)) {
        advance();
        AST_Node* node = node_init(NIL);
        return node;

	} else if (check(T_RPAR)) {
		advance();
		AST_Node* node = binary();
		consume(T_LPAR);
		return node;

	} else if (check(T_BUILDIN)) {
		advance();
		AST_Node* node = func_call();
		return node;

    } else {
        ERROR_RET(ERR_PARSE);
    }
}

AST_Node* scaling() {
	AST_Node* left = literal();

	while (true) {
		if (check(T_MUL)) {
			advance();
			AST_Node* right = scaling();
			AST_Node* node = node_init(MUL);
			node->left = left;
			node->right = right;
			left = node;

		} else if (check(T_DIV)) {
			advance();
			AST_Node* right = scaling();
			AST_Node* node = node_init(DIV);
			node->left = left;
			node->right = right;
			left = node;

		} else {
			break;
		}
	}

	return left;
}

AST_Node* linear() {
	AST_Node* left = scaling();

	while (true) {
		if (check(T_PLUS)) {
			advance();
			AST_Node* right = scaling();
			AST_Node* node = node_init(PLUS);
			node->left = left;
			node->right = right;
			left = node;

		} else if (check(T_MINUS)) {
			advance();
			AST_Node* right = scaling();
			AST_Node* node = node_init(MINUS);
			node->left = left;
			node->right = right;
			left = node;

		} else {
			break;
		}
	}

	return left;
}

AST_Type get_3_type() {
	T_Type ops[] = {T_DDEQ, T_NEQUAL, T_STHAN, T_GTHAN, T_SETHAN, T_GETHAN};
	AST_Type types[] = {ISEQ, ISNEQ, ISLESS, ISMORE, ISLESSEQ, ISMOREEQ};
	for (int i = 0; i < 6; i++) {
		if (check(ops[i]))
			return types[i];
	}

	return -1;
}

AST_Node* binary() {
	AST_Node* left = linear();

	while (true) {
		AST_Type type = get_3_type();
		if ((int) type != -1) {
			advance();
			AST_Node* right = linear();
			AST_Node* node = node_init(type);
			node->right = right;
			node->left = left;
			left = node;

		} else {
			break;
		}
	}

	return left;
}

// Expression parser
AST_Node* expr() {
	List* token_list = malloc(sizeof(List));
	List_init(token_list);

	AST_Node* node = binary_load(token_list);
	if (node == NULL) {
		node = parse_expression(token_list);
	}

	return node;

    //return binary();
}

// Normal parser
void block(Arr* stmts) {
	consume(T_CUYRBRACKET);

	while (!check(T_CUYLBRACKET)) {
		arr_append(stmts, (size_t) stmt());
	}

	consume(T_CUYLBRACKET);
}

AST_Node* _else() {
	AST_Node* node = node_init(ELSE);

	consume(T_ELSE);

    context_push(&(parser->c_stack));
	block(node->as.arr);
    context_pop(&(parser->c_stack));

	return node;
}

AST_Node* _while() {
	AST_Node* node = node_init(WHILE);

	consume(T_WHILE);
	consume(T_RPAR);

	if (!check(T_LPAR)) {
		node->left = expr();

    } else {
        ERROR_RET(ERR_PARSE);
    }

	consume(T_LPAR);

    context_push(&(parser->c_stack));
	if (check(T_BAR)) {
		advance();

		consume(T_ID);
		AST_Node* var = node_init(NNULL_VAR_DECL);
		var->as.var_name = parser->prev->value;
        Entry* entry = entry_init(var->as.var_name, E_VAR, (Expr_Type){IMPLICIT, false}, false, false, false);
        context_put(&(parser->c_stack), entry);
		arr_append(node->as.arr, (size_t) var);

		consume(T_BAR);
	}

	block(node->as.arr);
    context_pop(&(parser->c_stack));

	return node;
}

AST_Node* _if() {
	AST_Node* node = node_init(IF);

	consume(T_IF);
	consume(T_RPAR);

    if (!check(T_LPAR)) {
        node->left = expr();
    } else {
        ERROR_RET(ERR_PARSE);
    }

	consume(T_LPAR);

    context_push(&(parser->c_stack));
	if (check(T_BAR)) {
		advance();

		consume(T_ID);
		AST_Node* var = node_init(NNULL_VAR_DECL);
		var->as.var_name = parser->prev->value;
        Entry* entry = entry_init(var->as.var_name, E_VAR, (Expr_Type){IMPLICIT, false}, false, false, false);
        context_put(&(parser->c_stack), entry);
		arr_append(node->as.arr, (size_t) var);

		consume(T_BAR);
	}

	block(node->as.arr);
    context_pop(&(parser->c_stack));

	if (check(T_ELSE)) {
		node->right = _else();
	}

	return node;
}

AST_Node* under_var_decl() {
	AST_Node* node = node_init(VAR_ASSIGNMENT);

	consume(T_UNDER);
	node->as.var_name = parser->prev->value;

	consume(T_EQUAL);
	node->left = expr();
	consume(T_SEMI);

	return node;
}

Ret_Type_ type() {
	if(check(T_QUESTMARK)){
		advance();
	}

	Ret_Type_ type = get_ret_type();
	advance();

	return type;
}

AST_Node* var_decl() {
	AST_Node* node = node_init(VAR_DECL);

	bool can_mut;
	bool was_used = false;
	bool was_assigned = false;

	if (check(T_VAR)) {
		advance();
		can_mut = true;

	} else {
		consume(T_CONST);
		can_mut = false;
	}

	consume(T_ID);
	node->as.var_name = parser->prev->value;

	Ret_Type_ ret_type = IMPLICIT;
	if (check(T_DDOT)) {
		advance();
		ret_type = type();
	}

	Entry* entry = entry_init(node->as.var_name, E_VAR, (Expr_Type){ret_type, false}, can_mut, was_used, was_assigned);

	if (context_find(&(parser->c_stack), node->as.var_name, true) != NULL) {
		ERROR_RET(ERR_SEM_REDEF);
	}
	context_put(&(parser->c_stack), entry);

	consume(T_EQUAL);
	node->left = expr();
	consume(T_SEMI);

	return node;
}

AST_Node* func_call() {
	AST_Node* node = node_init(FUNC_CALL);
	node->as.func_data->var_name = parser->prev->value;

	consume(T_RPAR);

	while (!check(T_LPAR)) {
		AST_Node* param = expr();
		arr_append(node->as.func_data->arr, (size_t) param);

		if (!check(T_COMMA)) {
			break;
		}

		consume(T_COMMA);
	}

	consume(T_LPAR);

	return node;
}

AST_Node* assignment() {
	AST_Node* node;

	if (parser->prev->type == T_UNDER) {
		consume(T_EQUAL);
		node = expr();

	} else {
		node = node_init(VAR_ASSIGNMENT);
        Entry* entry = context_find(&(parser->c_stack), parser->prev->value, false);
        if(entry == NULL) exit(ERR_SEM_NOT_DEF_FNC_VAR);
		node->as.var_name = entry->key;
		consume(T_EQUAL);
		node->left = expr();
	}

	consume(T_SEMI);
	return node;
}

AST_Node* _return() {
	AST_Node* node = node_init(RETURN);
	consume(T_RETURN);

	if (!check(T_SEMI)) {
		node->left = expr();
	}

	consume(T_SEMI);

	return node;
}

AST_Node* stmt() {
	switch (parser->next->type) {
	case T_IF:
		return _if();

	case T_VAR:
	case T_CONST:
		return var_decl();

	case T_UNDER:
		return under_var_decl();

	case T_ID:
		advance();
		if (check(T_EQUAL)) {
			return assignment();

		} else if (check(T_RPAR)) {
			AST_Node* node = func_call();
			consume(T_SEMI);
			return node;

        } else {
            ERROR_RET(ERR_PARSE);
        }

	case T_BUILDIN:
		advance();
		AST_Node* node = func_call();
		consume(T_SEMI);
		return node;

	case T_RETURN:
		return _return();

	case T_WHILE:
		return _while();

        default:
            // TODO(Sigull) Add error message
            ERROR_RET(ERR_PARSE);
    }
}

Ret_Type_ get_ret_type() {
	if (!strcmp(parser->next->value, "void")) {
		if(parser->prev->type == T_QUESTMARK) {
			ERROR_RET(ERR_PARSE);
		}
		return R_VOID;
	} else if (!strcmp(parser->next->value, "i32")) {
		if(parser->prev->type == T_QUESTMARK) {
			return N_I32;
		} else {
			return R_I32;
		}
	} else if (!strcmp(parser->next->value, "f64")) {
		if(parser->prev->type == T_QUESTMARK) {
			return N_F64;
		} else {
			return R_F64;
		}
	} else if (!strcmp(parser->next->value, "[]u8")) {
		if(parser->prev->type == T_QUESTMARK) {
			return N_U8;
		} else {
			return R_U8;
		}
	}

    ERROR_RET(ERR_SEM_RET_TYPE_DISCARD);
}

AST_Node* func_decl() {
	AST_Node* node = node_init(FUNCTION_DECL);
	consume(T_PUB);
	consume(T_FN);
	consume(T_ID);

	context_push(&(parser->c_stack));

    const char* func_name = parser->prev->value;
    node->as.func_data->var_name = func_name;

	Entry* func = tree_find(parser->s_table, func_name);
	Function_Arg** args = (Function_Arg**)func->as.function_args->data;

    consume(T_RPAR);
    while(check(T_ID)) {
        advance();
		char* var_name = parser->prev->value;
        consume(T_DDOT);
		Ret_Type_ r_type = type();

		Entry* entry = entry_init(var_name, E_VAR, (Expr_Type){r_type, false}, false, false, false);
		(*args++)->arg_name = var_name;
		if(context_find(&(parser->c_stack), entry->key, true) != NULL) {
			ERROR_RET(ERR_SEM_REDEF);
		}
		context_put(&(parser->c_stack), entry);

		if (!check(T_COMMA)) {
			break;
		}

		consume(T_COMMA);
	}
	consume(T_LPAR);

    if (!check(T_DTYPE) && !check(T_VOID) && !check(T_QUESTMARK)) {
        ERROR_RET(ERR_PARSE);
    }
    type();

	block(node->as.func_data->arr);
    context_pop(&(parser->c_stack));

	return node;
}

AST_Node* decl() {
	return func_decl();
}

void prolog() {
	consume(T_CONST);
	consume(T_ID);
	if (strcmp(parser->prev->value, "ifj")) {
		ERROR_RET(ERR_PARSE);
	}

	consume(T_EQUAL);
	consume(T_IMPORT);
	consume(T_RPAR);
	consume(T_STRING);
	char* prolog_file = parser->prev->value;
	if(strcmp(prolog_file, "ifj24.zig")) {
		ERROR_RET(ERR_PARSE);
	}

	consume(T_LPAR);
	consume(T_SEMI);

	Entry* read_str =     entry_init("*readstr",   E_FUNC, (Expr_Type){N_U8, false}, false, false, false);
	Entry* read_int =     entry_init("*readi32",   E_FUNC, (Expr_Type){N_I32, false}, false, false, false);
	Entry* read_float =   entry_init("*readf64",   E_FUNC, (Expr_Type){N_F64, false}, false, false, false);
	Entry* write =        entry_init("*write",     E_FUNC, (Expr_Type){R_VOID, false}, false, false, false);
	Entry* int_to_float = entry_init("*i2f",       E_FUNC, (Expr_Type){R_F64, false}, false, false, false);
	Entry* float_to_int = entry_init("*f2i",       E_FUNC, (Expr_Type){R_I32, false}, false, false, false);
	Entry* string_func =  entry_init("*string",    E_FUNC, (Expr_Type){R_U8, false}, false, false, false);
	Entry* length =       entry_init("*length",    E_FUNC, (Expr_Type){R_I32, false}, false, false, false);
	Entry* concat =       entry_init("*concat",    E_FUNC, (Expr_Type){R_U8, false}, false, false, false);
	Entry* sub_string =   entry_init("*substring", E_FUNC, (Expr_Type){N_U8, false}, false, false, false);
	Entry* string_cmp =   entry_init("*strcmp",    E_FUNC, (Expr_Type){R_I32, false}, false, false, false);
	Entry* ord_func =     entry_init("*ord",       E_FUNC, (Expr_Type){R_I32, false}, false, false, false);
	Entry* chr_func =     entry_init("*chr",       E_FUNC, (Expr_Type){R_U8, false}, false, false, false);

	// write to stdout
	Function_Arg* arg_write = malloc(sizeof(Function_Arg));
	arg_write->arg_name = malloc(sizeof(char) * 5);
	strcpy(arg_write->arg_name, "term");
	arg_write->type = R_VOID;
	arr_append(write->as.function_args, (size_t)arg_write);

	// int to float
	Function_Arg* arg_int_to_float = malloc(sizeof(Function_Arg));
	arg_int_to_float->arg_name = malloc(sizeof(char) * 5);
	strcpy(arg_int_to_float->arg_name, "term");
	arg_int_to_float->type = R_I32;
	arr_append(int_to_float->as.function_args, (size_t)arg_int_to_float);

	// float to int
	Function_Arg* arg_float_to_int = malloc(sizeof(Function_Arg));
	arg_float_to_int->arg_name = malloc(sizeof(char) * 5);
	strcpy(arg_float_to_int->arg_name, "term");
	arg_float_to_int->type = R_F64;
	arr_append(float_to_int->as.function_args, (size_t)arg_float_to_int);

	// string
	Function_Arg* arg_string = malloc(sizeof(Function_Arg));
	arg_string->arg_name = malloc(sizeof(char) * 5);
	strcpy(arg_string->arg_name, "term");
	arg_string->type = R_STRING;
	arr_append(string_func->as.function_args, (size_t)arg_string);

	// length of string
	Function_Arg* arg_length = malloc(sizeof(Function_Arg));
	arg_length->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_length->arg_name, "s");
	arg_length->type = R_U8;
	arr_append(length->as.function_args, (size_t)arg_length);

	// string concatenation
	Function_Arg* arg_concat = malloc(sizeof(Function_Arg));
	arg_concat->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_concat->arg_name, "s");
	arg_concat->type = R_U8;
	arr_append(concat->as.function_args, (size_t)arg_concat);
	arr_append(concat->as.function_args, (size_t)arg_concat);

	// substring
	Function_Arg* arg_sub_string = malloc(sizeof(Function_Arg));
	arg_sub_string->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_sub_string->arg_name, "s");
	arg_sub_string->type = R_U8;
	arr_append(sub_string->as.function_args, (size_t)arg_sub_string);
	Function_Arg* arg_sub_string_int = malloc(sizeof(Function_Arg));
	arg_sub_string_int->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_sub_string_int->arg_name, "i");
	arg_sub_string_int->type = R_I32;
	arr_append(sub_string->as.function_args, (size_t)arg_sub_string_int);
	arr_append(sub_string->as.function_args, (size_t)arg_sub_string_int);

	// string compare
	Function_Arg* arg_string_cmp = malloc(sizeof(Function_Arg));
	arg_string_cmp->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_string_cmp->arg_name, "s");
	arg_string_cmp->type = R_U8;
	arr_append(string_cmp->as.function_args, (size_t)arg_string_cmp);
	arr_append(string_cmp->as.function_args, (size_t)arg_string_cmp);

	// ord
	Function_Arg* arg_ord_func = malloc(sizeof(Function_Arg));
	arg_ord_func->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_ord_func->arg_name, "s");
	arg_ord_func->type = R_U8;
	arr_append(ord_func->as.function_args, (size_t)arg_ord_func);
	Function_Arg* arg_ord_func_int = malloc(sizeof(Function_Arg));
	arg_ord_func_int->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_ord_func_int->arg_name, "i");
	arg_ord_func_int->type = R_I32;
	arr_append(ord_func->as.function_args, (size_t)arg_ord_func_int);

	// chr
	Function_Arg* arg_chr_func = malloc(sizeof(Function_Arg));
	arg_chr_func->arg_name = malloc(sizeof(char) * 2);
	strcpy(arg_chr_func->arg_name, "i");
	arg_chr_func->type = R_I32;
	arr_append(chr_func->as.function_args, (size_t)arg_chr_func);

	tree_insert(parser->s_table, read_str);
	tree_insert(parser->s_table, read_int);
	tree_insert(parser->s_table, read_float);
	tree_insert(parser->s_table, write);
	tree_insert(parser->s_table, int_to_float);
	tree_insert(parser->s_table, float_to_int);
	tree_insert(parser->s_table, string_func);
	tree_insert(parser->s_table, length);
	tree_insert(parser->s_table, concat);
	tree_insert(parser->s_table, sub_string);
	tree_insert(parser->s_table, string_cmp);
	tree_insert(parser->s_table, ord_func);
	tree_insert(parser->s_table, chr_func);
}

void func_head() {
    consume(T_PUB);
    consume(T_FN);
    consume(T_ID);

    const char* func_name = parser->prev->value;
    if(tree_find(parser->s_table, func_name) != NULL) {
        ERROR_RET(ERR_SEM_REDEF);
    }

    Entry* entry = entry_init(func_name, E_FUNC, (Expr_Type){R_VOID, false}, false, false, false);

    consume(T_RPAR);
    while(check(T_ID)) {
        advance();
        Function_Arg* arg = malloc(sizeof(Function_Arg));
        arg->arg_name = parser->prev->value;

        consume(T_DDOT);

        arg->type = type();

        arr_append(entry->as.function_args, (size_t)arg);

        if (!check(T_COMMA)) {
            break;
        }

        consume(T_COMMA);
    }
    consume(T_LPAR);

    if (!check(T_DTYPE) && !check(T_VOID) && !check(T_QUESTMARK)) {
        ERROR_RET(ERR_PARSE);
    }
    entry->ret_type = (Expr_Type){type(), false};

    tree_insert(parser->s_table, entry);
}

void parse(char* orig_input) {
    size_t len = strlen(orig_input) + 1;
    char* input = malloc(sizeof(char) * (len + 10));
    memcpy(input, orig_input, len * sizeof(char));

    // First pass
    // Load function heads
    lexer = init_lexer(input);
    parser = init_parser();

    advance();
    while(parser->next->type != T_EOF) {
        if(parser->next->type == T_PUB) {
            func_head();
        }
        advance();
    }

    // Second pass
    // Normal parsing
    lexer = init_lexer(input);
    parser_reset();

    advance();

    prolog();

	// if main is not declared, throw error
	if (!tree_find(parser->s_table, "main")) {
		ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
	}
#if DEBUG
	char graph_filename[] = "graph.txt";

    FILE* f = fopen(graph_filename, "w");
    if(f == NULL) return;
    fclose(f);
#endif

    Arr* nodes = arr_init();
    while(parser->next->type != T_EOF) {
        AST_Node* node = decl();
		check_semantics(node);
#if DEBUG
        generate_graph(node, graph_filename);
#endif
        arr_append(nodes, (size_t)node);
    }

	check_var_usage(parser->s_table);
    generate_code(nodes, parser->s_table);
}

int compile(char* input) {
	srand(28980);

	parse(input);

    return 0;
}
