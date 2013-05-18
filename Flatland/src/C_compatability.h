/*
 * C_compatability.h
 *
 * Header file for C_compatability.c.  Provides the appropriate function names
 * for Flatland C version modules to link to.
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
 *
 * Create: 12-Nov-02, KLS
 *
 */

#ifndef __C_COMPATABILITY_H
#define __C_COMPATABILITY_H

#include "gheader.h"


/* 2D_interactors.h */

struct myslider_array_type *make_my_sliders(int nsliders);

void set_sliders_params(struct myslider_array_type *a, int swidth, int sheight, int salley, double xstart, double ystart,double stoph);

void set_slider_mouse_state( int wx, int wy, struct myslider_array_type *a, int state );

void slider_picker( int wx, int wy, struct myslider_array_type *my_sliders, int *slider_index, double *value );

void reset_sliders( struct myslider_array_type *a );

void display_sliders( struct myslider_array_type *a );

struct myMCbutton_array_type *make_my_MCbuttons(int nbuttons);

void set_MCbutton_params(struct myMCbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart);

void MCbutton_up_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index );

void MCbutton_down_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index );

void display_MCbuttons( struct myMCbutton_array_type *a );

struct myTbutton_array_type *make_my_Tbuttons(int nbuttons);

void set_Tbutton_params(struct myTbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart);

void Tbutton_down_picker( int wx, int wy, struct  myTbutton_array_type *a, int *index );

void display_Tbuttons( struct myTbutton_array_type *a );

void get_subwindow_location (GLsizei new_win_x,
                             GLsizei new_win_y,
                             GLsizei *suggested_x,
                             GLsizei *suggested_y);


/* 3D_controls.h */

void flCreateButtonBoundingVols (OBJECT_TYPE *Button);

void flPreDraw3dButton (OBJECT_TYPE *o);

void flDraw3dButton (OBJECT_TYPE *o);

void flLabel3dButton (OBJECT_TYPE *o, char *labelString);


/* ReadWindowDefinition.h */

void ReadWindowDefinition( char *filename, Loop_Control_Struct *ctrl );


/* aux_funcs.h */

void short_sleep(long c1);

void draw_stroke_string( char *name, float height );

float stroke_string_width( char *name, float height );

int width_bitmap_string( void *font, char *name );

void draw_bitmap_string( void *font, char *name );

void calnormals( float *p0, float *p1, float *p2, float *n );

struct maze_type *makemaze(int nx, int ny, float xmin, float xmax, float ymin, float ymax );

struct maze_type *readmaze( char *filename );

void printmaze( struct maze_type *m );

struct rectangular_mesh_type *makerecmesh(int nx, int ny, float xmin, float xmax, float ymin, float ymax );

void mesh_normal_randomize( float mag, struct rectangular_mesh_type *m);

void mesh_rim_mountains( int rimwidth, float rimheight, float mag, struct rectangular_mesh_type *m);

void smooth_mesh_component( struct rectangular_mesh_type *m, int c);

void copy_mesh(struct rectangular_mesh_type *from_mesh, struct rectangular_mesh_type *to_mesh );

void delete_mesh(struct rectangular_mesh_type *m );


/* collab_funcs.h */

void collab_send_ctrl_msg(struct object *object, char key);

void collab_send_mpo_msg(struct object *object, float *P);

void collab_send_mtbuf_msg(struct object *object, MTBUF_HDR_TYPE hdr, int size, void *data);

void collab_send_objmsg_msg(struct object *from_object, struct object *to_object, char *message);

void collab_send_load_msg(struct apptype *App);

void collab_send_unload_msg(struct apptype *App);

void collab_controlfunc(struct object *object, char key);


/* dist_funcs.h */

void distributions_rantest();

float distributions_uniform( float a, float b );

void distributions_uniform_nD( int ndim, float *a , float *b, float *x );

int distributions_spirals( int nspirals, float gain, float rmax, float *x );

float distributions_normal(float mean, float sigma );

void distributions_normal_nD( int ndim, float *mean , float *sigma, float *x );

float distributions_exponential(float mean);

float distributions_poisson( float theta );

void distributions_spherical_angles(float *theta, float *phi );

void spherical_to_cartesian( float theta, float phi, float R, float *x, float *y, float *z );


/* dll_funcs.h */

int load_all_base_apps (APPLISTTYPE *whoknows);

int load_app (APPTYPE *App);

int unload_app (APPTYPE *App);


/* euler.h */

void MoveMatrix(float *matrix, float distance, char direction, bool inverse);

void TranslateMatrix(float *matrix,
                     float Xtrans, float Ytrans, float Ztrans,
                     bool inverse);

void RotateMatrixR(float *matrix, float new_angle, char angleCoord, bool inverse);

void RotateMatrixD(float *matrix, float new_angle, char angleCoord, bool inverse);

void ScaleMatrix(float *matrix, float Xscale, float Yscale, float Zscale, bool inverse);

bool RemoveObject(OBJECT_TYPE *object);

bool PlaceObject(OBJECT_TYPE *object, OBJECT_TYPE *parent);

bool ExtractObject(OBJECT_TYPE *object);

bool InsertObject(OBJECT_TYPE *object, OBJECT_TYPE *marker);

bool ChangeParentAll(OBJECT_TYPE *object, OBJECT_TYPE *target);

bool ChangeParentExtract(OBJECT_TYPE *object, OBJECT_TYPE *target);

bool ChangeParentAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);

bool ChangeParentExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);

bool ChangeParentTeleportAll(OBJECT_TYPE *object, OBJECT_TYPE *target);

bool ChangeParentTeleportExtract(OBJECT_TYPE *object, OBJECT_TYPE *target);

bool ChangeParentTeleportAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);

bool ChangeParentTeleportExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);

void GetMatrixTranslations(float *matrix, float &Xtrans, float &Ytrans, float &Ztrans);

void GetMatrixAnglesR(float *matrix, float &nXangle, float &nYangle, float &nZangle);

void GetMatrixAnglesD(float *matrix, float &nXangle, float &nYangle, float &nZangle);

void GetMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale);

void RemoveMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale);

void MatrixMultiply(float *matrix1, float *matrix2);

void ConverseMatrixMultiply(float *matrix1, float *matrix2);

void GetInverse(float *in_matrix, float *out_matrix);

void Stabilize(OBJECT_TYPE *object, OBJECT_TYPE *target);

void read_matrix_ang_w_scale(float *matrix, float &nxangle, float &nyangle, float &nzangle, float &xscale, float &yscale, float &zscale);

void Read_Matrix_Ang_W_ScaleD(float *matrix, float &nXangle, float &nYangle, float &nZangle, float &Xscale, float &Yscale, float &Zscale);


/* flatland_logo.h */

void logo_display (void);

void logoinit (void);

void Animate (void);

void logo_randomize (void);


/* flatland_thread.h */

void flThreadLaunch( THREAD_HANDLE *handle, THREAD_RETURN (*func)(void *), void *data );

void flThreadWait( THREAD_HANDLE *handle );

void flThreadWaitAll( );


/* glutmenu.h */

//void glutMenu (struct universe_type *w);

void menu_select (int mode);

void load_app_menu_select (int mode);

void unload_app_menu_select (int mode);

//int buildSelectObjMenu (struct universe_type *w);

void object_select (int mode);

void glut_menu_callback0 (int mode);

void glut_menu_callback1 (int mode);

void glut_menu_callback2 (int mode);

void glut_menu_callback3 (int mode);

void glut_menu_callback4 (int mode);

void glut_menu_callback5 (int mode);

void glut_menu_callback6 (int mode);

void glut_menu_callback7 (int mode);

void glut_menu_callback8 (int mode);

void glut_menu_callback9 (int mode);

//void buildMenuLoadApp (void);

//void buildMenuUnloadApp (void);

//void buildModCurrentObj (void);


/* graph_funcs.h */

double *makevector(void);

double *makematrix(void);

OBJECT_TYPE *makeobject(UNIVERSE_TYPE *w,char *name);

void delete_object_and_children_from_universe_by_name( UNIVERSE_TYPE *w, char *name );

void delete_object_and_children_from_universe( UNIVERSE_TYPE *w, OBJECT_TYPE *o );

void delete_objects_by_app( UNIVERSE_TYPE *w, struct apptype *app );

int delete_objects_by_app_ptr( UNIVERSE_TYPE *w, struct apptype *app );

int delete_objects_by_app_name( UNIVERSE_TYPE *w, char *name );

void free_object( OBJECT_TYPE *o );

void print_database( UNIVERSE_TYPE *w );

bool test_object_parenthood( OBJECT_TYPE *o, char *parentname );

OBJECT_TYPE *get_objects_current_parent_ptr( OBJECT_TYPE *o );

int *get_object_alive_flag_ptr( UNIVERSE_TYPE *w, OBJECT_TYPE *o );

int *get_object_alive_flag_ptr_by_name( UNIVERSE_TYPE *w, char *name );

int does_object_still_exist( int *object_alive_flag_ptr );

void give_up_on_object_alive_flag_ptr( int *object_alive_flag_ptr );

GRAPH_NODE_TYPE *makegraphnode(OBJECT_TYPE *self);

MPO_TYPE *makempotype (UNIVERSE_TYPE *w,OBJECT_TYPE *original_child);

OBJECT_TYPE *add_child_to_parent(UNIVERSE_TYPE *w,OBJECT_TYPE *o, char *name );

OBJECT_TYPE *add_child_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name);

OBJECT_TYPE *add_platform_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *platform_name, BV_TYPE *bvol);

OBJECT_TYPE *add_child_to_parent_by_name_via_platform( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name, BV_TYPE *bvol);

BV_TYPE *makebv(void);

BV_SET_TYPE *makebvset( int nbvols );

void freebvset( BV_SET_TYPE *bvs );

OBJ_CONTROLS_TYPE *makeControls(void);

OBJ_CONTROLS_ARRAY_TYPE *makeControlsSet( int ncontrols );

void freeControlSet( OBJ_CONTROLS_ARRAY_TYPE *cs );

UNIVERSE_TYPE *makeuniverse(char *name, int maxobjects, int maxxformcodes);

int database_find_empty_slot( UNIVERSE_TYPE *w);

OBJECT_TYPE *find_object_by_name( UNIVERSE_TYPE *w, char *name);

void free_object_from_database( UNIVERSE_TYPE *w, OBJECT_TYPE *o );

void save_current_GL_modelview_matrix( float *m );

void load_current_GL_modelview_matrix( float *m );

void load_GL_modelview_matrix( float *m );

void xform_update( UNIVERSE_TYPE *w );

void xform_update_recursive( UNIVERSE_TYPE *w, OBJECT_TYPE *current_object );

void update_Mpo_all_objects( UNIVERSE_TYPE *w );

void preswapbuffer_all_objects( UNIVERSE_TYPE *w );

void pregraphics_all_objects( UNIVERSE_TYPE *w );

void pre_draw_all_objects( UNIVERSE_TYPE *w );

void light_all_objects( UNIVERSE_TYPE *w );

void draw_all_objects( UNIVERSE_TYPE *w );

void transparent_draw_all_objects( UNIVERSE_TYPE *w );

void draw_all_bounding_spheres( UNIVERSE_TYPE *w );

void draw_all_bounding_cubes( UNIVERSE_TYPE *w );

void draw_all_bounding_planes(UNIVERSE_TYPE *w );

void draw_all_shadows( UNIVERSE_TYPE *w );

void sound_all_objects( UNIVERSE_TYPE *w );

int test_object_for_culling( OBJECT_TYPE *o );

void tpc_matrix_multiply( float *in4x4, float *in1x4, float *out1x4 );

void restructure_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot );

void restructure_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot);

void print_all_objects( UNIVERSE_TYPE *w );

void print_gl_matrix( float *x );

void eye_coords_in_my_space(UNIVERSE_TYPE *w, OBJECT_TYPE *o, float *point, float *result);

void glutMouse_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y );

void glutMotion_all_objects( UNIVERSE_TYPE *w, int x, int y );

void glutPassiveMotion_all_objects( UNIVERSE_TYPE *w, int x, int y );

void glutEntry_all_objects( UNIVERSE_TYPE *w, int state );

void glutKeyboard_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y );

void glutSpecial_all_objects( UNIVERSE_TYPE *w, int key, int x, int y );

void glutJoystick_all_objects( UNIVERSE_TYPE *w, unsigned int buttonMask, int x, int y, int z );

bool flSendMessage( OBJECT_TYPE *from_object, char *to_object_name, char *message );

bool flSendMessage( OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message );

void kls_3x3_by_1x3_matrix_multiply (float *in3x3, float *in1x3, float *out1x3);

void kls_3x3_matrix_inverse_almost(float in3x3[3][3], float *out3x3, float *det);

void kls_3x3_matrix_inverse (float in3x3[3][3], float *out3x3);

void root_coords_in_local_space (OBJECT_TYPE *local_object, float *coords);


/* graphics_funcs.h */

void draw_triangle (float *c1, float *c2, float *c3);

void draw_wire_triangle (float *c1, float *c2, float *c3);

void draw_cylinder (float c1, float c2, int c3);

void draw_circle (float c1, int c2);

void draw_cube (float c1, float c2, float c3);

void draw_frame(float c1, float c2, float c3, float c4, float c5);

void draw_baseless_pyramid(float c1, float c2, float c3);

void beacon_display (float c1, float c2, float c3);

void draw_beacon(float c1, float c2, float c3);

void draw_beacon_in_mesh(struct rectangular_mesh_type *c1, int c2, int c3);

void draw_mesh(struct rectangular_mesh_type *c1);

GLboolean flIsList (GLuint list);

GLuint flGenLists (GLsizei range, GLuint list);

void flNewList (GLuint list, GLenum mode);

void flEndList (void);

void flDeleteLists (GLuint list, GLsizei range);

void flCallList (GLuint list);

void flCallLists (GLsizei n, GLenum type, const GLvoid *lists);

void flInitLists (void);


/* graphics_loop.h */

void *GLUT_main( void *c1 );

void FlatlandInit( void );

void FlatlandDisplay( void );

void FlatlandReshape( int c1, int c2 );

void FlatlandMouse( int c1, int c2, int c3, int c4 );

void FlatlandJoystick( unsigned int buttonMask, int x, int y, int z );

void FlatlandMotion( int c1, int c2 );

void FlatlandPassiveMotion( int c1, int c2 );

void FlatlandEntry( int c1 );

void FlatlandKey( unsigned char key, int x, int y );

void FlatlandSpecial( int key, int x, int y );

void FlatlandIdle( void );

bool keyscan( OBJ_CONTROLS_ARRAY_TYPE *object_controls, char key );

bool tbe_invert_homogenious_4x4_matrix( float *in, float *out );


/* intersect.h */

INTERSECTION_EVENT_DATABASE_TYPE *make_intersection_event_database( BV_TYPE *bv, int maxitems );

void free_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db );

INTERSECTION_EVENT_DATABASE_TYPE *resize_up_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor );

INTERSECTION_EVENT_DATABASE_TYPE *resize_up_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor );


/* read_config.h */

APPTYPE *makeapp(int type);

APPLISTTYPE *makeapplist( int maxapps );

APPLISTTYPE *read_config_file( char *file );


/* shadows.h */

int shadow_init(OBJECT_TYPE *o, int plane_nbr, GLfloat sun [3], GLfloat four_points [4] [3]);

void draw_shadow_from_stencil(OBJECT_TYPE *o, int shadow_plane);

void findPlane(GLfloat c1[], GLfloat c2, GLfloat c3, GLfloat c4, GLfloat c5, GLfloat c6, GLfloat c7, GLfloat c8, GLfloat c9, GLfloat c10);

void shadowMatrix(GLfloat c1 [4][4], GLfloat c2[], GLfloat c3[]);

void draw_shadow(void display_func(OBJECT_TYPE *), int plane_nbr, OBJECT_TYPE *object, UNIVERSE_TYPE *w);

GLfloat shadow_setup (void);

void shadow_cleanup (void);


/* sound_funcs.h */

  /* all functions were "sl" functions and did not get the "fl_" prefix */


/* sound_thread.h */

void * sound_thread(void *c1);


/* usr_mtbuf.h */

int make_usrmtbuf(struct object *o, char * data_type, int num_elems);

void delete_usrmtbuf(struct object *o, int idx);

#endif
