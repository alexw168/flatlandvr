/*
 * read_config.h
 *
 * Header file for read_config.c
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

#ifndef READ_CONFIG_H
#define READ_CONFIG_H

//extern int ww0, wh0;
extern char sysname[], universe[];

typedef struct apptype {
        int type ; /* 0=base app, 1=optional app */
        bool loaded ; /* true = loaded, false = not loaded */
        bool unloadable ; /* true = loaded, and in the unload menu */
        char DLLloc[MAX_NAME_SIZE] ;
        char DLLname[MAX_NAME_SIZE] ;
        char appname[MAX_NAME_SIZE];
        char universe_name[MAX_NAME_SIZE] ;
        UNIVERSE_TYPE *universe_ptr ;
        char parent_object_name[MAX_NAME_SIZE] ;
        void *libc; /* Pointer to the loaded library */
        int nparams ;
        char ***params ;

        bool menu_entry_on_flag; /* if true add app to glut menu (default = true) */
        bool shared_flag;        /* Collaboration flag: true if app is shared (default = false) */
} APPTYPE ;

typedef struct applisttype {
	int napps ;
	int maxapps ;
	APPTYPE **applications ;
} APPLISTTYPE ;

APPTYPE *fl_makeapp(int type) ;
APPLISTTYPE *fl_makeapplist( int maxapps ) ;
APPLISTTYPE *fl_read_config_file( char *file, bool debug_config ) ;

#endif
