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
	assert(get_next_token(lexer)->type == T_SQRBRACKET);
	assert(get_next_token(lexer)->type == T_I32);
	assert(get_next_token(lexer)->type == T_MINUS);
	assert(get_next_token(lexer)->type == T_I32);
	assert(get_next_token(lexer)->type == T_PLUS);
	assert(get_next_token(lexer)->type == T_RPAR);
	assert(get_next_token(lexer)->type == T_F64);
	assert(get_next_token(lexer)->type == T_MINUS);
	assert(get_next_token(lexer)->type == T_I32);
	assert(get_next_token(lexer)->type == T_LPAR);
	assert(get_next_token(lexer)->type == T_SQLBRACKET);
}

TEST_F(test_lexer, random) {
	char input[] = "ahovno pub kokot";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_ID);
	assert(get_next_token(lexer)->type == T_PUB);
	assert(get_next_token(lexer)->type == T_ID);
}

TEST_F(test_lexer, basic) {
	char input[] = "var promenna = 231.12e-5";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_VAR);
	assert(get_next_token(lexer)->type == T_ID);
	assert(get_next_token(lexer)->type == T_EQUAL);
	assert(get_next_token(lexer)->type == T_F64);
}

TEST_F(test_lexer, error) {
	char input[] = "er@or";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_ID);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

/*
TEST_F(test_lexer, prologtoken) {
	char input[] = "const ifj = @import(\"ifj24.zig\");";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_PROLOG);
}
*/