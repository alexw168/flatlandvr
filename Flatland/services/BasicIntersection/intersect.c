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

/* =======================================================================

Description:
    intersect provides basic beaming and intersection between Flatland
    core defined bounding volume types (at present, just spheres).
    
    Beaming consists of a "beamer" bounding volume which emits a beam from
    it's center point through the surface point.  If the beam intersects with 
    the bounding sphere of a "beamee" who has beaming enabled, the appropriate
    callbacks in the beamee are called, and the bv database in the beamee is
    updated.

    Intersection occurs when an "intersecter" bv sphere impinges on the
    volume of an "intersectee" bv sphere.   The appropriate callbacks in the
    intersectee are called when intersection occurs and when it stops.  Also,
    the bv database of the intersectee is updated.  Note that the intersector
    database and callbacks are not touched.
    
Principal interface elements:

   init_beam_intersect (UNIVERSE_TYPE *w)
     Initializes the intersection library.  Must be called before the first
     beam intersections are done.

*  fl_make_intersection_event_database (BV_TYPE *bv, int maxitems)
     Creates the intersection event database for a bounding volume.

*  fl_free_intersection_db (INTERSECTION_EVENT_DATABASE_TYPE *db)
     Frees an intersection event database created by
     fl_make_intersection_event_database ().

   find_object_in_intersection_db (INTERSECTION_EVENT_DATABASE_TYPE *db,
                                   char *name)
     Finds an object by name in an intersection event database and returns
     true or false, depending on if the object was found.  Useful for checking
     for collisions between two specific objects.

   sphere_interaction_test_all_objects (UNIVERSE_TYPE *w,
                                        OBJECT_TYPE *beamer_object,
                                        int beaming_volume_number,
                                        int beaming_state)
     Usually called in a loop in a dedicated thread by all beamers and
     intersecters.  This function tests for beam and sphere-to-sphere
     intersections between the beamer bv and all other bv's.  

   is_bv_beamed (BV_TYPE *bv)
     Returns true or false depending on if the given bv is being beamed by
     anybody.

   is_bv_collided (BV_TYPE *bv)
     Returns true or false depending on if the given bv is collided with
     anybody.

   is_bv_beamed_by_object (BV_TYPE *bv, char* oname)
     Returns true or false depending on if the given bv is being beamed by one
     of the named object's bv's.

   is_bv_collided_with_object(BV_TYPE *bv, char* oname)
     Returns true or false depending on if the given bv is collided with one
     of the named object's bv's.
    
* These functions are in the Flatland core, but are important to the
  functioning of this service.

Object dependencies:
    Depends on the Flatland core bounding volume structures.  If they are
    changed, this code must be changed.
    
Configurable options:
    None
    
Threads created:
    None

Library requirements:
    Flatland basic setup

Location requirements:
    usually attached to "CenterOfTheUniverse", but placement is irrelevant

Controls:
    Keyboard:
        none
    Menu:
        see keyboard input
    Voice:
        none
    Beaming:
        none

  Maintenance history:
    Created: 2-15-00
    24-Aug-01, KLS:  fixed bug in resize code (forced use of *new* db)
    4-Nov-2002, KLS:  Split from the Flatland core

======================================================================= */

#include "gheader.h"
#include "BasicIntersect.h"

#ifndef USE_OLD_CPP_HEADERS
using namespace std;
#endif

void beam_intersect( UNIVERSE_TYPE *w , OBJECT_TYPE *o)
{

/* Old function, don;t use */

	OBJECT_TYPE *old_root,*obj ;
	float sc[4],ss[4],r2sphere,r2 ; /* Vertices of bounding sphere parameters */
	//int flag ;
	int i ;

			
	/* save old root object */
	old_root = w->root_object ;
	
	/* Restructure tree from object (wand) perspective */
	fl_restructure_tree( w, o ) ;
	
	/* Update all Mpo xforms */
	fl_update_Mpo_all_objects( w ) ;

  	/* Traverse tree to update xform inheritance Mco */
  	fl_xform_update( w ) ;

	/* Turn off beam unless at least beam intersection occurs */
  	o->beam_on_flag = 0 ;  

    	for( i=0 ; i<w->nobjects ; i++ ) {
    		obj = w->database[i].object ;
    		if( obj!=NULL && obj->drawfunc!=NULL && obj->drawing_on_flag==1 && 
		   obj->bounding_volume_set->bvset[1]->npoints >= 2 ) {
  			/* Xform bounding sphere parameters into object frame */
			fl_tpc_matrix_multiply(obj->Mco,obj->bounding_volume_set->bvset[1]->p[0],sc ) ;
			fl_tpc_matrix_multiply(obj->Mco,obj->bounding_volume_set->bvset[1]->p[1],ss ) ;
				
			/* Test if beam intersects the sphere */
			r2sphere =  (sc[0]-ss[0])*(sc[0]-ss[0]) + (sc[1]-ss[1])*(sc[1]-ss[1])+ (sc[2]-ss[2])*(sc[2]-ss[2]) ;
			r2 = sc[0]*sc[0] + sc[1]*sc[1] ;
			if( r2sphere >= r2 ) {
				obj->beam_sphere_hit_flag = 1 ;
				
				if( obj->beamable_flag==1)
					o->beam_on_flag = 1 ;
				if( obj->bounding_volume_set->nbvols>1 ) {
					obj->beam_rect_hit_flag = test_hull_beam_intersection( obj ) ;
				}
			} 
    		}
    	}

  	/* Put tree back */
	fl_restructure_tree( w, old_root ) ;
}

int test_hull_beam_intersection( OBJECT_TYPE *obj ) 
{
	float rv[9][4] ; /* Vertices of rectangular bounding volumn */
	float xmax,xmin,ymax,ymin,zmax,zmin ;
	int i,j,k ;
	
	xmax = -1.0e35 ;
	xmin = 1.0e35 ;
	ymax = -1.0e35 ;
	ymin = 1.0e35 ;
	zmax = -1.0e35 ;
	zmin = 1.0e35 ;
	
	for( k = 0 ; k < obj->bounding_volume_set->nbvols ; k++ )
	{
		if( obj->bounding_volume_set->bvset[k]->npoints==5) 
		{/* Test if a rect-soild is coded */
			/* Transform center and first 4 rect-solid vertices into wand frame */
			for( j=0 ; j<5 ; j++ ) 
			{
				fl_tpc_matrix_multiply(obj->Mco,
					obj->bounding_volume_set->bvset[k]->p[j],
					rv[j] ) ;
				if( rv[j][0]>xmax ) xmax = rv[j][0] ;
				if( rv[j][0]<xmin ) xmin = rv[j][0] ;
				if( rv[j][1]>ymax ) ymax = rv[j][1] ;
				if( rv[j][1]<ymin ) ymin = rv[j][1] ;
				if( rv[j][2]>zmax ) zmax = rv[j][2] ;
				if( rv[j][2]<zmin ) zmin = rv[j][2] ;
			}
			/* Reflect vertices about centroid to get the other four vertices */
			for( j=5 ; j<9 ; j++ ) {
				for( i=0 ; i<3 ; i++ ) {
					rv[j][i] = 2.0*rv[0][i] - rv[j-4][i] ; /* xn = xc - ( xp - xc ) */
				}			
				if( rv[j][0]>xmax ) xmax = rv[j][0] ;
				if( rv[j][0]<xmin ) xmin = rv[j][0] ;
				if( rv[j][1]>ymax ) ymax = rv[j][1] ;
				if( rv[j][1]<ymin ) ymin = rv[j][1] ;	
				if( rv[j][2]>zmax ) zmax = rv[j][2] ;
				if( rv[j][2]<zmin ) zmin = rv[j][2] ;
			}
		}
		if( xmin<=0.0 && xmax>0.0 && ymin<=0.0 && ymax>0.0 && zmin<=0.0 && zmax>0.0) {
			return(1) ;
		}
	}
	return(0) ;
}

void init_beam_intersect (UNIVERSE_TYPE *w)
{
        OBJECT_TYPE *obj;

	for(int i=0 ; i<w->nobjects ; i++ ) 
	{
		obj = w->database[i].object ;
		obj->beam_sphere_hit_flag = 0 ;
		obj->beam_rect_hit_flag = 0 ;
        }
}


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
    exit(1) ;
  }
  for( i=0 ; i<maxitems ; i++ ) {
    db->interactions[i] = (BV_INTERACTIONS_TYPE *)malloc( sizeof( BV_INTERACTIONS_TYPE ) ) ;
    if( db->interactions[i]==NULL ) {
      printf("fl_make_intersection_event_database- ERROR mallocing db->interactions[%d].\n",i) ;
      exit(1) ;
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

int find_object_in_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, char *name )
{
/* This routine searches a BVs databse for an interacetion with object_name name. Returns a 
   index in the database if found and -1 otherwise.  tpc 12/1/2000
*/
  int i ;
	
  for( i=0 ; i<db->numitems ; i++ ) {
    if( strcmp( name, db->interactions[i]->beamer_object->name )==0 )
    {
      return( i ) ;
    }
  }
  return(-1) ;
}

/* this was made an intermediate function to avoid excessive locking and
 * unlocking of the database (i.e., call this one if it isn't locked already,
 * call the function above if it is) */
int find_object_in_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, char *name )
{
  int retval;

  pthread_mutex_lock (&(db->bv->database_mutex));
  retval = find_object_in_intersection_db_locked (db, name);
  pthread_mutex_unlock (&(db->bv->database_mutex));

  return (retval);
}

int find_empty_slot_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE **db_ptr )
{
/* finds an empty slot in the interaction database for a bv.  New ones are
 * always added to the end of the list.  If there is no more room, it
 * automatically resizes.  tpc 12/6/2000 */
  INTERSECTION_EVENT_DATABASE_TYPE *db = *db_ptr;

  if( db->numitems >= db->max_numitems )
  {
    db = fl_resize_up_intersection_db_locked( db, 2 ) ;
    *db_ptr = db;  /* Change the db pointer in the host bv (or whoever) */
  }
  return( db->numitems++ ) ; /* The next empty slot, assuming it fills from
                                 index '0' upwards with no gaps */

}

/* this was made an intermediate function to avoid excessive locking and
 * unlocking of the database (i.e., call this one if it isn't locked already,
 * call the function above if it is) */
int find_empty_slot_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE **db_ptr )
{
  int retval;
  INTERSECTION_EVENT_DATABASE_TYPE *db = *db_ptr;

  pthread_mutex_lock (&(db->bv->database_mutex));
  retval = find_empty_slot_intersection_db_locked (db_ptr);
  pthread_mutex_unlock (&(db->bv->database_mutex));

  return (retval);
}


void minimum_distance_sq_point_to_line( float *p0, float *p1, float *p, float *d2, float *range )
{
/* Computes the minimum distance squared between a point "p" and a line defined by "p0" and "p1".
Assumes line is defined parametrically, using p0 and p1. That is, x = x0 + alpha*(x1-x0), and 
same for y and z. The point is p. Method takes derivative of d2 wrt alpha, sets to zero, solves 
for alpha, calculates nearest point on line to point, then calculates the distance squared.  Alpha
must be positive. This implies that the ray emminates from point p0 towards point p1. A negative 
one return means that the computation either failed or the intersection is behind the ray emmiter.
tpc 11/4/2000
*/
        float dp[3] ;
        float D2 ;
        float pc[3] ;
	float alpha ;
        int i ;
        
        D2 = 0.0 ;
        alpha = 0.0 ;
        for( i=0 ; i<3 ; i++ ) {
                dp[i] = p1[i] - p0[i] ;
                D2 += (dp[i]*dp[i]) ;
                alpha += ( (p[i] - p0[i])*dp[i] ) ;
        }
/* Can't exit.... collision break if we do
        if( alpha<0.0 ) {
		*d2 = -1 ;
		return ;
	}
*/
        if( D2==0.0 ) {
                printf("minimum_distance_sq_point_to_line- ERROR, p0 and p1 must be different points.\n") ;    
		*d2 = -1 ;     
                return ;
        }
        alpha /= D2 ;
	*range = alpha * sqrt( D2 ) ;
        *d2 = 0.0 ;
        for( i=0 ; i<3 ; i++ ) {
                pc[i] = p0[i] + alpha * dp[i] ;
                *d2 += (pc[i] - p[i])*(pc[i] - p[i]) ;
        }
}

void sphere_interaction_test_all_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state )
{
  /* wrapper for backward (non-shadow graph aware) compatability */
  sphere_interaction_test_all_objects( w, beamer_object, beaming_volume_number, beaming_state, 0 );
}

void sphere_interaction_test_all_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state, int shadow_index )
{
  sphere_interaction_test_list_objects( w, beamer_object, beaming_volume_number, beaming_state, shadow_index, w->database, w->nobjects);
}

void sphere_interaction_test_list_objects( UNIVERSE_TYPE *w, OBJECT_TYPE *beamer_object, int beaming_volume_number, int beaming_state, int shadow_index, DATABASE_TYPE *database, int nobjects )
{
/* This routine steps through the Universe database and runs the interaction test functions. THe concept is 
that of a beamer and beamed objects. THe beamer has a had a special bv defined ( bv number 'beaming_volume_number') 
as a sphere, with center and surface points (p0 and p1 in the bv definition).  These two points define the 
beam for the beamer object. The beam starts at p0 and emmits outwards through p1.  This sphere is also used 
to detect when the beamer is colliding with one of the beamed obejct's bvs. This routine transforms p0 and 
p1 into the roots coordinate system then applies the recursive test routine to each object.  Each object has 
the potential of a tree of bvs. Each bv has a dynamic flat database that holds a list of objects that are 
interacting with this bv. Each database entry contains a pointer to the beamer object, states defining if 
it is beamed or not, and collided or not, and parameters of the interactions.  In addition, the bv has four 
call backs that are invoked when a state change occures.  Even though the state may not be changing, the 
paramters of interaction are updated everytime an interaction is detected.  tpc 12/6/2000 */

/* Added the beaming state flag.  If the flag is zero, all beaming
 * interactions fail and all beamed objects will become unbeamed.  Any other
 * value will cause beaming as usual.  This allows the "beam" to be turned on
 * and off under control of the beaming agent (he who calls this function).
 * kls 12/27/00 */

  int i ;
  OBJECT_TYPE *beamed_object ;
  float p0[4],p1[4] ;
        
  if( beamer_object==NULL )
    return ;
  if( beamer_object->bounding_volume_set->nbvols<=beaming_volume_number) 
    return ;

  /* Assume that the interactor_volume BV for the beamer object is a sphere and that the center point
                and the surface point define the beam.  No beaming in "negative" space.
   */

  /* put beamer's points in root frame */
  fl_tpc_matrix_multiply(beamer_object->shadow_Mco [shadow_index],
    beamer_object->bounding_volume_set->bvset[beaming_volume_number]->p[0], p0 ) ;
  fl_tpc_matrix_multiply(beamer_object->shadow_Mco [shadow_index], 
    beamer_object->bounding_volume_set->bvset[beaming_volume_number]->p[1], p1 ) ;

  /* test list of objects in database for sphere interactions with beamer's beam and sphere */
  for( i=0 ; i<nobjects ; i++ ) {
    beamed_object = database[i].object ;
    if(!beamed_object) continue;
    if( strcmp(beamed_object->name,beamer_object->name)!=0) /* Don't beam yourself */
      sphere_interaction_bvset( beamer_object, beamed_object, beamed_object->bounding_volume_set, p0, p1, beaming_state, shadow_index ) ;
  }
}

void sphere_interaction_bvset( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_SET_TYPE *bvs, float *p0, float *p1, int beaming_state )
{
  sphere_interaction_bvset ( beamer_object, beamed_object, bvs, p0, p1, beaming_state, 0 );
}

void sphere_interaction_bvset( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_SET_TYPE *bvs, float *p0, float *p1, int beaming_state, int shadow_index )
{
/* Recursion framework to traverse the children bv for the beamed object.  See sphere_interaction_bv() for more details. */
  int j ;
  BV_TYPE *bv ;

  if( bvs == NULL ) 
    return ;

  for( j=0 ; j<bvs->nbvols ; j++) {
    bv = bvs->bvset[j] ;
    if( bv->beamable_flag==1 )
      sphere_interaction_bv( beamer_object, beamed_object, bv, p0, p1, beaming_state, shadow_index ) ;
    if( bv->children_beamable_flag==1 )
      sphere_interaction_bvset( beamer_object, beamed_object, bv->children_bvset, p0, p1, beaming_state, shadow_index ) ;
  }
}
 
void sphere_test_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object,
                     BV_TYPE *bv, float *p0, float *p1, int beaming_state,
                     bool *beamed, bool *collided, float *data,                   /* <-- outputs */
                     bool check_beaming, bool check_colliding, int shadow_index ) /* <-- inputs  */
{
/* Calculates the geometric interactions between the beamer's ray and sphere,
 * and the particular bv for the interactor object. One type of interaction 
 * is calcualted: beam intersection.
 *
 * The points p0 and p1 define the center 
 * and surface of the beamer's interaction volume, in root coordinates. They
 * also define the origan and direction of the beam.  Points sc and ss are the
 * center and surface points of the interactor object's current bv, also in
 * root coordinates. Note that the bv is assoicated with the beamed_object. 
 *  The other variables are defined as:
 *
 *   r2ss_sc	- the distance squared between the sc and ss points, ie the
 *                radius squared of the interactors bv,
 *   r2p0_sc 	- the distance squared between the p0 and sc points, ie the
 *                distance squared between the centers of the two spheres,
 *   r2p0_p1    - the distance squared between the p0 and p1 points, ie the
 *                radius squred of the beaming bv,
 *   r2		- the square of the nearest distance between the beam and the
 *                point sc,
 *   range	- the distance along the beam from point p0 to the nearest
 *                point between the beam and point sc.
 *   d2         - the distance squared resulting from the addition of the
 *                radius of the two spheres, ie the maximum distance squared
 *                between p0 and sc for a collision.
 *
 * NOTE: the parent function really should lock the bv->dabase_mutex before
 *       calling this function.  It isn't done here.
 *
 * tpc 12/3/2000
 * split into two functions 18/June/2003, kls
 */

/* *data can either be a pointer to at least 4 floats, or NULL, in which case
 * it will be ignored */

/* tucked a little comparison in the second test for beaming state.  if the
 * beaming state is TRUE, beaming occurs as normal.  If beaming_state is FALSE
 * it falls through to the "else if" which should set it to NOT_BEAMED.
 * kls 12/27/2000
 */

/* added more accurate calculations for beaming and collision.  An object will
 * be beamed anytime beaming is active and p0 is inside the object (in
 * addition to all previous cases).  Collisions are now between spheres
 * instead of between p0 and the object bounding sphere.
 * These mods include new var r2po_p1.
 * kls 09/14/2001
 */

/* this function split from sphere_interaction_bv().  Now it's purpose is to
 * make the determination of beaming and collision, but return the results
 * instead of modifying the database directly.  That functionality remains in
 * sphere_interaction_bv().  This was done to allow other collision and
 * beaming systems (cubes, for example) to do a quick check on the bv sphere
 * before doing other, more complicated checks.
 * kls 06/18/2003
 */

  float sc[4],ss[4] ;
  float r2ss_sc,r2p0_sc,r2p0_p1,r2,range,d2 ;

  /* initialize the return flags */
  *beamed = *collided = false;

  /* check to see if we are to do anything at all */
  if (!check_beaming && !check_colliding)
    return;

  /* transform center and surface points of beamed bv into root coordinates.
   * Note that p0 and p1 are already in root coodinates. */
  fl_tpc_matrix_multiply(beamed_object->shadow_Mco [shadow_index],bv->p[0],sc ) ;
  fl_tpc_matrix_multiply(beamed_object->shadow_Mco [shadow_index],bv->p[1],ss ) ;

  /* Calculate the squared distances defined above */
  r2ss_sc = (sc[0]-ss[0])*(sc[0]-ss[0]) + (sc[1]-ss[1])*(sc[1]-ss[1]) + 
		(sc[2]-ss[2])*(sc[2]-ss[2]) ;
  r2p0_sc = (sc[0]-p0[0])*(sc[0]-p0[0]) + (sc[1]-p0[1])*(sc[1]-p0[1]) + 
		(sc[2]-p0[2])*(sc[2]-p0[2]) ;
  r2p0_p1 = (p1[0]-p0[0])*(p1[0]-p0[0]) + (p1[1]-p0[1])*(p1[1]-p0[1]) + 
		(p1[2]-p0[2])*(p1[2]-p0[2]) ;
  
  /* Calculate the min distance from the beamer's beam and the center of the
   * beamed bv sphere. */
  minimum_distance_sq_point_to_line( p0, p1, sc, &r2, &range ) ;
  if( r2<0.0 )
  {
    return ; /* Either the interaction object is behind the beamer, or an
                error occured */
  }

  /* find square of the distance between the spheres */
  d2 = sqrt (r2ss_sc) + sqrt (r2p0_p1);
  d2 *= d2;

  /* First test. Test for sphere/sphere intertersection by comparing point to
   * center to bv radius */
  if( r2p0_sc<=d2 )
  {
    *collided = true;

    /* fill in the appropriate data for a sphere/sphere collision */
    if (data)
    {
      data[0] = r2p0_sc ;
      data[1] = r2ss_sc ;
      data[2] = r2 ;
      data[3] = range ;
    }
  }

  /* Second test. Test if beam from beamer is puncturing the beamed bv sphere */
  if( beaming_state && (r2>=0.0 && r2<=r2ss_sc && range > 0.0 ||
      r2p0_sc < r2ss_sc) ) /* this added for inside beaming  KLS */
  {
    *beamed = true;

    /* fill in the appropriate data for a beam/sphere collision */
    if (data)
    {
      data[0] = r2p0_sc ;
      data[1] = r2ss_sc ;
      data[2] = r2 ;
      data[3] = range ;
    }
  }
}

void sphere_test_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object,
                     BV_TYPE *bv, float *p0, float *p1, int beaming_state,
                     bool *beamed, bool *collided, float *data,/* <-- outputs */
                     bool check_beaming, bool check_colliding) /* <-- inputs  */
{
  sphere_test_bv( beamer_object, beamed_object,
                  bv, p0, p1, beaming_state,
                  beamed, collided, data,
                  check_beaming, check_colliding, 0 );
}

void sphere_interaction_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_TYPE *bv, float *p0, float *p1, int beaming_state )
{
  sphere_interaction_bv( beamer_object, beamed_object, bv, p0, p1, beaming_state, 0 );
}

void sphere_interaction_bv( OBJECT_TYPE *beamer_object, OBJECT_TYPE *beamed_object, BV_TYPE *bv, float *p0, float *p1, int beaming_state, int shadow_index )
{
/* KLS NOTE2: The bv, above, is the beamed bv (not the one running this
 * thread)
 *
 * KLS NOTE:  The calculations have been moved to sphere_test_bv().  Now only
 * the database modification is done here.
 *
 * tpc 12/3/2000
 * split into two functions 18/June/2003, kls
 */

  bool beamed, collided;
  int j ;
  int beamer_index ;
  float data [4];
  BV_INTERACTIONS_TYPE *inter ;

  /* lock the database */
  pthread_mutex_lock (&(bv->database_mutex));

  /* see if a database has been created for this bv */
  if (!bv->database)
  {
    pthread_mutex_unlock (&(bv->database_mutex));
    return; /* the database hasn't bee created... bail */
  }

  /* Check is beamer object already in the beamed objects bv */
  beamer_index = find_object_in_intersection_db_locked(bv->database, beamer_object->name ) ;

  /* execute the tests */
  sphere_test_bv (beamer_object, beamed_object, bv, p0, p1, beaming_state,
                  &beamed, &collided, data, true, true, shadow_index);

  /* First test. Test for sphere/sphere intertersection */
  if( collided )
  {
    if( beamer_index==-1 )
    {
      beamer_index = find_empty_slot_intersection_db_locked( &bv->database ) ;
      bv->database->interactions[beamer_index]->beamer_object = beamer_object ;
    }
    inter = bv->database->interactions[beamer_index] ; /* interaction db ptr */

    /* Update parameters in the database */
    for( j=0 ; j<4 ; j++ ) {
      inter->beamer_p0[j] = p0[j] ;
      inter->beamer_p1[j] = p1[j] ;
      inter->data[j] = data[j];
    }
    /* Test if a change in state has occured */
    if(inter->collision_state != COLLISION ) {
      inter->collision_state = COLLISION ;
      if( bv->bvcollisionfunc != NULL )
        (*bv->bvcollisionfunc)(bv, inter ) ;
    }
  }
  else if( beamer_index != -1 &&
           bv->database->interactions[beamer_index]->collision_state==COLLISION)
  {
    bv->database->interactions[beamer_index]->collision_state = NOTCOLLISION ;
    if( bv->bvuncollisionfunc != NULL )
      (*bv->bvuncollisionfunc)(bv, bv->database->interactions[beamer_index] ) ;
  }

  /* Second test. Test if beam from beamer is puncturing the beamed bv sphere */
  if( beamed )
  {
    if( beamer_index == -1 )
    {
      beamer_index = find_empty_slot_intersection_db_locked( &bv->database ) ;
      bv->database->interactions[beamer_index]->beamer_object = beamer_object ;
    }
    inter = bv->database->interactions[beamer_index] ; /* interaction db ptr */

    /* Update parameters in the databse */
    for( j=0 ; j<4 ; j++ )
    {
      inter->beamer_p0[j] = p0[j] ;
      inter->beamer_p1[j] = p1[j] ;
      inter->data[j] = data[j];
    }

    /* Test if a change in state has occured */
    if( inter->beam_state != BEAMED ) {    
      inter->beam_state = BEAMED ;
      if( bv->bvbeamedfunc != NULL )
        (*bv->bvbeamedfunc)(bv, inter ) ;
    }
  }
  else if( beamer_index != -1 &&
           bv->database->interactions[beamer_index]->beam_state == BEAMED )
  {
    bv->database->interactions[beamer_index]->beam_state = NOTBEAMED ;
    if( bv->bvunbeamedfunc != NULL )
      (*bv->bvunbeamedfunc)(bv, bv->database->interactions[beamer_index] ) ;
  }
  
  /* unlock the database */
  pthread_mutex_unlock (&(bv->database_mutex));
}

bool is_bv_beamed (BV_TYPE *bv)
{
  int i;

  pthread_mutex_lock (&(bv->database_mutex));

  for (i = 0; i < bv->database->numitems; i++)
    if (bv->database->interactions[i]->beam_state == BEAMED)
    {
      pthread_mutex_unlock (&(bv->database_mutex));
      return true;
    }

  pthread_mutex_unlock (&(bv->database_mutex));
  return false;
}

bool is_bv_collided (BV_TYPE *bv)
{
  int i;

  pthread_mutex_lock (&(bv->database_mutex));

  for (i = 0; i < bv->database->numitems; i++)
    if (bv->database->interactions[i]->collision_state == COLLISION)
    {
      pthread_mutex_unlock (&(bv->database_mutex));
      return true;
    }

  pthread_mutex_unlock (&(bv->database_mutex));
  return false;
}

bool is_bv_beamed_by_object(BV_TYPE *bv, char* oname)
{
  int i;

  pthread_mutex_lock (&(bv->database_mutex));

  for (i = 0; i < bv->database->numitems; i++)
    /* KLS: changed following line to what I *think* it should be */
//    if (bv->database->interactions[i]->collision_state == BEAMED &&
    if (bv->database->interactions[i]->beam_state == BEAMED &&
        strcmp(bv->database->interactions[i]->beamer_object->name,oname) == 0 )
    {
      pthread_mutex_unlock (&(bv->database_mutex));
      return true;
    }

  pthread_mutex_unlock (&(bv->database_mutex));
  return false;
}

bool is_bv_collided_with_object(BV_TYPE *bv, char* oname)
{
  int i;

  pthread_mutex_lock (&(bv->database_mutex));

  for (i = 0; i < bv->database->numitems; i++)
//if (!strcmp(bv->database->interactions[i]->beamer_object->name,"Tomarfakeear"))
    if (bv->database->interactions[i]->collision_state == COLLISION &&
        strcmp(bv->database->interactions[i]->beamer_object->name,oname) == 0 )
    {
      pthread_mutex_unlock (&(bv->database_mutex));
      return true;
    }

  pthread_mutex_unlock (&(bv->database_mutex));
  return false;
}

void pre_draw (OBJECT_TYPE *o)
{
  init_beam_intersect (o->universe);
}

extern "C"
{
/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
  float PTemp[9];               /* temporary transform storage */
  OBJECT_TYPE *Intersect;       /* This object */

  /* parse the parameter list */
//  for (int i=0; i < App->nparams; i++)
//  {
//  }

  /* initialize the object for the Simple Object */
  /* make new object, and attach with a platform */
  Intersect = fl_add_child_to_parent_by_name(App->universe_ptr,
                                          App->parent_object_name,
                                          App->appname);

  /* add the app name to the object, so it can be unloaded */
  /* removed:  we do not want this service unloaded */
//  Intersect->creator_app_ptr = App;

  /* set up the bounding volumes (position & culling) */
  Intersect->bounding_volume_set = fl_makebvset(2);

  /* set up the callbacks */
  Intersect->predrawfunc = pre_draw;

  /* attach the cube */
  PTemp[0] = 0.0;                                        /* x */
  PTemp[1] = 0.0;                                        /* y */
  PTemp[2] = 0.0;                                        /* z */ 
  PTemp[3] = 0.0;                                        /* pitch */
  PTemp[4] = 0.0;                                        /* yaw */
  PTemp[5] = 0.0;                                        /* roll */
  PTemp[6] = 1.0;                                        /* scale x */
  PTemp[7] = 1.0;                                        /* scale y */
  PTemp[8] = 1.0;                                        /* scale z */

  Intersect->orginal_first_node->MpoParams->P->assign( PTemp );
	
  Intersect->orginal_first_node->MpoParams->Po[0] = NOOP;
}
} // extern "C"
