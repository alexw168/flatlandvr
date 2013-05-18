/*
 * shadows.h
 *
 * Header file for shadows.cpp
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

#ifndef SHADOWS_H
#define SHADOWS_H

/* constants */
const GLfloat SHADOW_FACTOR = 4.0;  /* shadow factor used in polygon offset */
const GLfloat SHADOW_OFFSET = 0.5;  /* shadow offset used in polygon offset */

/* globals */
extern Boolean enableTrueShadows;

/* global functions */
int fl_shadow_init(OBJECT_TYPE *o,
                 int plane_nbr,
                 GLfloat sun [3],
                 GLfloat four_points [4] [3]);
void fl_draw_shadow_from_stencil(OBJECT_TYPE *o, int shadow_plane);
void fl_findPlane(GLfloat [], GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, 
               GLfloat, GLfloat, GLfloat, GLfloat);
void fl_shadowMatrix(GLfloat [4][4], GLfloat [], GLfloat []);
void fl_draw_shadow(void display_func(OBJECT_TYPE *),
                 int plane_nbr,
                 OBJECT_TYPE *object,
                 UNIVERSE_TYPE *w);
GLfloat fl_shadow_setup (void);
void fl_shadow_cleanup (void);

#ifdef NO_POFFSET
/* this overrides the glPolygonOffset() in GL.  This is because NVIDIA Linux
 * drivers don't support (in fact crash because of) this call.  It is here
 * because this is the only place (at this time) this function is used in the
 * Flatland kernel */
void glPolygonOffset(GLfloat dummy1, GLfloat dummy2);
#endif

#endif
