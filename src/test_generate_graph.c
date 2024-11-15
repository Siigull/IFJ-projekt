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

void generate_graph_node(AST_Node* node, const char* last_node);

void write_connection(const char* last_node, const char* current_node) {
    fprintf(file, "%s -> %s\n", last_node, current_node);
}

void generate_graph_func_call(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "FUNC_CALL", func_call_count++);

    write_connection(last_node, current_node);
}

void generate_graph_function_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "FUNC_DECL", func_decl_count++);

    write_connection(last_node, current_node);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_graph_var_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "VAR_DECL", var_decl_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
}

void generate_graph_var_assignment(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "VAR_ASSIGNMENT", var_assignment_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
}

void generate_graph_else(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "ELSE", else_count++);

    write_connection(last_node, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_graph_nnull_var_decl(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "NNUL", nnul_count++);

    write_connection(last_node, current_node);
}

void generate_graph_return(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "RETURN", return_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
}


void generate_graph_if(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "IF", if_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_graph_while(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "WHILE", while_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node);
    }
}

void generate_graph_nil(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "NIL", NIL_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_i32(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "I32", I32_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_f64(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "F64", F64_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_id(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "ID", ID_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_string(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "STRING", STRING_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_plus(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "+", PLUS_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_iseq(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "==", ISEQ_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_isneq(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "!=", ISNEQ_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_isless(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "<", ISLESS_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_ismore(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", ">", ISMORE_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_islesseq(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "<=", ISLESSEQ_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_ismoreeq(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", ">=", ISMOREEQ_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_minus(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "-", MINUS_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 
void generate_graph_mul(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "*", MUL_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 
void generate_graph_div(AST_Node* node, const char* last_node) {
    char current_node[30];
    sprintf(current_node, "%s%d", "/", DIV_count++);

    write_connection(last_node, current_node);

    generate_graph_node(node->left, current_node);
    generate_graph_node(node->right, current_node);
} 

void generate_graph_node(AST_Node* node, const char* last_node) {
    if (node == NULL) {
        return;
    }

    switch(node->type) {
        case IF:             generate_graph_if(node, last_node); break;
        case WHILE:          generate_graph_while(node, last_node); break;
        case FUNC_CALL:      generate_graph_func_call(node, last_node); break;
        case FUNCTION_DECL:  generate_graph_function_decl(node, last_node); break;
        case VAR_DECL:       generate_graph_var_decl(node, last_node); break;
        case VAR_ASSIGNMENT: generate_graph_var_assignment(node, last_node); break;
        case ELSE:           generate_graph_else(node, last_node); break;
        case NNULL_VAR_DECL: generate_graph_nnull_var_decl(node, last_node); break;
        case RETURN:         generate_graph_return(node, last_node); break;
        case NIL:            generate_graph_nil(node, last_node); break;
        case I32:            generate_graph_i32(node, last_node); break;
        case F64:            generate_graph_f64(node, last_node); break;
        case ID:             generate_graph_id(node, last_node); break;
        case STRING:         generate_graph_string(node, last_node); break;
        case PLUS:           generate_graph_plus(node, last_node); break;
        case ISEQ:           generate_graph_iseq(node, last_node); break;
        case ISNEQ:          generate_graph_isneq(node, last_node); break;
        case ISLESS:         generate_graph_isless(node, last_node); break;
        case ISMORE:         generate_graph_ismore(node, last_node); break;
        case ISLESSEQ:       generate_graph_islesseq(node, last_node); break;
        case ISMOREEQ:       generate_graph_ismoreeq(node, last_node); break;
        case MINUS:          generate_graph_minus(node, last_node); break;
        case MUL:            generate_graph_mul(node, last_node); break;
        case DIV:            generate_graph_div(node, last_node); break;
        
        default:             printf("Unknown node type!\n"); break;
    }
}

void generate_graph(AST_Node* node, const char* filepath) {
    file = fopen(filepath, "a");
    if(file == NULL) {
        return;
    }

    generate_graph_node(node, "root");

    fclose(file);
}