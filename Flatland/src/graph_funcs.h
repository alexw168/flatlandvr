/*
 * graph_funcs.h
 *
 * header for graph_funcs.cpp
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

#ifndef GRAPH_FUNCS_H
#define GRAPH_FUNCS_H

#include "hash.h"

typedef struct obj_controls_type {
  char menuname [MAX_NAME_SIZE];
  char voicename [MAX_NAME_SIZE];
  char keystroke;
  int mode;
  void (*control_function)(struct object *, char);
  struct obj_controls_array_type *children;
} OBJ_CONTROLS_TYPE;

typedef struct obj_controls_array_type {
  int ncontrols;
  OBJ_CONTROLS_TYPE **controls;
} OBJ_CONTROLS_ARRAY_TYPE;

#define xform_dir_flag shadow_xform_dir_flag[0]
#define xform_dir_flag_old shadow_xform_dir_flag_old[0]
#define root_object shadow_root_object[0]

typedef struct mpotype {
	struct object *original_child;
  //	int xform_dir_flag; /* 1 => orginal direction at creation time, -1 => reverse */
  int *shadow_xform_dir_flag;
  int *shadow_xform_dir_flag_old;

  mt_buf *P;	
  
  //	float *P;
  /* set of nine conical xform parameters (100 = x axis, 010 = y axis, etc)
				 		P[0] 100 translation, |
				 		P[1] 010 translation, | Code 0
				 		P[2] 001 translation, | 
				 		P[3] 100 rotation, | Code 1
				 		P[4] 010 rotation, | Code 2 > all angular degrees
				 		P[5] 001 rotation, | Code 3
				 		P[6] 100 scale, |
				 		P[7] 010 scale, | Code 4
				 		P[8] 001 scale. |
				 		
				 		Noop  | Code 5
				*/
	float *Pold; /* used for change detection */
  //	int xform_dir_flag_old; /* used for change detection */

	XFORM *Po; /* Set of up to universe->maxxformcodes codes that tells the tree traversal xform update routine
					the order to apply the effects of the xform parameters. Maxxformcodes should be >=5.
					
					If maxxformcodes = 10...
					
					Example: {0,1,2,3,4,5,5,5,5,5} performs a translation with parameters P[0-2],
					then performs three rotations in the order 1,2,3 ( about the x,y,z axis)
					using  parameters P[4-6], and finally scales with parameters P[7-9].
					
					If maxxformcodes = 5...
					
					Example: {2,5,5,5,5} performs a rotation about the 010 axis with 
					parameters P[4].
					
					This allows the user to code a transform sequence for each object.  This
					information is used at tree update time to produce the Mpo,the object to
					parent matrix, from the indenity matrix .
				*/	
	float *Mpo;  /*Transform from object coords to parent coods. */
  float **shadow_Mpo;
} MPO_TYPE;

typedef struct graphnode {
	struct object *self; /* self pointer to object the owns this node  */
	unsigned char parent_flag; /* 1=> this node points to the current parent object for this object */
	struct graphnode *neighbor_node; /* pointer to the neighbor node in the neighbor object in the graph */
	struct graphnode *right_sibling_node;/* link list pointer to node that points to the next node */
	struct graphnode *left_sibling_node;/* link list pointer to node that points to the previous node */
	struct mpotype *MpoParams; /* Pointer to shared matrix xform struct, that a) determines who is 
					the parent of the two objects sharing this, and b) the parameters of the 
					relative xform, used to calculate Mpo in the correct direction */
} GRAPH_NODE_TYPE;

typedef struct bv {
        /* npoints=type: 1=point, 2=sphere, 3=line, 4=rect. plane, 5=rect. solid, >=6 not allowed.
        	- Point: the point itself,
                - Sphere: first point is the center, second point is any point on surface,
                - Line: first point middle of line, next two points are line segment end points,
                - Rectangular Plane: First point center,next three points define the three vervices of the
                        corners of the plane in model corrodinates. 
                - Rectangular Solid- :  The first point is the cube centroid, while
                        the last four define corners of the soild volume
        */

  int npoints ; /* Number of points that are used to define the actual form of the bv.  The number 
                   of points is used to tell the system the type of bv form */
  float **p ;/* the array of npoints points [x,y,z,1] (transpose) that save the vertices of the bounding volumes */ 
  int beamable_flag ; /* true if this bv is beamable or collidable */
  int children_beamable_flag ; /* true if the children are beamable or collidable */
  void *data;

  /* Call backs for interactions */
  void (*bvbeamedfunc)(struct bv *bv, struct bv_interactions_type *inter) ;	
  void (*bvunbeamedfunc)(struct bv *bv, struct bv_interactions_type *inter) ;	
  void (*bvcollisionfunc)(struct bv *bv, struct bv_interactions_type *inter) ;	
  void (*bvuncollisionfunc)(struct bv *bv, struct bv_interactions_type *inter) ;
  struct object *parent_object ; /* points to the object that owns this bv, user set */
  struct intersection_event_database_type *database ;	/* INteraction databse for this bv */
  pthread_mutex_t database_mutex;         /* able to lock the database */
  struct bvset_type *children_bvset ; /* Set of children bvs */
} BV_TYPE ;


typedef struct bvset_type {
  int nbvols ;
  struct bv **bvset ;
} BV_SET_TYPE ;


typedef struct object {

	int *object_alive_flag;  /* Memory location of flag used by other 
						objects to test for the existance of this object.
						An integral part of object deletion and app unload.
						Value: greater than or equil to one->this object exists, 
						       less that zero->this object deleted. 
						The absolute value of this flag = number of other objects
						trying to use data or functions from the belly of this object. Initialized
						to one.  When value reached zero, then this flag my be deleted. tpc 9/19/2000

							flag = 1, when created, indicating that it is used by itself.
							flag = 2,3,4... as objects get the flag to use internal data or functions
							flat = negative (ie -4, -3, -2 ...) when it has been deleted.
							|flag| decreases as objects state that they are no longer 
								going to use the objects internals, thus releasing.
							flag = 0 when the object has been deleted, and everyone has released their 
								thier use of its internals. => it will be freed.

					    */
	char name[MAX_NAME_SIZE];
        struct universe_type *universe;
	struct apptype *creator_app_ptr;
        long int *sim_time_ptr;
        int *current_time_ptr;
	int nneighbors;
	int drawing_on_flag; /* if true, draw the object */
	int drawing_on_old_flag; /* if true, draw the object */
	int sounding_on_flag; /* if true, sonify the object */
	int viewable_flag; /* culling, if true, turn on drawing of polygons */
	int beam_sphere_hit_flag; /* true if beam intersects with bounding sphere volume */
	int beam_rect_hit_flag; /* true if beam intersects with bounding rect-solid volume */
	int beam_on_flag; /* true turns on a laser beam emmiting in the neg z direction if intersecting
				object is "beamable" */
	int beamable_flag; /* is another object is beam intersecting with this object, should the other
				object turn its "laser beam" on */
	int object_state0; /* State variables for general use */
	int object_state1;
	int object_state2;
	int object_state3;
	int object_state4;
	int object_state5;
	int object_state6;
	int object_state7;

	float object_param0; /* General object parameters */
	float object_param1;
	float object_param2;
	float object_param3;

        void *ptr0; /* General purpose pointers */
        void *ptr1;
        void *ptr2;
        void *ptr3;

        /* shadow production variables */
        bool shadows_on_flag[MAX_SHADOW_PLANES];/*draw shadows for this plane?*/
        GLfloat shadowPlanePts[MAX_SHADOW_PLANES][4][4];/* 4 pts make a plane */
        GLfloat shadowLightSrc[MAX_SHADOW_PLANES][4]; /* lght src coordinates */
        GLfloat shadowMat[MAX_SHADOW_PLANES][4][4]; /* shadow proj matrix */

	void (*transparentdrawfunc)(struct object *);           /* transparent draw callback */
	void (*preswapbufferfunc)(struct object *);        /* callback, called just before each swap buffer */
	void (*pregraphicsfunc)(struct object *);        /* once per frame callback */
	void (*predrawfunc)(struct object *);        /* before draw callback */
        void (*lightingfunc)(struct object *);       /* lighting callback */
	void (*drawfunc)(struct object *);           /* main draw callback */
        void (*trueShadowFunc)(struct object *);     /* true shadow callbk */
        void (*cheapShadowFunc)(struct object *);    /* cheap shadow callbk*/
	void (*soundfunc)(struct object *);          /* sound callback     */
        void (*platformfunc)(struct object *);       /* platform handler   */
	void (*predeletefunc)(struct object *);      /* is called prior to the deletion of the object. 
							 Is responsible for shutting down any threads associated
							 with this object and freeing user allocated memory. 
						         This func does not free the object itself.          
						      */
	void (*recievemessagefunc)( struct object *to, struct object *from, char *message );        /* message transfer callback */

	void (*glutMousefunc)(struct object *, int button, int state, int x, int y );        /* recieve glut mouse messages */
	void (*glutMotionfunc)(struct object *, int x, int y );        /* recieve glut mouse messages */
	void (*glutPassiveMotionfunc)(struct object *, int x, int y );        /* recieve glut mouse messages */
	void (*glutEntryfunc)(struct object *, int state );        /* recieve glut mouse messages */
	void (*glutKeyboardfunc)(struct object *, unsigned char key, int x, int y );        /* recieve glut keyboard messages */
	void (*glutKeyboardUpfunc)(struct object *, unsigned char key, int x, int y );        /* recieve glut keyboard up messages */
	void (*glutSpecialfunc)(struct object *, int key, int x, int y );        /* recieve glut special keyboard messages */
	void (*glutSpecialUpfunc)(struct object *, int key, int x, int y );        /* recieve glut special keyboard (up) messages */
        void (*glutSpaceballMotionfunc)(struct object *, int x, int y, int z );          /* receive glut Spaceball motion messages */
        void (*glutSpaceballRotatefunc)(struct object *, int x, int y, int z );        /* receive glut Spaceball rotation messages */
        void (*glutSpaceballButtonfunc)(struct object *, int button, int state );        /* receive glut Spaceball button messages */
        void (*glutTabletMotionfunc)(struct object *, int x, int y );                   /* receive glut Tablet motion messages */
        void (*glutTabletButtonfunc)(struct object *, int button, int state, int x, int y );
	void (*glutJoystickfunc)( struct object *, unsigned int buttonMask, int x, int y, int z );

	BV_SET_TYPE *bounding_volume_set; /* used for culling and selection */

        OBJ_CONTROLS_ARRAY_TYPE *object_controls; /* control interface specs */
	
  	GRAPH_NODE_TYPE *first_node; /* always contains pointer to parent object */
        GRAPH_NODE_TYPE **shadow_first_node;  

	GRAPH_NODE_TYPE *orginal_first_node;
	float *Mco; /* Transform from object coords to camera coords. */

  float **shadow_Mco;

        bool menu_entry_on_flag;  /* if true, add object to glut menu (default = true) */
        usr_mtbuf *object_mtbuf;  /* General-purpose user mutexed buffer */

        /* collaboration flags */
        bool shared_flag;         /* true if object is shared (default = false) */
        bool owner_flag;          /* true if object is owned by local user (default = true) */
        bool original_owner_flag; /* true if object is created (originally owned) by local user (default = true) */

} OBJECT_TYPE;

typedef struct {
	char name[MAX_NAME_SIZE];
	OBJECT_TYPE *object;
} DATABASE_TYPE;

typedef struct universe_type{
	char name[MAX_NAME_SIZE];
	int nobjects;
	int maxobjects;
	int maxxformcodes;
        HASH_TABLE *HashTable;
//	OBJECT_TYPE *root_object;
	DATABASE_TYPE *database;
        // shadow graph globals					//
        // default is 1, if there is no other graphs than the big cheese graph
        int num_shadows;
        bool *shadow_list;
        OBJECT_TYPE **shadow_root_object;
} UNIVERSE_TYPE;

// Function prototypes
double *fl_makevector(void);
double *fl_makematrix(void); 
OBJECT_TYPE *fl_makeobject(UNIVERSE_TYPE *w,char *name);
void fl_delete_object_and_children_from_universe_by_name( UNIVERSE_TYPE *w, char *name );
void fl_delete_object_and_children_from_universe( UNIVERSE_TYPE *w, OBJECT_TYPE *o );
int fl_delete_objects_by_app_ptr( UNIVERSE_TYPE *w, struct apptype *app );
int fl_delete_objects_by_app_name( UNIVERSE_TYPE *w, char *name );
void fl_free_object( OBJECT_TYPE *o );
void fl_print_database( UNIVERSE_TYPE *w );
bool fl_test_object_parenthood( OBJECT_TYPE *o, char *parentname ) ;
OBJECT_TYPE *fl_get_objects_current_parent_ptr( OBJECT_TYPE *o );
OBJECT_TYPE *fl_get_shadow_objects_current_parent_ptr( OBJECT_TYPE *o, int shadow_number );
int *fl_get_object_alive_flag_ptr( UNIVERSE_TYPE *w, OBJECT_TYPE *o );
int *fl_get_object_alive_flag_ptr_by_name( UNIVERSE_TYPE *w, char *name );
int fl_does_object_still_exist( int *object_alive_flag_ptr );
void fl_give_up_on_object_alive_flag_ptr( int *object_alive_flag_ptr ); 
GRAPH_NODE_TYPE *fl_makegraphnode(OBJECT_TYPE *self);
MPO_TYPE *fl_makempotype (UNIVERSE_TYPE *w,OBJECT_TYPE *original_child);
OBJECT_TYPE *fl_add_child_to_parent(UNIVERSE_TYPE *w,OBJECT_TYPE *o, char *name );
OBJECT_TYPE *fl_add_child_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name);
OBJECT_TYPE *fl_add_platform_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *platform_name, BV_TYPE *bvol);
OBJECT_TYPE *fl_add_child_to_parent_by_name_via_platform( UNIVERSE_TYPE *w, char
    *parent_name, char *new_child_name, BV_TYPE *bvol);

BV_TYPE *fl_makebv(void);
BV_SET_TYPE *fl_makebvset( int nbvols );
void fl_freebvset( BV_SET_TYPE *bvs );

OBJ_CONTROLS_TYPE *fl_makeControls(void);
OBJ_CONTROLS_ARRAY_TYPE *fl_makeControlsSet( int ncontrols );
void fl_freeControlSet( OBJ_CONTROLS_ARRAY_TYPE *cs );

UNIVERSE_TYPE *fl_makeuniverse(char *name, int maxobjects, int maxxformcodes);
int fl_database_find_empty_slot( UNIVERSE_TYPE *w);
OBJECT_TYPE *fl_find_object_by_name( UNIVERSE_TYPE *w, char *name);
void fl_free_object_from_database( UNIVERSE_TYPE *w, OBJECT_TYPE *o );

void fl_save_current_GL_modelview_matrix( float *m );
void fl_load_current_GL_modelview_matrix( float *m );
void fl_load_GL_modelview_matrix( float *m );
void fl_xform_update( UNIVERSE_TYPE *w );
void fl_shadow_xform_update( UNIVERSE_TYPE *w, int shadow_number );
void fl_xform_update_recursive( UNIVERSE_TYPE *w, OBJECT_TYPE *current_object );
void fl_shadow_xform_update_recursive( UNIVERSE_TYPE *w, OBJECT_TYPE *current_object, int shadow_number );
void fl_update_Mpo_all_objects( UNIVERSE_TYPE *w );
void fl_update_shadow_Mpo_all_objects( UNIVERSE_TYPE *w, int shadow_number );
void fl_preswapbuffer_all_objects( UNIVERSE_TYPE *w );
void fl_pregraphics_all_objects( UNIVERSE_TYPE *w );
void fl_pre_draw_all_objects( UNIVERSE_TYPE *w );
void fl_light_all_objects( UNIVERSE_TYPE *w );
void fl_draw_all_objects( UNIVERSE_TYPE *w );
void transparent_fl_draw_all_objects( UNIVERSE_TYPE *w );
void fl_draw_all_bounding_spheres( UNIVERSE_TYPE *w );
void fl_draw_all_bounding_cubes( UNIVERSE_TYPE *w );
void fl_draw_all_bounding_planes(UNIVERSE_TYPE *w );
void fl_draw_all_shadows( UNIVERSE_TYPE *w );
void fl_sound_all_objects( UNIVERSE_TYPE *w );
int fl_test_object_for_culling( OBJECT_TYPE *o );
void fl_tpc_matrix_multiply( float *in4x4, float *in1x4, float *out1x4 );
void fl_loadIdentity (float *T);
void fl_matrix_copy (float *in4x4, float *out4x4);
void fl_4x4_matrix_multiply (float *in1, float *in2, float *out);
float *fl_translatef (float *in4x4, float x, float y, float z);
float *fl_rotatef001 (float *in4x4, float q);
float *fl_rotatef010 (float *in4x4, float q);
float *fl_rotatef100 (float *in4x4, float q);
float *fl_scalef (float *in4x4, float x, float y, float z);
void fl_restructure_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot );
void fl_restructure_shadow_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot, int shadow_number );
void fl_restructure_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot );
void fl_restructure_shadow_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot, int shadow_number );
void fl_print_all_objects( UNIVERSE_TYPE *w );
void fl_print_all_shadow_objects( UNIVERSE_TYPE *w, int shadow_number );
void fl_print_gl_matrix( float *x );
void fl_eye_coords_in_my_space(UNIVERSE_TYPE *w, OBJECT_TYPE *o, float *point,
                      float *result);

void fl_glutMouse_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y );
void fl_glutMotion_all_objects( UNIVERSE_TYPE *w, int x, int y );
void fl_glutPassiveMotion_all_objects( UNIVERSE_TYPE *w, int x, int y );
void fl_glutEntry_all_objects( UNIVERSE_TYPE *w, int state );
void fl_glutKeyboard_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y );
void fl_glutKeyboardUp_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y );
void fl_glutSpecial_all_objects( UNIVERSE_TYPE *w, int key, int x, int y );
void fl_glutSpecialUp_all_objects( UNIVERSE_TYPE *w, int key, int x, int y );
void fl_glutSpaceballMotion_all_objects( UNIVERSE_TYPE *w, int x, int y, int z );
void fl_glutSpaceballRotate_all_objects( UNIVERSE_TYPE *w, int x, int y, int z );
void fl_glutSpaceballButton_all_objects( UNIVERSE_TYPE *w, int button, int state );
void fl_glutTabletMotion_all_objects( UNIVERSE_TYPE *w, int x, int y );
void fl_glutTabletButton_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y );
void fl_glutJoystick_all_objects( UNIVERSE_TYPE *w, unsigned int buttonMask, int x, int y, int z );

bool fl_SendMessage( OBJECT_TYPE *from_object, char *to_object_name, char *message );
bool fl_SendMessage( OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message );

void fl_kls_3x3_by_1x3_matrix_multiply (float *in3x3, float *in1x3, float *out1x3);
void fl_kls_3x3_matrix_inverse_almost(float in3x3[3][3], float *out3x3, float *det);
void fl_kls_3x3_matrix_inverse (float in3x3[3][3], float *out3x3);
void fl_root_coords_in_local_space (OBJECT_TYPE *local_object, float *coords);
int fl_get_shadow_graph(UNIVERSE_TYPE * w);
void fl_release_shadow_graph(UNIVERSE_TYPE * w, int shadow_graph_num);

#endif
