#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "word_type.h"


static int is_word(const char* str);
static int is_string(const char* str);
static int is_number(const char* str);
static int is_ipaddr(const char* str);


int striswt(const char* str, word_type wt) {
	if (!str)
		return 0;

	switch (wt) {
		case WORD:
			return is_word(str);
		case STRING:
			return is_string(str);
		case NUMBER:
			return is_number(str);
		case IPADDR:
			return is_ipaddr(str);
	}

	return 0;
}

static int is_word(const char* str) { //word can't contain spaces
	return (strcspn(str, " ") == strlen(str)) ? 1 : 0;
}

static int is_string(const char* str) { //any sequence can be string
	return 1;
}

static int is_number(const char* str) {
	return (strspn(str, "0123456789.-") == strlen(str)) ? 1 : 0;
}

static int is_ipaddr(const char* str) {
	const char* p = str;
	int i = 0;
	int d_nums = 0;

	for (i = 0; i < 3; i++) {
		d_nums = strspn(p, "0123456789");
		if (!d_nums || d_nums > 3) {
			return 0;
		}

		p += d_nums;
		if (*p != '.') {
			return 0;
		}

		p++;
	}

	d_nums = strspn(p, "0123456789");
	if (!d_nums || d_nums > 3) {
		return 0;
	}

	p += d_nums;

	if (*p != '\0') {
		return 0;
	}

	return 1;
}