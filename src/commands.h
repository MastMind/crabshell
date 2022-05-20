#ifndef COMMANDS_H
#define COMMANDS_H

#include "shell_commands.h"
#include "defines.h"


typedef struct command_s {
	char name[MAX_STRING];
	int (*callback_func)(CMD_ARGS);
	char script_path[MAX_STRING];
} command;

extern const command shell_commands[];
extern const unsigned int size_shell_commands;


#endif