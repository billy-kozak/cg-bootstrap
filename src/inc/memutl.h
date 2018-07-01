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
#ifndef MEMUTL_H_
#define MEMUTL_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdlib.h>
/******************************************************************************
*                                   MACROS                                    *
******************************************************************************/
#define NUMARGS(...) (sizeof((void*[]){__VA_ARGS__}) / sizeof(void*))
#define anynull(...) (anynull_f(NUMARGS(__VA_ARGS__), __VA_ARGS__))
/******************************************************************************
*                            FUNCTION DECLARATIONS                            *
******************************************************************************/
int anynull_f(size_t count, ...);
/*****************************************************************************/
#endif /* MEMUTL_H_ */