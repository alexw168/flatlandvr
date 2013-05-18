/*
 * world.h
 *
 * Header file for world.cpp
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
#ifndef WORLD_H
#define WORLD_H


typedef struct world_globals
{
  bool display_logo_flag ;
  bool display_runway_flag ;
  bool fill_flag ;
  bool flat_flag ;
  bool display_flag ;
  float earthxmax, earthzmax, earthxmin, earthzmin ;

  GLuint *PlanetList, *PlanetListFlat, *PlanetListTones ;
  rectangular_mesh_type *Earth_mesh, *Earth_mesh_flat ;


} WORLD_GLOBALS;

#endif //ndef WORLD_H
