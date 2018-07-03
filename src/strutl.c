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
#include "strutl.h"

#include "memutl.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
struct lstr strip(const char *str, size_t len)
{
	struct lstr ret = {0, NULL};
	const char *start = str;
	const char *end = str + len - 1;

	while((start < end) && isspace(*start)) {
		start += 1;
	}
	while((end > start) && isspace(*end)) {
		end -= 1;
	}

	if(isspace(*start)) {
		ret.len = 0;
	} else {
		ret.len = end - start + 1;
	}
	ret.str = start;

	return ret;
}
/*****************************************************************************/
struct lstr chop(const char *str, size_t len, char delim)
{
	struct lstr ret = {0, str};

	for(ret.len = 0; ret.len < len; ret.len++) {
		if(str[ret.len] == delim) {
			break;
		}
	}

	return ret;
}
/*****************************************************************************/
struct lstr stripl(struct lstr s)
{
	return strip(s.str, s.len);
}
/*****************************************************************************/
struct lstr chopl(struct lstr s, char delim)
{
	return chop(s.str, s.len, delim);
}
/*****************************************************************************/
int lstrcmp(struct lstr s1, struct lstr s2)
{
	if(s1.len != s2.len) {
		return s1.len - s2.len;
	} else {
		return memcmp(s1.str, s2.str, s1.len);
	}
}
/*****************************************************************************/
int lstrcmp2(struct lstr s1, const char *s2)
{
	for(size_t i = 0; 1; i ++) {
		if(s2[i] == '\0' && i == s1.len) {
			return 0;
		} else if(s2[i] == '\0' && i != s1.len) {
			return 1;
		} else if(s2[i] != '\0' && i == s1.len) {
			return (int)s2[i];
		} else if(s1.str[i] != s2[i]) {
			return (int)(s1.str[i] - s2[i]);
		}
	}
	/* Should never get here */
	return 0;
}
/*****************************************************************************/
void lstrcpy(char *dest, struct lstr src)
{
	memcpy(dest, src.str, src.len);
	dest[src.len] = '\0';
}
/*****************************************************************************/
char *lstrcpy2(struct lstr src)
{
	char *ret = calloc(src.len + 1, sizeof(*ret));

	if(ret == NULL) {
		return NULL;
	}

	memcpy(ret, src.str, src.len);
	return ret;
}
/*****************************************************************************/
char *concat_to(const char *s1, const char *s2, struct mem_chunk *chunk)
{
	char *dst;
	size_t len_1 = strlen(s1);
	size_t len_2 = strlen(s2);

	size_t len_total = len_1 + len_2 + 1;

	if(len_total < chunk->size) {
		if(realloc_chunk(chunk, len_total)) {
			return NULL;
		}
	}

	dst = chunk->mem;

	memcpy(dst, s1, len_1);
	memcpy(dst + len_1, s2, len_2);
	dst[len_total -1] = '\0';

	return dst;
}
/*****************************************************************************/