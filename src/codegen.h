#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "parser.h"
#include "ast.h"
#include "bvs.h"

#define PROG_START ".IFJcode24\nDEFVAR GF@__expression__result\nJUMP __func_main\n\n"
#define PROG_END "\tPOPFRAME\n\tEXIT int@0\n\n"

void generate_code(Arr* input, Tree* symtable);
#endif