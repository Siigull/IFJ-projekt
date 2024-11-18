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
	IMPLICIT,
} Ret_Type;

typedef struct {
	char* arg_name;
	Ret_Type type;
} Function_Arg;

typedef struct Entry {
	Entry_Type type;
	
	char* key; // TODO(Sigull): change const char* string to our own type
	Ret_Type ret_type;
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

typedef struct Context_Stack {
	int cur_nest;
	int max_nest;
	
	Tree** arr;
	Tree* global_table;
	int* path;
	int cur_path;
	int max_path;
	bool last_up;
} C_Stack;

Entry* entry_init(const char* key, Entry_Type type,
				  Ret_Type ret_type, bool can_mut);

Tree* tree_init();
void tree_destroy(Tree* tree);

void tree_insert(Tree* tree, Entry* entry);
bool tree_delete(Tree* tree, const char* key);
Entry* tree_find(Tree* tree, const char* key);
Entry* tree_pop(Tree* tree);

C_Stack init_c_stack(Tree* global_tree);
void context_push(C_Stack* stack);
bool context_pop(C_Stack* stack);
Entry* context_find(C_Stack* stack, const char* key);
void context_put(C_Stack* stack, Entry* entry);

void tree_print(Tree* tree);

#endif /* BVS_H */
