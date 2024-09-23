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

void lexer_skip_line(Lexer* lexer) {
	// end of line add?
	while (lexer->input[lexer->idr] != '\n') {
		lexer_advance(lexer);
	}
};

void lexer_skip_whitespace(Lexer* lexer) {
	char c = lexer->input[lexer->idr];
	while (c == 13 || c == 10 || c == ' ' || c == '\t' || c == '/') {
		if (c == '/' && lexer->input[lexer->idr + 1] == '/') {
			lexer_skip_line(lexer);
		} else if (c == '/') {
			break;
		}
		lexer_advance(lexer);
		c = lexer->input[lexer->idr];
	};
};
bool is_num(char c) {
	return c >= '0' && c <= '9';
}
bool is_operator(char c) {
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' ||
		c == '!') {
		return true;
	};
	return false;
};

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
		printf("%c\n", value[i]);
	}

	// partially initializing token but we need to ensure it has correct type
	TOKEN* token = init_token(value, type, len);

	while (token->type == T_UNDEF) {
		if (strcmp(token->value, "while") == 0) {
			token->type = T_WHILE;
			return token;
		}
		if (strcmp(token->value, "else") == 0) {
			token->type = T_ELSE;
			return token;
		}
		if (strcmp(token->value, "const") == 0) {
			token->type = T_CONST;
			return token;
		}
		if (strcmp(token->value, "fn") == 0) {
			token->type = T_FN;
			return token;
		}
		if (strcmp(token->value, "i32") == 0) {
			token->type = T_I32;
			return token;
		}
		if (strcmp(token->value, "f64") == 0) {
			token->type = T_F64;
			return token;
		}
		if (strcmp(token->value, "u8") == 0) {
			token->type = T_U8;
			return token;
		}
		if (strcmp(token->value, "var") == 0) {
			token->type = T_VAR;
			return token;
		}
		if (strcmp(token->value, "null") == 0) {
			token->type = T_NULL;
			return token;
		}
		if (strcmp(token->value, "pub") == 0) {
			token->type = T_PUB;
			return token;
		}
		if (strcmp(token->value, "return") == 0) {
			token->type = T_RETURN;
			return token;
		}
		if (strcmp(token->value, "void") == 0) {
			token->type = T_VOID;
			return token;
		}
		if (strcmp(token->value, "if") == 0) {
			token->type = T_IF;
			return token;
		}
		printf("%d %d", lexer->idl, lexer->idl);
		if (strcmp(token->value, "@import") == 0) {
			token->type = T_IMPORT;
			return token;
		}

		// TODO(VACKO): CHECK IF TOKEN IS NUM OR ID  327878 my_int42 "string3224.3432"
		int dot_counter = 0;
		for (unsigned int i = 0; i < len; i++) {
			printf("%c", value[i]);
			if (value[i] == '_' || isalpha(value[i]) != 0) {
				token->type = T_ID;
				if (value[i] != '_' && len != 1) {
					return token;

				} else {
					/**
					 * \todo Vacko: Handle _ string that is not valid, and should return 1 as an
					 * error code
					 */
					free(token);
					free(lexer);
					exit(1);
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

	if (type == T_OPERATOR) {
		if (strcmp(token->value, "+") == 0) {
			token->type = T_PLUS;
			return token;
		}
		if (strcmp(token->value, "-") == 0) {
			token->type = T_MINUS;
			return token;
		}
		if (strcmp(token->value, "*") == 0) {
			token->type = T_MUL;
			return token;
		}
		if (strcmp(token->value, "/") == 0) {
			token->type = T_DIV;
			return token;
		}
		if (strcmp(token->value, "=") == 0) {
			token->type = T_EQUAL;
			return token;
		}
		if (strcmp(token->value, "!") == 0) {
			token->type = T_EXCLEMARK;
			return token;
		}
		if (strcmp(token->value, "<") == 0) {
			token->type = T_STHAN;
			return token;
		}
		if (strcmp(token->value, ">") == 0) {
			token->type = T_GTHAN;
			return token;
		}
		if (strcmp(token->value, "<=") == 0) {
			token->type = T_SETHAN;
			return token;
		}
		if (strcmp(token->value, ">=") == 0) {
			token->type = T_GETHAN;
			return token;
		}
		token->type = T_ERR;
		return token;
	}
};

TOKEN* get_next_token(Lexer* lexer) {
	lexer_skip_whitespace(lexer);
	lexer->idl = lexer->idr;
	if (lexer->input[lexer->idr] != '\0') {
		if (isalpha(lexer->input[lexer->idr]) || lexer->input[lexer->idr] == '_' ||
			lexer->input[lexer->idr] == '@') {
			lexer_advance(lexer);
			while (is_num(lexer->input[lexer->idr]) || isalpha(lexer->input[lexer->idr]) != 0 ||
				   lexer->input[lexer->idr] == '_' || lexer->input[lexer->idr] == '.') {
				lexer_advance(lexer); // only idr advances, so we can scan it later
			}
			return find_token_value(lexer, T_UNDEF);
		}
		// parenthesis check
		if (lexer->input[lexer->idr] == '(') {
			lexer_advance(lexer);
			TOKEN* token = init_token("(", T_RPAR, 1);
			return token;
		}
		if (lexer->input[lexer->idr] == ')') {
			lexer_advance(lexer);
			TOKEN* token = init_token(")", T_LPAR, 1);
			return token;
		}
		if (lexer->input[lexer->idr] == '[') {
			lexer_advance(lexer);
			TOKEN* token = init_token("[", T_SQRBRACKET, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == ']') {
			lexer_advance(lexer);
			TOKEN* token = init_token("]", T_SQLBRACKET, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == '{') {
			lexer_advance(lexer);
			TOKEN* token = init_token("{", T_CUYRBRACKET, 1);
			return token;
		};
		if (lexer->input[lexer->idr] == '}') {
			lexer_advance(lexer);
			TOKEN* token = init_token("}", T_CUYLBRACKET, 1);
			return token;
		};
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
	} else {
		TOKEN* eoftoken = init_token("0", T_EOF, 0);
		return eoftoken;
	}
};
