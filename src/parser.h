#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "helper.h"

typedef struct Parser {
    Token* next;
    Token* prev;
} Parser;

void parse(const char* input);

#endif