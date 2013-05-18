/*
 * Flatland.h
 *
 * Header file for Flatland.cpp
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

#ifndef FLATLAND_H
#define FLATLAND_H

// Struct for passing arguments to glut
typedef struct 
{
    int	 *argc;
    char **argv;
} args_struct;

// globals 
extern Boolean do_exit;                         // exit now flag 
extern Boolean exit_now;			// everybody else is gone...
                                                // you exit now too!
extern struct applisttype *loadableApps;	// app specified in config file 
extern int maxobjects;						// MAX # object in the graph 
extern int maxxforms;						// MAX # of transforms between objects 
extern UNIVERSE_TYPE *Universe ;			// *THE* universe 
extern OBJECT_TYPE *CenterOfTheUniverse;

#endif
