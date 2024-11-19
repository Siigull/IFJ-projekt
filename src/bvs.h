/**
 * @file bvs.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Function and struct definition of red-black tree
 * @date 2024-09-21
 *
 */

#ifndef BVS_H
#define BVS_H

#include <stdbool.h>
#include "helper.h"

typedef enum { BLACK = 0, RED = 1 } Node_Color;

typedef enum {
	E_FUNC,
	E_VAR,
} Entry_Type;

typedef enum {
	R_VOID,
	R_I32,
	N_I32,
	R_F64,
	N_F64,
	R_U8,
	N_U8,
	R_STRING,
	R_BOOLEAN,
	IMPLICIT,
} Ret_Type_;

typedef struct {
	Ret_Type_ type;
	bool is_const_literal;
} Expr_Type;

typedef struct {
	char* arg_name;
	Ret_Type_ type;
} Function_Arg;

typedef struct Entry {
	Entry_Type type;
	
	const char* key; // TODO(Sigull): change const char* string to our own type
	Expr_Type ret_type;
	bool has_null;

	union {
		Arr* function_args;
		bool can_mut;
	} as;
} Entry;

typedef struct Node {
	Entry* entry;

	bool color;
	struct Node* parent;
	struct Node* left;
	struct Node* right;
} Node;

typedef struct Tree {
	Node* root;
} Tree;

typedef struct Context {

} Context;

typedef struct Context_Stack {
	int cur_nest;
	int max_nest;
	
	Context* arr;
} C_Stack;

Entry* entry_init(const char* key, Entry_Type type,
				  Expr_Type ret_type, bool can_mut);

Tree* tree_init();
void tree_destroy(Tree* tree);

void tree_insert(Tree* tree, Entry* entry);
bool tree_delete(Tree* tree, const char* key);
Entry* tree_find(Tree* tree, const char* key);

void tree_print(Tree* tree);

#endif /* BVS_H */
