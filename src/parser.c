#include "parser.h"
#include "ast.h"
#include "expressionparser.h"
#include "stack.h"
#include "string.h"
#include "test_generate_graph.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>

Lexer* lexer;
Parser* parser;
Token* debug_token_array;
int token_index = 0;

AST_Node* binary_load(List* tl);
AST_Node* binary();
AST_Node* func_call();
AST_Node* stmt();
Ret_Type get_ret_type();

void parser_reset() {
    parser->next = NULL;
    parser->prev = NULL;
}

Parser* init_parser() {
	Parser* parser = malloc(sizeof(Parser));

	parser->next = NULL;
	parser->prev = NULL;

	parser->s_table = tree_init();

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
		if (check(T_RPAR)) {
			AST_Node* node = func_call();
			return node;
		}
	} else if (check(T_STRING)) {
		advance();
		AST_Node* node = string();
		return node;
	} else if (check(T_I32) || check(T_F64) || check(T_U8) || check(T_NULL)) {
		advance();
	} else if (check(T_RPAR)) {
		advance();
		binary_load(tl);
		consume(T_LPAR);
		Token* token = malloc(sizeof(Token));
		memcpy(token, parser->prev, sizeof(Token));
		List_insertF(tl, token);
	} else if (check(T_BUILDIN)) {
		advance();
		AST_Node* node = func_call();
		return node;
	} else {
		exit(ERR_PARSE);
	}

	return NULL;
}

bool check_operator() {
	T_Type operator_types[10] =
		{T_PLUS, T_MINUS, T_MUL, T_DIV, T_DDEQ, T_NEQUAL, T_STHAN, T_GTHAN, T_SETHAN, T_GETHAN};
	int len = sizeof(operator_types) / sizeof(T_Type);
	for (int i = 0; i < len; i++) {
		if (check(operator_types[i])) {
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

	while (check_operator()) {
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
        node->as.f64 = strtof(parser->prev->value, &end);
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
		List_activeF(token_list);
		while (List_is_active(token_list)) {
			Token* temp;
			List_get_val(token_list, &temp);
			print_token(temp, stdout, false);
			printf(" ");
			List_active_next(token_list);
		}

		node = parse_expression(token_list);
		printf("\n");
	}

	return node;

	// return binary();
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

	block(node->as.arr);

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

	if (check(T_BAR)) {
		advance();

		consume(T_ID);
		AST_Node* var = node_init(NNULL_VAR_DECL);
		arr_append(node->as.arr, (size_t) var);

		consume(T_BAR);
	}

	block(node->as.arr);

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

	if (check(T_BAR)) {
		advance();

		consume(T_ID);
		AST_Node* var = node_init(NNULL_VAR_DECL);
		arr_append(node->as.arr, (size_t) var);

		consume(T_BAR);
	}

	block(node->as.arr);

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

AST_Node* var_decl() {
	AST_Node* node = node_init(VAR_DECL);

	bool can_mut;

	if (check(T_VAR)) {
		advance();
		can_mut = true;

	} else {
		consume(T_CONST);
		can_mut = false;
	}

	consume(T_ID);
	node->as.var_name = parser->prev->value;

	Ret_Type ret_type = IMPLICIT;
	if (check(T_DDOT)) {
		advance();
		consume(T_DTYPE);
		ret_type = get_ret_type();
	}

	Entry* entry = entry_init(node->as.var_name, E_VAR, ret_type, can_mut);

    if(tree_find(parser->s_table, node->as.var_name) != NULL) {
        ERROR_RET(ERR_SEM_NOT_DEF_FNC_VAR);
    }
    tree_insert(parser->s_table, entry);

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
		node->as.var_name = parser->prev->value;
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

Ret_Type get_ret_type() {
	if (strcmp(parser->prev->value, "void")) {
		return R_VOID;
	} else if (strcmp(parser->prev->value, "i32")) {
		return R_I32;
	} else if (strcmp(parser->prev->value, "f64")) {
		return R_F64;
	} else if (strcmp(parser->prev->value, "[]u8")) {
		return R_U8;
	}

    ERROR_RET(ERR_SEM_RET_TYPE_DISCARD);
}

AST_Node* func_decl() {
	AST_Node* node = node_init(FUNCTION_DECL);
	consume(T_PUB);
	consume(T_FN);
	consume(T_ID);

    const char* func_name = parser->prev->value;
    node->as.func_data->var_name = func_name;

    consume(T_RPAR);
    while(check(T_ID)) {
        advance();
        consume(T_DDOT);
        consume(T_DTYPE);

		if (!check(T_COMMA)) {
			break;
		}

		consume(T_COMMA);
	}
	consume(T_LPAR);

    if (!check(T_DTYPE) && !check(T_VOID)) {
        exit(ERR_PARSE);
    }
    advance();

	block(node->as.func_data->arr);

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

	consume(T_LPAR);
	consume(T_SEMI);
}

void func_head() {
    consume(T_PUB);
    consume(T_FN);
    consume(T_ID);

    const char* func_name = parser->prev->value;
    if(tree_find(parser->s_table, func_name) != NULL) {
        exit(ERR_SEM_REDEF);
    }

    Entry* entry = entry_init(func_name, E_FUNC, R_VOID, false);

    consume(T_RPAR);
    while(check(T_ID)) {
        advance();
        Function_Arg* arg = malloc(sizeof(Function_Arg));
        arg->arg_name = parser->prev->value;

        consume(T_DDOT);

        consume(T_DTYPE);
        arg->type = get_ret_type();
        arr_append(entry->as.function_args, (size_t)arg);

        if (!check(T_COMMA)) {
            break;
        }

        consume(T_COMMA);
    }
    consume(T_LPAR);

    if (!check(T_DTYPE) && !check(T_VOID)) {
        ERROR_RET(ERR_PARSE);
    } 
    advance();

    entry->ret_type = get_ret_type();

    tree_insert(parser->s_table, entry);
}

Arr* parse(char* orig_input) {
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

	char graph_filename[] = "graph.txt";

    FILE* f = fopen(graph_filename, "w");
    if(f == NULL) return NULL;
    fclose(f);

    Arr* nodes = arr_init();
    while(parser->next->type != T_EOF) {
        AST_Node* node = decl();
        generate_graph(node, graph_filename);
        arr_append(nodes, (size_t)node);
    }
    generate_code(nodes, parser->s_table);
}

int compile(char* input) {
    parse(input);
    return 0;
}
