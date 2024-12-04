/**
 * IFJ PROJEKT 2024
 * @file main.c
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @author David Bujzaš (xbujzad00@stud.fit.vutbr.cz)
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Main program for IFJ24 compiler
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

    size_t curr_size = 100;
    char* input = calloc(curr_size, sizeof(char));
    if(input == NULL) ERROR_RET(ERR_INTERN);

    while (fgets(buffer, 100, stdin) != NULL){
        if (strlen(input) + strlen(buffer) >= curr_size){
            curr_size *= 2;
            input = realloc(input, curr_size * sizeof(char));
            if(input == NULL) ERROR_RET(ERR_INTERN);
        }

        strcat(input, buffer);
	}

	return input;
}

int main() {
	char* input = load_input();
	return compile(input);
}
