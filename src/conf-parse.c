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
#include "conf-parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <errno.h>
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
enum conf_section {
	CONF_GENERAL,
	CONF_CONTROLLER_GROUPS,
	CONF_GROUP_PARAMS,

	CONF_NUM_SECTIONS
};
/******************************************************************************
*                              PRIVATE FUNCTIONS                              *
******************************************************************************/

/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
struct prog_conf *parse_conf(const char *conf_path)
{
	char *line = NULL;
	size_t line_buffer_size = 0;
	ssize_t len = 0;

	FILE *fp = fopen(conf_path, "r");
	if(fp == NULL) {
		perror("Error: unable to read configuration file");
		exit(-1);
	}

	while((len = getline(&line, &line_buffer_size, fp)) != -1) {
	}

	free(line);
	if(errno) {
		perror("Error reading config file");
		return NULL;
	}

	fclose(fp);
	return NULL;
}
/*****************************************************************************/
void destroy_conf(struct prog_conf *conf)
{
}
/*****************************************************************************/