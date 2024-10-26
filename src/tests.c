#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>

void test_input(char* input, char* expected_result) {
	// while token blbllalblbal, compare two strings
	Lexer* lexer = init_lexer(input);
	Token* token = get_next_token(lexer);
	while (token->type != T_EOF) {
		fprintf(stderr, "Token type: ");
		print_token(token, stderr);
		fprintf(stderr, "\t \n%s\n", token->value);
		token = get_next_token(lexer);
	}
};

int main() {
	// const ifj = @import("ifj24.zig"); jeden token
	char input1[200] = "\\\\Line one\n\\\\askdjhasd\n\n\\\\kjashd";
	char expected_result1[2] = {1};
	test_input(input1, expected_result1);
	return 0;
};