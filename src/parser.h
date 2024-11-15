/**
 * @file parser.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * 
 */

#ifndef PARSER_H
#define PARSER_H

#include "expressionparser.h"
#include "helper.h"
#include "ast.h"
#include "error.h"
#include "string.h"
#include "codegen.h"

typedef struct Parser {
    Token* next;
    Token* prev;

    Tree* s_table;
} Parser;

void parse(char* input);
int compile(char* input);
char* string_to_assembly(const char* string);

#endif