#include <stdlib.h>

#include "commands.h"


#define DEBUG


const command shell_commands[] = {
	{ "help",  cmd_help, "" },
	{ "exit",  cmd_exit, "" },
	{ "quit",  cmd_exit, "" },
#ifdef DEBUG
	{ "shell",  cmd_shell, "" },
#endif
	{ "", NULL, "" } //terminal element
};

const unsigned int size_shell_commands = sizeof(shell_commands);
