#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "parser.h"
#include "ast.h"
#include "bvs.h"

#define PROG_START ".IFJcode24\nDEFVAR GF@*expression*result\nJUMP *func*main\n\n"
#define PROG_END "\tPOPFRAME\n\tEXIT int@0\n\n"



void generate_prolog();
void generate_builtins();
void eval_exp(AST_Node* curr);
void generate_expression(AST_Node* curr, Tree* symtable);
void generate_if(AST_Node* curr, Tree* symtable);
void generate_return(AST_Node* curr, Tree* symtable);
void generate_while(AST_Node* curr, Tree* symtable);
void generate_func_call(AST_Node* curr, Tree* symtable);
void generate_statement(AST_Node* curr, Tree* symtable);
void generate_function_decl(AST_Node* curr, Tree* symtable);
void generate_var_decl(AST_Node* curr, Tree* symtable);
void generate_var_assignment(AST_Node* curr, Tree* symtable);


void generate_code(Arr* input, Tree* symtable);
#endif