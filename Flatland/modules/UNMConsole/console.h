/*
 * console.h
 *
 * Header file for console.c
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
#ifndef CONSOLE_H
#define CONSOLE_H

/* function prototypes */
void sound_console (OBJECT_TYPE *);
void draw_console (OBJECT_TYPE *);
void platform_handler_console (OBJECT_TYPE *);
void predraw_console (OBJECT_TYPE *o);
void make_console (void);

/* required "well known" init function */
extern "C"
{
void Flatland_init (APPTYPE *);
}

#endif
