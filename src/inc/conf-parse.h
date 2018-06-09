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
#ifndef CONF_PARSE_H_
#define CONF_PARSE_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdlib.h>
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
struct cgroup_controller {
	char *group;
	char *controller;
};
/*****************************************************************************/
struct cgroup_param {
	char *name;
	char *value;
};
/*****************************************************************************/
struct prog_conf {
	char *cgroup_prefix;

	size_t num_controllers;
	struct cgroup_controller *controllers;

	size_t num_params;
	struct cgroup_param *params;
};
/******************************************************************************
*                            FUNCTION DECLARATIONS                            *
******************************************************************************/
struct prog_conf *parse_conf(const char *conf_path);
void destroy_conf(struct prog_conf *conf);
/*****************************************************************************/
#endif /* CONF_PARSE_H_ */