/**
 * IFJ PROJEKT 2024
 * @file symtable.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Implementation of red-black tree and helper function for creation and deletion of entries
 * @date 2024-09-21
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"

#define HASH_CHARS 8

Entry* entry_init(const char* key, Entry_Type type,
				  Expr_Type ret_type, bool can_mut, bool was_used, bool was_assigned) {
	Entry* entry = malloc(sizeof(Entry));

	entry->type = type;
	entry->key = (char*)key;
	entry->ret_type = ret_type;
	entry->is_const_val = false;

	if (type == E_FUNC) {
		entry->as.function_args = arr_init();

	} else if (type == E_VAR) {
		entry->as.can_mut = can_mut;
		entry->was_used = was_used;
	}

	return entry;
}

Node* bvs_node_init(Entry* entry, Node* parent) {
	Node* node = malloc(sizeof(Node));

	node->parent = parent;
	node->entry = entry;
	node->color = RED;

	node->left = NULL;
	node->right = NULL;

	return node;
}

void entry_destroy(Entry* entry) {
	free(entry);
}

void node_destroy(Node* node) {
	entry_destroy(node->entry);
	free(node);
}

Tree* tree_init() {
	Tree* tree = malloc(sizeof(Tree));

	tree->root = NULL;

	return tree;
}

void destroy_traverse(Node* node) {
	if (node->left != NULL) {
		destroy_traverse(node->left);
	}

	if (node->right != NULL) {
		destroy_traverse(node->right);
	}

	node_destroy(node);
}

void tree_destroy(Tree* tree) {
	destroy_traverse(tree->root);
}

/**
 * @brief  a            a
 *		   |		 	|	
 *		   x     ->     y
 *		  / \          / \
 *	 	 b   y        x   d
 *		    / \      / \
 *		   c   d    b   c
 * 
 * @param node is x in diagram
 * @param recolor some rules for insertion need to invert colors of x and y
 */
void rotate_left(Node* node, bool recolor) {
	Node* temp = node->right;
	node->right = temp->left;
	if (node->right != NULL)
		node->right->parent = node;

	temp->parent = node->parent;
	node->parent = temp;

	temp->left = node;

	if (node == temp->parent->left)
		temp->parent->left = temp;
	else
		temp->parent->right = temp;

	if (recolor) {
		node->color = !node->color;
		temp->color = !temp->color;
	}
}

/**
 * @brief  a        a    
 *		   |	    |		 	
 *		   x   ->   y    
 *		  / \      / \   
 *	 	 y   b    c   x  
 *		/ \          / \  
 *	   c   d        d   b 
 * 
 * @param node is x in diagram
 * @param recolor some rules for insertion need to invert colors of x and y
 */
void rotate_right(Node* node, bool recolor) {
	Node* temp = node->left;
	node->left = temp->right;
	if (node->left != NULL)
		node->left->parent = node;

	temp->parent = node->parent;
	node->parent = temp;

	temp->right = node;

	if (node == temp->parent->left)
		temp->parent->left = temp;
	else
		temp->parent->right = temp;

	if (recolor) {
		node->color = !node->color;
		temp->color = !temp->color;
	}
}

void node_fix(Node* node) {
	// If node is root we just recolor root 
	if (node->parent->entry == NULL) {
		node->color = BLACK;
		return;
	}

	// If parent is black it means that no rules were broken
	//  
	if (node->parent->color == BLACK) {
		return;
	}

	// If its only ancestor is root which is black
	// Is taken care of by the previous rule. Here just for exhaustiveness
	if (node->parent->parent->entry == NULL) {
		return;
	}

	// Uncle is on right side
	if (node->parent->parent->left == node->parent) {

		// Uncle is red we just need to recolor nodes
		// The root node of this subtree is recolored to red
		//   which means we need to fix it recursively
		if (node->parent->parent->right != NULL && node->parent->parent->right->color == RED) {
			node->parent->color = BLACK;
			node->parent->parent->color = RED;
			node->parent->parent->right->color = BLACK;
			node_fix(node->parent->parent);

		// Uncle is black
		} else {

			// This situation is fixed by rotation but if 
			//   the inserted node is larger than its parent
			//   to first swap inserted node and its parent
			// If we didnt do that the color rules wouldnt hold
			if (node->parent->right == node) {
				rotate_left(node->parent, false);
				node = node->left;
			}

			// Rotation and recolor to push red up so we can 
			//   put it in to the uncles subtree and keep rules
			rotate_right(node->parent->parent, true);
		}

	} else {
		if (node->parent->parent->left != NULL && node->parent->parent->left->color == RED) {
			node->parent->color = BLACK;
			node->parent->parent->color = RED;
			node->parent->parent->left->color = BLACK;
			node_fix(node->parent->parent);

		} else {
			if (node->parent->left == node) {
				rotate_right(node->parent, false);
				node = node->right;
			}
			rotate_left(node->parent->parent, true);
		}
	}
}

/**
 * @brief Recursive call for insert
 * 
 * @param node 
 * @param entry 
 */
void node_insert(Node* node, Entry* entry) {
	int cmp = strcmp(entry->key, node->entry->key);

	// Smaller then current
	if (cmp < 0) {
		
		// Free space to insert
		if (node->left == NULL) {
			node->left = bvs_node_init(entry, node);
			node_fix(node->left);

		} else {
			// recursive call. We cant insert yet
			node_insert(node->left, entry);
		}

	} else if (cmp > 0) {
		if (node->right == NULL) {
			node->right = bvs_node_init(entry, node);
			node_fix(node->right);

		} else {
			node_insert(node->right, entry);
		}

	} else {
		// Same key. Destroy cur replace with insertee
		if (node->entry != NULL) {
			entry_destroy(node->entry);
		}

		node->entry = entry;
	}
}

/**
 * @brief Takes care of special cases and calls recursive insert
 * 
 * @param tree 
 * @param entry 
 */
void tree_insert(Tree* tree, Entry* entry) {
	// Tree is empty
	if (tree->root == NULL) {
		tree->root = bvs_node_init(entry, NULL);
		tree->root->color = BLACK;

		tree->root->parent = bvs_node_init(NULL, NULL);
		tree->root->parent->left = tree->root;

	} else {
		// Root has a parent which doesnt have an entry
		// It is needed when a rotation happens with root
		Node* temp = tree->root->parent;

		node_insert(tree->root, entry);

		// If a rotation happened on root and it has changed
		//   we need to relink tree->root to the new root
		if (tree->root != temp->left) {
			tree->root = temp->left;
		}
	}
}

Node* tree_find_traverse(Node* node, const char* key) {
	if (node == NULL) {
		return NULL;
	}

	int cmp = strcmp(key, node->entry->key);

	if (cmp < 0) {
		return tree_find_traverse(node->left, key);

	} else if (cmp > 0) {
		return tree_find_traverse(node->right, key);

	} else {
		return node;
	}
}

Entry* tree_find(Tree* tree, const char* key) {
	Node* node = tree_find_traverse(tree->root, key);
	if (node == NULL) {
		return NULL;
	}

	return node->entry;
}

/**
 * @brief Recursive traverse for tree_pop
 * 
 * @param node 
 * @return Entry* 
 */
Entry* tree_pop_traverse(Node** node) {
	if (*node == NULL) {
		return NULL;
	}

	if((*node)->left == NULL && (*node)->right == NULL) {
		Entry* entry = (*node)->entry;
		(*node) = NULL; 
		return entry;
	}

	Entry* entry = tree_pop_traverse(&((*node)->left));
	if(entry != NULL) return entry;

	return tree_pop_traverse(&((*node)->right));
}

/**
 * @brief For context_pop
 * 		  Takes the first node which has no children removes it from the tree
 * 			and returns its entry
 * 
 * @param node 
 * @return Entry* 
 */
Entry* tree_pop(Tree* tree) {
	Entry* entry_old = tree_pop_traverse(&(tree->root));
	if(entry_old == NULL) {
		return NULL;
	}
	
	Entry* entry = malloc(sizeof(Entry));
	memcpy(entry, entry_old, sizeof(Entry));

	return entry;
}

/**
 * @brief Preorder print of tree. It also prints the current depth
 * 
 * @param node 
 * @param depth 
 */
void node_print(Node* node, int depth) {
	if (node == NULL) {
		return;
	}

	printf("%d:%s|", depth, node->entry->key);
	if (node->color == 0) {
		printf("BLACK\n");
	} else {
		printf("RED\n");
	}

	node_print(node->left, depth + 1);
	node_print(node->right, depth + 1);
}

C_Stack init_c_stack(Tree* global_tree) {
	C_Stack stack;

	stack.max_nest = 8;
	stack.arr = malloc(sizeof(Tree*) * stack.max_nest);
	stack.cur_nest = -1;

	stack.global_table = global_tree;

	return stack;
}

void context_push(C_Stack* stack) {
	if(stack->cur_nest >= stack->max_nest - 2) {
		stack->max_nest *= 2;
		Tree** data = realloc(stack->arr, stack->max_nest * sizeof(Tree*));
		
		if(data == NULL) {
			free(stack->arr);
			exit(99);
		}

		stack->arr = data;
	}

	stack->cur_nest++;
	stack->arr[stack->cur_nest] = tree_init();
}

/**
 * @brief Generates a random base64. Its length is determined by HASH_CHARS
 * 
 * @return char* 
 */
char* get_path() {
	const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	char* out = calloc(HASH_CHARS + 2, sizeof(char));

	out[0] = '?';
	for(int i=1; i < HASH_CHARS + 1; i++) {
		int base64_index = rand() % 62;
		out[i] = (char)base64_chars[base64_index];
	}

	out[9] = '\0';

	return out;
}

/**
 * @brief Moves all variables created in the last context to global s-table
 * 		  It needs to assure every variable name is unique s it appends a 
 * 			question mark and a random base64 string after it to var name
 * 
 * @param stack 
 * @return true 
 * @return false 
 */
bool context_pop(C_Stack* stack) {
	if(stack->cur_nest < 0) {
		return false;
	}

	Entry* entry;
	while (true){
		// Get any var from context
		entry = tree_pop(stack->arr[stack->cur_nest]);
		if(entry == NULL) {
			break;
		}

		if(entry->key == NULL) {
			exit(99);
		}
		int orig_len = strlen(entry->key);

		// Append base64 to var name
		// Even though the chance is very low. If a name with the same name and
	 	//   the same base64 string already exists, generate base64 again
		while(true) {
			char* path = get_path();
			memcpy(((char*)entry->key) + orig_len, path, HASH_CHARS + 2);
			
			if(tree_find(stack->global_table, entry->key) == NULL) { 
				tree_insert(stack->global_table, entry);
				break;
			}
		}
	}

	stack->cur_nest--;

	return true;
}

/**
 * @brief Find var through all levels of nest in context
 * 
 * @param stack 
 * @param key 
 * @param global 
 * @return Entry* 
 */
Entry* context_find(C_Stack* stack, const char* key, bool global) {
	int cur_nest = stack->cur_nest;
	
	while(cur_nest >= 0) {
		Entry* entry = tree_find(stack->arr[cur_nest], key);
		
		if(entry != NULL) {
			return entry;
		}

		cur_nest--;
	}

	if(global) {
		return tree_find(stack->global_table, key);
	
	} else {
		return NULL;
	}
}

/**
 * @brief Insert var into the last context nest
 * 
 * @param stack 
 * @param entry 
 */
void context_put(C_Stack* stack, Entry* entry) {
	if(stack->cur_nest < 0) {
		tree_insert(stack->global_table, entry);
		return;
	}

	if(context_find(stack, entry->key, true) != NULL) {
		fprintf(stderr, "Returned error code: %d\n", 5);
		exit(5); // ERROR_RET(ERR_SEM_REDEF); 
			     // Dont want to import because of a single possible error
	}

	tree_insert(stack->arr[stack->cur_nest], entry);
}

/**
 * @brief Debug print tree
 * 
 * @param tree 
 */
void tree_print(Tree* tree) {
	node_print(tree->root, 0);
}
