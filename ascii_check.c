#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define VER "1.0"
#define MAX	1000

typedef unsigned char BYTE;

enum {OBJECTS = 1, RULES, STANDARD, EXTENDED};

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

	str[i] = '\0';
	return i;
}

bool is_standard(int ch)
{
	return (isprint(ch) || isspace(ch));
}

bool is_extended(int ch)
{
	return (isspace(ch) || isprint(ch) || (ch >= 128 && ch <= 255));
}

bool is_rules(int ch)
{
	return (is_standard(ch) || (BYTE) ch >= 160);
}

bool is_valid(int ch, int opt)
{
	switch (opt) {
		case OBJECTS:
			return is_standard(ch);
		case RULES:
			return is_rules(ch);
		case STANDARD:
			return is_standard(ch);
		case EXTENDED:
			return is_extended(ch);
		default:
			printf("?? opt=%d\n", opt); /* Not possible to get here */
			break;
	}

	printf("??? opt=%d\n", opt);
	return false; /* Also not possible to get here */
}

void write_bad_line(FILE *fp, BYTE *str, int size, int line_num, int opt)
{
	fprintf(fp, "Line %d: ", line_num);

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
	fprintf(fp, "Hex: ");

	for (int i = 0; i < size; i++)
		fprintf(fp, "%X ", str[i]);

	fprintf(fp, "\n\n");
}

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
			write_bad_line(fp_out, line, size, line_num, opt);
			write_bad_char(fp_out, bad_chars, cur_bad);
			cur_bad = 0;
		}

		++line_num;
	}

	return total_bad;
}

void results_header(FILE *fp)
{
	fputs("ASCII Check " VER " Results\n", fp);
	fputs("-----------------------\n\n", fp);

	fputs("This will show the line number, the line, and the "
		"problem character(s) in hexadecimal.\n"
		"The problem character(s) in the line "
		"will be shown as a '.' (period character).\n\n\n", fp);
}

int read_int()
{
	int ch = getchar();

	return (ch == '\n') ? getchar() - '0' : ch - '0';
}

int get_option()
{
	int opt;

	printf("-----OPTIONS AVAILABLE-----\n"
		"1. Objects\n"
		"2. Rules\n"
		"3. Standard\n"
		"4. Extended\n\n");

	while (1) {
		printf("What to check?\n");
		printf("(1-4): ");

		opt = read_int();
		if (opt >= OBJECTS && opt <= EXTENDED)
			break;

		printf("\nError: Invalid option\n\n");
	}

	getchar(); /* Remove newline from buffer */
	return opt;
}

char *get_results_filename(char *name)
{
	char *p = malloc(strlen(name) + strlen("_results.txt") + 1);

	if (p == NULL)
		return NULL;

	strcpy(p, name);
	strcat(p, "_results.txt");

	return p;
}

void scan(char *infile)
{
	FILE *fp_in;
	FILE *fp_out;
	char *outfile;
	int opt, total;

	printf("This will check a file for non-ASCII characters\n\n");

	if ((fp_in = fopen(infile, "r")) == NULL) {
		printf("Error: Can not open %s\n", infile);
		return;
	}

	printf("File is %s\n\n", infile);

	opt = get_option();

	if ((outfile = get_results_filename(infile)) == NULL) {
		printf("\nError: Can not allocate memory to save results\n");
		fclose(fp_in);
		return;
	}

	if ((fp_out = fopen(outfile, "w")) == NULL) {
		printf("\nError: Can not open file to save results\n");
		fclose(fp_in);
		return;
	}

	results_header(fp_out);

	total = get_bad_chars(fp_in, fp_out, opt);
	printf("\nFound %d non-ASCII character(s)\n", total);

	fclose(fp_out); /* Close file stream before remove */
	fclose(fp_in);

	if (total > 0)
		printf("Results saved to %s\n\n", outfile);
	else
		remove(outfile);

	free(outfile);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("ASCII Check " VER "\n\n"
		"Created by Russell Seifert\n"
		"Contribute at "
		"<https://github.com/seiruss/ascii-check>\n\n"
		"Usage:\n"
		"1) ascii_check [file] from CLI\n"
		"2) Drag the file on top of ascii_check\n\n");
	}
	else {
		scan(argv[1]);
	}

	#if defined(WIN32)
		printf("Press [Enter] to close\n");
		getchar();
	#endif

	return EXIT_SUCCESS;
}
