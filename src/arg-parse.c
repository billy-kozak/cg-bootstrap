/******************************************************************************
* Copyright (C) 2018  Billy Kozak                                             *
*                                                                             *
* This file is part of the cg-bootstrap software.                             *
*                                                                             *
* This program is free software: you can redistribute it and/or modify        *
* it under the terms of the GNU Lesser General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU Lesser General Public License for more details.                         *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "arg-parse.h"

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static const struct option GETOPT_OPTIONS[] = {
	{"file", required_argument, NULL, 'f'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, 0, 0}
};
/*****************************************************************************/
static const char* MISSING_FILE_ARG =
	"Error: file option requires an argument\n";
/*****************************************************************************/
static const char *OPT_STRING = "hf:";
/*****************************************************************************/
static const char *HELP_TEXT = "Sets up a cgroup for a program to run in.\n"
	"\n"
	"This utility executes a program within a specified set of\n"
	"cgroups after first (optionally) setting specified cgroup\n"
	"parameters and changing the user.\n";
/******************************************************************************
*                              PRIVATE FUNCTIONS                              *
******************************************************************************/
static int our_args_count(int argc, char **argv)
{
	for(int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		if(arg[0] != '-') {
			return i;
		} else if(strcmp(arg, "--") == 0) {
			return argc == (i + 1) ? argc : i + 1;
		}
	}

	return argc;
}
/*****************************************************************************/
static void parse_our_args(
	int our_argc, char **argv, struct prog_args *prog_args
) {
	bool flag = true;
	int opt_ind = 0;

	while(flag) {
		int c = getopt_long(
			our_argc, argv, OPT_STRING, GETOPT_OPTIONS, &opt_ind
		);
		switch(c) {
			case -1:
				flag = false;
				break;
			case 'f':
				if(optarg == NULL) {
					fprintf(
						stderr, "%s", MISSING_FILE_ARG
					);
					exit(-1);
				}

				prog_args->conf_path = optarg;
				break;
			case 'h':
				fprintf(stderr, "%s", HELP_TEXT);
				exit(0);
				break;
			case '?':
				exit(-1);
				break;
			default:
				fprintf(stderr, "should never get here\n");
				exit(-1);
				break;
		}
	}
}
/*****************************************************************************/
static int parse_their_args(
	int our_argc, int argc, char **argv, struct prog_args *prog_args
) {
	if(our_argc == argc) {
		return 0;
	}

	prog_args->additional_arg_count = argc - our_argc;
	prog_args->prog_path = argv[our_argc];

	prog_args->additional_args = calloc(
		prog_args->additional_arg_count + 1,
		sizeof(*(prog_args->additional_args))
	);
	prog_args->additional_args[0] = prog_args->prog_path;
	for(int i = 1; i < prog_args->additional_arg_count; i++) {
		prog_args->additional_args[i] = argv[our_argc + i];
	}

	return 0;
}
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
struct prog_args parse_args(int argc, char **argv)
{
	struct prog_args prog_args = {NULL, NULL, NULL, 0};
	int our_argc = our_args_count(argc, argv);

	parse_our_args(our_argc, argv, &prog_args);
	parse_their_args(our_argc, argc, argv, &prog_args);

	return prog_args;
}
/*****************************************************************************/
void print_prog_args(FILE *fp, const struct prog_args *prog_args)
{
	fprintf(
		fp, "conf: %s\n",
		prog_args->conf_path ? prog_args->conf_path : "NULL"
	);
	fprintf(
		fp, "prog exec: %s",
		prog_args->prog_path ? prog_args->prog_path : "NULL"
	);
	for(int i = 1; i < prog_args->additional_arg_count; i++) {
		fprintf(fp, " %s", prog_args->additional_args[i]);
	}
	fprintf(fp, "\n");
}
/*****************************************************************************/
void destroy_prog_args(struct prog_args *prog_args)
{
	free(prog_args->additional_args);
}
/*****************************************************************************/