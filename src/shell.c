#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"
#include "utils.h"
#include "defines.h"
#include "commands.h"
#include "syntax_tree.h"
#include "shell_commands.h"


static char cmd_history[CMD_HISTORY_LENGTH + 1][MAX_COMMAND_LENGTH];
static char line[MAX_COMMAND_LENGTH];
static uint32_t lcur = 0;
static uint32_t hcur = 0;
static uint32_t commands_length = 0;
static syntax_tree_t synt_tree = NULL;
static char username[32];

static fd_set rfds;
static fd_set ifds;

static void console_invitation(void);
static int parse_line(char* str);
static void push_console(char c);
static char handle_input(void);
static void cmd_to_history(void);
static void clear_line(void);
static command* load_commands(const char* path);
static void init_screen(void);


int shell_init(void) {
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	command* ext_shell_commands = load_commands("/etc/crabshell/data/commands.txt");

	if (!ext_shell_commands) {
		synt_tree = create_syntax_tree(shell_commands);
	} else {
		ext_shell_commands = (command*)realloc(ext_shell_commands, sizeof(command) * commands_length + size_shell_commands);
		if (!ext_shell_commands) {
			fprintf(stderr, "Error: can't allocate memory\n");
			return -1;
		}

		memcpy(ext_shell_commands + commands_length, shell_commands, size_shell_commands);

		synt_tree = create_syntax_tree(ext_shell_commands);
	}
	set_keypress();

	memset(cmd_history, 0, sizeof(cmd_history));

	init_screen();

	memset(line, 0, sizeof(line));
	lcur = 0;

	console_invitation();

	return 0;
}

int shell_process(void) {
	int retval = 0;

	char c = '\0';

	ifds = rfds;

	retval = select(FD_SETSIZE, &ifds, NULL, NULL, NULL);

	if ((retval < 0 && errno == EINTR) || retval == 0) {
		return 0;
	}

	if (retval < 0) {
		return 1;
	}

	int i = 0;

	for (i = 0; i < FD_SETSIZE; i++) {
		if (!FD_ISSET(i, &ifds)) {
			continue;
		}

		c = handle_input();
		push_console(c);
	}

	if (c != '\n') {
		return 0;
	}

	int ret = parse_line(line);
	memset(line, 0, sizeof(line));
	lcur = 0;

	console_invitation();

	return ret;
}
//++
void shell_close(void) {
	reset_keypress();
}

static void console_invitation(void) {
	if (getlogin_r(username, sizeof(username)))
		return;

	fprintf(stdout, "{ %s }> ", username);
	fflush(stdout);
}

static int parse_line(char* str) {
	char* lstr = strip_line(str);
	int ret = 0;
	syntax_tree_t tree = synt_tree;

	//fprintf(stdout, "Executed line: %s\n", lstr);

	if (!lstr) {
		fprintf(stderr, "Error: can't parse line: %s\n", str);
		fprintf(stderr, "Critical error! Shell closed!");

		return -1;
	}

	cmd_to_history();

	if (!strlen(lstr)) {
		free(lstr);
		return 0;
	}

	int match = match_syntax_tree(&tree, lstr, NULL);
	if (match) { //execute command
		ret = tree->func(lstr, tree->script_path);
	} else {
		fprintf(stdout, "Wrong command!\n");
		fflush(stdout);
	}

	free(lstr);
	return ret;
}

static void push_console(char c) {
	syntax_tree_t tree = NULL;
	int match = 0;
	char auto_complete[MAX_COMMAND_LENGTH];
	uint32_t offset = 0;
	uint32_t i = 0;


	switch (c) {
		case CTRL_BACKSPACE:
			if (lcur > 0) {
				fprintf(stdout, "\b \b");
				clear_line();
				lcur--;
				offset = strlen(line) - lcur - 1;
				for (i = lcur; i < offset + lcur + 1; i++) {
					line[i] = line[i + 1];
				}
				line[offset + lcur] = '\0';

				fprintf (stdout, "%s", line);
				if (offset) {
					fprintf(stdout, "\033[%uD", offset);
				}
			}
			break;
		case CTRL_UP:
			//fprintf(stdout, "Arrow up\n");
			clear_line();
			if (hcur > 0) {
				hcur--;
			}
			strcpy(line, cmd_history[hcur]);
			fprintf(stdout, "%s", line);
			lcur = strlen(line);
			break;
		case CTRL_DOWN:
			clear_line();
			//fprintf(stdout, "Arrow down\n");
			if (hcur < CMD_HISTORY_LENGTH && cmd_history[hcur][0]) {
				hcur++;
				memset(line, 0, sizeof(line));
				strcpy(line, cmd_history[hcur]);
			}

			fprintf(stdout, "%s", line);
			lcur = strlen(line);
			break;
		case CTRL_RIGHT:
			if (lcur < strlen(line)) {
				fprintf(stdout, "\033[1C");
				lcur++;
			}
			break;
		case CTRL_LEFT:
			if (lcur > 0) {
				fprintf(stdout, "\033[1D");
				lcur--;
			}
			break;
		case CTRL_ENTER:
			fprintf(stdout, "\n");
			break;
		case CTRL_TAB:
			//fprintf(stdout, "Line is:%s", line);
			memset(auto_complete, 0, sizeof(auto_complete));
			tree = synt_tree;
			match = match_syntax_tree(&tree, line, auto_complete);
			if (!match) {
				clear_line();
				//fprintf(stdout, "\nCommand is not matched! tree->word: %s\n", tree->word);
				//fprintf(stderr, "Auto completion: %s\n", auto_complete);
				//fprintf(stdout, "%s", auto_complete);
				lcur = strlen(line);
				strcpy(line + lcur, auto_complete);
				lcur = strlen(line);

				if (lcur > MAX_COMMAND_LENGTH)
					lcur = MAX_COMMAND_LENGTH;

				//clear_line();
				fprintf(stdout, "%s", line);
			}
			break;
		case CTRL_DEL:
			//fprintf(stdout, "Delete pressed\n");
				if (line[lcur] != 0) {
					fprintf(stdout, "\b \b");
					clear_line();
					offset = strlen(line) - lcur -1;
					//strcpy (&line[lcur], &line[lcur + 1]);
					for (i = lcur; i < offset + lcur + 1; i++) {
						line[i] = line[i + 1];
					}
					line[offset + lcur] = '\0';

					fprintf (stdout, "%s", line);
					if (offset) {
						fprintf(stdout, "\033[%uD", offset);
					}									}
			break;

		case CTRL_HOME:				
				clear_line();
				offset = strlen(line);
				fprintf (stdout, "%s", line);
				lcur = 0;
				if (offset) {
					fprintf(stdout, "\033[%uD", offset);
				}

			break;					

		case CTRL_END:	
				clear_line();
				lcur = strlen (line);
				fprintf (stdout, "%s", line);
			break;

		default:
			if (c < 32) //don't print character which lesser than minimal ascii code for printing
				break;

			clear_line();
			offset = strlen(line) - lcur;
			//strcpy(line + lcur + 1, line + lcur);
			line[offset + lcur + 1] = '\0';
			for (i = offset + lcur; i > lcur; i--) {
				line[i] = line[i - 1];
			}
			line[lcur] = c;

			if (lcur < MAX_COMMAND_LENGTH)
				lcur++;
		
			fprintf(stdout, "%s", line);
			if (offset) {
				fprintf(stdout, "\033[%uD", offset);
			}
	}


	fflush(stdout);
}

static char handle_input(void) {
	char c = getchar();

	if (c == 27) { //27 = esc sequence code
		c = getchar();
		if (c == '[') {
			c = getchar();
			switch (c) {
			case 'A':
				c = CTRL_UP;
				break;
			case 'B':
				c = CTRL_DOWN;
				break;
			case 'C':
				c = CTRL_RIGHT;
				break;
			case 'D':
				c = CTRL_LEFT;
				break;
			case '3':
				(void)getchar(); //need for jump delete key sequence - that ended by character '~`
				c = CTRL_DEL;
				break;
			case 'H':
				c = CTRL_HOME;
				break;
			case 'F':
				c = CTRL_END;
				break;
			}
		}
	}

	return c;
}

static void cmd_to_history(void) {
	if (!strlen(line))
		return;

	hcur = 0;
	while (cmd_history[hcur][0] && hcur < CMD_HISTORY_LENGTH) {
		hcur++;
	}

	if (hcur == CMD_HISTORY_LENGTH) {
		memcpy(cmd_history, cmd_history + 1, MAX_COMMAND_LENGTH * (CMD_HISTORY_LENGTH - 1));
		hcur--;	
	}

	strcpy(cmd_history[hcur], line);
	fflush(stdout);
	hcur++;
}

static void clear_line(void) {
	fprintf(stdout, "\33[2K\r");
	console_invitation();
}

static command* load_commands(const char* path) {
	if (!path || *path == '\0') {
		return NULL;
	}

	FILE* f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Warning: Can't open file %s\n", path);
		return NULL;
	}

	char* line = NULL;
	size_t len = 0;
	//ssize_t read = 0;
	command* ret = NULL;

	while (getline(&line, &len, f) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        *(line + strlen(line) - 1) = '\0';
        //fprintf(stdout, "Handling line: %s\n", line);

        ret = (command*)realloc(ret, sizeof(command) * (commands_length + 1));
        if (!ret) {
        	fprintf(stderr, "Warning: can't allocate memory\n");
        	break;
        }

        command* p_ret = ret + commands_length;

        //check line
        char* p = strstr(line, "  ");
        if (!p) {
        	fprintf(stdout, "Warning: bad line %s\n", line);
        	continue;
        }
        *p = '\0';
        p++;
        p += strspn(p, " ");

        strcpy(p_ret->name, line);
        strcpy(p_ret->script_path, p);

        p_ret->callback_func = cmd_call;
        commands_length++;
    }

	fclose(f);

	return ret;
}

static void init_screen(void) {
	//init message here
}
