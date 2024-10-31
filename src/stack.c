#include "stack.h"


void List_init(List *list ) {
	//setting all values of the doubly linked list to NULL or zero
	list->first = list->last = NULL;
	list->act = NULL;
	list->length = 0;
	return;
}

void List_delete(List *list ) {
	//going through the whole list and deleting elements one by one
	while(list->first != NULL && list->length != 0){
		
		//this is basically the same implementation as DeleteFirst, first I wanted to use the function itself
		//but documentation says to not call any other functions from the file, so Dispose was remade accordingly
		//and it still has the same functionality - removing first element until there is no other element

		List_elem* tmp_ptr = list->first; //temporary pointer to the element that is to be deleted
		if(list->length == 1 && list->first == list->last){ //deleted element is the only one in there
			//it was the first and also the last one, so we edit the list accordingly
			list->first = NULL;
			list->last = NULL;
		}//list with one element

		else{ //there are more elements
			//remove connection of the first element to the next one
			//and the next element becomes first in the list
			list->first->nextElement->previousElement = NULL;
			list->first = list->first->nextElement;
		}//list with more elements

		list->length--;
		free(tmp_ptr);
	}//removing elements

	//setting the list to values it would have after initialization
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
	if(new_elem == NULL){ // memory allocation error
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
 
	list->length++; //adding one element to the list - increment length
	new_elem->nextElement = NULL; //there is nothing after the last element
	new_elem->token = data; //filling element with correct data
	list->last = new_elem; //finally setting last element pointer to the newly created one
	
	return;
}

void List_activeF(List *list ) {
	list->act = list->first; //setting the first element of list as active
	return;
}

void List_activeL(List *list ) {
	list->act = list->last; //setting the last element of list as active
	return;
}//DLL_Last

void List_first_val(List *list, Token **dataPtr ) {
	if(list->first == NULL && list->last == NULL){ //empty list error
		return;
	}
	//non empty list
	if(dataPtr != NULL){
		*dataPtr = list->first->token; //setting dataPtr to the correct value of first element
	}
	return;
}

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

void List_removeF(List *list ) {
	if(list->length != 0){//list has to be non empty
		if(list->act == list->first)
			list->act = NULL;

		List_elem* tmp_ptr = list->first; //temporary pointer to the element that is to be deleted
		if(list->length == 1 && list->first == list->last){ //deleted element is the only one in there
			//it was the first and also the last one, so we edit the list accordingly
			list->first = NULL;
			list->last = NULL;
		}//list with one element

		else{ //there are more elements
			//remove connection of the first element to the next one
			//and the next element becomes first in the list
			list->first->nextElement->previousElement = NULL;
			list->first = list->first->nextElement;
		}//list with more elements

		list->length--;
		free(tmp_ptr);

	}//list not empty

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

void List_remove_after(List *list ) {
	if(list->act != NULL && list->act->nextElement != NULL){ //element has to be active and has to have a next element
		 
		List_elem* tmp_ptr = list->act->nextElement; //temporary pointer to the element that is to be deleted
		if(list->act->nextElement == list->last){ //deleted element is the last in the list
			list->last = list->act; //active element becomes the last in the list
			list->act->nextElement = NULL;
		}//deleted is last

		else{ //deleted element is not last
			//setting pointers of elements surrounding the deleted one accordingly
			// "překlenutí" - next element of active is the one after the deleted, 
			//previous element of the one after deleted is now the active element
			list->act->nextElement->nextElement->previousElement = list->act;
			list->act->nextElement = list->act->nextElement->nextElement;
		}//not last

		list->length--;
		free(tmp_ptr);
	}//list active, next element

	return;
}

void List_remove_before(List *list ) {
	if(list->act != NULL && list->act->previousElement != NULL){ //element has to be active and has to have a previous element
		
		List_elem* tmp_ptr = list->act->previousElement; //temporary pointer to the element that is to be deleted
		if(list->act->previousElement == list->first){ //deleted element is the first in the list
			list->first = list->act; //active element becomes the first in the list
			list->act->previousElement = NULL;
		}//deleted is first

		else{ //deleted element is not first
			//setting pointers of elements surrounding the deleted one accordingly
			// "překlenutí" - previous element of active is the one before the deleted, 
			//next element of the one before deleted is now the active element
			list->act->previousElement->previousElement->nextElement = list->act;
			list->act->previousElement = list->act->previousElement->previousElement;
		}//not first

		list->length--;
		free(tmp_ptr);
	}//list active, next element

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

void List_insert_before(List *list, Token* data ) {
	//list has to be active
	if(list->act != NULL){
		List_elem* new_elem = (List_elem*)malloc(sizeof(List_elem));
		if(new_elem == NULL){ // memory allocation error
			return;
		}
		list->length++;
		new_elem->token = data;
		if(list->act->previousElement == NULL){ // are inserting at the start of the list
			//this means that we cant set pointers of the previous element, because there is not one
			//the new element is also the first one, so its correctly set in the list
			list->act->previousElement = new_elem;
			new_elem->nextElement = list->act;
			//setting first element
			list->first = new_elem;
			new_elem->previousElement = NULL;
		}//list empty

		else{ // are inserting in the middle
			//have to make 4 connections between the elements - 2 with the next one after the inserted, 2 with the one before
			new_elem->previousElement = list->act->previousElement;
			list->act->previousElement->nextElement = new_elem;
			new_elem->nextElement = list->act;
			list->act->previousElement = new_elem;
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

void List_set_val(List *list, Token* data ) {
	//list has to be active
	if(list->act != NULL){
		list->act->token = data; //setting the active element to data parameter
	}
	return;
}

void List_active_next(List *list ) {
	//list has to be active
	if(list->act != NULL){
		//setting active element ot the next element of the current active
		List_elem* next = __next_el_macro;
		list->act = next;
		return;
	}
	else{
		return;
	}
}


void List_active_prev(List *list ) {
	if(list->act == NULL){
		return;
	}//not active

	//list has to be active
	if(list->act != NULL){
		//setting active element ot the previous element of the current active
		List_elem* previous = __prev_el_macro;
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


T_Type List_get_last_type(List* list){
	return list->first->token->type;
}

bool List_is_active(List *list ) {
	return (list->act == NULL) ? false : true;
}