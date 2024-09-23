#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

TOKEN* init_token(char* value, T_TYPE type, unsigned int length) {
	TOKEN* token = malloc(sizeof(TOKEN));
	token->value = value;
	token->type = type;
	token->length = length;
}

Lexer* init_lexer(char* input) {
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->input = input;
	lexer->input_len = strlen(input);
	// idr is main index, and idl will be used to scan the whole token for length and value
	lexer->idl = 0;
	lexer->idr = 0;
};

void lexer_advance(Lexer* lexer) {
	if (lexer->idr < lexer->input_len && lexer->input[lexer->idr] != '\0') lexer->idr++;
}

void lexer_skip_line(Lexer* lexer) {
	while (lexer->input[lexer->idr] != '\n') {
		lexer_advance(lexer);
	}
};

bool is_whitespace(char c) {
	return (c == ' ' || c == 10 || c == 13 || c == '\t' || c == '\v');
}

void lexer_skip_whitespace(Lexer* lexer) {
	char c = lexer->input[lexer->idr];
	if(c == '/' && lexer->input[lexer->idr + 1] == '/'){
		lexer_skip_line(lexer);
		return;
	}
	else{
		while (is_whitespace(c)) {
			lexer_advance(lexer);
			c = lexer->input[lexer->idr];
		}
	}
}

bool is_operator(char c) {
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' ||c == '!');
}

bool is_num(char c){
	return (c >= '0' && c <= '9');
}


TOKEN* find_token_value(Lexer* lexer, T_TYPE type) {
	if (type == T_EOF) {
		TOKEN* token = init_token("0", T_EOF, 0);
		return token;
	}

	unsigned int len = lexer->idr - lexer->idl;
	char* value = (char*) malloc(len + 1);
	// load the value
	for (unsigned int i = 0; i < len; i++) {
		value[i] = lexer->input[lexer->idl + i];
	}

	// partially initializing token but we need to ensure it has correct type
	TOKEN* token = init_token(value, type, len);

	// KEYWORD TOKENS - tokens that are yet to be defined
	if (token->type == T_UNDEF) {
		char* values[14] = {"while", "else", "const", "fn", "i32", "f64", "u8", "var", "null", "pub", "return", "void", "if", "@import"};
		T_TYPE types[14] = {T_WHILE, T_ELSE, T_CONST, T_FN, T_I32, T_F64, T_U8, T_VAR, T_NULL, T_PUB, T_RETURN, T_VOID, T_IF, T_IMPORT};
		for(int i = 0; i < 14; i++){
			if(!strcmp(token->value, values[i])){
				token->type = types[i];
				return token;
			}
		}
		//ID and number handling
		// TODO(VACKO): CHECK IF TOKEN IS NUM OR ID  327878 my_int42 "string3224.3432"
		int dot_counter = 0;
		for (unsigned int i = 0; i < len; i++) {
			if (value[i] == '_' || isalpha(value[i]) != 0) {
				token->type = T_ID;
				if (value[i] != '_' && len != 1) {
					return token;

				} else {
					/**
					 * \todo Vacko: Handle _ string that is not valid, and should return 1 as an
					 * error code
					 */
				}
			}
			if (value[i] == '.') {
				dot_counter++;
			}
		}
		if (dot_counter == 1) {
			token->type = T_F64;
			return token;
		} else if (dot_counter == 0) {
			token->type = T_I32;
			return token;
		} else {
			/**
			 * \todo Vacko: add an error code, multiple decimal dots, error code 1
			 * error code
			 */
		}
	}

	//OPERATOR TOKENS
	if (type == T_OPERATOR) {
		char* values[10] = {"+", "-", "*", "/", "=", "!", "<", ">", "<=", ">="};
		T_TYPE types[10] = {T_PLUS, T_MINUS, T_MUL, T_DIV, T_EQUAL, T_EXCLEMARK, T_STHAN, T_GTHAN, T_SETHAN, T_GETHAN};
		for(int i = 0; i < 10; i++){
			if(!strcmp(token->value, values[i])){
				token->type = types[i];
				return token;
			}
		}
	}
	//if we didn't match with anything, its error
	token->type = T_ERR;
	return token;
} // end of find_token_value

TOKEN* get_next_token(Lexer* lexer) {
	lexer_skip_whitespace(lexer);
	lexer->idl = lexer->idr;
	if (lexer->input[lexer->idr] != '\0') {
		if (isalpha(lexer->input[lexer->idr]) || lexer->input[lexer->idr] == '_' || lexer->input[lexer->idr] == '@' || is_num(lexer->input[lexer->idr])) {
			lexer_advance(lexer);
			while (is_num(lexer->input[lexer->idr]) || isalpha(lexer->input[lexer->idr]) != 0 ||
				   lexer->input[lexer->idr] == '_' || lexer->input[lexer->idr] == '.') {
				lexer_advance(lexer); // only idr advances, so we can scan it later
			}
			return find_token_value(lexer, T_UNDEF);
		}

		//handling brackets
		char brackets[6] = {'(', ')', '[', ']', '{', '}'};
		char* string_brackets[6] = {"(", ")", "[", "]", "{", "}"};
		T_TYPE bracket_types[6] = {T_LPAR, T_RPAR, T_SQRBRACKET, T_SQLBRACKET, T_CUYRBRACKET, T_CUYLBRACKET};
		for(int i = 0; i < 6; i++){
			if(lexer->input[lexer->idr] == brackets[i]){
				lexer_advance(lexer);
				return init_token(string_brackets[i], bracket_types[i], 1);
			}
		}

		if (lexer->input[lexer->idr] == '?') {
			lexer_advance(lexer);
			TOKEN* token = init_token("?", T_QUESTMARK, 1);
			return token;
		};

		if (is_operator(lexer->input[lexer->idr])) {
			while (is_operator(lexer->input[lexer->idr])) {
				lexer_advance(lexer);
			}
			return find_token_value(lexer, T_OPERATOR);
		}
	}
	else
		return init_token("0", T_EOF, 0);
} // end of get_next_token
