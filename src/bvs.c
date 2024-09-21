/**
 * @file bvs.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Implementation of red-black tree
 * @date 2024-09-21
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bvs.h"

Entry* entry_create(const char* key) {
    Entry* entry = malloc(sizeof(Entry));

    entry->key = key;

    return entry;
}

Node* node_init(Entry* entry, Node* parent) {
    Node* node = malloc(sizeof(Node));

    node->parent = parent;
    node->entry = entry;
    node->color = RED;

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
            }
            rotate_left(node->parent->parent, true);
        }
    }
}

void node_insert(Node* node, Entry* entry) {
    int cmp = strcmp(entry->key, node->entry->key);

    if (cmp < 0) {
        if (node->left == NULL) {
            node->left = node_init(entry, node);
            node_fix(node->left);

        } else {
            node_insert(node->left, entry);
        }
        
    } else if(cmp > 0) {
        if (node->right == NULL) {
            node->right = node_init(entry, node);
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
        tree->root = node_init(entry, NULL);
        tree->root->color = BLACK;

        tree->root->parent = node_init(NULL, NULL);
        tree->root->parent->left = tree->root;

    } else {
        Node* temp = tree->root->parent;
        
        node_insert(tree->root, entry);

        if (tree->root != temp->left) {
            tree->root = temp->left;
        }
    }
}

bool tree_delete(Tree* tree, const char* key);

Entry* tree_find_traverse(Node* node, const char* key) {
    if (node == NULL) {
        return NULL;
    }

    int cmp = strcmp(key, node->entry->key);

    if (cmp < 0) {
        tree_find_traverse(node->left, key);
        
    } else if(cmp > 0) {
        tree_find_traverse(node->right, key);

    } else {
        return node->entry;
    }
}

Entry* tree_find(Tree* tree, const char* key) {
    tree_find_traverse(tree->root, key);
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

void tree_print(Tree* tree) {
    node_print(tree->root, 0);
}
