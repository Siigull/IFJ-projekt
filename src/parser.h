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

typedef struct Parser {
    Token* next;
    Token* prev;
} Parser;

void parse(char* input);

#endif