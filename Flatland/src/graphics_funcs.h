/*
 * graphics_funcs.h
 *
 * Header file for graphics_funcs.c
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

#ifndef GRAPHICS_FUNCS_H
#define GRAPHICS_FUNCS_H

/* constants */
const int MAX_FL_LISTS = 1000;  /* max number of flLists that can be defined */

/* function prototypes */
void fl_draw_triangle (float *, float *, float *);
void fl_draw_wire_triangle (float *, float *, float *);
void fl_draw_cylinder (float, float, int);
void fl_draw_circle (float, int);
void fl_draw_cube (float, float, float);
void fl_draw_frame(float, float, float, float, float);
void fl_draw_baseless_pyramid(float, float, float);
void fl_beacon_display (float, float, float);
void fl_draw_beacon(float, float, float);
void fl_draw_beacon_in_mesh(struct rectangular_mesh_type *, int, int);
void fl_draw_mesh(struct rectangular_mesh_type *) ;
GLboolean fl_IsList (GLuint list) ;
GLuint fl_GenLists (GLsizei range, GLuint list) ;
void fl_NewList (GLuint list, GLenum mode) ;
void fl_EndList (void) ;
void fl_DeleteLists (GLuint list, GLsizei range) ;
void fl_CallList (GLuint list) ;
void fl_CallLists (GLsizei n, GLenum type, const GLvoid *lists) ;
void fl_InitLists (void) ;
//struct frustum_type **get_frustum_array (void) ;

#endif
