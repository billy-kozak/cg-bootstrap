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
#ifndef PATHUTL_H_
#define PATHUTL_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "memutl.h"

#include <stdlib.h>
/******************************************************************************
*                                   MACROS                                    *
******************************************************************************/
#define PATH_JOIN_NUMARGS(...) (sizeof((char*[]){__VA_ARGS__}) / sizeof(char*))
#define path_join(mem, ...) \
	path_join_f(mem, PATH_JOIN_NUMARGS(__VA_ARGS__), __VA_ARGS__)
/******************************************************************************
*                            FUNCTION DECLARATIONS                            *
******************************************************************************/
char *path_join_f(struct mem_chunk *mem, size_t count, ...);
int mkpath(char *path);
char *path_elems(char *path, char **saveptr);
/*****************************************************************************/
#endif /* PATHUTL_H_ */