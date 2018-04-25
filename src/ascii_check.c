#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "ascii_check.h"
#include "scan.h"

static struct option long_options[] =
{
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'v' },
	{ 0, 0, 0, 0 }
};

static void usage(char *argv[])
{
	printf("Usage: %s <OPTION> <FILE>\n\n", argv[0]);
	printf("Miscellaneous:\n"
		"  -h, --help	    Display this help\n"
		"  -v, --version	    version information\n\n"
		"Scanning options:\n"
		"  -o	Objects\n"
		"  -r	Rules\n"
		"  -s	Standard ASCII\n"
		"  -e	Extended ASCII\n\n");
}

static void usage_short(char *argv[], char *msg)
{
	printf("%s\n"
		"Usage: %s <OPTION> <FILE>\n"
		"Try '%s --help' for more information\n\n", msg, argv[0], argv[0]);
}

static void version(char *argv[])
{
	printf("%s 1.1\n", argv[0]);
	printf("Written by Russell Seifert\n");
	printf("Contribute at <https://github.com/seiruss/ascii-check>\n\n");
}

static int parse_args(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	opterr = 0; /* getopt() does not print its own error message */

	/* If a specific option was not selected, scan Standard */
	if (argc == 2)
		opt.type = STANDARD;

	while ((c = getopt_long(argc, argv, "hveors",
				long_options, &option_index)) != -1)
	{
		switch (c) {
			case 'e':
				opt.type = EXTENDED;
				break;
			case 'o':
				opt.type = OBJECTS;
				break;
			case 'r':
				opt.type = RULES;
				break;
			case 's':
				opt.type = STANDARD;
				break;
			case 'v':
				version(argv);
				opt.exit_after_args = true;
				return EXIT_SUCCESS;
			case '?':
				printf("Invalid option '%s'\n"
					"Usage: %s [OPTIONS] <FILE>\n"
					"Try '%s --help' for more information\n\n", \
					argv[optind - 1], argv[0], argv[0]);
				return EXIT_FAILURE;
			case 'h':
				usage(argv);
				opt.exit_after_args = true;
				return EXIT_SUCCESS;
			default:
				usage(argv);
				return EXIT_FAILURE;
		}
	}

	/* File name is one less than argc */
	if (argc - optind == 1) {
		opt.infile = argv[optind];
	}
	else {
		if (argc - optind == 0) {
			usage_short(argv, "File name is missing");
		}
		else {
			/* There are arguments after the file name */
			usage_short(argv, "Too many arguments");
		}

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int rc = EXIT_SUCCESS;
	opt.exit_after_args = false;

	if (argc < 2 || argc > 3) {
		usage(argv);
		rc = EXIT_FAILURE;
	}
	else {
		rc = parse_args(argc, argv);
		if (rc == EXIT_SUCCESS && !opt.exit_after_args)
			rc = scan_file();
	}

	return rc;
}
