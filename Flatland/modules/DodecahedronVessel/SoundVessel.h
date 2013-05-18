/*
 * sound_craft.h
 *
 * Header file for sound_craft.c
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

#ifndef SOUND_CRAFT_H
#define SOUND_CRAFT_H

/* Constants */
const int NUM_ENG_SOUNDS = 7;
const int IDX_ENG_BASE = 1;
const float ENG_BLENDING_DROPOFF_RATE = 3.0;
const float ENG_VOLUME = 0.5;

/* function prototypes */
void open_eng_sounds(void);
void set_eng_gains(const float *gains);
void set_eng_loc(float *loc);
int engine_sound(float velocity);

#endif
