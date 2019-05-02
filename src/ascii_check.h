#ifndef ASCII_CHECK_H
#define ASCII_CHECK_H

struct {
	bool exit_after_args;
	const char *infile;
	int type;
} opt;

enum {STANDARD = 1, EXTENDED};

#endif /* ASCII_CHECK_H */
