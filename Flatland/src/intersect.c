/*
intersect.c - Anything to do with object intersections

Copyright (C) 2003, University of New Mexico.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
             
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* 
 * Maintenance history
 *
 * Created: 2-15-00
 * 24-Aug-01, KLS:  fixed bug in resize code (forced use of *new* db)
 * 4-Nov-02, KLS:  Moved 90% of the functionality to an external service.  All
 *                 that remains is the functions to build and free the
 *                 bounding volumes themselves.
 */

#include "gheader.h"

INTERSECTION_EVENT_DATABASE_TYPE *fl_make_intersection_event_database_locked( BV_TYPE *bv, int maxitems )
{
/* Makes a interaction database, and allocates space for it, and initializes
 * pointers and variables.  tpc 12/6/2000 */
  int i,j ;
  INTERSECTION_EVENT_DATABASE_TYPE *db ;
	
  db = (INTERSECTION_EVENT_DATABASE_TYPE *)malloc( sizeof(INTERSECTION_EVENT_DATABASE_TYPE) ) ;
  if( db==NULL ) {
    printf("fl_make_intersection_event_database- ERROR mallocing db.\n") ;
    return(NULL) ;
  }
  db->bv = bv ; /* The bv associated with this data base */
  db->max_numitems = maxitems ;
  db->numitems = 0 ;
  db->interactions = (BV_INTERACTIONS_TYPE **)malloc( maxitems * sizeof( BV_INTERACTIONS_TYPE * ) ) ;
  if( db->interactions == NULL) {
    printf("fl_make_intersection_event_database- ERROR mallocing db->interactions.\n") ;
    exit(0) ;
  }
  for( i=0 ; i<maxitems ; i++ ) {
    db->interactions[i] = (BV_INTERACTIONS_TYPE *)malloc( sizeof( BV_INTERACTIONS_TYPE ) ) ;
    if( db->interactions[i]==NULL ) {
      printf("fl_make_intersection_event_database- ERROR mallocing db->interactions[%d].\n",i) ;
      exit(0) ;
    }
    db->interactions[i]->beamer_object = NULL ;
    db->interactions[i]->beam_state = BEAMEDUNDETERMINED ;   		
    db->interactions[i]->collision_state = COLLISIONUNDETERMINED ;
    for( j=0 ; j<4 ; j++ ) {
      db->interactions[i]->beamer_p0[j] = 0.0 ;
      db->interactions[i]->beamer_p1[j] = 0.0 ;
    }
    for( j=0 ; j<16 ; j++ )
      db->interactions[i]->data[j] = 0.0 ;
  }

  return( db ) ;
}

/* this was made an intermediate function to avoid excessive locking and
 * unlocking of the database (i.e., call this one if it isn't locked already,
 * call the function above if it is) */
INTERSECTION_EVENT_DATABASE_TYPE *fl_make_intersection_event_database( BV_TYPE *bv, int maxitems )
{
  INTERSECTION_EVENT_DATABASE_TYPE *db ;

  /* lock the database */
  pthread_mutex_lock (&(bv->database_mutex));
  db = fl_make_intersection_event_database_locked (bv, maxitems);
  pthread_mutex_unlock (&(bv->database_mutex));

  return (db);
}

void fl_free_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db )
{
  free( db->interactions ) ;
  free( db ) ;
}

INTERSECTION_EVENT_DATABASE_TYPE *fl_resize_up_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor )
{
/* Resizes upwards the interaction databse for a bv */

  INTERSECTION_EVENT_DATABASE_TYPE *newdb ;
  int newmaxitems ;
  int i,j ;
	
  newmaxitems = db->max_numitems * resize_factor ;
  newdb = fl_make_intersection_event_database_locked( db->bv, newmaxitems ) ;
  newdb->numitems = db->numitems ;
	 
  for( i=0 ; i<db->numitems ; i++ ) {
    newdb->interactions[i]->beamer_object = db->interactions[i]->beamer_object  ;
    newdb->interactions[i]->beam_state = db->interactions[i]->beam_state ;   		
    newdb->interactions[i]->collision_state = db->interactions[i]->collision_state  ;
    for( j=0 ; j<4 ; j++ ) {
      newdb->interactions[i]->beamer_p0[j] = db->interactions[i]->beamer_p0[j] ;
      newdb->interactions[i]->beamer_p1[j] = db->interactions[i]->beamer_p1[j] ;
    }
    for( j=0 ; j<16 ; j++ )
      newdb->interactions[i]->data[j] = db->interactions[i]->data[j] ;
  }
  fl_free_intersection_db( db ) ;

  return( newdb ) ;
}

/* this was made an intermediate function to avoid excessive locking and
 * unlocking of the database (i.e., call this one if it isn't locked already,
 * call the function above if it is) */
INTERSECTION_EVENT_DATABASE_TYPE *fl_resize_up_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor )
{
  INTERSECTION_EVENT_DATABASE_TYPE *newdb ;

  pthread_mutex_lock (&(db->bv->database_mutex));
  newdb = fl_resize_up_intersection_db_locked (db, resize_factor);
  pthread_mutex_unlock (&(db->bv->database_mutex));

  return (newdb);
}

