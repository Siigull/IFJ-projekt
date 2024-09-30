#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Token* init_token(char* value, T_TYPE type, unsigned int length) {
	Token* token = malloc(sizeof(Token));
	token->value = value;
	token->type = type;
	token->length = length;

	return token;
}

Lexer* init_lexer(char* input) {
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->input = input;
	lexer->input_len = strlen(input);
	// idr is main index, and idl will be used to scan the whole token for length and value
	lexer->idl = 0;
	lexer->idr = 0;

	return lexer;
}

void lexer_advance(Lexer* lexer) {
	if (lexer->idr < lexer->input_len && lexer->input[lexer->idr] != '\0')
		lexer->idr++;
	else {
		// handle error
		exit(1);
	}
}

void lexer_skip_line(Lexer* lexer) {
	while (lexer->input[lexer->idr] != '\n') {
		lexer_advance(lexer);
	}
	lexer_advance(lexer);
};

bool is_whitespace(char c) {
	return (c == ' ' || c == 10 || c == 13 || c == '\t' || c == '\v');
}

void lexer_skip_whitespace(Lexer* lexer) {
	char c = lexer->input[lexer->idr];
	while(is_whitespace(c)){
		lexer_advance(lexer);
		c = lexer->input[lexer->idr];
		if (c == '/' && lexer->input[lexer->idr + 1] == '/') {
			lexer_skip_line(lexer);
		}
		c = lexer->input[lexer->idr];
	}
}

bool is_operator(char c) {
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' ||
			c == '!');
}

bool is_num(char c) {
	return (c >= '0' && c <= '9');
}

bool is_num_token(Token* token) {
	int err_count = 0;
	for (unsigned int i = 0; i < token->length; i++) {
		if (is_num(token->value[i]) == false) {
			err_count++;
		}
	}
	if (err_count == 0) {
		return true;
	} else {
		return false;
	}
};

bool is_id_token(Token* token) {
	int i = 0;
	int match_undr = 0;
	while (token->value[i] == '_' || isalpha(token->value[i]) != 0) {
		if (token->value[i] == '_') {
			match_undr++;
		}
		i++;
		while (token->value[i] == '_' || isalpha(token->value[i]) != 0 || is_num(token->value[i])) {
			i++;
		}
	}
	if (token->length == 1 && match_undr != 0) {
		return false;

	} else if (i == token->length) {
		return true;
	}
	return false;
}

bool is_float_token(Token* token) {
	int i = 0;
	int prefix_match = 0;
	while (token->value[i] == '-') {
		i++;
	}
	if (is_num(token->value[i])) {
		i++;
		prefix_match++;
		while (is_num(token->value[i]) && token->value[i] != '0') {
			if (token->value[i - 1] == '0') {
				return false;
			} // cant read another number if 0 was read, need .
			i++;
		}
	}
	if (prefix_match == 0) {
		return false;
	}
	if (token->value[i] == '.') {
		i++;
		int mistake_check = 0;
		while (is_num(token->value[i])) {
			i++;
			mistake_check++;
		}
		if (mistake_check == 0) {
			return false;
		}
	}
	if (token->value[i] == 'e' || token->value[i] == 'E') {
		i++;
		if (token->value[i] == '+' || token->value[i] == '-') {
			i++;
		}
		int must_match = 0;
		while (is_num(token->value[i])) {
			i++;
			must_match++;
		}
		if (must_match == 0) {
			return false;
		}
	}
	if (token->length == i) {
		return true;
	}
	return false;
};

Token* find_token_value(Lexer* lexer, T_TYPE type) {
    if (type == T_EOF) {
        Token* token = init_token("0", T_EOF, 0);
        return token;
    }

    unsigned int len = lexer->idr - lexer->idl;
    char* value = (char*) malloc(len + 1);
    // load the value
    for (unsigned int i = 0; i < len; i++) {
        value[i] = lexer->input[lexer->idl + i];
    }
    value[len] = '\0';
	// partially initializing token but we need to ensure it has correct type
	Token* token = init_token(value, type, len);

	// KEYWORD TOKENS - tokens that are yet to be defined
	if (token->type == T_UNDEF) {
		char* values[14] = {"while",
							"else",
							"const",
							"fn",
							"i32",
							"f64",
							"[]u8",
							"var",
							"null",
							"pub",
							"return",
							"void",
							"if",
							"@import"};
		T_TYPE types[14] = {T_WHILE,
							T_ELSE,
							T_CONST,
							T_FN,
							T_DTYPE,
							T_DTYPE,
							T_DTYPE,
							T_VAR,
							T_NULL,
							T_PUB,
							T_RETURN,
							T_VOID,
							T_IF,
							T_IMPORT};
		for (int i = 0; i < 14; i++) {
			if (!strcmp(token->value, values[i])) {
				token->type = types[i];
				return token;
			}
		}
		if(token->length == 1 && token->value[0] == '_'){
			token->type = T_UNDER;
			return token;
		}

		// first we look if we start with number, if yes number is decided
		if (is_num(value[0])) {
			if (is_num_token(token)) {
				token->type = T_I32;
				return token;
			}
			if (is_float_token(token)) {
				token->type = T_F64;
				return token;
			}
		}

		if (is_id_token(token)) {
			token->type = T_ID;
			return token;
		}
		exit(1);
	}

	// OPERATOR TOKENS
	if (type == T_OPERATOR) {
		char* values[12] = {"+", "-", "*", "/", "=", "!", "<", ">", "<=", ">=", "==", "!="};
		T_TYPE types[12] = {T_PLUS,
							T_MINUS,
							T_MUL,
							T_DIV,
							T_EQUAL,
							T_EXCLEMARK,
							T_STHAN,
							T_GTHAN,
							T_SETHAN,
							T_GETHAN,
							T_DDEQ,
							T_NEQUAL};
		for (int i = 0; i < 12; i++) {
			if (!strcmp(token->value, values[i])) {
				token->type = types[i];
				return token;
			}
		}
	}

	if (type == T_STRING) {
		lexer_advance(lexer);
		return token;
	}


	if(type == T_BUILDIN){
// error TODO handling of buildin function that do not exist
	return token;
	};


	// if we didn't match with anything, its error
	if(lexer->idr == lexer->input_len){
		return init_token("0", T_EOF, 0);
	}
	else{
		exit(1);
	}
} // end of find_token_value

Token* get_next_token(Lexer* lexer) {
	lexer_skip_whitespace(lexer);
	lexer->idl = lexer->idr;
	if (lexer->input[lexer->idr] != '\0') {
		//normal strings
		if (lexer->input[lexer->idr] == '"') {
			lexer->idl++;
			lexer_advance(lexer);
			while (lexer->input[lexer->idr] != '"') { 
				if (lexer->input[lexer->idr] == BACKSLASH) {
					if (lexer->input[lexer->idr+1] == '"') {
						lexer_advance(lexer);
					}
				}
				lexer_advance(lexer);
			}
			return find_token_value(lexer, T_STRING);
		}//normal strings

		/*//multiline strings
		if(lexer->input[lexer->idr] == BACKSLASH && lexer->input[lexer->idr+1] == BACKSLASH){
			lexer->idl+=2;
			lexer_advance(lexer);
			lexer_advance(lexer);
			bool not_continuing = true;
			while(lexer->input[lexer->idr] != '\n' && not_continuing){

			}
			return find_token_value(lexer, T_STRING);
		}//multiline*/


		if(lexer->input[lexer->idr] == 'i' && lexer->input[lexer->idr + 1] == 'f' && lexer->input[lexer->idr] == 'j' && lexer->input[lexer->idr] == '.'){
			for(int i = 0; i < 4; i++){
				lexer_advance(lexer);
			}
			// now scanning inbuild function
			while(is_num(lexer->input[lexer->idr]) || isalpha(lexer->input[lexer->idr] != 0)){
				lexer_advance(lexer);
			}
			return find_token_value(lexer, T_BUILDIN);
		}



		if (isalpha(lexer->input[lexer->idr]) || lexer->input[lexer->idr] == '_' ||
			lexer->input[lexer->idr] == '@' || is_num(lexer->input[lexer->idr])) {
			int has_number = 0;
			if (is_num(lexer->input[lexer->idr]) == true) {
				has_number++;
			}

			lexer_advance(lexer);
			// first char is read and hasnumber is activated based on what it is
			if (has_number == 0) {
				while (is_num(lexer->input[lexer->idr]) || isalpha(lexer->input[lexer->idr]) != 0 ||
					   lexer->input[lexer->idr] == '_' || lexer->input[lexer->idr] == '.') {
					lexer_advance(lexer); // only idr advances, so we can scan it later
				}
				return find_token_value(lexer, T_UNDEF);
			}
			// if we read number we will still read chars and nums
			else if (has_number == 1) {
				while (is_num(lexer->input[lexer->idr]) || lexer->input[lexer->idr] == '.' ||
					   	isalpha(lexer->input[lexer->idr]) != 0) {
					if (lexer->input[lexer->idr] != 'e' && lexer->input[lexer->idr] != 'E') {
						lexer_advance(lexer);
					} else {
						lexer_advance(lexer);
						if (lexer->input[lexer->idr] == '+' || lexer->input[lexer->idr] == '-') {
							lexer_advance(lexer);
						}
					}
				}
				return find_token_value(lexer, T_UNDEF);
			}
		}
		if (lexer->input[lexer->idr] == '[' && lexer->input[lexer->idr + 1] == ']' &&
			lexer->input[lexer->idr + 2] == 'u' && lexer->input[lexer->idr + 3] == '8') {
			lexer_advance(lexer);
			lexer_advance(lexer);
			lexer_advance(lexer);
			lexer_advance(lexer);
			return find_token_value(lexer, T_UNDEF);
		};
		// handling brackets
		char brackets[6] = {'(', ')', '[', ']', '{', '}'};
		char* string_brackets[6] = {"(", ")", "[", "]", "{", "}"};
		T_TYPE bracket_types[6] =
			{T_RPAR, T_LPAR, T_SQRBRACKET, T_SQLBRACKET, T_CUYRBRACKET, T_CUYLBRACKET};
		for (int i = 0; i < 6; i++) {
			if (lexer->input[lexer->idr] == brackets[i]) {
				lexer_advance(lexer);
				return init_token(string_brackets[i], bracket_types[i], 1);
			}
		}
		if (lexer->input[lexer->idr] == '|') {
			lexer_advance(lexer);
			Token* token = init_token("|", T_VBAR, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == '?') {
			lexer_advance(lexer);
			Token* token = init_token("?", T_QUESTMARK, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == ';') {
			lexer_advance(lexer);
			Token* token = init_token(";", T_SEMI, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == ',') {
			lexer_advance(lexer);
			Token* token = init_token(",", T_COMMA, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == ':') {
			lexer_advance(lexer);
			Token* token = init_token(":", T_DDOT, 1);
			return token;
		};

		if (is_operator(lexer->input[lexer->idr])) {
			while (is_operator(lexer->input[lexer->idr])) {
				lexer_advance(lexer);
			}
			return find_token_value(lexer, T_OPERATOR);
		}
	}
	if(lexer->idr == lexer->input_len){
		return init_token("0", T_EOF, 0);
	}
	else{
		exit(1);
	}
} // end of get_next_token

void print_token(Token* token, FILE* out) {
	switch (token->type) {
	case T_ID:
		fprintf(out, "T_ID");
		break;
	case T_F64:
		fprintf(out, "T_F64");
		break;
	case T_I32:
		fprintf(out, "T_I32");
		break;
	case T_U8:
		fprintf(out, "T_U8");
		break;
	case T_DOT:
		fprintf(out, "T_DOT");
		break;
	case T_QUOTATION:
		fprintf(out, "T_QUOTATION");
		break;
	case T_QUESTMARK:
		fprintf(out, "T_QUESTMARK");
		break;
	case T_EXCLEMARK:
		fprintf(out, "T_EXCLEMARK");
		break;
	case T_EXPONENT:
		fprintf(out, "T_EXPONENT");
		break;
	case T_STRING:
		fprintf(out, "T_STRING");
		break;
	case T_COMMENT:
		fprintf(out, "T_COMMENT");
		break;
	case T_RPAR:
		fprintf(out, "T_RPAR");
		break;
	case T_LPAR:
		fprintf(out, "T_LPAR");
		break;
	case T_SQRBRACKET:
		fprintf(out, "T_SQRBRACKET");
		break;
	case T_SQLBRACKET:
		fprintf(out, "T_SQLBRACKET");
		break;
	case T_CUYRBRACKET:
		fprintf(out, "T_CUYRBRACKET");
		break;
	case T_CUYLBRACKET:
		fprintf(out, "T_CUYLBRACKET");
		break;
	case T_CONST:
		fprintf(out, "T_CONST");
		break;
	case T_IF:
		fprintf(out, "T_IF");
		break;
	case T_ELSE:
		fprintf(out, "T_ELSE");
		break;
	case T_FN:
		fprintf(out, "T_FN");
		break;
	case T_PUB:
		fprintf(out, "T_PUB");
		break;
	case T_RETURN:
		fprintf(out, "T_RETURN");
		break;
	case T_VAR:
		fprintf(out, "T_VAR");
		break;
	case T_VOID:
		fprintf(out, "T_VOID");
		break;
	case T_WHILE:
		fprintf(out, "T_WHILE");
		break;
	case T_IMPORT:
		fprintf(out, "T_IMPORT");
		break;
	case T_SEMI:
		fprintf(out, "T_SEMI");
		break;
	case T_DDOT:
		fprintf(out, "T_DDOT");
		break;
	case T_PLUS:
		fprintf(out, "T_PLUS");
		break;
	case T_MINUS:
		fprintf(out, "T_MINUS");
		break;
	case T_MUL:
		fprintf(out, "T_MUL");
		break;
	case T_DIV:
		fprintf(out, "T_DIV");
		break;
	case T_EQUAL:
		fprintf(out, "T_EQUAL");
		break;
	case T_NEQUAL:
		fprintf(out, "T_NEQUAL");
		break;
	case T_GTHAN:
		fprintf(out, "T_GTHAN");
		break;
	case T_GETHAN:
		fprintf(out, "T_GETHAN");
		break;
	case T_STHAN:
		fprintf(out, "T_STHAN");
		break;
	case T_SETHAN:
		fprintf(out, "T_SETHAN");
		break;
	case T_OPERATOR:
		fprintf(out, "T_OPERATOR");
		break;
	case T_UNDEF:
		fprintf(out, "T_UNDEF");
		break;
	case T_NULL:
		fprintf(out, "T_NULL");
		break;
	case T_EOF:
		fprintf(out, "T_EOF");
		break;
	case T_ERR:
		fprintf(out, "T_ERR");
		break;
	case T_COMMA:
		fprintf(out, "T_COMMA");
		break;
	case T_DTYPE:
		fprintf(out, "T_DTYPE");
		break;
	case T_DDEQ:
		fprintf(out, "T_DDEQ");
		break;
	case T_UNDER:
		fprintf(out, "T_UNDER");
		break;
	default:
		fprintf(out, "Unknown token");
		break;
	}
}
