/*
 * 3D_controls.h
 *
 * Header file for 3D_controls.c
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

#ifndef THREED_CONTROLS_H
#define THREED_CONTROLS_H

void fl_CreateButtonBoundingVols (OBJECT_TYPE *Button);
void fl_PreDraw3dButton (OBJECT_TYPE *o);
void fl_Draw3dButton (OBJECT_TYPE *o);
void fl_Label3dButton (OBJECT_TYPE *o, char *labelString);

#endif
