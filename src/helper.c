/**
 * @file helper.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief 
 * @date 2024-09-27
 * 
 */

#include "helper.h"

void arr_append(Arr* arr, AST_Node* el) {
    if (arr->length >= arr->capacity) {
        if (arr->capacity == 0) {
            arr->capacity = 2;

        } else {
            arr->capacity *= 2;
        }

        void* data = arr->data;
        arr->data = realloc(arr->data, arr->capacity * sizeof(AST_Node*));
        if (arr->data == NULL) {
            free(data);
            exit(99);
        }
    }

    arr->data[arr->length++] = el;
}

Arr* arr_init() {
    Arr* arr = malloc(sizeof(Arr));
    arr->length = 0;
    arr->capacity = 0;
    arr->element_length;

    return arr;
}

void arr_destroy(Arr* arr) {
    free(arr->data);
    free(arr);
}
