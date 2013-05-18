/*
 * sound_funcs.h
 *
 * Header file for sound_funcs.c
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

#ifndef SOUND_FUNCS_H
#define SOUND_FUNCS_H

/* globals */
extern int MAX_SOUND_LISTS; 
extern GLfloat current_sound_matrix[16];

/* function prototypes */
void slInit (void);
void slLoadMatrix (GLfloat mtx[16]);
void slHeadPosition (GLfloat pos[]);
GLuint slGenLists (GLsizei range);
GLboolean slIsList (GLuint list);
void slDeleteLists (GLuint list, GLsizei range);
void slNewList (GLuint list, char *file);
void slCallList (GLuint list);
void slVolumeList (GLuint list, float vol);
void slAttenuationList (GLuint list, float vol);
void slLoopList (GLuint list, int loop);
void slPositionList (GLuint list, GLfloat pos[4]);
void slPriorityList (GLuint list, float pri);
void slStopList (GLuint list);
void slRewindList (GLuint list);
void slUpdatePositions (void);

#endif
