/**
 * @file main.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @author David Bujzaš
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief
 * @date 2024-09-15
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "parser.h"

char* load_input(){
	char buffer[100];
    char* input = malloc(100*sizeof(char));
    if(input == NULL) ERROR_RET(ERR_INTERN);
    size_t curr_size = 100;
    while(fgets(buffer, 100, stdin) != NULL){
        if(strlen(input) + strlen(buffer) >= curr_size){
            curr_size *= 2;
            input = realloc(input, curr_size * sizeof(char));
            if(input == NULL) ERROR_RET(ERR_INTERN);
        }
        strcat(input, buffer);
	}
	return input;
}

int mainawdasd() {
	char* input = load_input();
	return compile(input);
}
