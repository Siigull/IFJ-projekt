/**
 * IFJ PROJEKT 2024
 * @file stack.h
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Header file of stack for IFJ 2024
 * Note: these functions were implemented by the same author
 * as a part of IAL homework1
 * 
 * @date 2024-10-08
 * 
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

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

/**
 * @brief initializes list by setting default values
 * 
 * @param list list to initialize
 */
void List_init(List* list);

// insertions

/**
 * @brief Inserts a new element (Token*) to the first position in the list
 * 
 * @param list list to insert to
 * @param data token to insert
 */
void List_insertF(List* list, Token* data);

/**
 * @brief Inserts new element to the last position (end of the list)
 * 
 * @param list list to insert to
 * @param data token to insert
 */
void List_insertL(List* list, Token* data);

/**
 * @brief Insert a new element to the position next to the active element
 * if list is not active, nothing happens
 * 
 * @param list list to insert to
 * @param data token to insert
 */
void List_insert_after(List* list, Token* data);

// removing elements

/**
 * @brief Removes element located on the last position
 * in case of stack its analogy to pop
 * 
 * @param list list to remove the last element from
 */
void List_removeL(List* list);

// activity functions

/**
 * @brief Sets the activity of the list to the last element
 * 
 * @param list to set the activity in
 */
void List_activeL(List* list);

/**
 * @brief Moves the activity of the list to the element previous of
 * the currently active one
 * 
 * @param list to move the activity in
 */
void List_active_prev(List* list);

// get token pointers

/**
 * @brief gets pointer to Token* that points to the last
 * element of the list
 * 
 * @param list to get the data from
 * @param dataPtr pointer that will be filled with data
 */
void List_last_val(List* list, Token** dataPtr);

/**
 * @brief gets pointer to Token* from the currently active element
 * if list is not active, nothing happens
 * 
 * @param list list to get the data from
 * @param dataPtr pointer that will be filled with data
 */
void List_get_val(List* list, Token** dataPtr);

/**
 * @brief Gets token type of current active element
 * 
 * @param list list to get the token type from
 * @return T_Type token type of active element 
 */
T_Type List_get_first_type(List* list);

#endif