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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
/******************************************************************************
*                              PUBLIC FUNCTIONS                               *
******************************************************************************/
int main (int argc, char **argv)
{
	struct prog_args args = parse_args(argc, argv);

	if(args.prog_path == NULL) {
		destroy_prog_args(&args);
		return 0;
	} else if(execvp(args.prog_path, args.additional_args)) {
		perror("Error: unable to execute");
		destroy_prog_args(&args);
		return 1;
	}

	return 0;
}
/*****************************************************************************/