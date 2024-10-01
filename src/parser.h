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

typedef struct Parser {
    Token* next;
    Token* prev;

    Tree* s_table;
} Parser;

void parse(char* input);

#endif