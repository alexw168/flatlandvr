/* 
 * sound_thread.h
 *
 * Header file for fl_sound_thread.c
 *
 * Copyright (C) 2003, University of New Mexico.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *           
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * pfh 12/15/99
 */

#if !defined(SOUND_THREAD_H)
#define SOUND_THREAD_H

#if defined(ENABLE_SDL)

// globals
extern char sound_server [];
extern int max_oc_sounds;

// Prototypes
void * fl_sound_thread(void *);

#endif // ENABLE_SDL

#endif // SOUND_THREAD_H
