/**
 * IFJ PROJEKT 2024
 * @file expressionparser.h
 * @author Martin Vaculík (xvaculm00@stud.fit.vutbr.cz)
 * @brief Header file of expression parser for IFJ 2024
 * 
 * @date 2024-10-15
 * 
 */

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#define TABLE_SIZE 14

typedef enum {
    N, // no precedence
    L, // left precedence
    R, // right precedence
    M, // equal precedence
} precedence;

/*
Rules: E  → E + E
       E  → E - E
       E  → E * E
        E  → E / E
        E → E == E
        E  → (E)
        E  → id
        E  → E relacni operator E

*/

typedef enum {
    E_PLUS_E,
    E_MINUS_E,
    E_TIMES_E,
    E_DIV_E,
    E_EQ_E,
    E_E,
    E_ID,
    E_NEQ_E,
    E_GET_E,
    E_GT_E,
    E_ST_E,
    E_SET_E,
    E_ZAVAROKA_DEADLY_E
} rules;


/**
 * @brief Initializies a DLL, which will be used as stack later on 
 * 
 * @param Stack 
 */
void init_stack(List* Stack);

/**
 * @brief Parses and expression, called from parser
 * 
 * @param list, list of tokens that are supposed to represent an expression 
 * @return AST_Node* 
 */
AST_Node* parse_expression(List* list);
/**
 * @brief Does an operation with stacks based on the given rule
 * 
 * @param rule does a reduction based o given rule, E ← E + E  
                                                    E ← E - E  
                                                    E ← E * E  
                                                    E ← E / E  
                                                    E ← E == E  
                                                    E ← (E)  
                                                    E ← id  
                                                    E ← E relacni operator E
    takes an enum rule 
 * @param Stack, stack where rule should be processed
 */
void handle_rule(int rule, List* Stack);

/**
 * @brief if precedence is none, throws error, if its one, finds reduction rule, if its two, pushes on stack symbol plus reduction symbol
 * if its equal, pushs only symbol to stack
 * 
 * @param precedence 
 * @param Stack, stack where operetion is made
 * @param input , stack where we get symbols to push
 */
void handle_precedence(int precedence, List* Stack, List* input);
/**
 * @brief insert dollar to a list
 * 
 * @param input list
 */
void insert_dollar(List* input);
/**
 * @brief looks if reduction rule is possible, than calls handle rule 
 * 
 * @param Stack, stack where we are reducing 
 * @return int, rule 
 */
int find_reduction_rule(List* Stack);
/**
 * @brief find index of nonterminal symbols that will be an input to our precedence table to which operation are we doing 
 * 
 * @param list 
 * @return int 
 */
int find_precedence_index(List* list);
/**
 * @brief looks if the T_Type is nonTerminal or Terminal symbol
 * 
 * @param type 
 * @param isProcessed 
 * @return true 
 * @return false 
 */
bool is_nonTerm(T_Type type, bool isProcessed);
/**
 * @brief looks if the T_Type is an symbol that represents ID 
 * 
 * @param type 
 * @return true 
 * @return false 
 */
bool is_id(T_Type type);
#endif