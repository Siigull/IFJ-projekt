/**
 * IFJ PROJEKT 2024
 * @file parser.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Implementation of parser
 *
 */

#include "parser.h"

#if DEBUG
	#include "test_generate_graph.h"
#endif

// global vars to simplify and shorten function calls 
Lexer* lexer;
Parser* parser;

AST_Node* binary_load(List* tl);
AST_Node* func_call();
AST_Node* stmt();
Expr_Type get_ret_type();

Expr_Type null_to_nnul(Expr_Type type) {
    if(type == N_I32 || type == N_F64 || type == N_U8) {
        return (Expr_Type){type - 1};
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
	free(parser->prev);
	parser->prev = parser->next;
	parser->next = get_next_token(lexer);
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
	node->as.string = parser->prev->value;

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

// Expression load to expression parser
AST_Node* expr() {
	List* token_list = malloc(sizeof(List));
	List_init(token_list);

	AST_Node* node = binary_load(token_list);
	if (node == NULL) {
		node = parse_expression(token_list);
	}

	return node;
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
        Entry* entry = entry_init(var->as.var_name, E_VAR, IMPLICIT, false, false, false);
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
        Entry* entry = entry_init(var->as.var_name, E_VAR, IMPLICIT, false, false, false);
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

Expr_Type type() {
	if(check(T_QUESTMARK)){
		advance();
	}

	Expr_Type type = get_ret_type();

	return type;
}

AST_Node* var_decl() {
	AST_Node* node = node_init(VAR_DECL);

	bool can_mut = false;
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

	Expr_Type ret_type = IMPLICIT;
	if (check(T_DDOT)) {
		advance();
		ret_type = type();
	}

	Entry* entry = entry_init(node->as.var_name, E_VAR, ret_type, can_mut, was_used, was_assigned);
	
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
        if(entry == NULL) {
			ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
		}

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
            ERROR_RET(ERR_PARSE);
    }
}

Expr_Type get_ret_type() {
	bool has_null = parser->prev->type == T_QUESTMARK;

	if(check(T_VOID)) {
		advance();
		
	} else {
		consume(T_DTYPE);
	}

	if (!strcmp(parser->prev->value, "void")) {
		if(has_null) {
			ERROR_RET(ERR_PARSE);
		}
		return R_VOID;
	} else if (!strcmp(parser->prev->value, "i32")) {
		return R_I32 + has_null;

	} else if (!strcmp(parser->prev->value, "f64")) {
		return R_F64 + has_null;
		
	} else if (!strcmp(parser->prev->value, "[]u8")) {
		return R_U8 + has_null;
	}

    ERROR_RET(ERR_PARSE);
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

	// loop for parsing arguments
    while(check(T_ID)) {
        advance();
		char* var_name = parser->prev->value;

        consume(T_DDOT);
		Expr_Type r_type = type();

		// Adding arguments to func symtable entry
		Entry* entry = entry_init(var_name, E_VAR, r_type, false, false, false);
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

	// Function has to have type
    if (!check(T_DTYPE) && !check(T_VOID) && !check(T_QUESTMARK)) {
        ERROR_RET(ERR_PARSE);
    }
    type();

	block(node->as.func_data->arr);
    context_pop(&(parser->c_stack));

	return node;
}

AST_Node* decl() {
	// Global variables don't exist, only functions
	return func_decl();
}

void prolog() {
	// Parsing if prolog is correctly imported in program
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

	// Adding builtin functions to symtable
	Entry* read_str =     entry_init("*readstr",   E_FUNC, N_U8, false, false, false);
	Entry* read_int =     entry_init("*readi32",   E_FUNC, N_I32, false, false, false);
	Entry* read_float =   entry_init("*readf64",   E_FUNC, N_F64, false, false, false);
	Entry* write =        entry_init("*write",     E_FUNC, R_VOID, false, false, false);
	Entry* int_to_float = entry_init("*i2f",       E_FUNC, R_F64, false, false, false);
	Entry* float_to_int = entry_init("*f2i",       E_FUNC, R_I32, false, false, false);
	Entry* string_func =  entry_init("*string",    E_FUNC, R_U8, false, false, false);
	Entry* length =       entry_init("*length",    E_FUNC, R_I32, false, false, false);
	Entry* concat =       entry_init("*concat",    E_FUNC, R_U8, false, false, false);
	Entry* sub_string =   entry_init("*substring", E_FUNC, N_U8, false, false, false);
	Entry* string_cmp =   entry_init("*strcmp",    E_FUNC, R_I32, false, false, false);
	Entry* ord_func =     entry_init("*ord",       E_FUNC, R_I32, false, false, false);
	Entry* chr_func =     entry_init("*chr",       E_FUNC, R_U8, false, false, false);

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
	arg_string->type = R_TERM_STRING;
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

    Entry* entry = entry_init(func_name, E_FUNC, R_VOID, false, false, false);

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
    entry->ret_type = type();

    tree_insert(parser->s_table, entry);
}

Arr* parse(char* orig_input) {
    size_t len = strlen(orig_input) + 1;
    char* input = malloc(sizeof(char) * (len + 10));
    memcpy(input, orig_input, len * sizeof(char));

    // First pass
    // Load function heads to be able to call functions out of order
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
	// generate d2lang graph of AST
	char graph_filename[] = "graph.txt";

    FILE* f = fopen(graph_filename, "w");
    if(f == NULL) ERROR_RET(99);
    fclose(f);
#endif

    Arr* nodes = arr_init();
    while(parser->next->type != T_EOF) {
        AST_Node* node = decl();
		check_semantics(node);
#if DEBUG
        generate_graph(node, graph_filename, parser->s_table);
#endif
        arr_append(nodes, (size_t)node);
    }

	check_var_usage(parser->s_table);

	return nodes;
}

int compile(char* input) {
	srand(28980);

	Arr* nodes = parse(input);

    generate_code(nodes, parser->s_table);

    return 0;
}
