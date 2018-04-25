#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "scan.h"
#include "ascii_check.h"
#include "line.h"

static bool is_standard(int ch)
{
	return (isprint(ch) || isspace(ch));
}

static bool is_extended(int ch)
{
	return (isprint(ch) || isspace(ch) || (ch >= 128 && ch <= 255));
}

static bool is_rules(int ch)
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

	/* Also not possible to get here */
	printf("??? opt=%d\n", opt);
	return false;
}

static int get_bad_chars(FILE *fp_in, FILE *fp_out)
{
	BYTE line[MAX];
	BYTE bad_chars[MAX];
	int size = 0;
	int total_bad = 0;
	int cur_bad = 0;
	int line_num = 0;

	while ((size = read_line(line, MAX, fp_in)) > 0) {
		++line_num;

		for (int i = 0; i < size; i++) {
			if (!is_valid(line[i], opt.type)) {
				bad_chars[cur_bad++] = line[i];
				++total_bad;
			}
		}

		if (cur_bad > 0) {
			write_bad_line(fp_out, line, size, line_num, opt.type);
			write_bad_char(fp_out, bad_chars, cur_bad);
			cur_bad = 0;
		}
	}

	return total_bad;
}

static char *get_outname(const char *name)
{
	char *p = malloc(strlen(name) + strlen("_results.txt") + 1);

	if (p == NULL)
		return NULL;

	strcpy(p, name);
	strcat(p, "_results.txt");

	return p;
}

int scan_file()
{
	FILE *fp_in;
	FILE *fp_out;
	char *outfile;
	int total;

	if ((fp_in = fopen(opt.infile, "r")) == NULL) {
		printf("Error: Can not open %s\n", opt.infile);
		return EXIT_FAILURE;
	}

	if ((outfile = get_outname(opt.infile)) == NULL) {
		printf("Error: Can not allocate memory to save results\n");
		fclose(fp_in);
		return EXIT_FAILURE;
	}

	if ((fp_out = fopen(outfile, "w")) == NULL) {
		printf("Error: Can not open file to save results\n");
		fclose(fp_in);
		return EXIT_FAILURE;
	}

	results_header(fp_out);

	total = get_bad_chars(fp_in, fp_out);
	printf("Found %d non-ASCII character(s)\n", total);

	fclose(fp_out); /* Close file stream before remove */
	fclose(fp_in);

	if (total > 0)
		printf("Results saved to %s\n", outfile);
	else
		remove(outfile);

	free(outfile);
	return EXIT_SUCCESS;
}
