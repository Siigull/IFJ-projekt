/**
 * IFJ PROJEKT 2024
 * @file helper.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Implementation used for arrays holding AST nodes in our AST
 * 
 * @date 2024-09-27
 * 
 */

#include "helper.h"

void extend_array(Arr* arr) {
    if (arr->capacity == 0) {
        arr->capacity = 2;

    } else {
        arr->capacity *= 2;
    }

    size_t* data = realloc(arr->data, arr->capacity * sizeof(size_t));
    if (data == NULL) {
        free(arr->data);
        exit(99);
    }

    arr->data = data;
}

void arr_append(Arr* arr, size_t address) {
    if (arr->length >= arr->capacity) {
        extend_array(arr);
    }

    arr->data[arr->length++] = address;
}

Arr* arr_init() {
    Arr* arr = malloc(sizeof(Arr));
    arr->length = 0;
    arr->capacity = 0;
    arr->data = NULL;

    return arr;
}

void arr_destroy(Arr* arr) {
    free(arr->data);
    free(arr);
}
