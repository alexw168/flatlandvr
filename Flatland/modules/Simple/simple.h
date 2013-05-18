/*
 * simple.h
 *
 * Include file for simple.c
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

typedef struct simple_globals
{
  bool data_ready;      /* Has the data been read in? */
  char data_file [MAX_NAME_SIZE]; /* Name of the input data file */
  GLfloat face_colors [6] [3] [4];  /* RGBA diffuse, ambient, and specular 
                                       propertiesof each face of the cube */
  bool sounds_opened;   /* Flag indicating if sounds have been opened */
  int snd_list;         /* id number of the sound associated w/ this object */
} SIMPLE_GLOBALS;
