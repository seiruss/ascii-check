#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define VER "BETA"
#define MAX	1000

enum {OBJECTS = 1, RULES, STANDARD, EXTENDED};

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

	// not storing for formatting reasons in results
	// need to check if isspace is needed anymore
	// or only look for space ' ' and not newlines
	//if (ch == '\n')
	//	str[i++] = ch;

	str[i] = '\0';

	return i;
}

int is_standard(int ch)
{
	return (isprint(ch) || isspace(ch));
}

int is_extended(int ch)
{
	if (isspace(ch))
		return 1;

	return ((isprint(ch)) || (ch >= 128 && ch <= 255));
}

int is_rules(int ch)
{
	return (is_standard(ch) || (BYTE) ch >= 160);
}

int is_valid(int ch, int opt)
{
	switch (opt) {
		case OBJECTS:
			return is_standard(ch);
			break;
		case RULES:
			return is_rules(ch);
			break;
		case STANDARD:
			return is_standard(ch);
			break;
		case EXTENDED:
			return is_extended(ch);
			break;
		default:
			printf("?? opt=%d\n", opt); /* Not possible to get here */
			break;
	}

	printf("??? opt=%d\n", opt);
	return 0; /* Also not possible to get here */
}

/*void print_bad_line(BYTE *str, int size, int opt)
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
}*/

void write_bad_line(FILE *fp, BYTE *str, int size, int line_num, int opt)
{
	fprintf(fp, "%d: ", line_num);

	for (int i = 0; i < size; i++) {
		if (!is_valid(str[i], opt))
			fputc('.', fp);
		else
			fputc(str[i], fp);
	}

	fputc('\n', fp);
}

void write_bad_char(FILE *fp, BYTE *str, int size)
{
	for (int i = 0; i < size; i++)
		fprintf(fp, "%X ", str[i]);

	fprintf(fp, "\n\n");
}

/*
change to store lines and characters in array
create results file only if non-ascii chars found

*/
int get_bad_chars(FILE *fp_in, FILE *fp_out, char opt)
{
	BYTE line[MAX];
	BYTE bad_chars[MAX];
	int size = 0;
	int total_bad = 0;
	int cur_bad = 0;
	int line_num = 1;

	while ((size = read_line(line, MAX, fp_in)) > 0) {
		for (int i = 0; i < size; i++) {
			if (!is_valid(line[i], opt)) {
				bad_chars[cur_bad++] = line[i];
				++total_bad;
			}
		}

		if (cur_bad > 0) {
			// may have a flag to print to screen instead of file
			//print_bad_char(bad_chars, cur_bad);
			//print_bad_line(line, size, opt);
			write_bad_line(fp_out, line, size, line_num, opt);
			write_bad_char(fp_out, bad_chars, cur_bad);
			cur_bad = 0;
		}

		++line_num;
	}

	return total_bad;
}

int read_char()
{
	int ch = getchar();

	return (ch == '\n') ? getchar() : ch;
}

int get_option()
{
	int opt;

	printf("-----OPTIONS AVAILABLE-----\n"
		"1. Objects\n"
		"2. Rules\n"
		"3. Standard\n"
		"4. Extended\n\n"
		"What to check?\n"
		"(1-4): ");

	//scanf(" %d", &opt); /* Leaves newline in buffer */
	opt = read_char();
	opt -= '0';

	while (opt < OBJECTS || opt > EXTENDED) {
		printf("\nError: Invalid option\n\n");
		printf("What to check?\n");
		printf("(1-4): ");
		//scanf(" %d", &opt);
		opt = read_char();
		opt -= '0';
	}

	return opt;
}

char *get_output_filename(char *name)
{
	char *dot = strrchr(name, '.');
	char *p = NULL;

	if (dot == NULL) {
		// no extension in orginal file. add _results.txt to it
		p = malloc(strlen(name) + strlen("_results.txt") + 1);
		if (p == NULL) {
			printf("Error: Can not allocate memory to save results\n");
			exit(1);
		}
		strcpy(p, name);
		strcat(p, "_results.txt");
	}
	else {
		// there is a dot. remove dot and all after and add _results.txt
		p = malloc(strlen(name) - strlen(dot) + strlen("_results.txt") + 1);
		if (p == NULL) {
			printf("Error: Can not allocate memory to save results\n");
			exit(1);
		}
		// copy name - dot into p
		strncpy(p, name, strlen(name) - strlen(dot));
		// add results
		strcat(p, "_results.txt");
	}

	return p;
}

int main(int argc, char *argv[])
{
	FILE *fp_in;
	FILE *fp_out;
	int opt;
	int n;
	char *res;

	if (argc != 2) {
		printf("ASCII Check " VER "\n\n"
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

	if ((fp_in = fopen(argv[1], "r")) == NULL) {
		printf("Error: Can not open %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	res = get_output_filename(argv[1]);

	if ((fp_out = fopen(res, "w")) == NULL) {
		printf("Error: Can not open file to save results\n");
		free(res);
		fclose(fp_in);
		return EXIT_FAILURE;
	}

	printf("File is %s\n\n", argv[1]);

	opt = get_option();

	fputs("ASCII Check " VER " Results\n", fp_out);
	fputs("-----------------------\n\n", fp_out);
	fputs("This will show the line number, the line, and the "
		"problem character(s) in hexadecimal.\n", fp_out);
	fputs("The problem character(s) in the line "
		"will be shown as a '.' (period character).\n\n\n", fp_out);

	n = get_bad_chars(fp_in, fp_out, opt);
	printf("\nFound %d non-ASCII character(s)\n", n);

	if (n > 0)
		printf("Results saved to %s\n\n", res);
	else
		remove(res);

	free(res);
	fclose(fp_out);
	fclose(fp_in);

	printf("Press [Enter] to close\n");
	getchar();
	getchar();

	return EXIT_SUCCESS;
}
