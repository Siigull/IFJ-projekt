#include "parser.h"

Lexer* lexer;
Parser* parser;

Parser* init_parser() {
    Parser* parser = malloc(sizeof(Parser));

    parser->next = NULL;
    parser->prev = NULL;

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
        exit(2);
    }
}

bool check(T_Type type) {
    return parser->next->type == type;
}

// Expression parser
AST_Node* expr() {
    printf("expr parser takes over\n");
    return NULL;
}

AST_Node* _if() {
    return NULL;
}

AST_Node* var_decl() {
    return NULL;
}

AST_Node* assignment() {
    return NULL;
}

AST_Node* func_call() {
    advance();
    if (check(T_EQUAL)) {
        return assignment;
    }

    return NULL;
}

AST_Node* _return() {
    return NULL;
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

AST_Node* func_decl() {
    AST_Node* node = node_init(FUNCTION_DECLARATION);
    consume(T_PUB);
    consume(T_FN);
    consume(T_ID);

    // TODO(Sigull) Add func to bvs

    consume(T_LPAR);
    while(check(T_ID)) {
        advance();
        consume(T_DDOT);
        consume(T_TYPE);

        if (!check(T_COMMA)) {
            break;
        }
    }
    consume(T_RPAR);

    if (check(T_TYPE)) {
        // TODO(Sigull) Add return type to func
    }

    consume(T_CUYLBRACKET);
    
    while(!check(T_CUYRBRACKET)) {
        arr_append(node->as.arr, stmt());
    }

    // TODO(Sigull) Check if non void func has return

    consume(T_CUYRBRACKET);

    return node;
}

// Normal parser
AST_Node* decl() {
    return func_decl();
}

void parse(const char* input) {
    lexer = init_lexer(input);
    parser = init_parser();

    advance();

    // TODO(Sigull) Přidat prolog

    while(parser->next != T_EOF) {
        decl();
    }
        
}