#include "test_generate_graph.h"
#include <stdio.h>

int func_call_count = 0;
int func_decl_count = 0;
int var_decl_count = 0;
int var_assignment_count = 0;
int else_count = 0;
int return_count = 0;
int if_count = 0;
int nnul_count = 0;
int while_count = 0;
FILE* file = NULL;

void generate_node(AST_Node* node, const char* last_node);

void write_connection(const char* last_node, const char* current_node) {
    fprintf(file, "%s -> %s\n", last_node, current_node);
}

void generate_func_call(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "FUNC_CALL", func_call_count++);

    write_connection(last_node, current_node);
}

void generate_function_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "FUNC_DECL", func_decl_count++);

    write_connection(last_node, current_node);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        generate_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_var_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "VAR_DECL", var_decl_count++);

    write_connection(last_node, current_node);

    generate_node(node->left, current_node);
}

void generate_var_assignment(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "VAR_ASSIGNMENT", var_assignment_count++);

    write_connection(last_node, current_node);

    generate_node(node->left, current_node);
}

void generate_else(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "ELSE", else_count++);

    write_connection(last_node, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_nnull_var_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "NNUL", nnul_count++);

    write_connection(last_node, current_node);
}

void generate_return(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "RETURN", return_count++);

    write_connection(last_node, current_node);

    generate_node(node->left, current_node);
}


void generate_if(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "IF", if_count++);

    write_connection(last_node, current_node);

    generate_node(node->left, current_node);
    generate_node(node->right, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_while(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "WHILE", while_count++);

    write_connection(last_node, current_node);

    generate_node(node->left, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_node(AST_Node* node, const char* last_node) {
    if (node == NULL) {
        return;
    }

    switch(node->type) {
        case IF:             generate_if(node, last_node); break;
        case WHILE:          generate_while(node, last_node); break;
        case FUNC_CALL:      generate_func_call(node, last_node); break;
        case FUNCTION_DECL:  generate_function_decl(node, last_node); break;
        case VAR_DECL:       generate_var_decl(node, last_node); break;
        case VAR_ASSIGNMENT: generate_var_assignment(node, last_node); break;
        case ELSE:           generate_else(node, last_node); break;
        case NNULL_VAR_DECL: generate_nnull_var_decl(node, last_node); break;
        case RETURN:         generate_return(node, last_node); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

void generate_graph(AST_Node* node, const char* filepath) {
    file = fopen(filepath, "a");
    if(file == NULL) {
        return;
    }

    generate_node(node, "root");

    fclose(file);
}