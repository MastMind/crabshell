#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <stdint.h>

#include "word_type.h"
#include "commands.h"
#include "defines.h"


struct syntax_tree_node;
typedef struct syntax_tree_node* syntax_tree_t;

typedef struct syntax_tree_node {
	char word[MAX_STRING];
	word_type wtype;
	syntax_tree_t* node;
	syntax_tree_t parent;
	uint32_t nodeSize;
	int (*func)(CMD_ARGS);
	char script_path[MAX_STRING];
} syntax_tree;

syntax_tree_t create_syntax_tree(const command* cmds);
int match_syntax_tree(syntax_tree_t* tree, const char* line, char* auto_complete); //string matches to syntax tree return valuse: 0 - don't match; 1 - match;



#endif