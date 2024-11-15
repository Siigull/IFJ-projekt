
#include "codegen.h"


void generate_prolog(){
    fprintf(stdout, PROG_START);
    return;
}


void generate_builtins(){
    //WRITE
    fprintf(stdout, "LABEL *write\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*to*write\n");
    fprintf(stdout, "\tMOVE LF@*to*write LF@*param0\n");
    fprintf(stdout, "\tWRITE LF@*to*write\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //i2f
    fprintf(stdout, "LABEL *i2f\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tINT2FLOAT LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //f2i
    fprintf(stdout, "LABEL *f2i\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tFLOAT2INT LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //LENGTH
    fprintf(stdout, "LABEL *length\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*strlen\n");
    fprintf(stdout, "\tMOVE LF@*strlen LF@*param0\n");
    fprintf(stdout, "\tSTRLEN LF@*strlen LF@*strlen\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*strlen\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //CONCATENATE
    fprintf(stdout, "LABEL *concat\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tMOVE LF@*first LF@*param0\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tMOVE LF@*second LF@*param1\n");
    fprintf(stdout, "\tCONCAT LF@*return*value LF@*first LF@*second\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
    //todo add all builtin functions

    //readstr
    fprintf(stdout, "LABEL *readstr\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tREAD LF@*return*value string\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //readi32
    fprintf(stdout, "LABEL *readstr\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tREAD LF@*return*value int\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");

    //readf64
    fprintf(stdout, "LABEL *readstr\n");
    fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tREAD LF@*return*value float\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
}

void eval_exp(AST_Node* curr){
    //todo
    return;
}

//value of return of fnc, value to assign into var -> GF@__expression__result global variable
//maybe change this
void generate_expression(AST_Node* curr, Tree* symtable){
    //todo
    return;
}

void generate_if(AST_Node* curr, Tree* symtable){
    //todo
}

void generate_while(AST_Node* curr, Tree* symtable){
    //todo
}

void generate_return(AST_Node* curr, Tree* symtable){
    if(curr->left != NULL){
        generate_expression(curr->left, symtable);
    }
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
}

void generate_func_call(AST_Node* curr, Tree* symtable){
    fprintf(stdout, "\tCREATEFRAME\n");
    //push all parameters here
    Arr* params = curr->as.func_data->arr;
    for(int i = 0; i < params->length; i++){
        AST_Node* param = (AST_Node*)((params->data)[i]);
        fprintf(stdout, "\tDEFVAR TF@*param%d\n", i);
        generate_expression(param, symtable);
        fprintf(stdout, "\tMOVE TF@*param%d GF@*expression*result\n", i);
        //fprintf(stdout, "\tPUSHS GF@*expression*result\n");
    }

    //fprintf(stdout, "\tPUSHFRAME\n");
    fprintf(stdout, "\tCALL %s\n", curr->as.func_data->var_name);
    //fprintf(stdout, "\tPOPFRAME\n\n");
}

void generate_var_decl(AST_Node* curr, Tree* symtable){
    fprintf(stdout, "\tDEFVAR LF@%s\n", curr->as.var_name);
    generate_expression(curr->left, symtable);
    fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
}

void generate_var_assignment(AST_Node* curr, Tree* symtable){
    //add variable that will store from global frame into the variable
    //case when var is _ 
    generate_expression(curr->left, symtable);
    fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
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
    fprintf(stdout, "LABEL *%s\n", curr->as.func_data->var_name);
    bool main = (!strcmp(curr->as.func_data->var_name, "main"));
    if(main){
        fprintf(stdout, "\tCREATEFRAME\n");
        fprintf(stdout, "\tPUSHFRAME\n");
    }
    else{
        fprintf(stdout, "\tPUSHFRAME\n");
        fprintf(stdout, "\tDEFVAR LF@*return*value\n");
        fprintf(stdout, "\tMOVE LF@*return*value nil@nil\n");
    }

    Entry* arg_entry = tree_find(symtable, curr->as.func_data->var_name);
    Arr* args = arg_entry->as.function_args;
    // create local variables for all argument 
    // and continuously pop arguments from stack into them
    for(int i = 0; i < args->length; i++){
        Function_Arg* argument = (Function_Arg*)((args->data)[i]);
        fprintf(stdout, "\tDEFVAR LF@%s\n", argument->arg_name); 
        fprintf(stdout, "\tMOVE LF@%s LF@%s*param\n", argument->arg_name, argument->arg_name);
    }

    // statements in as->func_data as an array
    // generate all of them
    Arr* statements = curr->as.func_data->arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        if(stmt->type == RETURN && !strcmp(curr->as.func_data->var_name, "main")) break;
        generate_statement(stmt, symtable);
    }

    if(main){
        fprintf(stdout, PROG_END);
    }
    return;
}


void generate_code(Arr* input, Tree* symtable){
    generate_prolog();
    //start on main
    generate_builtins();
    for(int i = 0; i < input->length; i++){
        AST_Node* curr = (AST_Node*)((input->data)[i]);
        generate_function_decl(curr, symtable);
    }
}