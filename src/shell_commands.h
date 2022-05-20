#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H


#define DEBUG
#define CMD_ARGS const char* func_name, const char* script_path


int cmd_help(CMD_ARGS);
int cmd_exit(CMD_ARGS);
int cmd_call(CMD_ARGS);
#ifdef DEBUG
int cmd_shell(CMD_ARGS);
#endif

#endif