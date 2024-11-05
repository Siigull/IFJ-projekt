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

const char* type_to_string(T_Type type) {
	switch (type) {
	case T_F64:
		return "T_F64";
	case T_I32:
		return "T_I32";
	case T_U8:
		return "T_U8";
	case T_DTYPE:
		return "T_DTYPE";
	case T_DOT:
		return "T_DOT";
	case T_QUOTATION:
		return "T_QUOTATION";
	case T_QUESTMARK:
		return "T_QUESTMARK";
	case T_EXCLEMARK:
		return "T_EXCLEMARK";
	case T_EXPONENT:
		return "T_EXPONENT";
	case T_STRING:
		return "T_STRING";
	case T_COMMENT:
		return "T_COMMENT";
	case T_BUILDIN:
		return "T_BUILDIN";
	case T_RPAR:
		return "T_RPAR";
	case T_LPAR:
		return "T_LPAR";
	case T_SQRBRACKET:
		return "T_SQRBRACKET";
	case T_SQLBRACKET:
		return "T_SQLBRACKET";
	case T_CUYRBRACKET:
		return "T_CUYRBRACKET";
	case T_CUYLBRACKET:
		return "T_CUYLBRACKET";
	case T_BAR:
		return "T_BAR";
	case T_CONST:
		return "T_CONST";
	case T_IF:
		return "T_IF";
	case T_ELSE:
		return "T_ELSE";
	case T_FN:
		return "T_FN";
	case T_PUB:
		return "T_PUB";
	case T_RETURN:
		return "T_RETURN";
	case T_VAR:
		return "T_VAR";
	case T_VOID:
		return "T_VOID";
	case T_WHILE:
		return "T_WHILE";
	case T_IMPORT:
		return "T_IMPORT";
	case T_SEMI:
		return "T_SEMI";
	case T_DDOT:
		return "T_DDOT";
	case T_COMMA:
		return "T_COMMA";
	case T_UNDER:
		return "T_UNDER";
	case T_ID:
		return "T_ID";
	case T_PLUS:
		return "T_PLUS";
	case T_MINUS:
		return "T_MINUS";
	case T_MUL:
		return "T_MUL";
	case T_DIV:
		return "T_DIV";
	case T_DOLLARLIST:
		return "T_DOLLARLIST";
	case T_DDEQ:
		return "T_DDEQ";
	case T_LEFTSHIFTLIST:
		return "T_LEFTSHIFTLIST";
	case T_RIGHTSHIFTLIST:
		return "T_RIGHTSHIFTLIST";
	case T_EQUAL:
		return "T_EQUAL";
	case T_NEQUAL:
		return "T_NEQUAL";
	case T_GTHAN:
		return "T_GTHAN";
	case T_GETHAN:
		return "T_GETHAN";
	case T_STHAN:
		return "T_STHAN";
	case T_SETHAN:
		return "T_SETHAN";
	case T_OPERATOR:
		return "T_OPERATOR";
	case T_UNDEF:
		return "T_UNDEF";
	case T_NULL:
		return "T_NULL";
	case T_EOF:
		return "T_EOF";
	case T_ERR:
		return "T_ERR";
	case T_PROLOG:
		return "T_PROLOG";
	default:
		return "UNKNOWN_TYPE"; // If the type is not recognized
	}
}

bool is_nonTerm(T_Type type, bool isProcessed) {
	if (type == T_LEFTSHIFTLIST || type == T_RIGHTSHIFTLIST || isProcessed == true) {
		return false;
	} else if (type == T_ID || type == T_PLUS || type == T_MINUS || type == T_MUL ||
			   type == T_DIV || type == T_LPAR || type == T_RPAR || type == T_DOLLARLIST ||
			   type == T_I32 || type == T_F64 || type == T_STRING) {
		return true;
	} else {
		exit(2);
	}
}

bool is_id(T_Type type) {
	if (type == T_ID || type == T_F64 || type == T_I32) {
		return true;
	}
	return false;
}

int find_precedence_index(List* list) {
	List_activeL(list);
	// we take always last as a precedence
	Token* current;
	// IS DEQ CARE
	while (1) // THIS CAN BE DONE BETTER
	{
		List_get_val(list, &current);
		if ((current->type == T_ID && current->isProcessed == false) ||
			(current->type == T_I32 && current->isProcessed == false) || current->type == T_F64) {
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
	if (rule == 0) {
		// handling <E+E rule
		AST_Node* firstNode = Stack->last->token->node;
		AST_Node* secondeNode = Stack->last->previousElement->previousElement->token->node;
		AST_Node* plusnode = node_init(PLUS);
		plusnode->left = firstNode;
		plusnode->right = secondeNode;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		Token* plustoken = init_token("0", T_ID, 1); // has to be ID, plus is in node
		plustoken->node = plusnode;
		plustoken->isProcessed = true;
		List_insertL(Stack, plustoken);
	} else if (rule == 1) {
		// handling <E-E rule
		AST_Node* firstNode = Stack->last->token->node;
		AST_Node* secondeNode = Stack->last->previousElement->previousElement->token->node;
		AST_Node* minusnode = node_init(MINUS);
		minusnode->left = firstNode;
		minusnode->right = secondeNode;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		Token* minustoken = init_token("0", T_ID, 1); // has to be ID, plus is in node
		minustoken->node = minusnode;
		minustoken->isProcessed = true;
		List_insertL(Stack, minustoken);

	} else if (rule == 2) {
		// handling <E*E rule
		AST_Node* firstNode = Stack->last->token->node;
		AST_Node* secondeNode = Stack->last->previousElement->previousElement->token->node;
		AST_Node* mulnode = node_init(MUL);
		mulnode->left = firstNode;
		mulnode->right = secondeNode;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		Token* multoken = init_token("0", T_ID, 1); // has to be ID, plus is in node
		multoken->node = mulnode;
		multoken->isProcessed = true;
		List_insertL(Stack, multoken);
	} else if (rule == 3) {
		// handling <E/E rule
		AST_Node* firstNode = Stack->last->token->node;
		AST_Node* secondeNode = Stack->last->previousElement->previousElement->token->node;
		AST_Node* divnode = node_init(DIV);
		divnode->left = firstNode;
		divnode->right = secondeNode;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		Token* divtoken = init_token("0", T_ID, 1); // has to be ID, plus is in node
		divtoken->node = divnode;
		divtoken->isProcessed = true;
		List_insertL(Stack, divtoken);
	} else if (rule == 4) {
		// handling  E → E == E, <E==E rule
		//  from rule 2, has minus node names but works the same
		AST_Node* firstNode = Stack->last->token->node;
		AST_Node* secondeNode = Stack->last->previousElement->previousElement->token->node;
		AST_Node* minusnode = node_init(ISEQ);
		minusnode->left = firstNode;
		minusnode->right = secondeNode;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		Token* minustoken = init_token("0", T_ID, 1); // has to be ID, plus is in node
		minustoken->node = minusnode;
		minustoken->isProcessed = true;
		List_insertL(Stack, minustoken);
	} else if (rule == 5) {
		// ( E ), we do not care at all about the brackets and thats why we just remove them and the
		// token will remain the same
		Token* processed_token = Stack->last->previousElement->token;
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_insertL(Stack, processed_token);
	} else if (rule == 6) {
		// <i
		Token* processed_token = Stack->last->token;
		AST_Type nodetype;
		if (processed_token->type == T_ID) {
			nodetype = ID;
		} else if (processed_token->type == T_I32) {
			nodetype = I32;
		} else if (processed_token->type == T_F64) {
			nodetype = F64;
		} else {
			exit(2);
		}
		AST_Node* node = node_init(nodetype);
		// here we should add value to the nodes
		if (nodetype == ID || nodetype == I32) {
			node->as.var_name = processed_token->value;
		}
		processed_token->isProcessed = true;
		processed_token->node = node;
		// now the token is prepared to be pushed as an E symbol (isProcessed)
		List_activeL(Stack);
		List_removeL(Stack);
		List_removeL(Stack);
		List_insertL(Stack, processed_token);
	}
}

int find_reduction_rule(List* Stack) {
	List_activeL(Stack);
	// need to add isProcessed check, so we wont take <i+i as and rule, However this situation
	// shouldnt occour.
	fprintf(stderr, "BEFORE SEG FULT\n");
	if (is_id(Stack->last->token->type) && Stack->last->previousElement->token->type == T_PLUS &&
		is_id(Stack->last->previousElement->previousElement->token->type)) {
		return 0;
	}
	if (is_id(Stack->last->token->type) && Stack->last->previousElement->token->type == T_MINUS &&
		is_id(Stack->last->previousElement->previousElement->token->type)) {
		return 1;
	}
	if (is_id(Stack->last->token->type) && Stack->last->previousElement->token->type == T_MUL &&
		is_id(Stack->last->previousElement->previousElement->token->type)) {
		return 2;
	}
	if (is_id(Stack->last->token->type) && Stack->last->previousElement->token->type == T_DIV &&
		is_id(Stack->last->previousElement->previousElement->token->type)) {
		return 3;
	}
	if (is_id(Stack->last->token->type) && Stack->last->previousElement->token->type == T_DDEQ &&
		is_id(Stack->last->previousElement->previousElement->token->type)) {
		return 4;
	}
	if (Stack->last->token->type == T_LPAR && is_id(Stack->last->previousElement->token->type) &&
		Stack->last->previousElement->previousElement->token->type == T_RPAR) {
		return 5;
	}
	if (is_id(Stack->last->token->type)) {
		return 6;
	}
	exit(2);
}

void handle_precedence(int precedence, List* Stack, List* input) {
	// N = 0 mistake, L = >, R = <, M = =
	// L means reduction, R means push to stack
	if (precedence == 0) {
		exit(2);
	} else if (precedence == 1) {
		// HERE SHOULD AST CREATION START

		int rule = find_reduction_rule(Stack);
		handle_rule(rule, Stack);
	} else if (precedence == 2) {
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

	} else if (precedence == 3) {
	}
}

void print_list(const List* list) {
	if (list == NULL || list->first == NULL) {
		fprintf(stderr, "List is empty.\n");
		return;
	}

	List_elem* current = list->first; // Start with the first element
	while (current != NULL) {         // Iterate through the list
		// Use type_to_string to print the name of the type
		fprintf(stderr, "%s ", type_to_string(current->token->type));
		current = current->nextElement; // Move to the next element
	}
	fprintf(stderr, "\n"); // Print a newline at the end
}

AST_Node* parse_expression(List* input) {
	List* Stack = malloc(sizeof(List));
	if (Stack == NULL) {
		exit(3213); // lol
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
		fprintf(stderr,
				"BEFORE OP: %d, STACK: %d INPUT: %d \n",
				precendence_operation,
				precedence_stack,
				precendence_input);
		print_list(Stack);
		print_list(input);
		handle_precedence(precendence_operation, Stack, input);
		fprintf(stderr, "AFTER\n");
		print_list(Stack);
		print_list(input);
		if (List_get_first_type(Stack) == T_DOLLARLIST &&
			List_get_first_type(input) == T_DOLLARLIST && Stack->length == 2 &&
			input->length == 1) {
			break;
		}
	}

	fprintf(stderr,
			"sda%s %s",
			Stack->last->token->node->right->as.var_name,
			Stack->last->token->node->left->as.var_name);

	return Stack->last->token->node; // returning last contain of stack that should be Stack:
									 // $E(which is ID that is processed)
}