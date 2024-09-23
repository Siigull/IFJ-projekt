#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>

void test_input(char* input, char* expected_result) {
	// while token blbllalblbal, compare two strings
	Lexer* lexer = init_lexer(input);
	TOKEN* token = get_next_token(lexer);
	while (token->type != T_EOF) {
		printf("Token type: %d,\n", token->type);
		printf("Token value:%s, \n", token->value);
		token = get_next_token(lexer);
	}
};

int main() {
	// const ifj = @import("ifj24.zig"); jeden token
	char input1[50] = "@import _";
	char expected_result1[2] = {1};
	test_input(input1, expected_result1);
	return 0;
};