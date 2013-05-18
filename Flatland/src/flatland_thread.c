/*
flatland_thread.cxx.  functions for simple access to threads

Copyright (C) 2003, University of New Mexico.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
             
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* 
 * Maintenance history
 *
 * Created: 2-02-01 TBE
 *
 */

#ifdef WIN32
#  include "Flatland.h"
#  include "libFlatland.h"
#else
#  include "gheader.h"
#endif

struct flThreadListStruct
{
	int Size;
	int Number;
	THREAD_HANDLE *Handle;
};


flThreadListStruct flThreadList = { 0, 0, NULL };


void fl_ThreadLaunch( THREAD_HANDLE *handle, THREAD_RETURN (*func)(void *), void *data )
{
	int i;

#ifdef ENABLE_SDL
	*handle = SDL_CreateThread( func, data );
#else
	pthread_create( handle, NULL, func, data );
	pthread_detach( *handle );
#endif
	if ( flThreadList.Number == flThreadList.Size )
	{
		if ( flThreadList.Size == 0 ) 
		{
			flThreadList.Size = 4;
			flThreadList.Handle = new THREAD_HANDLE [ flThreadList.Size ];
			if ( !flThreadList.Handle )
			{
				exit( 1 );
			}
		}
		else
		{
			THREAD_HANDLE *temp = flThreadList.Handle;
			flThreadList.Size *= 2;
			flThreadList.Handle = new THREAD_HANDLE [ flThreadList.Size ];
			if ( !flThreadList.Handle )
			{
				exit( 1 );
			}
			for( i=0; i<flThreadList.Number; i++ )
			{
				flThreadList.Handle[ i ] = temp[ i ];
			}
			delete[] temp;
		}
	}
	flThreadList.Handle[ flThreadList.Number++ ] = *handle;
}


void fl_ThreadWait( THREAD_HANDLE *handle )
{
#ifdef ENABLE_SDL
	int ret = 0;
	SDL_WaitThread( *handle, &ret );
#else
	void *ret = NULL;
	pthread_join( *handle, &ret );
#endif
}


void fl_ThreadWaitAll( )
{
	int i;

	for( i=0; i<flThreadList.Number; i++ )
	{
		fl_ThreadWait( &flThreadList.Handle[ i ] );
	}
}

