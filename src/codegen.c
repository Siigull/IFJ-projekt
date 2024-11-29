/**
 * IFJ PROJEKT 2024
 * @file codegen.c
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Implementation of IFJ24 code generator, generates from given AST
 *  
 * @date 2024-11-15
 * 
 */

#include "codegen.h"


// prolog of each program
void generate_prolog(){
    fprintf(stdout, PROG_START);
    return;
}

void builtin_start(char* func_name){
    fprintf(stdout, "LABEL %s\n", func_name);
    fprintf(stdout, "\tCREATEFRAME\n");
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

// generate all built-in function (ifj.func_name)
void generate_builtins(){
    //WRITE
    // pops parameter into *to*write and outputs it
    builtin_start("*write");
    fprintf(stdout, "\tDEFVAR LF@*to*write\n");
    fprintf(stdout, "\tPOPS LF@*to*write\n");
    fprintf(stdout, "\tWRITE LF@*to*write\n");
    builtin_end();

    //i2f
    // pops param into local variable, converts it to float and moves it into
    // global return value
    builtin_start("*i2f");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tPOPS LF@*return*value\n");
    fprintf(stdout, "\tINT2FLOAT LF@*return*value LF@*return*value\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //f2i
    // same as i2f, but converts to integer rather than float
    builtin_start("*f2i");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tPOPS LF@*return*value\n");
    fprintf(stdout, "\tFLOAT2INT LF@*return*value LF@*return*value\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //LENGTH
    // string length using strlen instruction
    builtin_start("*length");
    fprintf(stdout, "\tDEFVAR LF@*strlen\n");
    fprintf(stdout, "\tPOPS LF@*strlen\n");
    fprintf(stdout, "\tSTRLEN LF@*strlen LF@*strlen\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*strlen\n");
    builtin_end();

    //CONCATENATE
    // concatenation of two strings as params
    builtin_start("*concat");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tPOPS LF@*second\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tPOPS LF@*first\n");
    fprintf(stdout, "\tCONCAT LF@*return*value LF@*first LF@*second\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //readstr, readi32, readf64
    // reads from stdin - functionf or each data type
    builtin_reads("*readstr", "string");
    builtin_reads("*readi32", "int");
    builtin_reads("*readf64", "float");

    //ord
    // second param has to be in string - <0, length-1>
    // first param cant be null - its strlen cant be 0
    builtin_start("*ord");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tPOPS LF@*second\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tPOPS LF@*first\n");
    fprintf(stdout, "\tDEFVAR LF@*ord*length\n");
    fprintf(stdout, "\tSTRLEN LF@*ord*length LF@*first\n");
    fprintf(stdout, "\tJUMPIFEQ *ord*err LF@*ord*length int@0\n");
    fprintf(stdout, "\tSUB LF@*ord*length LF@*ord*length int@1\n");
    fprintf(stdout, "\tLT GF@*tmp*res LF@*second int@0\n");
    fprintf(stdout, "\tJUMPIFEQ *ord*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*second LF@*ord*length\n");
    fprintf(stdout, "\tJUMPIFEQ *ord*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tSTRI2INT LF@*return*value LF@*first LF@*second\n");
    fprintf(stdout, "\tLABEL *ord*end\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();
    fprintf(stdout, "\tLABEL *ord*err\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@0\n");
    fprintf(stdout, "\tJUMP *ord*end\n");

    //chr
    // convert integer to char
    builtin_start("*chr");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tPOPS LF@*return*value\n");
    fprintf(stdout, "\tINT2CHAR LF@*return*value LF@*return*value\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    builtin_start("*string");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tPOPS LF@*return*value\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //strcmp
    // 
    builtin_start("*strcmp");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*res\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tPOPS LF@*second\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tPOPS LF@*first\n");
    fprintf(stdout, "\tEQ LF@*res LF@*first LF@*second\n");
    fprintf(stdout, "\tJUMPIFEQ *strcmp*skipeq LF@*res bool@false\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@0\n");
    fprintf(stdout, "\tJUMP *strcmp*end\n");
    fprintf(stdout, "\tLABEL *strcmp*skipeq\n");
    fprintf(stdout, "\tLT LF@*res LF@*first LF@*second\n");
    fprintf(stdout, "\tJUMPIFEQ *strcmp*skiplt LF@*res bool@false\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@-1\n");
    fprintf(stdout, "\tJUMP *strcmp*end\n");
    fprintf(stdout, "\tLABEL *strcmp*skiplt\n");
    fprintf(stdout, "\tMOVE LF@*return*value int@1\n");
    fprintf(stdout, "\tLABEL *strcmp*end\n");
    fprintf(stdout, "\tMOVE GF@*return*val LF@*return*value\n");
    builtin_end();

    //substring
    // param err states : second < 0, second > third, third < 0,
    // second >= strlen(first), third > strlen(first)
    builtin_start("*substring");
    fprintf(stdout, "\tDEFVAR LF@*return*value\n");
    fprintf(stdout, "\tDEFVAR LF@*third\n");
    fprintf(stdout, "\tPOPS LF@*third\n");
    fprintf(stdout, "\tDEFVAR LF@*second\n");
    fprintf(stdout, "\tPOPS LF@*second\n");
    fprintf(stdout, "\tDEFVAR LF@*first\n");
    fprintf(stdout, "\tPOPS LF@*first\n");
    //checking conditions
    fprintf(stdout, "\tLT GF@*tmp*res LF@*second int@0\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tLT GF@*tmp*res LF@*third int@0\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*second LF@*third\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tSTRLEN GF@*tmp*res LF@*first\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*third GF@*tmp*res\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");
    fprintf(stdout, "\tSTRLEN GF@*tmp*res LF@*first\n");
    fprintf(stdout, "\tEQ GF@*expression*result LF@*second GF@*tmp*res\n");
    fprintf(stdout, "\tGT GF@*tmp*res LF@*second GF@*tmp*res\n");
    fprintf(stdout, "\tOR GF@*tmp*res GF@*expression*result GF@*tmp*res\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*err GF@*tmp*res bool@true\n");

    // end of error checking
    fprintf(stdout, "\tMOVE LF@*return*value string@\n");
    fprintf(stdout, "\tDEFVAR LF@*start*index\n");
    fprintf(stdout, "\tDEFVAR LF@*res\n");
    fprintf(stdout, "\tMOVE LF@*start*index LF@*second\n");
    fprintf(stdout, "\tLABEL *substring*while\n");
    fprintf(stdout, "\tEQ LF@*res LF@*start*index LF@*third\n");
    fprintf(stdout, "\tJUMPIFEQ *substring*loop*end LF@*res bool@true\n");

    fprintf(stdout, "\tGETCHAR LF@*res LF@*first LF@*start*index\n");
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
}//is_operator_arithmetic

bool is_data_type(AST_Type type){
    if(type == I32 || type == F64 ||type == STRING){
        return true;
    }

    return false;
}//is_data_type

// postorder traversal through the expression tree
// all evaluations and computations are made on stack
void eval_exp(AST_Node* curr, Tree* symtable){
    if(curr->left != NULL) eval_exp(curr->left, symtable);
    if(curr->right != NULL) eval_exp(curr->right, symtable);

    // here current instruction can be processed
    // it can be arithmetic operation, function call, data type, id or null
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
            case DIV: // based on type of operation chooses correct division instruction
                if(curr->as.expr_type.type == R_F64 || curr->as.expr_type.type == N_F64){
                    fprintf(stdout, "\tDIVS\n");
                }
                else if(curr->as.expr_type.type == R_I32 || curr->as.expr_type.type == N_I32){
                    fprintf(stdout, "\tIDIVS\n");
                }
                break;
        }
    }//arithmetic
    else if(curr->type == FUNC_CALL){
        generate_func_call(curr, symtable);
        // generates function call and based on the type of function,
        // pushes the return value to stack
        Entry* e = tree_find(symtable, curr->as.func_data->var_name);
        if(e != NULL){
            //user-defined functions
            if(e->ret_type.type == R_VOID){
                // void fnc implicitly pushes nil (will not be used)
                // checked by semantic analysis
                fprintf(stdout, "\tPUSHS nil@nil\n");
            }
            else{
                // non void function, push return value
                fprintf(stdout, "\tPUSHS GF@*return*val\n");
            }
        }
        else{
            //builtin case
            fprintf(stdout, "\tPUSHS GF@*return*val\n");
        }
    }//fnc_call
    else if(is_data_type(curr->type)){
        //current node is a data type - push it to stack
        switch(curr->type){
            case I32:
                fprintf(stdout, "\tPUSHS int@%d\n", curr->as.i32);
                break;
            case F64:
                fprintf(stdout, "\tPUSHS float@%a\n", curr->as.f64);
                break;
            case STRING:
                // string has to be converted into correct format (use of string_to_assembly from parser)
                fprintf(stdout, "\tPUSHS string@%s\n", string_to_assembly(curr->as.string));
                break;
        }
    }//data_types
    else if(curr->type == ID){
        // identifier just pushes its value
        fprintf(stdout, "\tPUSHS LF@%s\n", curr->as.var_name);
    }//ID
    else if(curr->type == NIL){
        fprintf(stdout, "\tPUSHS nil@nil\n");
    }//null
    return;
}//eval_exp

bool is_relation_op(AST_Type type){
    if(type == ISEQ || type == ISNEQ || type == ISLESS || type == ISMORE || type == ISLESSEQ || type == ISMOREEQ) 
        return true;

    return false;
}//is_relation_op

//condition structure - root is relation operator, left and right children are classic expressions
void eval_condition(AST_Node* curr, Tree* symtable){
    // condition can either be with relation operator, or just nullable data type
    // relation operator evaluates expression in same way as normal expressions
    // nullable condition has result in *tmp*res - used for nnull var decl
    if(is_relation_op(curr->type)){
        if(curr->left != NULL){
            // left side of relation op
            generate_expression(curr->left, symtable);
            fprintf(stdout, "\tMOVE GF@*lhs GF@*expression*result\n");
        }
        if(curr->right != NULL){
            // right side of relation op
            generate_expression(curr->right, symtable);
            fprintf(stdout, "\tMOVE GF@*rhs GF@*expression*result\n");
        }
    }

    switch(curr->type){
        // RELATION OPERATORS
        case ISEQ:
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISNEQ:
            // !(==)
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
            // < || ==
            fprintf(stdout, "\tLT GF@*tmp*res GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tOR GF@*expression*result GF@*expression*result GF@*tmp*res\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
        case ISMOREEQ:
            // > || ==
            fprintf(stdout, "\tGT GF@*tmp*res GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*lhs GF@*rhs\n");
            fprintf(stdout, "\tOR GF@*expression*result GF@*expression*result GF@*tmp*res\n");
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*expression*result\n");
            break;
            // NULLABLE DATA TYPES - compare with null
        case STRING:
        case I32:
        case F64:
        case ID:
            fprintf(stdout, "\tMOVE GF@*tmp*res LF@%s\n", curr->as.var_name);
            fprintf(stdout, "\tEQ GF@*expression*result LF@%s nil@nil\n", curr->as.var_name);
            fprintf(stdout, "\tNOT GF@*expression*result GF@*expression*result\n");
            break;
        case FUNC_CALL:
            generate_func_call(curr, symtable);
            fprintf(stdout, "\tMOVE GF@*tmp*res GF@*return*val\n");
            fprintf(stdout, "\tEQ GF@*expression*result GF@*return*val nil@nil\n");
            fprintf(stdout, "\tNOT GF@*expression*result GF@*expression*result\n");
            break;
    }//root node evaluation
    return;
}//eval_condition


// value of return of fnc, value to assign into var
// conditions ... -> GF@*expression*result global variable
void generate_expression(AST_Node* curr, Tree* symtable){
    eval_exp(curr, symtable);
    fprintf(stdout, "\tPOPS GF@*expression*result\n");
    return;
}//generate_expression

void predefine_vars(AST_Node* curr, Tree* symtable){
    // goes through all nest levels of if-else and while statements
    // and pre-defines all variables to not cause error in interpretation
    // after this instead of defining a new variable, it just assigns
    Arr* statements = curr->as.arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* curr = (AST_Node*)((statements->data)[i]);
        if(curr->type == VAR_DECL){
            fprintf(stdout, "\tDEFVAR LF@%s\n", curr->as.var_name);
        }
        if(curr->type == NNULL_VAR_DECL){
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
}//predefine_vars

void generate_else(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, int* label_counter){
    // statemetents are in as->arr
    Arr* statements = curr->as.arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, label_counter);
    }
    return;
}//generate_else


void generate_if(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, bool inside_if_loop, int* label_counter){
    // if not already in a nest level, predefine vars
    // for if and for else blocks
    if(!inside_if_loop){
        predefine_vars(curr, symtable);
        predefine_vars(curr->right, symtable);
    }//predefining variables

    //condition evaluation
    eval_condition(curr->left, symtable);
    int tmp = *label_counter;
    // condition is false if evaluated expression is false
    fprintf(stdout, "\tJUMPIFEQ *else*%s*%d*%d*%d GF@*expression*result bool@false\n", func_name, stmt_index, nest, tmp);

    // first statement in as->arr can be nnull var decl
    int i = 0;
    Arr* statements = curr->as.arr;
    if(statements->length != 0) {
        AST_Node* first = (AST_Node*)((statements->data)[0]);
        if(first->type == NNULL_VAR_DECL){
            fprintf(stdout, "\tMOVE LF@%s GF@*tmp*res\n", first->as.var_name);
            i = 1;
        }
    }
    
    // generate all statements
    for(; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, label_counter);
    }

    // end of if block, start of else block
    fprintf(stdout, "\tJUMP *end*of*if*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);
    fprintf(stdout, "\tLABEL *else*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);

    // generate else
    generate_else(curr->right, symtable, func_name, stmt_index, nest, label_counter);
    fprintf(stdout, "\tLABEL *end*of*if*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);
    return;
}//generate_if

void generate_while(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, int nest, bool inside_if_loop, int* label_counter){
    // if not already in a nest level, predefine vars
    if(!inside_if_loop) predefine_vars(curr, symtable);
    
    int tmp = *label_counter;
    
    // condition evaluation
    fprintf(stdout, "\tLABEL *while*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);
    eval_condition(curr->left, symtable);
    // condition is false if evaluated expression is false
    // end the loop if condition is false
    fprintf(stdout, "\tJUMPIFEQ *end*loop*%s*%d*%d*%d GF@*expression*result bool@false\n", func_name, stmt_index, nest, *label_counter);

    // first can be nnull var decl - same as in if-else statement
    int i = 0;
    Arr* statements = curr->as.arr;
    if(statements->length != 0) {
        AST_Node* first = (AST_Node*)((statements->data)[0]);
        if(first->type == NNULL_VAR_DECL){
            fprintf(stdout, "\tMOVE LF@%s GF@*tmp*res\n", first->as.var_name);
            i = 1;
        }
    }

    // generating all statements
    for(; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, func_name, i, nest+1, true, label_counter);
    }

    // jump back to start of the loop
    fprintf(stdout, "\tJUMP *while*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);
    fprintf(stdout, "\tLABEL *end*loop*%s*%d*%d*%d\n", func_name, stmt_index, nest, tmp);
    return;
}//generate_while


void generate_return(AST_Node* curr, Tree* symtable, const char* func_name){
    // in case of main function, use exit instead of return
    if(!strcmp(func_name, "main")){
        fprintf(stdout, "\tPOPFRAME\n");
        fprintf(stdout, "\tEXIT int@0\n");
    }
    else{
        // if function is non-void, expression of return is left child
        if(curr->left != NULL){
            generate_expression(curr->left, symtable);
            fprintf(stdout, "\tMOVE GF@*return*val GF@*expression*result\n");
        }
        fprintf(stdout, "\tPOPFRAME\n");
        fprintf(stdout, "\tRETURN\n\n");
    }
}//generate_return


void generate_func_call(AST_Node* curr, Tree* symtable){
    // push all parameters here
    // uses pascal-like convention of passing parameters for a function
    // each parameter can be an expression (FUNEXP)
    Arr* params = curr->as.func_data->arr;
    for(int i = 0; i < params->length; i++){
        AST_Node* param = (AST_Node*)((params->data)[i]);
        generate_expression(param, symtable);
        fprintf(stdout, "\tPUSHS GF@*expression*result\n");
    }

    fprintf(stdout, "\tCALL %s\n", curr->as.func_data->var_name);
}//generate_func_call


void generate_var_decl(AST_Node* curr, Tree* symtable){
    // if not "_" pseudo-var, declare the variable
    if(strcmp(curr->as.var_name, "_")){
        fprintf(stdout, "\tDEFVAR LF@%s\n", curr->as.var_name);
        generate_expression(curr->left, symtable);
        fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
    }
}//generate_var_decl


void generate_var_assignment(AST_Node* curr, Tree* symtable){
    // if not "_" pseudo-var, evaluate the expression and assign value
    generate_expression(curr->left, symtable);
    if(strcmp(curr->as.var_name, "_")){
        fprintf(stdout, "\tMOVE LF@%s GF@*expression*result\n\n", curr->as.var_name);
    }
}//generate_var_assignment


void generate_function_decl(AST_Node* curr, Tree* symtable){
    fprintf(stdout, "LABEL %s\n", curr->as.func_data->var_name);
    bool main = (!strcmp(curr->as.func_data->var_name, "main"));
    if(main){
        // in main function, dont declare return value
        fprintf(stdout, "\tCREATEFRAME\n");
        fprintf(stdout, "\tPUSHFRAME\n");
    }
    else{
        // implicitly moves null into return value to avoid errors
        // semantic analysis makes sure it doesn't happen when it shouldn't
        fprintf(stdout, "\tCREATEFRAME\n");
        fprintf(stdout, "\tPUSHFRAME\n");
        fprintf(stdout, "\tDEFVAR LF@*return*value\n");
        fprintf(stdout, "\tMOVE LF@*return*value nil@nil\n");
    }

    Entry* arg_entry = tree_find(symtable, curr->as.func_data->var_name);
    Arr* args = arg_entry->as.function_args;
    // create local variables for all arguments
    // and continuously pop parameters from stack into them
    // uses pascal-like convention - parameters are popped from last to first
    for(int i = args->length - 1; i >= 0; i--){
        Function_Arg* argument = (Function_Arg*)((args->data)[i]);
        fprintf(stdout, "\tDEFVAR LF@%s\n", argument->arg_name); 
        fprintf(stdout, "\tPOPS LF@%s\n", argument->arg_name);
    }

    // each function has its label_counter for unique labels
    int* label_counter = malloc(sizeof(int));
    if(label_counter == NULL) ERROR_RET(ERR_INTERN);
    *label_counter = 0;

    // statements in as->func_data as an array
    // generate all of them
    Arr* statements = curr->as.func_data->arr;
    for(int i = 0; i < statements->length; i++){
        AST_Node* stmt = (AST_Node*)((statements->data)[i]);
        generate_statement(stmt, symtable, curr->as.func_data->var_name, i, 0, false, label_counter);
    }

    if(main){
        // main function should have exit instead of return
        fprintf(stdout, PROG_END);
    }
    else if(arg_entry->ret_type.type == R_VOID){
        // void function - implicit return from function
        // because the statement doesnt explicitly doesnt need to be there
        fprintf(stdout, "\tPOPFRAME\n");
        fprintf(stdout, "\tRETURN\n");
    
    } else {
        // non void function with no return statement found
        // ---> error 6
        fprintf(stdout, "\tEXIT int@6\n");
    }

    return;
}//generate_function_decl


void generate_statement(AST_Node* curr, Tree* symtable, const char* func_name, int stmt_index, 
                        int nest, bool inside_if_loop, int* label_counter){
    //choose which type of statement to generate
    switch(curr->type){
        case IF:
            *label_counter += 1;
            generate_if(curr, symtable, func_name, stmt_index, nest, inside_if_loop, label_counter);
            return;
        case RETURN:
            generate_return(curr, symtable, func_name);
            return;
        case WHILE:
            *label_counter += 1;
            generate_while(curr, symtable, func_name, stmt_index, nest, inside_if_loop, label_counter);
            return;
        case FUNC_CALL:
            generate_func_call(curr, symtable);
            return;
        case VAR_DECL:
            if(inside_if_loop){ 
                // inside nesting, just assign, dont declare
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