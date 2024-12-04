/**
 * IFJ PROJEKT 2024
 * @file test_generate_graph.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Visual representation of tree for IFJ 2024
 * Note: this file is only used in debug build, meaning that
 * the standards like not using global variables are not strictly followed
 * 
 * @date 2024-11-13
 * 
 */

#include "test_generate_graph.h"

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
int in_expr = 0;
FILE* file = NULL;

Tree* s_table;

void generate_graph_node(AST_Node* node, char* last_node, char* path);

char* string_write(const char* string) {
	int len = strlen(string);
	int new_len = len;

	for (int i = 0; i < len; i++) {
		if (string[i] < 32 || string[i] == ' ' || string[i] == '#' || string[i] == '\\') {
			new_len += 4;
		}
	}

	char* new_string = calloc(new_len + 1, sizeof(char));
	char* out = new_string;

	for (int i = 0; i < len; i++) {
		if (string[i] < 32 || string[i] == ' ' || string[i] == '#' || string[i] == '\\') {
			sprintf(new_string, "\\\\%03d", string[i]);
			new_string += 5;

		} else {
			new_string[0] = string[i];
			new_string += 1;
		}
	}

	return out;
}

char* ret_type_to_string[] = {"NULL", "R_I32","N_I32","R_F64","N_F64","R_U8","N_U8","IMPLICIT"};

char* enum_to_shape[] = {"rectangle", "diamond", "parallelogram", "hexagon", 
                         "stored_data", "circle", "oval"};
typedef enum Shapes {
    BASE,
    DIAMOND,
    SLANT,
    HEXAGON,
    BEVEL,
    CIRCLE,
    OVAL,

    PILL = 100,
} Shapes;


void set_shape(char* node, Shapes shape) {
    if(in_expr) {
        fprintf(file, "%s.shape: %s\n", node, enum_to_shape[BASE]);
        fprintf(file, "%s.style{ border-radius: 999 }\n", node);
        return;
    }

    switch(shape) {
        case BASE:
            fprintf(file, "%s.shape: %s\n", node, enum_to_shape[shape]);
            break;
        case DIAMOND:
            fprintf(file, "%s.shape: %s\n", node, enum_to_shape[shape]);
            fprintf(file, "%s.style{ fill: \"#ffffff\" }\n", node);
            break;
        case PILL:
            fprintf(file, "%s.style{ border-radius: 999 }\n", node);
        default:
            break;
    }
}

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
    if(strlen(data)) {
        fprintf(file, "%s: {tooltip: %s}\n", node, data);
    }   
}

void generate_graph_func_call(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "FUNC_CALL", func_call_count++);

    write_data(current_node, node->as.func_data->var_name);
    set_shape(current_node, BASE);
    write_connection(last_node, current_node, path);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        in_expr++;
        generate_graph_node((AST_Node*)arr->data[i], current_node, "args");
        in_expr--;
    }
}

void generate_graph_function_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "FUNC_DECL", func_decl_count++);

    write_connection(last_node, current_node, path);
    set_shape(current_node, BASE);

    Arr* arr = node->as.func_data->arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_var_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "VAR_DECL", var_decl_count++);
    
    Entry* entry = tree_find(s_table, node->as.var_name);

    char data_buf[50];
    sprintf(data_buf, "%s %s", node->as.var_name, ret_type_to_string[entry->ret_type]);

    write_data(current_node, data_buf);
    write_connection(last_node, current_node, path);
    set_shape(current_node, BASE);

    in_expr++;
    generate_graph_node(node->left, current_node, "expr");
    in_expr--;
}

void generate_graph_var_assignment(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "VAR_ASSIGNMENT", var_assignment_count++);

    write_data(current_node, node->as.var_name);
    write_connection(last_node, current_node, path);
    set_shape(current_node, BASE);

    in_expr++;
    generate_graph_node(node->left, current_node, "expr");
    in_expr--;
}

void generate_graph_else(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "ELSE", else_count++);

    write_connection(last_node, current_node, path);
    set_shape(current_node, BASE);

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_nnull_var_decl(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "NNUL", nnul_count++);

    write_connection(last_node, current_node, path);
    set_shape(current_node, BASE);
}

void generate_graph_return(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "RETURN", return_count++);

    write_connection(last_node, current_node, path);

    set_shape(current_node, BASE);

    in_expr++;
    generate_graph_node(node->left, current_node, "expr");
    in_expr--;
}


void generate_graph_if(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "IF", if_count++);

    write_connection(last_node, current_node, path);

    set_shape(current_node, DIAMOND);

    in_expr++;
    generate_graph_node(node->left, current_node, "expr");
    in_expr--;
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

    set_shape(current_node, DIAMOND);

    in_expr++;
    generate_graph_node(node->left, current_node, "expr");
    in_expr--;

    Arr* arr = node->as.arr;
    for(int i=0; i < arr->length; i++) {
        generate_graph_node((AST_Node*)arr->data[i], current_node, "body");
    }
}

void generate_graph_nil(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "NIL", NIL_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
} 

void generate_graph_i32(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "I32", I32_count++);

    char data[30];
    sprintf(data, "%d", node->as.i32);

    write_data(current_node, data);
    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
} 

void generate_graph_f64(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "F64", F64_count++);

    char data[30];
    sprintf(data, "%lf", node->as.f64);

    write_data(current_node, data);
    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
} 

void generate_graph_id(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "ID", ID_count++);

    const char* old_name = node->as.var_name;
    char* new_name = calloc(strlen(old_name), sizeof(char));
    for(int i=0; old_name[i] != '\0' && old_name[i] != '?'; i++){
        new_name[i] = node->as.var_name[i];
    }

    set_shape(current_node, PILL);
    
    Entry* entry = tree_find(s_table, node->as.var_name);

    char data_buf[50];
    sprintf(data_buf, "%s %s", new_name, ret_type_to_string[entry->ret_type]);

    write_data(current_node, data_buf);
    write_connection(last_node, current_node, path);

    free(new_name);
} 

void generate_graph_string(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "STRING", STRING_count++);

    set_shape(current_node, PILL);
    write_data(current_node, string_write(node->as.string));
    write_connection(last_node, current_node, path);
} 

void generate_graph_plus(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "+", PLUS_count++);

    set_shape(current_node, PILL);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);
    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_iseq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "==", ISEQ_count++);

    set_shape(current_node, PILL);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);
    write_connection(last_node, current_node, path);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_isneq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "!=", ISNEQ_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_isless(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "LESS", ISLESS_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
}

void generate_graph_ismore(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "MORE", ISMORE_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_islesseq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "LESS_EQ", ISLESSEQ_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_ismoreeq(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "MORE_EQ", ISMOREEQ_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_minus(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "-", MINUS_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_mul(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "*", MUL_count++);

    write_connection(last_node, current_node, path);
    set_shape(current_node, PILL);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

void generate_graph_div(AST_Node* node, char* last_node, char* path) {
    char current_node[30];
    sprintf(current_node, "%s(%d)", "/", DIV_count++);

    set_shape(current_node, PILL);
    write_connection(last_node, current_node, path);
    write_data(current_node, ret_type_to_string[node->as.expr_type]);

    generate_graph_node(node->left, current_node, "left");
    generate_graph_node(node->right, current_node, "right");
} 

bool is_expr(AST_Node* node) {
    AST_Type expr_types[] = {NIL, I32, F64, ID, STRING, PLUS, ISEQ, ISNEQ, ISLESS, ISMORE, ISLESSEQ, ISMOREEQ, MINUS, MUL, DIV};

    for(int i=0; i < 15; i++) {
        if(expr_types[i] == node->type) {
            return true;
        }
    } 

    return false;
}

void generate_graph_node(AST_Node* node, char* last_node, char* path) {
    if (node == NULL) {
        return;
    }

    if(is_expr(node)) {
        in_expr++;
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
        
        default:             fprintf(stderr, "Unknown node type!\n"); break;
    }

    if(is_expr(node)) {
        in_expr--;
    }
}

void generate_graph(AST_Node* node, const char* filepath, Tree* s_table_) {
    file = fopen(filepath, "a");
    if(file == NULL) {
        return;
    }

    s_table = s_table_;

    generate_graph_node(node, "root", "functions");

    fclose(file);
}