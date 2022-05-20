#ifndef WORD_TYPE_H
#define WORD_TYPE_H


typedef enum word_type_s {
	WORD = 0,
	STRING,
	NUMBER,
	IPADDR,
} word_type;

int striswt(const char* str, word_type wt);


#endif