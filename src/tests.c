#include "lexer.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>

void test_input(char* input, char* expected_result) {
	// while token blbllalblbal, compare two strings
	Lexer* lexer = init_lexer(input);
	Token* token = get_next_token(lexer);
	while (token->type != T_EOF) {
		fprintf(stderr, "Token type: ");
		print_token(token, stderr, false);
		fprintf(stderr, "\t %s\n", token->value);
		token = get_next_token(lexer);
	}
};

char* load_inputos(const char* filepath) {
	char* input;
	FILE* f = fopen(filepath, "r");
	if (f == NULL) {
		return input;
	}

	char temp;
	int len = 0;
	while (fscanf(f, "%c", &temp) == 1) {
		len++;
	}
	rewind(f);

	input = calloc(len + 10, sizeof(char));

	for (int i = 0; fscanf(f, "%c", input + i) == 1; i++)
		;

	return input;
}

void generate_debug_token_arr() {
	FILE* f = fopen("src/example_multiline.ifj", "r");
	if (f == NULL) {
		return;
	}

	char temp;
	int len = 0;
	while (fscanf(f, "%c", &temp) == 1) {
		len++;
	}
	rewind(f);

	char* input = calloc(len + 10, sizeof(char));

	for (int i = 0; fscanf(f, "%c", input + i) == 1; i++)
		;

	Lexer* lexer = init_lexer(input);

	int count = 1;
	Token* token;

	printf("{");
	do {
		token = get_next_token(lexer);
		FILE* placeholder_file;

		char* val = print_token(token, placeholder_file, true);
		printf("{%s, \"%s\", %d},",
			   print_token(token, placeholder_file, true),
			   token->value,
			   token->length);

		if (count % 3 == 0) {
			printf("\n");
		}

		count++;
	} while (token->type != T_EOF);

	printf("}\n");
}

int main() {
	// const ifj = @import("ifj24.zig"); jeden token
	// char input1[50] = "456e2 7.89E-2 test1iden _identf";
	// char expected_result1[2] = {1};
	// test_input(input1, expected_result1);

	generate_debug_token_arr();

	char* temp = load_inputos("examples/6.ifj");
	compile(temp);
	return 0;
};