/**
 * IFJ PROJEKT 2024
 * @file stack.c
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Implemenation of stack for IFJ 2024
 * 
 * @date 2024-10-08
 * 
 */

#include "stack.h"

void List_init(List *list ) {
	//setting all values of the doubly linked list to NULL or zero
	list->first = list->last = NULL;
	list->act = NULL;
	list->length = 0;
	return;
}

void List_insertF(List *list, Token* data ) {
	List_elem* new_elem = (List_elem*)malloc(sizeof(List_elem));
	if(new_elem == NULL){ // memory allocation error
		return;
	}
	//have to divide empty and non empty lists when inserting a new element
	if(list->length == 0 && list->first == NULL && list->last == NULL){ // list is empty
		//empty means that the newly created element is also the last one and there is nothing after it
		list->last = new_elem;
		new_elem->nextElement = NULL;
	}//empty list

	else{ // list is not empty
		//non empty means there is something after it - have to set pointers connecting them
		list->first->previousElement = new_elem;
		new_elem->nextElement = list->first;
	}//non empty list

	list->length++; //adding one element to the list - increment length
	new_elem->previousElement = NULL; //there is nothing before the first element
	new_elem->token = data; //filling element with correct data
	list->first = new_elem; //finally setting first element pointer to the newly created one
	
	return;
}//DLL_InsertFirst

void List_insertL(List *list, Token* data ) {
	List_elem* new_elem = (List_elem*)malloc(sizeof(List_elem));
	if(new_elem == NULL){ 
		return;
	}
	//have to divide empty and non empty lists when inserting a new element
	if(list->length == 0){ // list is empty
		//empty means that the newly created element is also the first one in the list and there is nothing before it
		list->first = new_elem;
		new_elem->previousElement = NULL;
	}//empty list

	else{ // list is not empty
		//non empty means there is something before the new element - we have to set pointers connecting them
		list->last->nextElement = new_elem;
		new_elem->previousElement = list->last;
	}//non empty list
 
	list->length++; 
	new_elem->nextElement = NULL; 
	new_elem->token = data;  
	list->last = new_elem;
	return;
}


void List_activeL(List *list ) {
	list->act = list->last; //setting the last element of list as active
	return;
}//DLL_Last

void List_last_val(List *list, Token **dataPtr ) {
	if(list->first == NULL && list->last == NULL){ //empty list error
		return;
	}
	//non empty list
	if(dataPtr != NULL){
		*dataPtr = list->last->token; //setting dataPtr to the correct value of last element
	}
	return;
}

void List_removeL(List *list ) {
	if(list->length != 0){//list has to be non empty
		if(list->act == list->last)
			list->act = NULL;

		List_elem* tmp_ptr = list->last; //temporary pointer to the element that is to be deleted
		if(list->length == 1 && list->first == list->last){ //deleted element is the only one in there
			//it was the first and also the last one, so we edit the list accordingly
			list->first = NULL;
			list->last = NULL;
		}//list with one element

		else{ //there are more elements
			//remove connection of the last element to the previous one
			//and the previous element becomes last in the list
			list->last->previousElement->nextElement = NULL;
			list->last = list->last->previousElement;
		}//list with more elements

		list->length--;
		free(tmp_ptr);

	}//list not empty

	return;
}

void List_insert_after(List *list, Token* data ) {
	//list has to be active
	if(list->act != NULL){
		List_elem* new_elem = (List_elem*)malloc(sizeof(List_elem));
		if(new_elem == NULL){ // memory allocation error
			return;
		}
		list->length++;
		new_elem->token = data;
		if(list->act->nextElement == NULL){ // are inserting at the end of the list
			//this means that we cant set pointers of the next element, because there is not one
			//the new element is also the last one, so its correctly set in the list
			list->act->nextElement = new_elem;
			new_elem->previousElement = list->act;
			//setting last element
			list->last = new_elem;
			new_elem->nextElement = NULL;
		}//list empty

		else{ // are inserting in the middle
			//have to make 4 connections between the elements - 2 with the next one after the inserted, 2 with the one before
			new_elem->nextElement = list->act->nextElement;
			list->act->nextElement->previousElement = new_elem;
			new_elem->previousElement = list->act;
			list->act->nextElement = new_elem;
		}//list not empty

	}//list active

	return;
}

void List_get_val(List *list, Token **dataPtr ) {
	if(list->act == NULL){ //non active list error
		return;
	}
	//active list
	if(dataPtr != NULL){
		*dataPtr = list->act->token; // setting dataPtr to the correct value of active element
	}
	return;
}


void List_active_prev(List *list ) {
	if(list->act == NULL){
		return;
	}//not active

	//list has to be active
	if(list->act != NULL){
		//setting active element ot the previous element of the current active
		List_elem* previous = list->act->previousElement;
		list->act = previous;
		return;
	}//active list
	else{
		return;
	}
}
T_Type List_get_first_type(List* list){
	return list->first->token->type;
};