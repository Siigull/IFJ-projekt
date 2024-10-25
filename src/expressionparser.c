#include "expressionparser.h"

int precedence_table[TABLE_SIZE][TABLE_SIZE] = {
	//   id +  -  *  /  (  )  $
	{N, L, L, L, L, N, L, L}, // id
	{R, L, L, R, R, R, L, L}, // +
	{R, L, L, R, R, R, L, L}, // -
	{R, L, L, L, L, R, L, L}, // *
	{R, L, L, L, L, R, L, L}, // /
	{R, R, R, R, R, R, M, N}, // (
	{N, L, L, L, L, N, L, L}, // )
	{R, R, R, R, R, R, N, R}  // $

};

void init_stack(List* Stack){
	List_init(Stack);
	Token* dollartemp = init_token("", T_DOLLARLIST, 1);
	List_insertF(Stack, dollartemp);
}

AST_Node* parse_expression(List* list) {
	List* Stack;
	init_stack(Stack);
}
