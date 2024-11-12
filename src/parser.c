#include "parser.h"
#include "ast.h"
#include "stack.h"
#include "string.h"
Lexer* lexer;
Parser* parser;
Token* debug_token_array;
int token_index = 0;

AST_Node* binary_load(List* tl);
AST_Node* func_call();
AST_Node* stmt();
Ret_Type get_ret_type();

Parser* init_parser() {
    Parser* parser = malloc(sizeof(Parser));

    parser->next = NULL;
    parser->prev = NULL;

    parser->s_table = tree_init();

    return parser;
}

void advance() {
    // free(parser->prev); // TODO(Sigull): Is this really how this is freed ?
    // parser->prev = parser->next;
    // parser->next = get_next_token(lexer);

    parser->prev = parser->next;
    parser->next = &(debug_token_array[token_index++]);
}

void consume(T_Type type) {
    if (parser->next->type == type) {
        advance();

    } else {
        exit(ERR_PARSE);
    }
}

bool check(T_Type type) {
    return parser->next->type == type;
}

AST_Node* literal_load(List* tl) {
    Token* token = malloc(sizeof(Token));
    memcpy(token, parser->next, sizeof(Token));
    List_insertF(tl, token);

    if (check(T_ID)) {
        advance();
        if(check(T_RPAR)) {
            AST_Node* node = func_call();
            return node;
        }

    } else if (check(T_I32) || check(T_F64) || 
               check(T_U8)  || check(T_STRING) ||
               check(T_NULL)) {
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
        return node;

    } else {
        exit(ERR_PARSE);
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

// Expression parser
AST_Node* expr() {
    List* token_list = malloc(sizeof(List));
    List_init(token_list);

    AST_Node* node = binary_load(token_list);
    if (node == NULL) {
        List_activeF(token_list);
        while(List_is_active(token_list)){
            Token* temp;
            List_get_val(token_list, &temp);
            print_token(temp, stdout, false);
            printf(" ");
            List_active_next(token_list);
        }

        // printf("expr parser takes over\n");
        printf("\n");
    } 
    
    return node;
}

// Normal parser
void block(Arr* stmts) {
    consume(T_CUYRBRACKET);

    while(!check(T_CUYLBRACKET)) {
        arr_append(stmts, (size_t)stmt());
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
        exit(ERR_PARSE);
    }

    consume(T_LPAR);

    if (check(T_BAR)) {
        advance();

        consume(T_ID);
        AST_Node* var = node_init(NNULL_VAR_DECL);
        arr_append(node->as.arr, (size_t)node);

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
        exit(ERR_PARSE);
    }

    consume(T_LPAR);

    if (check(T_BAR)) {
        advance();

        consume(T_ID);
        AST_Node* var = node_init(NNULL_VAR_DECL);
        arr_append(node->as.arr, (size_t)node);

        consume(T_BAR);        
    }

    block(node->as.arr);

    if (check(T_ELSE)) {
        node->right = _else();
    }

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

    Entry* entry = entry_init(node->as.var_name, E_VAR,
                              ret_type, can_mut);

    if(tree_find(parser->s_table, node->as.var_name) != NULL) {
        exit(ERR_SEM_NOT_DEF_FNC_VAR);
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

    while(!check(T_LPAR)) {
        AST_Node* param = expr();
        arr_append(node->as.func_data->arr, (size_t)param);

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
    switch(parser->next->type) {
        case T_IF:
            return _if();

        case T_VAR:
        case T_CONST:
            return var_decl();

        case T_ID:
            advance();
            if(check(T_EQUAL)) {
                return assignment();

            } else if(check(T_RPAR)){
                AST_Node* node = func_call();
                consume(T_SEMI);
                return node;

            } else {
                exit(ERR_PARSE);
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
            exit(ERR_PARSE);
    }
}

Ret_Type get_ret_type() {
    if (strcmp(parser->prev->value, "void")) {
        return R_VOID;
    } else if(strcmp(parser->prev->value, "i32")) {
        return R_I32;
    } else if(strcmp(parser->prev->value, "f64")) {
        return R_F64;
    } else if(strcmp(parser->prev->value, "[]u8")) {
        return R_U8;
    }

    exit(ERR_SEM_RET_TYPE_DISCARD);
}

AST_Node* func_decl() {
    AST_Node* node = node_init(FUNCTION_DECL);
    consume(T_PUB);
    consume(T_FN);
    consume(T_ID);

    const char* func_name = parser->prev->value;
    node->as.func_data->var_name = func_name;

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
        exit(ERR_PARSE);
    } 
    advance();

    entry->ret_type = get_ret_type();

    tree_insert(parser->s_table, entry);

    block(node->as.func_data->arr);

    return node;
}

AST_Node* decl() {
    return func_decl();
}

void prolog() {
    consume(T_CONST);
    consume(T_ID);
    if(strcmp(parser->prev->value, "ifj")) {
        exit(ERR_PARSE);
    }

    consume(T_EQUAL);
    consume(T_IMPORT);
    consume(T_RPAR);
    consume(T_STRING);
    char* prolog_file = parser->prev->value;

    consume(T_LPAR);
    consume(T_SEMI);
}

void parse(char* orig_input) {
    size_t len = strlen(orig_input) + 1;
    char* input = malloc(sizeof(char) * (len + 10));
    memcpy(input, orig_input, len * sizeof(char));

    lexer = init_lexer(input);
    parser = init_parser();

    Token token_arr[] = {{T_CONST, "const", 5},{T_ID, "ifj", 3},{T_EQUAL, "=", 1},
                        {T_IMPORT, "@import", 7},{T_RPAR, "(", 1},{T_STRING, "ifj24.zig", 9},
                        {T_LPAR, ")", 1},{T_SEMI, ";", 1},{T_PUB, "pub", 3},
                        {T_FN, "fn", 2},{T_ID, "main", 4},{T_RPAR, "(", 1},
                        {T_LPAR, ")", 1},{T_VOID, "void", 4},{T_CUYRBRACKET, "{", 1},
                        {T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},{T_STRING, "Zadejte cislo pro vypocet faktorialu\n", 38},
                        {T_LPAR, ")", 1},{T_SEMI, ";", 1},{T_CONST, "const", 5},
                        {T_ID, "a", 1},{T_EQUAL, "=", 1},{T_BUILDIN, "ifj.readi32", 11},
                        {T_RPAR, "(", 1},{T_LPAR, ")", 1},{T_SEMI, ";", 1},
                        {T_IF, "if", 2},{T_RPAR, "(", 1},{T_ID, "a", 1},
                        {T_LPAR, ")", 1},{T_BAR, "|", 1},{T_ID, "val", 3},
                        {T_BAR, "|", 1},{T_CUYRBRACKET, "{", 1},{T_IF, "if", 2},
                        {T_RPAR, "(", 1},{T_ID, "val", 3},{T_STHAN, "<", 1},
                        {T_I32, "0", 1},{T_LPAR, ")", 1},{T_CUYRBRACKET, "{", 1},
                        {T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},{T_STRING, "Faktorial ", 10},
                        {T_LPAR, ")", 1},{T_SEMI, ";", 1},{T_BUILDIN, "ifj.write", 9},
                        {T_RPAR, "(", 1},{T_ID, "val", 3},{T_LPAR, ")", 1},
                        {T_SEMI, ";", 1},{T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},
                        {T_STRING, " nelze spocitat\n", 17},{T_LPAR, ")", 1},{T_SEMI, ";", 1},
                        {T_CUYLBRACKET, "}", 1},{T_ELSE, "else", 4},{T_CUYRBRACKET, "{", 1},
                        {T_VAR, "var", 3},{T_ID, "d", 1},{T_DDOT, ":", 1},
                        {T_DTYPE, "f64", 3},{T_EQUAL, "=", 1},{T_BUILDIN, "ifj.i2f", 7},
                        {T_RPAR, "(", 1},{T_ID, "val", 3},{T_LPAR, ")", 1},
                        {T_SEMI, ";", 1},{T_VAR, "var", 3},{T_ID, "vysl", 4},
                        {T_DDOT, ":", 1},{T_DTYPE, "f64", 3},{T_EQUAL, "=", 1},
                        {T_F64, "1.0", 3},{T_SEMI, ";", 1},{T_WHILE, "while", 5},
                        {T_RPAR, "(", 1},{T_ID, "d", 1},{T_GTHAN, ">", 1},
                        {T_I32, "0", 1},{T_LPAR, ")", 1},{T_CUYRBRACKET, "{", 1},
                        {T_ID, "vysl", 4},{T_EQUAL, "=", 1},{T_ID, "vysl", 4},
                        {T_MUL, "*", 1},{T_ID, "d", 1},{T_SEMI, ";", 1},
                        {T_ID, "d", 1},{T_EQUAL, "=", 1},{T_ID, "d", 1},
                        {T_MINUS, "-", 1},{T_F64, "1.0", 3},{T_SEMI, ";", 1},
                        {T_CUYLBRACKET, "}", 1},{T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},
                        {T_STRING, "Vysledek: ", 10},{T_LPAR, ")", 1},{T_SEMI, ";", 1},
                        {T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},{T_ID, "vysl", 4},
                        {T_LPAR, ")", 1},{T_SEMI, ";", 1},{T_BUILDIN, "ifj.write", 9},
                        {T_RPAR, "(", 1},{T_STRING, " = ", 3},{T_LPAR, ")", 1},
                        {T_SEMI, ";", 1},{T_CONST, "const", 5},{T_ID, "vysl_i32", 8},
                        {T_EQUAL, "=", 1},{T_BUILDIN, "ifj.f2i", 7},{T_RPAR, "(", 1},
                        {T_ID, "vysl", 4},{T_LPAR, ")", 1},{T_SEMI, ";", 1},
                        {T_BUILDIN, "ifj.write", 9},{T_RPAR, "(", 1},{T_ID, "vysl_i32", 8},
                        {T_LPAR, ")", 1},{T_SEMI, ";", 1},{T_BUILDIN, "ifj.write", 9},
                        {T_RPAR, "(", 1},{T_STRING, "\n", 2},{T_LPAR, ")", 1},
                        {T_SEMI, ";", 1},{T_CUYLBRACKET, "}", 1},{T_CUYLBRACKET, "}", 1},
                        {T_ELSE, "else", 4},{T_CUYRBRACKET, "{", 1},{T_BUILDIN, "ifj.write", 9},
                        {T_RPAR, "(", 1},{T_STRING, "Faktorial pro null nelze spocitat\n", 35},{T_LPAR, ")", 1},
                        {T_SEMI, ";", 1},{T_CUYLBRACKET, "}", 1},{T_CUYLBRACKET, "}", 1},
                        {T_EOF, "0", 0},};

    // Token token_arr[] = {{T_PUB, "pub", 3},       {T_FN, "fn", 2}, 
    //                      {T_ID, "main", 4},       {T_RPAR, "(", 1},
    //                      {T_LPAR, ")", 1},        {T_VOID, "void", 4},
    //                      {T_CUYRBRACKET, "{", 1}, {T_CONST, "const", 5}, 
    //                      {T_ID, "a", 1},          {T_EQUAL, "=", 1},
    //                      {T_I32, "10", 2},        {T_PLUS, "+", 1},
    //                      {T_RPAR, "(", 1},        {T_I32, "69", 2},
    //                      {T_DIV, "/", 1},         {T_I32, "420", 2},
    //                      {T_LPAR, ")", 1},        {T_SEMI, ";", 1},
    //                      {T_IF, "if", 2},         {T_RPAR, "(", 1}, 
    //                      {T_ID, "a", 1},          {T_LPAR, ")", 1},
    //                      {T_BAR, "|", 1},         {T_ID, "A", 1},
    //                      {T_BAR, "|", 1},         {T_CUYRBRACKET, "{", 1}, 
    //                      {T_CUYLBRACKET, "}", 1}, {T_CUYLBRACKET, "}", 1},
    //                      {T_EOF, "\0", 1}};

    debug_token_array = token_arr;

    advance();

    prolog();

    while(parser->next->type != T_EOF) {
        AST_Node* node = decl();
    }
}