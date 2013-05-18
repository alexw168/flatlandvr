/*
 * flatland_thread.h
 *
 * include file for flatland_thread.c
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
 */

#ifndef FLATLAND_THREAD_H
#define FLATLAND_THREAD_H

#ifndef ENABLE_SDL
#	include <pthread.h>
#	define THREAD_RETURN void *
#	define THREAD_RETURN_VALUE NULL
#	define THREAD_HANDLE pthread_t
#else
#	include <SDL/SDL_thread.h>
#	define THREAD_RETURN int
#	define THREAD_RETURN_VALUE 0
#	define THREAD_HANDLE SDL_Thread *
#endif

void fl_ThreadLaunch( THREAD_HANDLE *handle, THREAD_RETURN (*func)(void *), void *data );
void fl_ThreadWait( THREAD_HANDLE *handle );
void fl_ThreadWaitAll( );

#endif
