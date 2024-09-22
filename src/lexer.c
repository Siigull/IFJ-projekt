#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

TOKEN* init_token(char* value, T_TYPE type, unsigned int length) {
	TOKEN* token = malloc(sizeof(TOKEN));
	token->value = value;
	token->type = type;
	token->length = length;
};
Lexer* init_lexer(char* input) {
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->input = input;
	lexer->input_len = strlen(input);
	// idr is main index, and idl will be used to scan the whole token for length and value
	lexer->idl = 0;
	lexer->idr = 0;
};

void lexer_advance(Lexer* lexer) {
	if (lexer->idr < lexer->input_len && lexer->input[lexer->idr] != '\0') {
		lexer->idr += 1;
	}
}

void lexer_skip_whitespace(Lexer* lexer) {
	char c = lexer->input[lexer->idr];
	if (c == 13 || c == 10 || c == ' ' || c == '\t') {
		lexer_advance(lexer);
	};
};
bool is_num(char c) {
	return c >= '0' && c <= '9';
}
bool is_operator(char c) {
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' ||
		c == '!') {
	};
};

TOKEN* get_next_token(Lexer* lexer) {
	lexer_skip_whitespace(lexer);
	lexer->idl = lexer->idr;
	if (lexer->input[lexer->idr] != '\0') {
	}
};
