#include "lexer.hh"
#include <gtest/gtest.h>

using namespace ::testing;
using namespace std;

class test_lexer : public Test {
  protected:
	Lexer* lexer;
};

TEST_F(test_lexer, mathfloat) {
	char input[] = "[23-423+(12.33-2)]";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_SQRBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_PLUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_F64);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_SQLBRACKET);
}

TEST_F(test_lexer, prolog) {
	char input1[] = "const ifj = @import(\" ifj24.zig \");";
	lexer = init_lexer(input1);
	EXPECT_EQ(get_next_token(lexer)->type, T_CONST);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_IMPORT);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
}

TEST_F(test_lexer, random) {
	char input[] = "test1 pub test2";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_PUB);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
}

TEST_F(test_lexer, basic) {
	char input[] = "var promenna = 231.12e-5";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_F64);
}
TEST_F(test_lexer, funcdeclaration) {
	char input[] = "pub fn build(x : []u8, y : []u8) []u8 {const res = x + y;return res;}";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_PUB);
	EXPECT_EQ(get_next_token(lexer)->type, T_FN);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_DDOT);
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE);
	EXPECT_EQ(get_next_token(lexer)->type, T_COMMA);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_DDOT);
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYRBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_CONST);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_PLUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_RETURN);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYLBRACKET);
}

TEST_F(test_lexer, error) {
	char input[] = "er@or";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, keywords) {
	char input[] = "test pub test2 pub fn const else if i32 f64 null return var void while";
	lexer = init_lexer(input);
	T_TYPE current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_ID) << "test";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_PUB) << "pub";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_ID) << "test2";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_PUB) << "pub";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_FN) << "fn";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_CONST) << "const";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_ELSE) << "else";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_IF) << "if";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_DTYPE) << "i32";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_DTYPE) << "f64";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_NULL) << "null";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_RETURN) << "return";
	current_type = get_next_token(lexer)->type;
	// EXPECT_Ecurrent_typepe, T_DTYPE) << "u8";
	EXPECT_EQ(current_type, T_VAR) << "var";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_VOID) << "void";
	current_type = get_next_token(lexer)->type;
	EXPECT_EQ(current_type, T_WHILE) << "while";
	current_type = get_next_token(lexer)->type;
}

TEST_F(test_lexer, numbers) {
	char input[] = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 15.2 14.325 15e-2 16.0000 13.2";
	lexer = init_lexer(input);
	for (int i = 0; i < 16; i++) {
		EXPECT_EQ(get_next_token(lexer)->type, T_I32) << i;
	}
	for (int i = 16; i < 21; i++) {
		EXPECT_EQ(get_next_token(lexer)->type, T_F64) << i;
	}
}

TEST_F(test_lexer, float_error) {
	char input[] = "1. 1.1. 1.1e 1.1e+ 1.1e- 1.1e+1.1 1.1e-1.1 .223";
	lexer = init_lexer(input);
	for (int i = 0; i < 8; i++) {
		EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
	}
}

TEST_F(test_lexer, error2) {
	char input[] = "01.111 1.23e+2";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error3) {
	char input[] = "_ = 5";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error4) {
	char input[] = "var #invalid = 10";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error5) {
	char input[] = "var num = 10 @ 5;";
	lexer = init_lexer(input);
	get_next_token(lexer);
	get_next_token(lexer);
	get_next_token(lexer);
	get_next_token(lexer);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error6) {
	char input[] = "var []u8 = \"Hello World;\n";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR) << "var";
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE) << "[]u8";
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL) << "=";
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, strings) {
	char input[] = "\"\", \"string\", \"#23@#bvcn\", value = ";
	lexer = init_lexer(input);
	Token* token = get_next_token(lexer);
	EXPECT_EQ(token->type, T_STRING);
	EXPECT_EQ(token->length, 0);
	EXPECT_EQ(get_next_token(lexer)->type, T_COMMA);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_COMMA);
	token = get_next_token(lexer);
	EXPECT_EQ(token->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_COMMA);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
}

/*TEST_F(test_lexer, strings_multi_line){
	char input[] = "\\\\hovnohovno";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	printf("%s", input);
}*/

TEST_F(test_lexer, string_err){
	char input[] = " cbjkkvb2id\"; ";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}