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
#ifndef ARG_PARSE_H_
#define ARG_PARSE_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdio.h>
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
struct prog_args {
	char *conf_path;
	char *prog_path;

	char **additional_args;
	int additional_arg_count;
};
/******************************************************************************
*                            FUNCTION DECLARATIONS                            *
******************************************************************************/
struct prog_args parse_args(int argc, char **argv);
void destroy_prog_args(struct prog_args *args);
void print_prog_args(FILE *fp, const struct prog_args *prog_args);
/*****************************************************************************/
#endif /* ARG_PARSE_H_ */