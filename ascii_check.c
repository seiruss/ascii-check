#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX	1000
#define OBJECTS 1
#define RULES 2

typedef unsigned char BYTE;

int read_line(BYTE *str, int size, FILE *fp)
{
	int i = 0;
	int ch;

	while ((ch = fgetc(fp)) != EOF && ch != '\n') {
		if (i < size) {
			str[i++] = ch;
		}
		else {
			printf("\nError: line has %d characters or more\n", MAX);
			return 0; /* To break out of the read_line while loop */
		}
	}

	if (ch == '\n')
		str[i++] = ch;

	str[i] = '\0';

	return i;
}

int is_valid(int ch, int opt)
{
	if (opt == RULES && ((BYTE) ch) >= 160)
		return 1;

	if (ch == '\x0A' || ch == '\x0D' || ch == '\t')
		return 1;

	if (isprint(ch))
		return 1;

	return 0;
}

void print_bad_line(BYTE *str, int size, int opt)
{
	printf("\nProblem line:\n");

	for (int i = 0; i < size; i++) {
		if (!is_valid(str[i], opt))
			putchar('.');
		else
			putchar(str[i]);
	}
}

void print_bad_char(BYTE *str, int size)
{
	printf("\nProblem character(s) in hex: ");

	for (int i = 0; i < size; i++)
		printf("%X ", str[i]);
}

int get_bad_chars(FILE *fp, char opt)
{
	BYTE line[MAX];
	BYTE bad_chars[MAX];
	int size = 0;
	int total_bad = 0;
	int cur_bad = 0;

	while ((size = read_line(line, MAX, fp)) > 0) {
		for (int i = 0; i < size; i++) {
			if (!is_valid(line[i], opt)) {
				bad_chars[cur_bad++] = line[i];
				++total_bad;
			}
		}

		if (cur_bad > 0) {
			print_bad_char(bad_chars, cur_bad);
			print_bad_line(line, size, opt);
			cur_bad = 0;
		}
	}

	return total_bad;
}

int get_option()
{
	int opt;

	printf("-----OPTIONS AVAILABLE-----\n");
	printf("1. Objects\n"
		"2. Rules\n\n"
		"Checking objects or rules?\n"
		"(1-2): ");

	scanf(" %d", &opt);
	return opt;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int n;
	int opt;

	if (argc != 2) {
		printf("ASCII Check 1.0\n\n"
		"Created by Russell Seifert\n"
		"Contribute at "
		"<https://github.com/seiruss/ascii-check>\n\n"
		"Usage:\n"
		"1) ascii_check [file] from CLI\n"
		"2) Drag the text file on top of ascii_check\n\n");

		printf("Press [Enter] to close\n");
		getchar();
		return EXIT_FAILURE;
	}

	printf("This tool will check a file for non-ASCII characters\n\n");

	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Error: Can not open %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	opt = get_option();
	if (opt != OBJECTS && opt != RULES) {
		printf("\nError: Invalid option = %d\n", opt);
		fclose(fp);
		return EXIT_FAILURE;
	}

	printf("\nFile is %s\n", argv[1]);

	n = get_bad_chars(fp, opt);
	printf("\n\nFound %d non-ASCII character(s)\n", n);

	printf("Press [Enter] to close\n");
	getchar();
	getchar();

	fclose(fp);
	return EXIT_SUCCESS;
}
