/*
 * intersect.h
 *
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
 * Header file for intersect.c
 */

#ifndef INTERSECT_H
#define INTERSECT_H

enum BEAM_STATE { BEAMED, NOTBEAMED, BEAMEDUNDETERMINED } ;
enum COLLISION_STATE { COLLISION, NOTCOLLISION, COLLISIONUNDETERMINED } ;


typedef struct bv_interactions_type {
  BEAM_STATE beam_state ;
  COLLISION_STATE collision_state ;
  struct object *beamer_object ;
  /* the coordiantes of the beamer's p0 and p1 in root frame */
  float beamer_p0[4] ;
  float beamer_p1[4] ;
  /* an array of interaction parameters that may be usefult to the beamed object. */
  float data[16] ;
} BV_INTERACTIONS_TYPE ;


typedef struct intersection_event_database_type {
  struct bv *bv ;
  int numitems ; /* Current number of flatland objects that are actively beaming this object */
  int max_numitems ;
  struct bv_interactions_type **interactions; /* List of objects that are interacting with this BV, and the states */	
} INTERSECTION_EVENT_DATABASE_TYPE ;

INTERSECTION_EVENT_DATABASE_TYPE *fl_make_intersection_event_database( BV_TYPE *bv, int maxitems ) ;
void fl_free_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db ) ;
INTERSECTION_EVENT_DATABASE_TYPE *fl_resize_up_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor ) ;
INTERSECTION_EVENT_DATABASE_TYPE *fl_resize_up_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor ) ;


#endif
