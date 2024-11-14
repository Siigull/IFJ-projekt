
#include "codegen.h"

void generate_prolog(){
    fprintf(stdout, PROG_START);
    return;
}

void eval_exp(AST_Node* curr){

    return;
}

//value of return of fnc, value to assign into var -> GF@__expression__result global variable
//maybe change this
void generate_expression(AST_Node* curr, Tree* symtable){
    //printf(stdout, "DEFVAR LF@__res_left\n");
    //printf(stdout, "DEFVAR LF@__res_right\n");   
    //eval_exp(curr);

    //printf(stdout, "MOVE GF@__expression__result LF@res\n");
    return;
}

void generate_if(AST_Node* curr, Tree* symtable){

}

void generate_return(AST_Node* curr, Tree* symtable){
    if(curr->left != NULL){
        generate_expression(curr->left, symtable);
    }
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
}

void generate_while(AST_Node* curr, Tree* symtable){

}

void generate_func_call(AST_Node* curr, Tree* symtable){

}

void generate_var_decl(AST_Node* curr, Tree* symtable){

}

void generate_var_assignment(AST_Node* curr, Tree* symtable){
    // var x = f(y, z);
    // push params
    // push frame
    // move x,


    //add variable that will store from global frame into the variable
    //case when var is _ 
    generate_expression(curr->left, symtable);
    if(curr->as.var_name != NULL)
        fprintf(stdout, "MOVE LF@%s \n ", curr->as.var_name);
}

void generate_statement(AST_Node* curr, Tree* symtable){
    switch(curr->type){
        case IF:
            generate_if(curr, symtable);
            return;
        case RETURN:
            generate_return(curr, symtable);
            return;
        case WHILE:
            generate_while(curr, symtable);
            return;
        case FUNC_CALL:
            generate_func_call(curr, symtable);
            return;
        case VAR_DECL:
            generate_var_decl(curr, symtable);
            return;
        case VAR_ASSIGNMENT:
            generate_var_assignment(curr, symtable);
            return;
    }
    return;
}

void generate_function_decl(AST_Node* curr, Tree* symtable){
    fprintf(stdout, "LABEL __func_%s\n", curr->as.func_data->var_name);
    fprintf(stdout, "\tCREATEFRAME\n");
    fprintf(stdout, "\tPUSHFRAME\n");

    Entry* arg_entry = tree_find(symtable, curr->as.func_data->var_name);
    Arr* args = arg_entry->as.function_args;
    // create local variables for all argument 
    // and continuously pop arguments from stack into them
    for(int i = 0; i < args->length; i++){
        Function_Arg* argument = (Function_Arg*)((args->data)[i]);
        fprintf(stdout, "\tDEFVAR LF@%s\n", argument->arg_name); 
        fprintf(stdout, "\tPOPS LF@%s\n", argument->arg_name);
    }

    // statements in as->func_data as an array
    // generate all of them
    Arr* statements = curr->as.func_data->arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        if(stmt->type == RETURN && !strcmp(curr->as.func_data->var_name, "main")) break;
        generate_statement(stmt, symtable);
    }

    if(!strcmp(curr->as.func_data->var_name, "main")){
        fprintf(stdout, "\tPOPFRAME\n");
        fprintf(stdout, PROG_END);
    }
    return;
}


void generate_code(Arr* input, Tree* symtable){
    generate_prolog();
    //start on main
    for(int i = 0; i < input->length; i++){
        AST_Node* curr = (AST_Node*)((input->data)[i]);
        generate_function_decl(curr, symtable);
    }
}