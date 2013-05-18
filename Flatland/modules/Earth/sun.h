/*
 * sun.h
 *
 * Header file for sun.c
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

#ifndef SUN_H
#define SUN_H

#include "flatland_app.h"


typedef struct sun_globals
{
  bool display_sun_flag ;

} SUN_GLOBALS;

/* Constants */
const GLfloat sunx=930.0,suny=4000.0,sunz=462.0;        /* sun coords */

/* function prototypes */
void platform_handle_sun (OBJECT_TYPE *);
void sound_sun (OBJECT_TYPE *);
void lighting_sun (OBJECT_TYPE *);
void draw_sun (OBJECT_TYPE *);
void init_sun (APPTYPE *);
#endif
