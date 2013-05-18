/*
 * runway.h
 *
 * Header file for runway.c
 * 
 * Copyright (C) 2003, University of New Mexico.
 * 
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2.1 of the License, or (at your option) any later version.
 * 
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef RUNWAY_H
#define RUNWAY_H

#include "flatland_app.h"

/* Constants */
const GLfloat RUNWAY_FACTOR = 2.0;
const GLfloat RUNWAY_OFFSET = 0.1;

/* function prototypes */
void platform_handle_runway (OBJECT_TYPE *);
void sound_runway (OBJECT_TYPE *);
void draw_runway (OBJECT_TYPE *);
void init_runway (APPTYPE *, char *);

#endif
