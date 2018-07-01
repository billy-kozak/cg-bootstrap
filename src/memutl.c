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
#include "memutl.h"

#include <stdlib.h>
#include <string.h>
#include<stdarg.h>
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
int anynull_f(size_t count, ...)
{
	int ret = 0;
	va_list ap;

	va_start(ap, count);
	for(size_t i = 0; i < count; i++) {
		void *arg = va_arg(ap, void*);

		if(arg == NULL) {
			ret = 1;
			break;
		}
	}
	va_end(ap);

	return ret;
}
/*****************************************************************************/