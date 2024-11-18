/**
 * @file codegen.c
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Implementation of IFJ24 code generator, generates from given AST
 * 
 * @todo remove symtable where not needed, add/revamp builtins, add evaluation of conditions of loops and ifs
 * 
 * @date 2024-11-15
 * 
 */

#include "codegen.h"


void generate_prolog(){
    fprintf(stdout, PROG_START);
    return;
}

void builtin_start(char* func_name){
    fprintf(stdout, "LABEL %s\n", func_name);
    fprintf(stdout, "\tPUSHFRAME\n");
    return;
}

void builtin_end(){
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
}

void builtin_reads(char* func_name, char* type){
    builtin_start(func_name);
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tREAD LF@*return*value %s\n", type);
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();
}

void generate_builtins(){
    //WRITE
    builtin_start("*write");
    fprintf(stdout, "\tDEFVAR LF@*to*write\n");
    fprintf(stdout, "\tMOVE LF@*to*write LF@*param0\n");
    fprintf(stdout, "\tWRITE LF@*to*write\n");
    builtin_end();

    //i2f
    builtin_start("*i2f");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tINT2FLOAT LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //f2i
    builtin_start("*f2i");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tFLOAT2INT LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //LENGTH
    builtin_start("*length");
    fprintf(stdout, "\tDEFVAR LF@*strlen\n");
    fprintf(stdout, "\tMOVE LF@*strlen LF@*param0\n");
    fprintf(stdout, "\tSTRLEN LF@*strlen LF@*strlen\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*strlen\n");
    builtin_end();

    //CONCATENATE
    builtin_start("*concat");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tMOVE LF@*first LF@*param0\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tMOVE LF@*second LF@*param1\n");
    fprintf(stdout, "\tCONCAT LF@*return*value LF@*first LF@*second\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //readstr, readi32, readf64
    builtin_reads("*readstr", "string");
    builtin_reads("*readi32", "int");
    builtin_reads("*readf64", "float");

    //ord
    builtin_start("*ord");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tSTRI2INT LF@*return*value LF@*param0 LF@*param1\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //chr
    builtin_start("*chr");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tINT2CHAR LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    builtin_start("*string");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tMOVE LF@*return*value LF@*param0\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //strcmp
    builtin_start("*strcmp");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*res\n");
    fprintf(stdout, "\tEQ LF@*res LF@*param0 LF@*param1\n");
    fprintf(stdout, "\tJUMPIFEQ *strcmp*skipeq LF@*res bool@false\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@0\n");
    fprintf(stdout, "\tJUMP *strcmp*end\n");
    fprintf(stdout, "\tLABEL *strcmp*skipeq\n");
    fprintf(stdout, "\tLT LF@*res LF@*param0 LF@*param1\n");
    fprintf(stdout, "\tJUMPIFEQ *strcmp*skiplt LF@*res bool@false\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@-1\n");
    fprintf(stdout, "\tJUMP *strcmp*end\n");
    fprintf(stdout, "\tLABEL *strcmp*skiplt\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@1\n");
    fprintf(stdout, "\tLABEL *strcmp*end\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //substring
    builtin_start("*substring");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    //checking conditions
    fprintf(stdout, "\tLT GF@*tmp*res LF@*param1 int@0\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tLT GF@*tmp*res LF@*param2 int@0\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*param1 LF@*param2\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tSTRLEN GF@*tmp*res LF@*param0\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*param2 GF@*tmp*res\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tSTRLEN GF@*tmp*res LF@*param0\n");
    fprintf(stdout, "\tEQ GF@*expression*result LF@*param1 GF@*tmp*res\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*param1 GF@*tmp*res\n");
    fprintf(stdout, "\tOR GF@*tmp*res GF@*expression*result GF@*tmp*res\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");

    fprintf(stdout, "\tMOVE LF@*return*value string@\n");
    fprintf(stdout, "\tDEFVAR LF@*start*index\n");
    fprintf(stdout, "\tDEFVAR LF@*res\n");
    fprintf(stdout, "\tMOVE LF@*start*index LF@*param1\n");
    fprintf(stdout, "\tLABEL *substring*while\n");
    fprintf(stdout, "\tEQ LF@*res LF@*start*index LF@*param2\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*loop*end LF@*res bool@true\n");

    fprintf(stdout, "\tGETCHAR LF@*res LF@*param0 LF@*start*index\n");
    fprintf(stdout, "\tCONCAT LF@*return*value LF@*return*value LF@*res\n");

    fprintf(stdout, "\tADD LF@*start*index LF@*start*index int@1\n");
    fprintf(stdout, "\tJUMP *substring*while\n");
    fprintf(stdout, "\tLABEL *substring*loop*end\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();
    //error state
    fprintf(stdout, "\tLABEL *substring*err\n");
    fprintf(stdout, "\tMOVE LF@*return*value nil@nil\n");
    fprintf(stdout, "\tJUMP *substring*loop*end\n");
}

bool is_operator_arithmetic(AST_Type type){
    if(type == PLUS || type == MINUS || type == MUL || type == DIV){
        return true;
    }

    return false;
}

bool is_data_type(AST_Type type){
    if(type == I32 || type == F64 ||type == STRING){
        return true;
    }

    return false;
}

//postorder traversal through the expression tree
void eval_exp(AST_Node* curr, Tree* symtable, int inside_fnc_call){
    if(curr->left != NULL) eval_exp(curr->left, symtable, inside_fnc_call);
    if(curr->right != NULL) eval_exp(curr->right, symtable, inside_fnc_call);

    //here current instruction can be processed
    if(is_operator_arithmetic(curr->type)){
        switch(curr->type){
            case PLUS:
                fprintf(stdout, "\tADDS\n");
                break;
            case MINUS:
                fprintf(stdout, "\tSUBS\n");
                break;
            case MUL:
                fprintf(stdout, "\tMULS\n");
                break;
            case DIV:
                if(curr->as.expr_type == R_F64 || curr->as.expr_type == N_F64){
                    fprintf(stdout, "\tDIVS\n");
                }
                else if(curr->as.expr_type == R_I32 || curr->as.expr_type == N_I32){
                    fprintf(stdout, "\tIDIVS\n");
                }
                break;
        }
    }
    else if(curr->type == FUNC_CALL){
        generate_func_call(curr, symtable, ++inside_fnc_call);
        Entry* e = tree_find(symtable, curr->as.func_data->var_name);
        if(e != NULL){
            //user-defined functions
            if(e->ret_type == R_VOID){
                fprintf(stdout, "\tPUSHS nil@nil\n");
            }
            else{
                fprintf(stdout, "\tPUSHS GF@*return*val\n");
            }
        }
        else{
            //builtin case
            fprintf(stdout, "\tPUSHS GF@*return*val\n");
        }
    }
    else if(is_data_type(curr->type)){
        switch(curr->type){
            //ast_node union doesnt have correct values TODO TODO TODO
            case I32:
                fprintf(stdout, "\tPUSHS int@%d\n", curr->as.i32);
                break;
            case F64:
                fprintf(stdout, "\tPUSHS float@%a\n", curr->as.f64);
                break;
            case STRING:
                fprintf(stdout, "\tPUSHS string@%s\n", string_to_assembly(curr->as.string));
                break;
        }
    }
    else if(curr->type == ID){
        fprintf(stdout, "\tPUSHS LF@%s\n", curr->as.var_name);
    }
    else if(curr->type == NIL){
        fprintf(stdout, "\tPUSHS nil@nil\n");
    }
    return;
}

bool is_relation_op(AST_Type type){
    if(type == ISEQ || type == ISNEQ || type == ISLESS || type == ISMORE || type == ISLESSEQ || type == ISMOREEQ) 
        return true;

    return false;
}

//condition structure - root is relation operator, left and right children are classic expressions
void eval_condition(AST_Node* curr, Tree* symtable, int inside_fnc_call){
    if(is_relation_op(curr->type)){
        if(curr->left != NULL){
            generate_expression(curr->left, symtable, inside_fnc_call);
            fprintf(stdout, "\tMOVE GF@*lhs GF@*expression*result\n");
        }
        if(curr->right != NULL){
            generate_expression(curr->right, symtable, inside_fnc_call);
            fprintf(stdout, "\tMOVE GF@*rhs GF@*expression*result\n");
        }
    }

    switch(curr->type){
        case ISEQ:
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISNEQ:
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tNOT GF@*expression*result GF@*expression*result\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISLESS:
            fprintf(stdout, "\tLT GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISMORE:
            fprintf(stdout, "\tGT GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISLESSEQ:
            fprintf(stdout, "\tLT GF@*tmp*res GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tOR GF@*expression*result GF@*expression*result GF@*tmp*res\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISMOREEQ:
            fprintf(stdout, "\tGT GF@*tmp*res GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tOR GF@*expression*result GF@*expression*result GF@*tmp*res\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case STRING:
        case I32:
        case F64:
        case ID:
            fprintf(stdout, "\tMOVE GF@*tmp*res LF@%s\n", curr->as.var_name);
            fprintf(stdout, "\tEQ GF@*expression*result LF@%s nil@nil\n", curr->as.var_name);
            fprintf(stdout, "\tNOT GF@*expression*result GF@*expression*result\n");
            break;
        case FUNC_CALL:
            generate_func_call(curr, symtable, ++inside_fnc_call);
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*return*val\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*return*val nil@nil\n");
            fprintf(stdout, "\tNOT GF@*expression*result GF@*expression*result\n");
            break;
            //todo todo
    }
    return;
}


//value of return of fnc, value to assign into var -> GF@*expression*result global variable
//maybe change this
void generate_expression(AST_Node* curr, Tree* symtable, int inside_fnc_call){
    eval_exp(curr, symtable, inside_fnc_call);
    fprintf(stdout, "\tPOPS GF@*expression*result\n");
    return;
}

void predefine_vars(AST_Node* curr, Tree* symtable){
    Arr* statements = curr->as.arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* curr = (AST_Node*)((statements->data)[i]);
        if(curr->type == VAR_DECL){
            fprintf(stdout, "\tDEFVAR LF@%s\n", curr->as.var_name);
        }

        if(curr->type == WHILE){
            predefine_vars(curr, symtable);
        }
        if(curr->type == IF){
            predefine_vars(curr, symtable);
            predefine_vars(curr->right, symtable); //else part
        }
    }
}

void generate_else(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest){
    Arr* statements = curr->as.arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, 0);
    }
    return;
}


void generate_if(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, bool inside_if_loop){
    if(!inside_if_loop){
        predefine_vars(curr, symtable);
        predefine_vars(curr->right, symtable);
    }
    eval_condition(curr->left, symtable, 0);
    // condition is false if evaluated expression is 0
    fprintf(stdout, "\tJUMPIFEQ *else*%s*%d*%d GF@*expression*result bool@false\n", func_name, stmt_index, nest);

    int i = 0;
    Arr* statements = curr->as.arr;
    AST_Node* first = (AST_Node*)((statements->data)[0]);
    if(first->type == NNULL_VAR_DECL){
        fprintf(stdout, "\tDEFVAR LF@%s\n", first->as.var_name);
        fprintf(stdout, "\tMOVE LF@%s GF@*tmp*res\n", first->as.var_name);
        i = 1;
    }

    for(; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, 0);
    }
    fprintf(stdout, "\tJUMP *end*of*if*%s*%d*%d\n", func_name, stmt_index, nest);

    fprintf(stdout, "\tLABEL *else*%s*%d*%d\n", func_name, stmt_index, nest);
    generate_else(curr->right, symtable, func_name, stmt_index, nest);
    fprintf(stdout, "\tLABEL *end*of*if*%s*%d*%d\n", func_name, stmt_index, nest);
    return;
}

void generate_while(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, bool inside_if_loop){
    if(!inside_if_loop) predefine_vars(curr, symtable);
    
    fprintf(stdout, "\tLABEL *while*%s*%d*%d\n", func_name, stmt_index, nest);
    eval_condition(curr->left, symtable, 0);
    // condition is false if evaluated expression is 0
    fprintf(stdout, "\tJUMPIFEQ *end*loop*%s*%d*%d GF@*expression*result bool@false\n", func_name, stmt_index, nest);

    int i = 0;
    Arr* statements = curr->as.arr;
    AST_Node* first = (AST_Node*)((statements->data)[0]);
    if(first->type == NNULL_VAR_DECL){
        fprintf(stdout, "\tDEFVAR LF@%s\n", first->as.var_name);
        fprintf(stdout, "\tMOVE LF@%s GF@*tmp*res\n", first->as.var_name);
        i = 1;
    }

    for(; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, 0);
    }

    fprintf(stdout, "\tJUMP *while*%s*%d*%d\n", func_name, stmt_index, nest);
    fprintf(stdout, "\tLABEL *end*loop*%s*%d*%d\n", func_name, stmt_index, nest);
    return;
}


void generate_return(AST_Node* curr, Tree* symtable){
    if(curr->left != NULL){
        generate_expression(curr->left, symtable, 0);
        fprintf(stdout, "\tMOVE GF@*return*val GF@*expression*result\n");
    }
    fprintf(stdout, "\tPOPFRAME\n");
    fprintf(stdout, "\tRETURN\n\n");
}


void generate_func_call(AST_Node* curr, Tree* symtable, int inside_fnc_call){
    if(inside_fnc_call > 1){
        fprintf(stdout, "\tPUSHFRAME\n");
    }

    fprintf(stdout, "\tCREATEFRAME\n");
    //push all parameters here
    Arr* params = curr->as.func_data->arr;
    for(int i = 0; i < params->length; i++){
        AST_Node* param = (AST_Node*)((params->data)[i]);
        fprintf(stdout, "\tDEFVAR TF@*param%d\n", i);
        generate_expression(param, symtable, inside_fnc_call);
        fprintf(stdout, "\tMOVE TF@*param%d GF@*expression*result\n", i);
    }
    fprintf(stdout, "\tCALL %s\n", curr->as.func_data->var_name);

    if(inside_fnc_call > 1){
        fprintf(stdout, "\tPOPFRAME\n");
    }
}


void generate_var_decl(AST_Node* curr, Tree* symtable){
    if(strcmp(curr->as.var_name, "_")){
        fprintf(stdout, "\tDEFVAR LF@%s\n", curr->as.var_name);
        generate_expression(curr->left, symtable, 0);
        fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
    }
}


void generate_var_assignment(AST_Node* curr, Tree* symtable){
    //based on evaluated expression, save its value into the variable
    generate_expression(curr->left, symtable, 0);
    if(strcmp(curr->as.var_name, "_")){
        fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
    }
}//generate_var_assignment


void generate_function_decl(AST_Node* curr, Tree* symtable){
    fprintf(stdout, "LABEL %s\n", curr->as.func_data->var_name);
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
    // create local variables for all arguments
    // and continuously pop arguments from stack into them
    for(int i = 0; i < args->length; i++){
        Function_Arg* argument = (Function_Arg*)((args->data)[i]);
        fprintf(stdout, "\tDEFVAR LF@%s\n", argument->arg_name); 
        fprintf(stdout, "\tMOVE LF@%s LF@*param%d\n", argument->arg_name, i);
    }

    // statements in as->func_data as an array
    // generate all of them
    Arr* statements = curr->as.func_data->arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        if(stmt->type == RETURN && !strcmp(curr->as.func_data->var_name, "main")) break;
        generate_statement(stmt, symtable, curr->as.func_data->var_name, i, 0, false, 0);
    }

    if(main){
        fprintf(stdout, PROG_END);
    }
    return;
}//generate_function_decl


void generate_statement(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, bool inside_if_loop, int inside_fnc_call){
    //choose which type of statement to generate
    switch(curr->type){
        case IF:
            generate_if(curr, symtable, func_name, stmt_index, nest, inside_if_loop);
            return;
        case RETURN:
            generate_return(curr, symtable);
            return;
        case WHILE:
            generate_while(curr, symtable, func_name, stmt_index, nest, inside_if_loop);
            return;
        case FUNC_CALL:
            generate_func_call(curr, symtable, 1);
            return;
        case VAR_DECL:
            if(inside_if_loop){
                generate_var_assignment(curr, symtable);
            }
            else{     
                generate_var_decl(curr, symtable);
            }
            return;
        case VAR_ASSIGNMENT:
            generate_var_assignment(curr, symtable);
            return;
    }
    return;
}//generate_statement


void generate_code(Arr* input, Tree* symtable){
    //program start ensures we jump into main function directly
    generate_prolog();

    //generate all builtin functions
    generate_builtins();

    //generate all user defined functions
    for(int i = 0; i < input->length; i++){
        AST_Node* curr = (AST_Node*)((input->data)[i]);
        generate_function_decl(curr, symtable);
    }
}//generate_code