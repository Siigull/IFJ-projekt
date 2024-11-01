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
	R_F64,
	R_U8,
	IMPLICIT,
} Ret_Type;

typedef struct Entry {
	Entry_Type type;
	
	const char* key; // TODO(Sigull): change const char* string to our own type
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

Entry* entry_init(const char* key, Entry_Type type,
				  Ret_Type ret_type, bool has_null,
				  bool can_mut);

Tree* tree_init();
void tree_destroy(Tree* tree);

void tree_insert(Tree* tree, Entry* entry);
bool tree_delete(Tree* tree, const char* key);
Entry* tree_find(Tree* tree, const char* key);

void tree_print(Tree* tree);

#endif /* BVS_H */
