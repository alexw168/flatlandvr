/*
 * gheader.h
 *
 * header to contain things that are common to *everybody* and needed before
 * anything else.
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
 *
 * NOTE: only things that ABSOLUTELY MUST be declared early should be put in
 *       here.  NOTHING ELSE!!!
 */

#ifndef GHEADER_H
#define GHEADER_H

/* a TEMPORARY solution to force the inclusion of GL EXT prototypes */
/* THIS SHOULD BE REMOVED ASAP!!!! */
#define GL_GLEXT_PROTOTYPES 1

/* Defines declared by the configurator */
#include <config.h>

/* preprossesor definitions */
#define Flatland_version_C
#define Flatland_version_D

#ifdef __cplusplus
#	ifndef USE_OLD_CPP_HEADERS
#		include <iostream>
#	else
#		include <iostream.h>
#	endif

#	ifndef USE_OLD_CPP_HEADERS
#		include <sstream>
#	else
#		include <strstream.h>
#		define istringstream istrstream
#	endif
#endif
#include <GL/glut.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include <sys/time.h>

#include <signal.h>
#ifdef ENABLE_SDL
  #include <SDL/SDL.h>
  #include <SDLnet.h>
#endif
#include <string.h>
#include <assert.h>

/* global constants */
const int MAX_NAME_SIZE=100;
/* TODO: make the following configureable somehow, not hardwired */
/* TODO: get the following statement out of this file */
const int MAX_SHADOW_PLANES=1;  /* how many shadow planes -*per object*- */
const GLfloat rad2deg = 57.29578;
const GLfloat deg2rad = 0.0174532;
const GLfloat PI = 3.1415926535897932384;
const GLfloat PI2 = 2.0 * 3.1415926535897932384;
enum { X = 0, Y, Z, W };
enum { A = 0, B, C, D };
typedef enum {TRANSLATE, ROTATE100, ROTATE010, ROTATE001, SCALE, NOOP} XFORM;

/* these are included here to assure the correct ordering */
#ifdef __cplusplus
 #include "genexc.h"
 #include "ctmutex.h"
 #include "mtbuf.h"
 #include "usr_mtbuf.h"
#endif
#include "glutmenu.h"
#include "graph_funcs.h"
#include "TrackerClientAccess.h"
#include "Flatland.h"
#include "graphics_loop.h"
#include "2D_interactors.h"
#include "aux_funcs.h"
#include "read_config.h"
#include "dll_funcs.h"
#include "shadows.h"
#include "flatland_logo.h"
#include "graphics_funcs.h"
#include "dist_funcs.h"
#include "TransformationNodeAccess.h"
#include "sound_thread.h"
#include "sound_client.h"
#include "sound_funcs.h"
#include "intersect.h"
#include "3D_controls.h"
#include "ReadWindowDefinition.h"
#include "flatland_thread.h"
#include "euler.h"
#include "collab_funcs.h"
#include "C_compatability.h"
#include "hash.h"

/* fix for compatability with flatland B */
/* #define current_main_window Loop_Control->Window */

#endif
