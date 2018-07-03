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
#include "pathutl.h"

#include "memutl.h"

#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
int mkpath(char *path)
{
	char *saveptr = NULL;
	char *elem;
	struct stat sb;

	while((elem = path_elems(path, &saveptr)) != NULL) {
		if(stat(elem, &sb) != 0) {
			if(mkdir(elem, 0644)) {
				return 1;
			}
		}
	}

	return 0;
}
/*****************************************************************************/
char *path_elems(char *path, char **saveptr)
{
	char *ptr = *saveptr;
	if(ptr == NULL) {
		if(path[0] == '/') {
			*saveptr = &path[1];
			return "/";
		} else {
			*saveptr = &path[0];
			return "./";
		}
	}

	if(ptr != path && ptr[-1] == '\0') {
		ptr[-1] = '/';
	}

	if(ptr[0] == '\0') {
		return NULL;
	}

	while(*ptr == '/') {
		ptr += 1;
	}

	if(*ptr == '\0') {
		*saveptr = ptr;
		return NULL;
	}

	while(*ptr != '/' && *ptr != '\0') {
		ptr += 1;
	}

	if(*ptr == '/') {
		*ptr = '\0';
		ptr += 1;
	}

	*saveptr = ptr;
	return path;
}
/*****************************************************************************/
char *path_join_f(struct mem_chunk *chunk, size_t count, ...)
{
	char *path = NULL;
	size_t path_len = 0;
	va_list ap;

	va_start(ap, count);
	for(size_t i = 0; i < count; i++) {
		char *str = va_arg(ap, char*);

		size_t len = strlen(str);

		if(i != (count - 1) && str[len - 1] != '/') {
			path_len += len + 1;
		} else {
			path_len += len;
		}
	}
	va_end(ap);

	if(chunk->size < (path_len + 1)) {
		if(realloc_chunk(chunk, path_len + 1)) {
			return NULL;
		}
	}
	path = chunk->mem;

	va_start(ap, count);
	for(size_t k = 0, i = 0; i < count; i++) {
		char *str = va_arg(ap, char*);

		for(size_t j = 0; str[j] != '\0'; j++) {
			path[k] = str[j];
			k += 1;
		}

		if(i != (count - 1) && path[k - 1] != '/') {
			path[k] = '/';
			k += 1;
		}
	}
	va_end(ap);

	path[path_len] = '\0';


	return path;
}
/*****************************************************************************/