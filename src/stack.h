#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#define __next_el_macro list->act->nextElement;
#define __prev_el_macro list->act->previousElement;


typedef struct List_elem{
    struct List_elem* nextElement;
    struct List_elem* previousElement;
    Token* token;
}List_elem;


typedef struct List{
    List_elem* first;
    List_elem* last;
    List_elem* act;
    int length;
}List;