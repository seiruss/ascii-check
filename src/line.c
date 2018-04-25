#include <stdio.h>
#include <stdbool.h>
#include "scan.h"
#include "ascii_check.h"

int read_line(BYTE *str, int size, FILE *fp)
{
	int i = 0;
	int ch;

	while ((ch = fgetc(fp)) != EOF && ch != '\n') {
		if (i < size) {
			str[i++] = ch;
		}
		else {
			printf("Error: line has %d characters or more\n", MAX);
			return 0; /* To break out of the read_line while loop */
		}
	}

	str[i] = '\0';
	return i;
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

void results_header(FILE *fp)
{
	fputs("ASCII Check Results\n", fp);
	fputs("-------------------\n\n", fp);

	fputs("The problem character(s) will be shown as a '.' "
		"(period character).\nThese character(s) will be "
		"shown in hexadecimal below the relevant line.\n\n", fp);

	if (opt.type == RULES) {
		fputs("------------------------------------------------"
			"--------------------------------------------------"
			"-----------------------------------\n", fp);
		fputs("          DOMAIN NAME          |            "
			"LAYER NAME            |            RULE NAME            "
			"|            COMMENTS\n", fp);
		fputs("------------------------------------------------"
			"--------------------------------------------------"
			"-----------------------------------\n\n", fp);
	}
}
