/**
 * @file helper.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <ast.h>

typedef struct {
    size_t length;
    size_t capacity;
    size_t element_length;

    AST_Node** data;
} Arr;

void arr_append(Arr* arr, AST_Node* el);

Arr* arr_init();

void arr_destroy(Arr* arr);

#endif
