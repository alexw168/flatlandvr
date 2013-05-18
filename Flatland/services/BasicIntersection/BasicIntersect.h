/*
 * BasicIntersect.h
 *
 * Header file for the BasicIntersect service.

 * Copyright (C) 2003, University of New Mexico.

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

 */

#ifndef BASICINTERSECT_H
#define BASICINTERSECT_H

void beam_intersect( UNIVERSE_TYPE *w , OBJECT_TYPE *o);
int test_hull_beam_intersection( OBJECT_TYPE *obj );
void init_beam_intersect (UNIVERSE_TYPE *w);

int find_object_in_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, char *name ) ;
int find_object_in_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, char *name );
int find_empty_slot_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db ) ;
int find_empty_slot_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE **db ) ;

void minimum_distance_sq_point_to_line( float *p0, float *p1, float *p, float *d2, float *range ) ;
void sphere_interaction_test_all_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state ) ;
void sphere_interaction_test_list_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state, int shadow_index, DATABASE_TYPE *database, int nobjects );
void sphere_interaction_bvset( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_SET_TYPE *bvs, float *p0, float *p1, int beaming_state ) ;
void sphere_interaction_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_TYPE *bv, float *p0, float *p1, int beaming_state ) ;
void sphere_interaction_test_all_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state, int shadow_index) ;
void sphere_interaction_bvset( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_SET_TYPE *bvs, float *p0, float *p1, int beaming_stat, int shadow_index ) ;
void sphere_interaction_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_TYPE *bv, float *p0, float *p1, int beaming_stat, int shadow_index ) ;
bool is_bv_beamed (BV_TYPE *bv) ;
bool is_bv_beamed (BV_TYPE *bv) ;
bool is_bv_collided (BV_TYPE *bv) ;
bool is_bv_beamed_by_object(BV_TYPE *bv, char* oname) ;
bool is_bv_collided_with_object(BV_TYPE *bv, char* oname) ;

#endif
