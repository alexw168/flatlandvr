/*
ctmutex.c.  general purpose mutex class

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
 * Created: 1-17-00
 */

#include "gheader.h"

mutex::mutex(void)
{
    if(pthread_mutex_init(&Mutex,NULL)){
       SemException.message("Could Not Create Mutex");
       throw SemException;
    }
}

mutex::~mutex(void)
{
    if(pthread_mutex_destroy(&Mutex)){
       SemException.message("Could Not Destroy Mutex");
       throw SemException;
    }
}

void mutex::lock(void)
{
      pthread_mutex_lock(&Mutex);
}

void mutex::unlock(void)
{
    pthread_mutex_unlock(&Mutex);
}





