#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
#include "defines.h"


static struct termios stored_settings;

static int chrinstr(char c, const char* str);


void set_keypress(void) {
	struct termios new_settings;

	tcgetattr(0, &stored_settings);

	new_settings = stored_settings;
	new_settings.c_lflag &= ~( ICANON | ECHO );

	tcsetattr(0, TCSANOW, &new_settings);
}

void reset_keypress(void)
{
	tcsetattr(0, TCSANOW, &stored_settings);
}

char* strip_line(char* line) {
	const char cset[] = " \t\b\n";

	if (!line) {
		return NULL;
	}

	char* str = line;

	str += strlen(str);
	while (chrinstr(*(str--), cset));

	*(str + 1) = '\0';
	str = line + strspn(line, cset);

	return strdup(str);
}

static int chrinstr(char c, const char* str) {
	if (!str)
		return 0;

	uint32_t i = 0;

	for (i = 0; i < strlen(str); i++) {
		if (c == str[i]) {
			return 1;
		}
	}

	return 0;
}