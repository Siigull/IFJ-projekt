/**
 * IFJ PROJEKT 2024
 * @file test_generate_graph.h
 * @author Daniel Pelánek (xpeland00@stud.fit.vutbr.cz)
 * @brief Header file for graph generation
 * 
 * @date 2024-11-13
 * 
 */

#ifndef TEST_GENERATE_GRAPH_H
#define TEST_GENERATE_GRAPH_H

#include "ast.h"
#include <stdio.h>
#include <string.h>

void generate_graph(AST_Node* node, const char* filepath);

#endif