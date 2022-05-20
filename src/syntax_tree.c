#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <stdio.h>

#include "syntax_tree.h"
#include "utils.h"


static syntax_tree_t create_node(const char* wrd, word_type wt, int (*f)(CMD_ARGS), const char* scr_path);
static void add_node(syntax_tree_t tree, syntax_tree_t node);
static word_type spectow(const char* str);


syntax_tree_t create_syntax_tree(const command* cmds) {
	syntax_tree_t ret = create_node("", WORD, NULL, "");

	if (!ret) {
		return NULL;
	}

	while (strcmp(cmds->name, "")) {
		//parse cmds->name into words
		syntax_tree_t p = ret;
		char* p_str = strdup(cmds->name);

		if (!p_str) {
			//something wrong
			break;
		}
		char* str = strip_line(p_str);

		if (!str) {
			//something wrong
			break;
		}

		char* s = str;
		int spec_word = 0;
		uint32_t len = 0;

		while (*s != '\0') {
			if (*s != '\"') {
				spec_word = 0;
				len = strcspn(s, " ");
			} else {
				s++;
				spec_word = 1;
				len = strcspn(s, "\"");
			}

			if (*(s + len) == ' ' || *(s + len) == '\"') {
				*(s + len) = '\0';
				len++;
				if (spec_word && *(s + len) != '\0')
					len++;
			}

			//search node
			uint32_t i = 0;
			int flag_dub = 0;
			
			while (p && i < p->nodeSize) {
				if ((!strcmp((*(p->node + i))->word, "") && (*(p->node + i))->wtype == spectow(s)) || //case for special word
				(!strcmp((*(p->node + i))->word, s))) { //case for non special word
					p = *(p->node + i);
					flag_dub = 1;
					break;
				}

				i++;
			}
			//end search

			if (!p) {
				//something wrong
				break;
			}

			if (flag_dub && *(s + len) == '\0') {
				p->func = cmds->callback_func;
				break;
			}

			if (i == p->nodeSize && !flag_dub) {
				syntax_tree_t node = create_node(spec_word ? "" : s, spec_word ? spectow(s) : WORD, (*(s + len) == '\0') ? cmds->callback_func : NULL, cmds->script_path);
				add_node(p, node);
				p = node;
			}

			s += len;
		}

		free(str);
		free(p_str);

		cmds++;
	}

	return ret;
}

int match_syntax_tree(syntax_tree_t* tree, const char* line, char* auto_complete) {
	if (!tree || !(*tree) || !line || !strlen(line))
		return 0;

	syntax_tree_t p = *tree;
	char* ss = strdup(line);
	char* str = strip_line(ss);
	char comp_tail[MAX_COMMAND_LENGTH];
	int ret = 0;

	if (!ss || !str) {
		//something wrong
		goto end;
	}

	char* s = str;
	int spec_word = 0;
	uint32_t len = 0;

	while (*s != '\0') {
		memset(comp_tail, 0, sizeof(comp_tail));

		if (*s != '\"') {
			spec_word = 0;
			len = strcspn(s, " ");
		} else {
			s++;
			spec_word = 1;
			len = strcspn(s, "\"");
		}
		if (*(s + len) == ' ' || *(s + len) == '\"') {
			*(s + len) = '\0';
			len++;
			if (spec_word && *(s + len) != '\0')
				len++;
		}

		//search in tree
		uint32_t i = 0;
		int flag_autocompleted = 0;
		
		if (!p->nodeSize)
			goto end;

		while (p && i < p->nodeSize) {
			if (((*(p->node + i))->wtype != WORD && striswt(s, (*(p->node + i))->wtype)) || 
			((*(p->node + i))->wtype == WORD && !strcmp((*(p->node + i))->word, s))) {
				memset(comp_tail, 0, sizeof(comp_tail));
				p = *(p->node + i);
				i = 0;
				break;
			}

			if (strstr((*(p->node + i))->word, s) == (*(p->node + i))->word && auto_complete) {
				if (flag_autocompleted) { //more that one autocomplete
					if (*comp_tail)
						fprintf(stdout, "\n%s%s\n", s, comp_tail);
					fprintf(stdout, "%s\n", (*(p->node + i))->word);
					memset(comp_tail, 0, sizeof(comp_tail));
				} else {
					strcpy(comp_tail, (*(p->node + i))->word + strlen(s));
					strcpy(comp_tail + strlen(comp_tail), " ");
					flag_autocompleted = 1;
				}
			}

			i++;
		}
		//end search
		if (!p) {
			//something wrong
			goto end;
		}

		if (i == p->nodeSize && i > 0) {//not found
			if (auto_complete) {
				strcpy(auto_complete, comp_tail);
			}

			goto end;
		}

		s += len;
	}

	if (p->func) {
		ret = 1;
	}

end:
	*tree = p;
	if (str)
		free(str);
	if (ss)
		free(ss);

	return ret;
}

static syntax_tree_t create_node(const char* wrd, word_type wt, int (*f)(CMD_ARGS), const char* scr_path) {
	if (!wrd)
		return NULL;

	syntax_tree_t ret = malloc(sizeof(syntax_tree));

	if (!ret) {
		return NULL;
	}

	strcpy(ret->word, wrd);
	strcpy(ret->script_path, scr_path);
	ret->wtype = wt;
	ret->node = NULL;
	ret->parent = NULL;
	ret->nodeSize = 0;
	ret->func = f;

	return ret;
}

static void add_node(syntax_tree_t tree, syntax_tree_t node) {
	if (!tree || !node)
		return;

	tree->node = (syntax_tree_t*)realloc(tree->node, sizeof(syntax_tree) * (tree->nodeSize + 1));

	if (!tree->node)
		return;

	*(tree->node + tree->nodeSize) = node;
	tree->nodeSize++;

	node->parent = tree;
}

static word_type spectow(const char* str) {
	if (!strcmp(str, "string")) {
		return STRING;
	} else if (!strcmp(str, "num")) {
		return NUMBER;
	} else if (!strcmp(str, "ip")) {
		return IPADDR;
	} else {
		return WORD;
	}
}