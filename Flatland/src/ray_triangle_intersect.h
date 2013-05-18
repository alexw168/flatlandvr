// ray_triangle_intersect.h
// Header for ray-triangle intersection algorithm.
//
// Copyright (C) 2003, University of New Mexico.
// 
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//           
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Takeshi Hakamata <tkh@ahpcc.unm.edu>

#ifndef RAY_TRIANGLE_INTERSECT_H
#define RAY_TRIANGLE_INTERSECT_H

#include "graph_funcs.h"

#define NO_COLLISION '0'
#define COL_INVALID  'X'

extern void fl_bbox_intersection_init(UNIVERSE_TYPE* w);
extern void fl_bbox_intersection_test(OBJECT_TYPE* beamer, OBJECT_TYPE* beamed, BV_TYPE* bv);

#endif //RAY_TRIANGLE_INTERSECT_H
