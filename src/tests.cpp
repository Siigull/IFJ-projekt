#include <gtest/gtest.h>
#include "lexer.hh"

using namespace ::testing;
using namespace std;

class test_lexer : public Test {
	protected: 
		Lexer* lexer;
};

TEST_F(test_lexer, random){
	char input[] = "ahovno pub kokot";
	lexer = init_lexer(input);
	assert(get_next_token(lexer)->type == T_ID);
	assert(get_next_token(lexer)->type == T_PUB);
	assert(get_next_token(lexer)->type == T_ID);
}

TEST_F(test_lexer, error){
	char input[] = "er@or";
	lexer = init_lexer(input);
	ASSERT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}