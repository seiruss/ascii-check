#ifndef ASCII_CHECK_H
#define ASCII_CHECK_H

struct {
	bool exit_after_args;
	const char *infile;
	int type;
} opt;

enum {OBJECTS = 1, RULES, STANDARD, EXTENDED};

#endif /* ASCII_CHECK_H */
