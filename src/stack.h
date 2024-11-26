/**
 * IFJ PROJEKT 2024
 * @file stack.c
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Header file of stack for IFJ 2024
 * 
 * @date 2024-10-08
 * 
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#define __next_el_macro list->act->nextElement;
#define __prev_el_macro list->act->previousElement;

typedef struct List_elem {
	struct List_elem* nextElement;
	struct List_elem* previousElement;
	Token* token;
} List_elem;

typedef struct List {
	List_elem* first;
	List_elem* last;
	List_elem* act;
	int length;
} List;

void List_init(List* list);
void List_delete(List* list);

// insertions
void List_insertF(List* list, Token* data);
void List_insertL(List* list, Token* data);
void List_insert_after(List* list, Token* data);
void List_insert_before(List* list, Token* data);

// removing elements
void List_removeF(List* list);
void List_removeL(List* list);
void List_remove_after(List* list);
void List_remove_before(List* list);

// activity functions
void List_activeF(List* list);
void List_activeL(List* list);
void List_active_next(List* list);
void List_active_prev(List* list);
bool List_is_active(List* list);

// get token pointers
void List_first_val(List* list, Token** dataPtr);
void List_last_val(List* list, Token** dataPtr);
void List_get_val(List* list, Token** dataPtr);
T_Type List_get_first_type(List* list);
T_Type List_get_last_type(List* list);
void List_set_val(List* list, Token* data);

#endif