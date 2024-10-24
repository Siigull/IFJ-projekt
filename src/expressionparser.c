#include "expressionparser.h"



void init_stack(List* Stack){
	List_init(Stack);
	Token* dollartemp = init_token("", T_DOLLARLIST, 1);
	List_insertF(Stack, dollartemp);
}




AST_Node* parse_expression(List* list) {
	List* Stack;
	init_stack(Stack);
}
