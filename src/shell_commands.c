#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell_commands.h"
#include "defines.h"

#define  HELP_FILE "/etc/crabshell/data/helptext.txt"


int cmd_help(CMD_ARGS) {
	FILE* f = fopen(HELP_FILE, "r");

	if (!f) {
		fprintf(stderr, "Can't load file %s\n", HELP_FILE);
		return 0;
	}

	char* line = NULL;
	size_t len = 0;

	while (getline(&line, &len, f) != -1) {
		fprintf(stdout, "%s", line);
	}

	fclose(f);
	if (line) {
		free(line);
	}

	fprintf(stdout, "\n");

	return 0;
}

int cmd_exit(CMD_ARGS) {
	return 1;
}

int cmd_call(CMD_ARGS) {
	char buff[MAX_STRING];

	sprintf(buff, "%s '%s'", script_path, func_name);
	char* const argv[] = { "/bin/sh", "-c", buff, NULL };

	pid_t pid = fork();
	if (pid < 0) {
		//error
		return 0;
	} else if (pid == 0) {
		//(void)system(buff);
		execve(argv[0], argv, NULL);
		exit(0);
	} else {
		int zero = 0;
		//waitpid(pid, &zero, WNOHANG);
		waitpid(pid, &zero, 0);
	}

	fprintf(stdout, "\n");

	return 0;
}

#ifdef DEBUG

int cmd_shell(CMD_ARGS) {
	char* const argv[] = { "/bin/sh", NULL };
	execve(argv[0], argv, NULL);

	return 0;
}

#endif
