#include "shell.h"


int main() {
	if (shell_init())
		return -1;

	while (!shell_process());

	shell_close();

	return 0;
}