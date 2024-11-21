/**
 * IFJ PROJEKT 2024
 * @file helper.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>

typedef enum {
    S_I32,
    S_F64,
    S_U8_ARR,
} Symbol_Type;

typedef struct {
    Symbol_Type type;
    const char* ident;
} Symbol;

typedef enum {
    A_SYMBOL,
    A_AST,
} Arr_Type;

typedef struct {
    size_t length;
    size_t capacity;

    size_t* data;
} Arr;

void arr_append(Arr* arr, size_t address);

Arr* arr_init();

void arr_destroy(Arr* arr);

#endif
