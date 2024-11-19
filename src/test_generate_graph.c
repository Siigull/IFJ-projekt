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
int RETURN_count = 0;
int NIL_count = 0;
int I32_count = 0;
int F64_count = 0;
int ID_count = 0;
int STRING_count = 0;
int PLUS_count = 0;
int ISEQ_count = 0;
int ISNEQ_count = 0;
int ISLESS_count = 0;
int ISMORE_count = 0;
int ISLESSEQ_count = 0;
int ISMOREEQ_count = 0;
int MINUS_count = 0;
int MUL_count = 0;
int DIV_count = 0;
FILE* file = NULL;

void generate_graph_node(AST_Node* node, char* last_node, char* path);

void write_connection(char* last_node, char* current_node, 
                      char* where) {

    if(last_node[0] == '*') {
        last_node[0] = '!';
    }
    if(current_node[0] == '*') {
        current_node[0] = '!';
    }
    fprintf(file, "%s -> %s : %s\n", last_node, current_node, where);
}

void write_data(const char* node, const char* data) {
    fprintf(file, "%s: {tooltip: %s}\n", node, data);
}

void generate_graph_func_call(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "FUNC_CALL", func_call_count++);

    write_connection(last_node, current_node, path);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "args");
    }
}

void generate_graph_function_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "FUNC_DECL", func_decl_count++);

    write_connection(last_node, current_node, path);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_var_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "VAR_DECL", var_decl_count++);
    
    write_data(current_node, node->as.var_name);
    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "expr");
}

void generate_graph_var_assignment(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "VAR_ASSIGNMENT", var_assignment_count++);

    write_data(current_node, node->as.var_name);
    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "expr");
}

void generate_graph_else(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "ELSE", else_count++);

    write_connection(last_node, current_node, path);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_nnull_var_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "NNUL", nnul_count++);

    write_connection(last_node, current_node, path);
}

void generate_graph_return(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "RETURN", return_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "expr");
}


void generate_graph_if(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "IF", if_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "expr");
    generate_graph_node(node->right, current_node, "else");

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_while(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "WHILE", while_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "expr");

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_nil(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "NIL", NIL_count++);

    write_connection(last_node, current_node, path);
} 

void generate_graph_i32(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "I32", I32_count++);

    write_connection(last_node, current_node, path);
} 

void generate_graph_f64(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "F64", F64_count++);

    write_connection(last_node, current_node, path);
} 

void generate_graph_id(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "ID", ID_count++);

    write_data(current_node, node->as.var_name);
    write_connection(last_node, current_node, path);
} 

void generate_graph_string(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "STRING", STRING_count++);

    write_connection(last_node, current_node, path);
} 

void generate_graph_plus(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "+", PLUS_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_iseq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "==", ISEQ_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_isneq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "!=", ISNEQ_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_isless(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "LESS", ISLESS_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
}

void generate_graph_ismore(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "MORE", ISMORE_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_islesseq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "LESS_EQ", ISLESSEQ_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_ismoreeq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "MORE_EQ", ISMOREEQ_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_minus(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "-", MINUS_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_mul(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "*", MUL_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_div(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "/", DIV_count++);

    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_node(AST_Node* node, char* last_node, char* path) {
    if (node == NULL) {
        return;
    }

    switch(node->type) {
        case IF:             generate_graph_if(node, last_node, path); break;
        case WHILE:          generate_graph_while(node, last_node, path); break;
        case FUNC_CALL:      generate_graph_func_call(node, last_node, path); break;
        case FUNCTION_DECL:  generate_graph_function_decl(node, last_node, path); break;
        case VAR_DECL:       generate_graph_var_decl(node, last_node, path); break;
        case VAR_ASSIGNMENT: generate_graph_var_assignment(node, last_node, path); break;
        case ELSE:           generate_graph_else(node, last_node, path); break;
        case NNULL_VAR_DECL: generate_graph_nnull_var_decl(node, last_node, path); break;
        case RETURN:         generate_graph_return(node, last_node, path); break;
        case NIL:            generate_graph_nil(node, last_node, path); break;
        case I32:            generate_graph_i32(node, last_node, path); break;
        case F64:            generate_graph_f64(node, last_node, path); break;
        case ID:             generate_graph_id(node, last_node, path); break;
        case STRING:         generate_graph_string(node, last_node, path); break;
        case PLUS:           generate_graph_plus(node, last_node, path); break;
        case ISEQ:           generate_graph_iseq(node, last_node, path); break;
        case ISNEQ:          generate_graph_isneq(node, last_node, path); break;
        case ISLESS:         generate_graph_isless(node, last_node, path); break;
        case ISMORE:         generate_graph_ismore(node, last_node, path); break;
        case ISLESSEQ:       generate_graph_islesseq(node, last_node, path); break;
        case ISMOREEQ:       generate_graph_ismoreeq(node, last_node, path); break;
        case MINUS:          generate_graph_minus(node, last_node, path); break;
        case MUL:            generate_graph_mul(node, last_node, path); break;
        case DIV:            generate_graph_div(node, last_node, path); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

void generate_graph(AST_Node* node, const char* filepath) {
    file = fopen(filepath, "a");
    if(file == NULL) {
        return;
    }

    generate_graph_node(node, "root", "functions");

    fclose(file);
}