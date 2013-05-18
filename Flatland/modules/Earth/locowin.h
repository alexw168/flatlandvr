/*
 * locowin.h
 *
 * Header file for locowin.c
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

#ifndef LOCOWIN_H
#define LOCOWIN_H

#include "flatland_app.h"

/* function prototypes */
void world_loco_display (void);
void world_loco_mouse (int button, int state, int x, int y);
void world_loco_motion (int x, int y);
void world_loco_key (unsigned char c, int x, int y);
void world_loco_reshape (int w, int h);
void world_loco_visibility (int state);
void init_loco_window(void);
#endif
