#include "parser.h"
#include "ast.h"
#include "string.h"
Lexer* lexer;
Parser* parser;
Token* debug_token_array;
int token_index = 0;

Parser* init_parser() {
    Parser* parser = malloc(sizeof(Parser));

    parser->next = NULL;
    parser->prev = NULL;

    parser->s_table = tree_init();

    return parser;
}

void advance() {
    // free(parser->prev);
    // parser->prev = parser->next;
    // parser->next = get_next_token(lexer);

    parser->prev = parser->next;
    parser->next = &(debug_token_array[token_index++]);
}

void consume(T_Type type) {
    if (parser->next->type == type) {
        advance();

    } else {
        exit(2);
    }
}

bool check(T_Type type) {
    return parser->next->type == type;
}

// Expression parser
AST_Node* expr() {
    advance();
    printf("expr parser takes over\n");
    return NULL;
}

// Normal parser
AST_Node* stmt();

AST_Node* _else() {
    AST_Node* node = node_init(ELSE);

    consume(T_CUYRBRACKET);

    while(!check(T_CUYLBRACKET)) {
        arr_append(node->as.arr, (size_t)stmt());
    }

    consume(T_CUYLBRACKET);

    return node;
}

AST_Node* _if() {
    AST_Node* node = node_init(IF);
    
    consume(T_IF);
    consume(T_RPAR);

    if (!check(T_LPAR)) {
        node->left = expr();
    } else {
        exit(2);
    }

    consume(T_LPAR);

    if (check(T_BAR)) {
        advance();

        consume(T_ID);
        AST_Node* var = node_init(NNULL_VAR_DECL);
        arr_append(node->as.arr, (size_t)node);

        consume(T_BAR);        
    }

    consume(T_CUYRBRACKET);

    while(!check(T_CUYLBRACKET)) {
        arr_append(node->as.arr, (size_t)stmt());
    }

    consume(T_CUYLBRACKET);

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

    tree_insert(parser->s_table, entry);

    consume(T_EQUAL);
    node->left = expr();
    consume(T_SEMI);

    return node;
}

AST_Node* func_call() {
    AST_Node* node = node_init(FUNC_CALL);
    node->as.var_name = parser->prev->value;

    consume(T_RPAR);

    while(check(T_ID)) {
        advance();

        if (!check(T_COMMA)) {
            break;
        }

        consume(T_COMMA);
    }

    consume(T_LPAR);
    consume(T_SEMI);

    return node;
}

AST_Node* assignment() {
    advance();
    if (check(T_RPAR)) {
        return func_call();
    }

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
        node->left = expr;
    }

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
            return func_call();

        case T_RETURN:
            return _return();

        default:
            // TODO(Sigull) Add error message
            exit(2);
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

    exit(4);
}

AST_Node* func_decl() {
    AST_Node* node = node_init(FUNCTION_DECL);
    consume(T_PUB);
    consume(T_FN);
    consume(T_ID);

    const char* func_name = parser->prev->value;

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
        exit(2);
    } 
    advance();

    Entry* entry = entry_init(func_name, E_FUNC, get_ret_type(), false);
    tree_insert(parser->s_table, entry);

    consume(T_CUYRBRACKET);
    
    while(!check(T_CUYLBRACKET)) {
        arr_append(node->as.arr, (size_t)stmt());
    }

    consume(T_CUYLBRACKET);

    return node;
}

AST_Node* decl() {
    return func_decl();
}

void parse(char* orig_input) {
    size_t len = strlen(orig_input) + 1;
    char* input = malloc(sizeof(char) * (len + 10));
    memcpy(input, orig_input, len * sizeof(char));

    lexer = init_lexer(input);
    parser = init_parser();

    Token token_arr[] = {{T_PUB, "pub", 3},       {T_FN, "fn", 2}, 
                         {T_ID, "main", 4},       {T_RPAR, "(", 1},
                         {T_LPAR, ")", 1},        {T_VOID, "void", 4},
                         {T_CUYRBRACKET, "{", 1}, {T_CONST, "const", 5}, 
                         {T_ID, "a", 1},          {T_EQUAL, "=", 1},
                         {T_I32, "10", 2},        {T_SEMI, ";", 1},
                         {T_IF, "if", 2},         {T_RPAR, "(", 1}, 
                         {T_ID, "a", 1},          {T_LPAR, ")", 1},
                         {T_BAR, "|", 1},         {T_ID, "A", 1},
                         {T_BAR, "|", 1},         {T_CUYRBRACKET, "{", 1}, 
                         {T_CUYLBRACKET, "}", 1}, {T_CUYLBRACKET, "}", 1},
                         {T_EOF, "\0", 1}};

    debug_token_array = token_arr;

    advance();

    // TODO(Sigull) Přidat prolog

    while(parser->next->type != T_EOF) {
        AST_Node* node = decl();
    }
}