/**
 * IFJ PROJEKT 2024
 * @file expressionparser.c
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Implementation of expression parser for IFJ 2024
 *
 * @date 2024-10-15
 *
 */

#include "expression_parser.h"

int precedence_table[TABLE_SIZE][TABLE_SIZE] = {

//   id +  -  *  /  (  )  $  == != <  >  <= >=
	{N, L, L, L, L, N, L, L, L, L, L, L, L, L}, // id
	{R, L, L, R, R, R, L, L, L, L, L, L, L, L}, // +
	{R, L, L, R, R, R, L, L, L, L, L, L, L, L}, // -
	{R, L, L, L, L, R, L, L, L, L, L, L, L, L}, // *
	{R, L, L, L, L, R, L, L, L, L, L, L, L, L}, // /
	{R, R, R, R, R, R, M, R, R, R, R, R, R, R}, // (
	{N, L, L, L, L, N, L, L, L, L, L, L, L, L}, // )
	{R, R, R, R, R, R, N, R, R, R, R, R, R, R}, // $
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // ==
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // !=
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // <
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // >
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // <=
	{R, R, R, R, R, R, L, L, N, N, N, N, N, N}, // >=
};

AST_Type get_type(T_Type type) {
	switch (type) {
	case T_I32:
		return I32;
	case T_F64:
		return F64;
	case T_STRING:
		return STRING;
	case T_ID:
		return ID;
	case T_NULL:
		return NIL;
	}
	ERROR_RET(ERR_PARSE);
	return 42;
}
AST_Type get_type_from_rule(int rule) {
	switch (rule) {
	case E_PLUS_E:
		return PLUS;
	case E_MINUS_E:
		return MINUS;
	case E_TIMES_E:
		return MUL;
	case E_DIV_E:
		return DIV;
	case E_EQ_E:
		return ISEQ;
	case E_NEQ_E:
		return ISNEQ;
	case E_GET_E:
		return ISMOREEQ;
	case E_GT_E:
		return ISMORE;
	case E_ST_E:
		return ISLESS;
	case E_SET_E:
		return ISLESSEQ;
	}
	ERROR_RET(ERR_PARSE);
	return 42;
}

void adjust_stack_rule_E_ID(List* Stack, Token* processed) {
	List_activeL(Stack);
	List_removeL(Stack);
	List_removeL(Stack);
	List_insertL(Stack, processed);
}
void adjust_stack_rule_E_E(List* Stack, Token* processed) {
	List_removeL(Stack);
	List_removeL(Stack);
	List_removeL(Stack);
	List_removeL(Stack);
	List_insertL(Stack, processed);
}
void adjust_stack_else(List* Stack, AST_Node* middle_node) {
	List_removeL(Stack);
	List_removeL(Stack);
	List_removeL(Stack);
	List_removeL(Stack);
	Token* middle_token = init_token("0", T_ID, 1); // has to be ID, plus is in node
	middle_token->node = middle_node;
	middle_token->isProcessed = true;
	List_insertL(Stack, middle_token);
}

bool is_nonTerm(T_Type type, bool isProcessed) {
	if (type == T_LEFTSHIFTLIST || type == T_RIGHTSHIFTLIST || isProcessed == true) {
		return false;
	} else if (type == T_ID || type == T_PLUS || type == T_MINUS || type == T_MUL ||
			   type == T_DIV || type == T_LPAR || type == T_RPAR || type == T_DOLLARLIST ||
			   type == T_I32 || type == T_F64 || type == T_STRING || type == T_DDEQ ||
			   type == T_GETHAN || type == T_GTHAN || type == T_SETHAN || type == T_STHAN ||
			   type == T_NEQUAL || type == T_BUILDIN || type == T_NULL) {
		return true;
	} else {
		ERROR_RET(2);
	}
}

bool is_id(T_Type type) {
	if (type == T_ID || type == T_F64 || type == T_I32 || type == T_STRING || type == T_DDEQ ||
		type == T_NEQUAL || type == T_GTHAN || type == T_GETHAN || type == T_SETHAN ||
		type == T_STHAN || type == T_BUILDIN || type == T_NULL) {
		return true;
	}
	return false;
}

int find_precedence_index(List* list) {
	List_activeL(list);
	// we take always last as a precedence
	Token* current;
	while (1) 
	{
		List_get_val(list, &current);
		if (((current->type == T_ID && current->isProcessed == false) ||
			 (current->type == T_I32 && current->isProcessed == false) || current->type == T_F64 ||
			 current->type == T_ID || current->type == T_STRING || current->type == T_NULL ||
			 current->type == T_BUILDIN) &&
			current->isProcessed == false) {
			return 0;
		} else if (current->type == T_PLUS) {
			return 1;
		} else if (current->type == T_MINUS) {
			return 2;
		} else if (current->type == T_MUL) {
			return 3;
		} else if (current->type == T_DIV) {
			return 4;
		} else if (current->type == T_RPAR) {
			return 5;
		} else if (current->type == T_LPAR) {
			return 6;
		} else if (current->type == T_DOLLARLIST) {
			return 7;
		} else if (current->type == T_DDEQ) {
			return 8;
		} else if (current->type == T_NEQUAL) {
			return 9;
		} else if (current->type == T_STHAN) {
			return 10;
		} else if (current->type == T_GTHAN) {
			return 11;
		} else if (current->type == T_SETHAN) {
			return 12;
		} else if (current->type == T_GETHAN) {
			return 13;
		} else {
			List_active_prev(list);
		}
	}
	// error checking
}

void init_stack(List* Stack) {
	List_init(Stack);
	Token* dollartemp = init_token("", T_DOLLARLIST, 1);
	List_insertF(Stack, dollartemp);
}

void insert_dollar(List* input) {
	Token* dollartemp = init_token("", T_DOLLARLIST, 1);
	List_insertF(input, dollartemp);
}

void handle_rule(int rule, List* Stack) {
	if (rule == E_ID) {
		// <i
		Token* processed_token = Stack->last->token;
		if (processed_token->type == T_BUILDIN) {
			// TODOVACKO
			processed_token->isProcessed = true;
			adjust_stack_rule_E_ID(Stack, processed_token);
			return;
		}
		AST_Type nodetype = get_type(processed_token->type);
		AST_Node* node = node_init(nodetype);
		// here we should add value to the nodes
		processed_token->isProcessed = true;
		processed_token->node = node;
		// now the token is prepared to be pushed as an E symbol (isProcessed)
		char* end;
		if (processed_token->type == T_I32) {
			node->as.i32 = strtol(processed_token->value, &end, 10);

		} else if (processed_token->type == T_F64) {
			node->as.f64 = strtod(processed_token->value, &end);

		} else if (processed_token->type == T_ID) {
			node->as.var_name = processed_token->value;

		} else if (processed_token->type == T_STRING) {
			node->as.string = processed_token->value;
		}
		adjust_stack_rule_E_ID(Stack, processed_token);
	} else if (rule == E_E) {
		Token* processed_token = Stack->last->previousElement->token;
		adjust_stack_rule_E_E(Stack, processed_token);
	} else {
		// handling <E+E rule
		AST_Node* right_node = Stack->last->token->node;
		AST_Node* left_node = Stack->last->previousElement->previousElement->token->node;
		AST_Node* middle_node = node_init(get_type_from_rule(rule));
		middle_node->right = right_node;
		middle_node->left = left_node;
		adjust_stack_else(Stack, middle_node);
	}
}

int find_reduction_rule(List* Stack) {
	// need to add isProcessed check, so we wont take <i+i as and rule, However this situation
	// shouldnt occour.

	List_activeL(Stack);
	Token* last = Stack->last->token;
	Token* prev = Stack->last->previousElement->previousElement->token;
	bool id_first = is_id(last->type);
	bool id_second = is_id(prev->type);
	bool id_middle = is_id(Stack->last->previousElement->token->type);
	T_Type op = Stack->last->previousElement->token->type;
	if (Stack->last->previousElement->token->type == T_LEFTSHIFTLIST) {
		return E_ID;
	}
	if (id_first && id_second) {
		switch (op) {
		case T_PLUS:
			return E_PLUS_E;
		case T_MINUS:
			return E_MINUS_E;
		case T_MUL:
			return E_TIMES_E;
		case T_DIV:
			return E_DIV_E;
		case T_DDEQ:
			return E_EQ_E;
		case T_NEQUAL:
			return E_NEQ_E;
		case T_GETHAN:
			return E_GET_E;
		case T_GTHAN:
			return E_GT_E;
		case T_SETHAN:
			return E_SET_E;
		case T_STHAN:
			return E_ST_E;
		default:
			ERROR_RET(2);
		}
	} else if (last->type == T_LPAR && id_middle && prev->type == T_RPAR) {
		return E_E;
	} else if (id_first) {
		return E_ID;
	} else {
		ERROR_RET(2);
	}
}

void handle_precedence(int precedence, List* Stack, List* input) {
	// N = 0 mistake, L = >, R = <, M = =
	// L means reduction, R means push to stack
	if (precedence == N) {
		ERROR_RET(2);
	} else if (precedence == L) {
		// HERE SHOULD AST CREATION START

		int rule = find_reduction_rule(Stack);
		handle_rule(rule, Stack);
	} else if (precedence == R) {
		// we are active on last nonterminal symbol, we want to push < and token behind it
		Token* shifttoken;
		Token* transitionToken;
		shifttoken = init_token("0", T_LEFTSHIFTLIST, 1);
		List_insert_after(Stack, shifttoken);
		List_last_val(input, &transitionToken);
		List_insertL(Stack, transitionToken);
		List_removeL(input);
		// all operations have been completed and now we can safely return back to loop
		List_activeL(Stack);
		List_activeL(input);

	} else if (precedence == M) {
		Token* transitionToken;
		List_last_val(input, &transitionToken);
		List_insertL(Stack, transitionToken);
		List_removeL(input);
		List_activeL(Stack);
		List_activeL(input);
	}
}

AST_Node* parse_expression(List* input) {
	List* Stack = malloc(sizeof(List));
	if (Stack == NULL) {
		ERROR_RET(ERR_INTERN);
	}
	init_stack(Stack);

	insert_dollar(input); // inserts first because input looks like $i*i+i
	// now we have one list that works like stack, and one normal list that is input
	// until both contain dollar and Stack has E behind it we work
	// we need to transform all tokens like flaot and I32 etc to id tokens.
	List_activeL(Stack);
	List_activeL(input);
	int precedence_stack, precendence_input, precendence_operation = 0;

	while (1) {
		precedence_stack = find_precedence_index(Stack);
		precendence_input = find_precedence_index(input);
		precendence_operation = precedence_table[precedence_stack][precendence_input];
		handle_precedence(precendence_operation, Stack, input);
		if (List_get_first_type(Stack) == T_DOLLARLIST &&
			List_get_first_type(input) == T_DOLLARLIST && Stack->length == 2 &&
			input->length == 1) {
			break;
		}
	}

	return Stack->last->token->node; // returning last contain of stack that should be Stack:
									 // $E(which is ID that is processed)
}
