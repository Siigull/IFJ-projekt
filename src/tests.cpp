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
	char input[] = "ahovno pub kokot";
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

/*
TEST_F(test_lexer, prologtoken) {
	char input[] = "const ifj = @import(\"ifj24.zig\");";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_PROLOG);
}
*/