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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, random) {
	char input[] = "test1 pub test2";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_PUB);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, basic) {
	char input[] = "var promenna = 231.12e-5";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_F64);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, float_error) {
	char input[] = "1. 1.1. 1.1e 1.1e+ 1.1e- 1.1e+1.1 1.1e-1.1 .223 123.1E+ 0.23e- 0.001e";
	lexer = init_lexer(input);
	for (int i = 0; i < 11; i++) {
		EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
	}
}

//taken from zig language reference
TEST_F(test_lexer, floats){
	char input[] = "123.0E+77 123.0 123.0e+77 3.141592653589793 1.23e-10 4.56231e+42 0.0e0 0.023E000045 0.23e-15";
	lexer = init_lexer(input);
	for(int i = 0; i < 9; i++){
		EXPECT_EQ(get_next_token(lexer)->type, T_F64);

	}
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, error2) {
	char input[] = "01.111 1.23e+2 0000124415";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error3) {
	char input[] = "var #invalid = 10";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, error4) {
	char input[] = "var num = 10 @ 5;";
	lexer = init_lexer(input);
	get_next_token(lexer);
	get_next_token(lexer);
	get_next_token(lexer);
	get_next_token(lexer);
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
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

//HERE TOKEN->VALUE HAS THE ESCAPE CHAR IN IT (LITERALLY)
TEST_F(test_lexer, string_escape){
	char input[] = "\"str \\t \\r \\\\ \\n ing\"";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

/*TEST_F(test_lexer, strings_multi_line){
	char input[] = "\\\\multilinestring";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	printf("%s", input);
}*/


TEST_F(test_lexer, string_hex){
	char input[] = "\"Ahoj\\n \\\"Sve'te \\\\\\x22 \"";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, comment){
	char input[] = 	"var input = \"jejda\" //comment comment comment\n pub fn";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_PUB);
	EXPECT_EQ(get_next_token(lexer)->type, T_FN);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, comment_error){
	char input[] = 	"var input = \"jejda\" //comment comment comment pub fn bomba";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_VAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, ids){
	char input[] = " _ideNtIfi3Cat___2or1 ___________identifier";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, I32_minus){
	char input[] = " -1 ";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, F64_minus){
	char input[] = " -1.0 ";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_F64);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}

TEST_F(test_lexer, unrecognized_input_err){
	char input[] = " Č ";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, string_err_end) {
	char input[] = "\"Hello World;\n;";
	lexer = init_lexer(input);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, string_err_start) {
	char input[] = "Hello World;\n\";";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EXIT(get_next_token(lexer), ExitedWithCode(1), ".*");
}

TEST_F(test_lexer, pseudo_var){
	char input[] = "_ = foo";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_UNDER);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EOF);
}


/*
TEST_F(test_lexer, no_spaces){
	char input[] = "varx=-5";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
}
*/

TEST_F(test_lexer, built_in_one){
	char input[] = "ifj.length ifj.substring";
	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_BUILDIN);
	EXPECT_EQ(get_next_token(lexer)->type, T_BUILDIN);
}


/**
"	const ifj = @import(\"ifj24.zig\"); \n 
	pub fn f (x : i32) i32    // seznam parametru \n 
	{ //deklarace funkce; v IFJ24 nejsou blokove komentare \n
		if(x<10){return x-1;} else {const y = x - 1; // cannot redefine x (shadowing is forbidden) \n
		const res = g(y); \n 
		return res; \n
		} \n
	}"; 
 */
TEST_F(test_lexer, complex){
	char input[] = "const ifj = @import(\"ifj24.zig\"); \n pub fn f (x : i32) i32    // seznam parametru \n { //deklarace funkce; v IFJ24 nejsou blokove komentare \n if(x<10){return x-1;}else{const y = x - 1; // cannot redefine x (shadowing is forbidden) \n const res = g(y); \nreturn res; \n} \n}";

	lexer = init_lexer(input);
	EXPECT_EQ(get_next_token(lexer)->type, T_CONST);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_IMPORT);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_STRING);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_PUB);
	EXPECT_EQ(get_next_token(lexer)->type, T_FN);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_DDOT);
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_DTYPE);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYRBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_IF);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_STHAN);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYRBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_RETURN);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYLBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_ELSE);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYRBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_CONST);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_MINUS);
	EXPECT_EQ(get_next_token(lexer)->type, T_I32);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_CONST);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_EQUAL);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_RPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_LPAR);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_RETURN);
	EXPECT_EQ(get_next_token(lexer)->type, T_ID);
	EXPECT_EQ(get_next_token(lexer)->type, T_SEMI);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYLBRACKET);
	EXPECT_EQ(get_next_token(lexer)->type, T_CUYLBRACKET);
}


