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
#ifndef STRUTL_H_
#define STRUTL_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "memutl.h"

#include <stdlib.h>
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
struct lstr {
	size_t len;
	const char *str;
};
/******************************************************************************
*                            FUNCTION DECLARATIONS                            *
******************************************************************************/
struct lstr strip(const char *str, size_t len);
struct lstr chop(const char *str, size_t len, char delim);
struct lstr stripl(struct lstr s);
struct lstr chopl(struct lstr s, char delim);
int lstrcmp(struct lstr s1, struct lstr s2);
int lstrcmp2(struct lstr s1, const char *s2);
void lstrcpy(char *dest, struct lstr src);
char *lstrcpy2(struct lstr src);
char *concat_to(const char *s1, const char *s2, struct mem_chunk *chunk);
/*****************************************************************************/
#endif /* STRUTL_H_ */