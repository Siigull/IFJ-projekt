/**
 * IFJ PROJEKT 2024
 * @file symtable.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Function and struct definition of red-black tree
 * @date 2024-09-21
 *
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "helper.h"

/**
 * @brief Return type used in semantics and s-table entries
 * 
 */
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
	R_NULL,
	R_TERM_STRING,
	IMPLICIT,
} Expr_Type;

/**
 * @brief Function argument in arr of function_args
 * 
 */
typedef struct {
	char* arg_name;
	Expr_Type type;
} Function_Arg;

/**
 * @brief Types of entry
 * 
 */
typedef enum {
	E_FUNC,
	E_VAR,
} Entry_Type;

/**
 * @brief Var/Func entry in node of red-black tree
 * 
 */
typedef struct Entry {
	Entry_Type type; // Var or function

	const char* key; // id of func or var
	Expr_Type ret_type; // Only Ret_Type_ in struct used here
	
	// Used in Semantic analysis
	bool was_used; 
	bool was_assigned;

	// Used in semantic analysis. Not required by assignment
	// Equivalent of comptime val in zig
	// Inserted into expressions as literal. Retype only in relational operators
	bool is_const_val;
	union {
		int i32;
		double f64;
	} const_val;

	// Based on entry type
	// function_args if func, can_mut for vars
	union {
		Arr* function_args;
		bool can_mut;
	} as;
} Entry;

/**
 * @brief Red-black tree node color to int/bool
 * 
 */
typedef enum { BLACK = 0, RED = 1 } Node_Color;

/**
 * @brief Node of red-black tree. false->black
 * 
 */
typedef struct Node {
	Entry* entry;

	bool color;
	struct Node* parent;
	struct Node* left;
	struct Node* right;
} Node;

/**
 * @brief Red-black tree
 * 
 */
typedef struct Tree {
	Node* root;
} Tree;

/**
 * @brief Stack of contexts to know the scope of vars
 * 		  Global s-table is the same in parser 
 * 
 */
typedef struct Context_Stack {
	int cur_nest;
	int max_nest;

	Tree** arr;
	Tree* global_table;
} C_Stack;

Entry* entry_init(const char* key, Entry_Type type,
				  Expr_Type ret_type, bool can_mut, bool was_used, bool was_assigned);

/**
 * @brief Returns allocated tree
 * 
 * @return Tree* 
 */
Tree* tree_init();

/**
 * @brief Frees all nodes but not tree itself
 * 
 * @param tree 
 */
void tree_destroy(Tree* tree);

/**
 * @brief Inserts entry based on its key, rewrites if exists
 * 
 * @param tree 
 * @param entry 
 */
void tree_insert(Tree* tree, Entry* entry);

/**
 * @brief Remove node with same key in entry
 * 
 * @param tree 
 * @param key 
 * @return true if entry with key exists
 * @return false if entry with key doesnt exist
 */
bool tree_delete(Tree* tree, const char* key);

/**
 * @brief Returns entry with same key
 * 
 * @param tree 
 * @param key 
 * @return Entry* 
 */
Entry* tree_find(Tree* tree, const char* key);
/**
 * @brief Returns and removes the first not null node with no children
 * 		  Traverse is first left then right
 * 
 * @param tree 
 * @return Entry* 
 */
Entry* tree_pop(Tree* tree);

/**
 * @brief Returns c_stack as value
 * 		  Stack of trees/s-tables allocates 8 max_nest
 * 
 * @param global_tree 
 * @return C_Stack 
 */
C_Stack init_c_stack(Tree* global_tree);

/**
 * @brief When parser enters new nest increases cur_nest 
 * 		  Doubles max_nest with realloc if needs more
 * 
 * @param stack 
 */
void context_push(C_Stack* stack);

/**
 * @brief When parser goes out of a nest decreases cur_nest
 * 		  It also gives all variables from context a unique name
 * 		  	and puts them to the global symbol table 
 * 
 * @param stack 
 * @return true 
 * @return false 
 */
bool context_pop(C_Stack* stack);

/**
 * @brief Finds var entry. Searches through all levels of nest
 * 
 * @param stack 
 * @param key 
 * @param global It also searches functions which are in global s-table
 * @return Entry* 
 */
Entry* context_find(C_Stack* stack, const char* key, bool global);

/**
 * @brief Inserts var into current context
 * 
 * @param stack 
 * @param entry 
 */
void context_put(C_Stack* stack, Entry* entry);

/**
 * @brief Prints tree for debug in preorder 
 * 
 * @param tree 
 */
void tree_print(Tree* tree);

#endif /* SYMTABLE_H */
