/**
 * @file bvs.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Implementation of red-black tree and helper function for creation and deletion of entries
 * @date 2024-09-21
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bvs.h"

Entry* entry_init(const char* key, Entry_Type type,
				  Ret_Type ret_type, bool can_mut) {
	Entry* entry = malloc(sizeof(Entry));

	entry->type = type;
	entry->key = (char*)key;
	entry->ret_type = ret_type;

	if (type == E_FUNC) {
		entry->as.function_args = arr_init();
	} else if (type == E_VAR) {
		entry->as.can_mut = can_mut;
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
	if (node->parent->entry == NULL) {
		node->color = BLACK;
		return;
	}

	if (node->parent->parent->entry == NULL) {
		return;
	}

	if (node->parent->color == BLACK) {
		return;
	}

	if (node->parent->parent->left == node->parent) {
		if (node->parent->parent->right != NULL && node->parent->parent->right->color == RED) {
			node->parent->color = BLACK;
			node->parent->parent->color = RED;
			node->parent->parent->right->color = BLACK;
			node_fix(node->parent->parent);

		} else {
			if (node->parent->right == node) {
				rotate_left(node->parent, false);
				node = node->left;
			}
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

void node_insert(Node* node, Entry* entry) {
	int cmp = strcmp(entry->key, node->entry->key);

	if (cmp < 0) {
		if (node->left == NULL) {
			node->left = bvs_node_init(entry, node);
			node_fix(node->left);

		} else {
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
		if (node->entry != NULL) {
			entry_destroy(node->entry);
		}

		node->entry = entry;
	}
}

void tree_insert(Tree* tree, Entry* entry) {
	if (tree->root == NULL) {
		tree->root = bvs_node_init(entry, NULL);
		tree->root->color = BLACK;

		tree->root->parent = bvs_node_init(NULL, NULL);
		tree->root->parent->left = tree->root;

	} else {
		Node* temp = tree->root->parent;

		node_insert(tree->root, entry);

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

Entry* tree_pop_traverse(Node* node) {
	if (node == NULL) {
		return NULL;
	}

	if(node->left == NULL && node->right == NULL) {
		return node->entry;
	}

	Entry* entry = tree_pop_traverse(node->left);
	if(entry != NULL) return entry;

	return tree_pop_traverse(node->right);
}

Entry* tree_pop(Tree* tree) {
	Entry* entry_old = tree_pop_traverse(tree->root);
	if(entry_old == NULL) {
		return NULL;
	}
	
	Entry* entry = malloc(sizeof(Entry));
	memcpy(entry, entry_old, sizeof(Entry));

	tree_delete(tree, entry->key);

	return entry;
}

Node* transplant(Node* from, Node* to) {
	if (to->parent->left == to) {
		to->parent->left = from;

	} else {
		to->parent->right = from;
	}

	if (from != NULL) {
		from->parent = to->parent;
	}

	node_destroy(to);

	return from;
}

Node* find_smallest_node(Node* node) {
	if (node->left == NULL) {
		return node;
	}

	return find_smallest_node(node->left);
}

void node_delete_fix(Node* parent, bool dir) {
	if (parent->entry == NULL) {
		if (parent->left != NULL) {
			parent->left->color = BLACK;
			return;
		}
	}

	if (dir == 0) {
		if (parent->left != NULL && parent->left->color == RED) {
			parent->left->color = BLACK;
			return;
		}

		Node* sibling = parent->right;
		if (sibling != NULL && sibling->color == RED) {
			sibling->color = BLACK;
			parent->color = RED;
			sibling = sibling->left;
			rotate_left(parent, false);
		}

		if ((sibling->right == NULL || sibling->right->color == BLACK) &&
			(sibling->left == NULL || sibling->left->color == BLACK)) {
			sibling->color = RED;
			node_delete_fix(parent->parent, parent->parent->right == parent);

		} else {
			if (sibling->right == NULL || sibling->right->color == BLACK) {
				sibling->right->color = BLACK;
				sibling->color = RED;
				sibling = sibling->left;
				rotate_right(sibling, false);
			}

			sibling->color = parent->color;
			parent->color = BLACK;
			sibling->left->color = BLACK;
			rotate_left(parent, false);
			return;
		}

	} else {
		if (parent->right != NULL && parent->right->color == RED) {
			parent->right->color = BLACK;
			return;
		}

		Node* sibling = parent->left;
		if (sibling != NULL && sibling->color == RED) {
			sibling->color = BLACK;
			parent->color = RED;
			sibling = sibling->right;
			rotate_right(parent, false);
		}

		if ((sibling->right == NULL || sibling->right->color == BLACK) &&
			(sibling->left == NULL || sibling->left->color == BLACK)) {
			sibling->color = RED;
			node_delete_fix(parent->parent, parent->parent->right == parent);

		} else {
			if (sibling->left == NULL || sibling->left->color == BLACK) {
				sibling->right->color = BLACK;
				sibling->color = RED;
				sibling = sibling->right;
				rotate_left(sibling, false);
			}

			sibling->color = parent->color;
			parent->color = BLACK;
			sibling->left->color = BLACK;
			rotate_right(parent, false);
			return;
		}
	}
}

void node_delete(Node* node) {
	bool orig_color = node->color;

	Node* from_parent = node->parent;
	bool child_dir = node->parent->right == node;

	if (node->left == NULL) {
		transplant(node->right, node);

	} else if (node->right == NULL) {
		transplant(node->left, node);

	} else {
		Node* smallest = find_smallest_node(node->right);
		orig_color = smallest->color;
		from_parent = smallest->parent;
		child_dir = 0;
		if (smallest == node->right) {
			child_dir = 1;
		}
		Entry* temp = node->entry;
		node->entry = smallest->entry;
		smallest->entry = temp;
		transplant(smallest->right, smallest);
	}

	if (orig_color == BLACK) {
		node_delete_fix(from_parent, child_dir);
	}
}

bool tree_delete(Tree* tree, const char* key) {
	Node* node = tree_find_traverse(tree->root, key);

	if (node == NULL)
		return false;

	if(tree->root == node){
		entry_destroy(node->entry);
		free(node->parent);
		free(node);
		tree->root = NULL;
	
	} else {
		node_delete(node);
	}	

	return true;
}

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

	stack.path = malloc(sizeof(int) * 4);
	stack.max_path = 4;
	stack.cur_path = 0;
	stack.path[0] = 0;

	stack.last_up = true;

	stack.global_table = global_tree;

	return stack;
}

void extend_path(C_Stack* stack, bool up) {
	if(stack->cur_path >= stack->max_path - 1) {
		stack->max_path *= 2;
		stack->path = realloc(stack->path, stack->max_path);
		if(stack->path == NULL) {
			exit(99);
		}
	}

	if(stack->last_up == up) {
		stack->path[stack->cur_path]++;
	} else {
		stack->path[--(stack->cur_path)] = 1;
	}
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

	extend_path(stack, true);

	stack->cur_nest++;
	stack->arr[stack->cur_nest] = tree_init();
}

bool context_pop(C_Stack* stack) {
	if(stack->cur_nest < 0) {
		return false;
	}

	Entry* entry;
	while (true){
		entry = tree_pop(stack->arr[stack->cur_nest]);
		if(entry == NULL) {
			break;
		}
		
		if(tree_find(stack->global_table, entry->key) != NULL) {
			entry->key = realloc(entry->key, strlen(entry->key) + (stack->cur_path * 2) + 1);
			if(entry->key == NULL) {
				exit(99);
			}
			int i = 0;
			int len = strlen(entry->key);
			for(; i < stack->cur_path; i++) {
				entry->key[len + (i * 2)] = (char)stack->path[i] << 8;
				entry->key[len + (i * 2) + 1] = (char)stack->path[i];
			}
			tree_insert(stack->global_table, entry);
		}
	}

	extend_path(stack, false);

	stack->cur_nest--;

	return true;
}

Entry* context_find(C_Stack* stack, const char* key) {
	int cur_nest = stack->cur_nest;
	
	while(cur_nest >= 0) {
		Entry* entry = tree_find(stack->arr[cur_nest], key);
		
		if(entry != NULL) {
			return entry;
		}

		cur_nest--;
	}

	return tree_find(stack->global_table, key);
}

void context_put(C_Stack* stack, Entry* entry) {
	if(stack->cur_nest < 0) {
		tree_insert(stack->global_table, entry);
	}

	tree_insert(stack->arr[stack->cur_nest], entry);
}

void tree_print(Tree* tree) {
	node_print(tree->root, 0);
}
