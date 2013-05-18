/*
 * C_compatability.c
 *
 * Copyright (C) 2003, University of New Mexico.
 * 
 * This file provides compatability functions for Flatland C compatability.
 * All of the externalized core functions were renamed for Flatland D.  This
 * file maps the old Flatland C function names to the new Flatland D
 * function names.
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
 * Create: 12-Nov-02, KLS
 *
 */

#include "C_compatability.h"

/* 2D_interactors.h */

struct myslider_array_type *make_my_sliders(int nsliders) 
{
  return (fl_make_my_sliders(nsliders));
}

void set_sliders_params(struct myslider_array_type *a, int swidth, int sheight, int salley, double xstart, double ystart,double stoph)
{
  fl_set_sliders_params(a, swidth, sheight, salley, xstart, ystart, stoph);
}

void set_slider_mouse_state( int wx, int wy, struct myslider_array_type *a, int state )
{
  fl_set_slider_mouse_state( wx, wy, a, state);
}

void slider_picker( int wx, int wy, struct myslider_array_type *my_sliders, int *slider_index, double *value )
{
  fl_slider_picker(wx, wy, my_sliders, slider_index, value);
}

void reset_sliders( struct myslider_array_type *a )
{
  fl_reset_sliders( a );
}

void display_sliders( struct myslider_array_type *a )
{
  fl_display_sliders( a );
}

struct myMCbutton_array_type *make_my_MCbuttons(int nbuttons)
{
  return (fl_make_my_MCbuttons(nbuttons));
}

void set_MCbutton_params(struct myMCbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart)
{
  fl_set_MCbutton_params(a, swidth, sheight, xalley, yalley, xstart, ystart);
}

void MCbutton_up_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index )
{
  fl_MCbutton_up_picker( wx, wy, a, index );
}

void MCbutton_down_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index )
{
  fl_MCbutton_down_picker( wx, wy, a, index );
}

void display_MCbuttons( struct myMCbutton_array_type *a )
{
  fl_display_MCbuttons( a );
}

struct myTbutton_array_type *make_my_Tbuttons(int nbuttons)
{
  return (fl_make_my_Tbuttons(nbuttons));
}

void set_Tbutton_params(struct myTbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart)
{
  fl_set_Tbutton_params(a, swidth, sheight, xalley, yalley, xstart, ystart);
}

void Tbutton_down_picker( int wx, int wy, struct  myTbutton_array_type *a, int *index )
{
  fl_Tbutton_down_picker( wx, wy, a, index );
}

void display_Tbuttons( struct myTbutton_array_type *a )
{
  fl_display_Tbuttons( a );
}

void get_subwindow_location (GLsizei new_win_x,
                                GLsizei new_win_y,
                                GLsizei *suggested_x,
                                GLsizei *suggested_y)
{
  fl_get_subwindow_location (new_win_x,
                             new_win_y,
                             suggested_x,
                             suggested_y);
}

/* 3D_controls.h */

void flCreateButtonBoundingVols (OBJECT_TYPE *Button)
{
  fl_CreateButtonBoundingVols (Button);
}

void flPreDraw3dButton (OBJECT_TYPE *o)
{
  fl_PreDraw3dButton (o);
}

void flDraw3dButton (OBJECT_TYPE *o)
{
  fl_Draw3dButton (o);
}

void flLabel3dButton (OBJECT_TYPE *o, char *labelString)
{
  fl_Label3dButton (o, labelString);
}

/* ReadWindowDefinition.h */

void ReadWindowDefinition( char *filename, Loop_Control_Struct *ctrl )
{
  fl_ReadWindowDefinition( filename, ctrl );
}

/* aux_funcs.h */

void short_sleep(long c1)
{
  fl_short_sleep(c1);
}

void draw_stroke_string( char *name, float height )
{
  fl_draw_stroke_string( name, height );
}

float stroke_string_width( char *name, float height )
{
  return (fl_stroke_string_width( name, height ));
}

int width_bitmap_string( void *font, char *name )
{
  return (fl_width_bitmap_string( font, name ));
}

void draw_bitmap_string( void *font, char *name )
{
  fl_draw_bitmap_string( font, name );
}

void calnormals( float *p0, float *p1, float *p2, float *n )
{
  fl_calnormals( p0, p1, p2, n );
}

struct maze_type *makemaze(int nx, int ny, float xmin, float xmax, float ymin, float ymax )
{
  return (fl_makemaze(nx, ny, xmin, xmax, ymin, ymax ));
}

struct maze_type *readmaze( char *filename )
{
  return (fl_readmaze( filename ));
}

void printmaze( struct maze_type *m )
{
  fl_printmaze( m );
}

struct rectangular_mesh_type *makerecmesh(int nx, int ny, float xmin, float xmax, float ymin, float ymax )
{
  return (fl_makerecmesh( nx, ny, xmin, xmax, ymin, ymax ));
}

void mesh_normal_randomize( float mag, struct rectangular_mesh_type *m)
{
  fl_mesh_normal_randomize( mag, m);
}

void mesh_rim_mountains( int rimwidth, float rimheight, float mag, struct rectangular_mesh_type *m)
{
  fl_mesh_rim_mountains( rimwidth, rimheight, mag, m);
}

void smooth_mesh_component( struct rectangular_mesh_type *m, int c)
{
  fl_smooth_mesh_component( m, c);
}

void copy_mesh(struct rectangular_mesh_type *from_mesh, struct rectangular_mesh_type *to_mesh )
{
  fl_copy_mesh( from_mesh, to_mesh );
}

void delete_mesh(struct rectangular_mesh_type *m )
{
  fl_delete_mesh( m );
}

/* collab_funcs.h */

void collab_send_ctrl_msg(struct object *object, char key)
{
  fl_collab_send_ctrl_msg(object, key);
}

void collab_send_mpo_msg(struct object *object, float *P)
{
  fl_collab_send_mpo_msg(object, P);
}

void collab_send_mtbuf_msg(struct object *object, MTBUF_HDR_TYPE hdr, int size, void *data)
{
  fl_collab_send_mtbuf_msg(object, hdr, size, data);
}

void collab_send_objmsg_msg(struct object *from_object, struct object *to_object, char *message)
{
  fl_collab_send_objmsg_msg(from_object, to_object, message);
}

void collab_send_load_msg(struct apptype *App)
{
  fl_collab_send_load_msg(App);
}

void collab_send_unload_msg(struct apptype *App)
{
  fl_collab_send_unload_msg(App);
}

void collab_controlfunc(struct object *object, char key)
{
  fl_collab_controlfunc(object, key);
}

/* dist_funcs.h */

void distributions_rantest()
{
  fl_distributions_rantest();
}

float distributions_uniform( float a, float b )
{
  return (fl_distributions_uniform( a, b ));
}

void distributions_uniform_nD( int ndim, float *a , float *b, float *x )
{
  fl_distributions_uniform_nD( ndim, a , b, x );
}

int distributions_spirals( int nspirals, float gain, float rmax, float *x )
{
  return (fl_distributions_spirals( nspirals, gain, rmax, x ));
}

float distributions_normal(float mean, float sigma )
{
  return (fl_distributions_normal(mean, sigma ));
}

void distributions_normal_nD( int ndim, float *mean , float *sigma, float *x )
{
  fl_distributions_normal_nD( ndim, mean , sigma, x );
}

float distributions_exponential(float mean)
{
  return (fl_distributions_exponential(mean));
}

float distributions_poisson( float theta )
{
  return (fl_distributions_poisson( theta ));
}

void distributions_spherical_angles(float *theta, float *phi )
{
  fl_distributions_spherical_angles(theta, phi );
}

void spherical_to_cartesian( float theta, float phi, float R, float *x, float *y, float *z )
{
  fl_spherical_to_cartesian( theta, phi, R, x, y, z );
}

/* dll_funcs.h */

int load_all_base_apps (APPLISTTYPE *whoknows)
{
  return (fl_load_all_base_apps (whoknows));
}

int load_app (APPTYPE *App)
{
  return (fl_load_app (App));
}

int unload_app (APPTYPE *App)
{
  return (fl_unload_app (App));
}

/* euler.h */

void MoveMatrix(float *matrix, float distance, char direction, bool inverse)
{
  fl_MoveMatrix(matrix, distance, direction, inverse);
}

void TranslateMatrix(float *matrix,
                            float Xtrans, float Ytrans, float Ztrans,
                            bool inverse)
{
  fl_TranslateMatrix(matrix,
                     Xtrans, Ytrans, Ztrans,
                     inverse);
}

void RotateMatrixR(float *matrix, float new_angle, char angleCoord, bool inverse)
{
  fl_RotateMatrixR(matrix, new_angle, angleCoord, inverse);
}

void RotateMatrixD(float *matrix, float new_angle, char angleCoord, bool inverse)
{
  fl_RotateMatrixD(matrix, new_angle, angleCoord, inverse);
}

void ScaleMatrix(float *matrix, float Xscale, float Yscale, float Zscale, bool inverse)
{
  fl_ScaleMatrix(matrix, Xscale, Yscale, Zscale, inverse);
}

bool RemoveObject(OBJECT_TYPE *object)
{
  return (fl_RemoveObject(object));
}

bool PlaceObject(OBJECT_TYPE *object, OBJECT_TYPE *parent)
{
  return (fl_PlaceObject(object, parent));
}

bool ExtractObject(OBJECT_TYPE *object)
{
  return (fl_ExtractObject(object));
}

bool InsertObject(OBJECT_TYPE *object, OBJECT_TYPE *marker)
{
  return (fl_InsertObject(object, marker));
}

bool ChangeParentAll(OBJECT_TYPE *object, OBJECT_TYPE *target)
{
  return (fl_ChangeParentAll(object, target));
}

bool ChangeParentExtract(OBJECT_TYPE *object, OBJECT_TYPE *target)
{
  return (fl_ChangeParentExtract(object, target));
}

bool ChangeParentAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
{
  return (fl_ChangeParentAllInsert(object, marker));
}

bool ChangeParentExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
{
  return (fl_ChangeParentExtractInsert(object, marker));
}

bool ChangeParentTeleportAll(OBJECT_TYPE *object, OBJECT_TYPE *target)
{
  return (fl_ChangeParentTeleportAll(object, target));
}

bool ChangeParentTeleportExtract(OBJECT_TYPE *object, OBJECT_TYPE *target)
{
  return (fl_ChangeParentTeleportExtract(object, target));
}

bool ChangeParentTeleportAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
{
  return (fl_ChangeParentTeleportAllInsert(object, marker));
}

bool ChangeParentTeleportExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
{
  return (fl_ChangeParentTeleportExtractInsert(object, marker));
}

void GetMatrixTranslations(float *matrix, float &Xtrans, float &Ytrans, float &Ztrans)
{
  fl_GetMatrixTranslations(matrix, Xtrans, Ytrans, Ztrans);
}

void GetMatrixAnglesR(float *matrix, float &nXangle, float &nYangle, float &nZangle)
{
  fl_GetMatrixAnglesR(matrix, nXangle, nYangle, nZangle);
}

void GetMatrixAnglesD(float *matrix, float &nXangle, float &nYangle, float &nZangle)
{
  fl_GetMatrixAnglesD(matrix, nXangle, nYangle, nZangle);
}

void GetMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale)
{
  fl_GetMatrixScale(matrix, Xscale, Yscale, Zscale);
}

void RemoveMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale)
{
  fl_RemoveMatrixScale(matrix, Xscale, Yscale, Zscale);
}

void MatrixMultiply(float *matrix1, float *matrix2)
{
  fl_MatrixMultiply(matrix1, matrix2);
}

void ConverseMatrixMultiply(float *matrix1, float *matrix2)
{
  fl_MatrixMultiply(matrix1, matrix2);
}

void GetInverse(float *in_matrix, float *out_matrix)
{
  fl_GetInverse(in_matrix, out_matrix);
}

void Stabilize(OBJECT_TYPE *object, OBJECT_TYPE *target)
{
  fl_Stabilize(object, target);
}

void Read_Matrix_Ang_W_Scale(float *matrix, float &nXangle, float &nYangle, float &nZangle, float &Xscale, float &Yscale, float &Zscale)
{
  fl_Read_Matrix_Ang_W_Scale(matrix, nXangle, nYangle, nZangle, Xscale, Yscale, Zscale);
}

void Read_Matrix_Ang_W_ScaleD(float *matrix, float &nXangle, float &nYangle, float &nZangle, float &Xscale, float &Yscale, float &Zscale)
{
  fl_Read_Matrix_Ang_W_ScaleD(matrix, nXangle, nYangle, nZangle, Xscale, Yscale, Zscale);
}


/* flatland_logo.h */

void logo_display (void)
{
  fl_logo_display ();
}

void logoinit (void)
{
  fl_logoinit ();
}

void Animate (void)
{
  fl_Animate ();
}

void logo_randomize (void)
{
  fl_logo_randomize ();
}


/* flatland_thread.h */

void flThreadLaunch( THREAD_HANDLE *handle, THREAD_RETURN (*func)(void *), void *data )
{
  fl_ThreadLaunch( handle, func, data );
}

void flThreadWait( THREAD_HANDLE *handle )
{
  fl_ThreadWait( handle );
}

void flThreadWaitAll( )
{
  fl_ThreadWaitAll( );
}


/* glutmenu.h */

/*
void glutMenu (struct universe_type *w)
{
  fl_glutMenu (w);
}
*/

void menu_select (int mode)
{
  fl_menu_select (mode);
}

void load_app_menu_select (int mode)
{
  fl_load_app_menu_select (mode);
}

void unload_app_menu_select (int mode)
{
  fl_load_app_menu_select (mode);
}

/*
int buildSelectObjMenu (struct universe_type *w)
{
  return (fl_buildSelectObjMenu (w));
}
*/

void object_select (int mode)
{
  fl_object_select (mode);
}

void glut_menu_callback0 (int mode)
{
  fl_glut_menu_callback0 (mode);
}

void glut_menu_callback1 (int mode)
{
  fl_glut_menu_callback1 (mode);
}

void glut_menu_callback2 (int mode)
{
  fl_glut_menu_callback2 (mode);
}

void glut_menu_callback3 (int mode)
{
  fl_glut_menu_callback3 (mode);
}

void glut_menu_callback4 (int mode)
{
  fl_glut_menu_callback4 (mode);
}

void glut_menu_callback5 (int mode)
{
  fl_glut_menu_callback5 (mode);
}

void glut_menu_callback6 (int mode)
{
  fl_glut_menu_callback6 (mode);
}

void glut_menu_callback7 (int mode)
{
  fl_glut_menu_callback7 (mode);
}

void glut_menu_callback8 (int mode)
{
  fl_glut_menu_callback8 (mode);
}

void glut_menu_callback9 (int mode)
{
  fl_glut_menu_callback9 (mode);
}

/*
void buildMenuLoadApp (void)
{
  fl_buildMenuLoadApp ();
}

void buildMenuUnloadApp (void)
{
  fl_buildMenuUnloadApp ();
}

void buildModCurrentObj (void)
{
  fl_buildModCurrentObj ();
}
*/


/* graph_funcs.h */

double *makevector(void)
{
  return (fl_makevector());
}

double *makematrix(void)
{
  return (fl_makematrix());
}

OBJECT_TYPE *makeobject(UNIVERSE_TYPE *w,char *name)
{
  return (fl_makeobject(w,name));
}

void delete_object_and_children_from_universe_by_name( UNIVERSE_TYPE *w, char *name )
{
  fl_delete_object_and_children_from_universe_by_name( w, name );
}

void delete_object_and_children_from_universe( UNIVERSE_TYPE *w, OBJECT_TYPE *o )
{
  fl_delete_object_and_children_from_universe( w, o );
}

int delete_objects_by_app_ptr( UNIVERSE_TYPE *w, struct apptype *app )
{
  return (fl_delete_objects_by_app_ptr( w, app ));
}

int delete_objects_by_app_name( UNIVERSE_TYPE *w, char *name )
{
  return (fl_delete_objects_by_app_name( w, name ));
}

void free_object( OBJECT_TYPE *o )
{
  fl_free_object( o );
}

void print_database( UNIVERSE_TYPE *w )
{
  fl_print_database( w );
}

bool test_object_parenthood( OBJECT_TYPE *o, char *parentname )
{
  return (fl_test_object_parenthood( o, parentname ) );
}

OBJECT_TYPE *get_objects_current_parent_ptr( OBJECT_TYPE *o )
{
  return (fl_get_objects_current_parent_ptr( o ));
}

int *get_object_alive_flag_ptr( UNIVERSE_TYPE *w, OBJECT_TYPE *o )
{
  return (fl_get_object_alive_flag_ptr( w, o ));
}

int *get_object_alive_flag_ptr_by_name( UNIVERSE_TYPE *w, char *name )
{
  return (fl_get_object_alive_flag_ptr_by_name( w, name ));
}

int does_object_still_exist( int *object_alive_flag_ptr )
{
  return (fl_does_object_still_exist( object_alive_flag_ptr ));
}

void give_up_on_object_alive_flag_ptr( int *object_alive_flag_ptr )
{
  fl_give_up_on_object_alive_flag_ptr( object_alive_flag_ptr );
}

GRAPH_NODE_TYPE *makegraphnode(OBJECT_TYPE *self)
{
  return (fl_makegraphnode(self));
}

MPO_TYPE *makempotype (UNIVERSE_TYPE *w,OBJECT_TYPE *original_child)
{
  return (fl_makempotype (w,original_child));
}

OBJECT_TYPE *add_child_to_parent(UNIVERSE_TYPE *w,OBJECT_TYPE *o, char *name )
{
  return (fl_add_child_to_parent(w,o, name ));
}

OBJECT_TYPE *add_child_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name)
{
  return (fl_add_child_to_parent_by_name( w, parent_name, new_child_name));
}

OBJECT_TYPE *add_platform_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *platform_name, BV_TYPE *bvol)
{
  return (fl_add_platform_to_parent_by_name( w, parent_name, platform_name, bvol));
}

OBJECT_TYPE *add_child_to_parent_by_name_via_platform( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name, BV_TYPE *bvol)
{
  return (fl_add_child_to_parent_by_name_via_platform( w, parent_name, new_child_name, bvol));
}

BV_TYPE *makebv(void)
{
  return (fl_makebv());
}

BV_SET_TYPE *makebvset( int nbvols )
{
  return (fl_makebvset( nbvols ));
}

void freebvset( BV_SET_TYPE *bvs )
{
  fl_freebvset( bvs );
}

OBJ_CONTROLS_TYPE *makeControls(void)
{
  return (fl_makeControls());
}

OBJ_CONTROLS_ARRAY_TYPE *makeControlsSet( int ncontrols )
{
  return (fl_makeControlsSet( ncontrols ));
}

void freeControlSet( OBJ_CONTROLS_ARRAY_TYPE *cs )
{
  fl_freeControlSet( cs );
}

UNIVERSE_TYPE *makeuniverse(char *name, int maxobjects, int maxxformcodes)
{
  return (fl_makeuniverse(name, maxobjects, maxxformcodes));
}

int database_find_empty_slot( UNIVERSE_TYPE *w)
{
  return (fl_database_find_empty_slot( w));
}

OBJECT_TYPE *find_object_by_name( UNIVERSE_TYPE *w, char *name)
{
  return (fl_find_object_by_name( w, name));
}

void free_object_from_database( UNIVERSE_TYPE *w, OBJECT_TYPE *o )
{
  fl_free_object_from_database( w, o );
}

void save_current_GL_modelview_matrix( float *m )
{
  fl_save_current_GL_modelview_matrix( m );
}

/* appears to have been prototyped but never implemented */
/*
void load_current_GL_modelview_matrix( float *m )
{
  fl_load_current_GL_modelview_matrix( m );
}
*/

void load_GL_modelview_matrix( float *m )
{
  fl_load_GL_modelview_matrix( m );
}

void xform_update( UNIVERSE_TYPE *w )
{
  fl_xform_update( w );
}

void xform_update_recursive( UNIVERSE_TYPE *w, OBJECT_TYPE *current_object )
{
  fl_xform_update_recursive( w, current_object );
}

void update_Mpo_all_objects( UNIVERSE_TYPE *w )
{
  fl_update_Mpo_all_objects( w );
}

void preswapbuffer_all_objects( UNIVERSE_TYPE *w )
{
  fl_preswapbuffer_all_objects( w );
}

void pregraphics_all_objects( UNIVERSE_TYPE *w )
{
  fl_pregraphics_all_objects( w );
}

void pre_draw_all_objects( UNIVERSE_TYPE *w )
{
  fl_pre_draw_all_objects( w );
}

void light_all_objects( UNIVERSE_TYPE *w )
{
  fl_light_all_objects( w );
}

void draw_all_objects( UNIVERSE_TYPE *w )
{
  fl_draw_all_objects( w );
}

void transparent_draw_all_objects( UNIVERSE_TYPE *w )
{
  fl_draw_all_objects( w );
}

void draw_all_bounding_spheres( UNIVERSE_TYPE *w )
{
  fl_draw_all_bounding_spheres( w );
}

void draw_all_bounding_cubes( UNIVERSE_TYPE *w )
{
  fl_draw_all_bounding_cubes( w );
}

void draw_all_bounding_planes(UNIVERSE_TYPE *w )
{
  fl_draw_all_bounding_planes(w );
}

void draw_all_shadows( UNIVERSE_TYPE *w )
{
  fl_draw_all_shadows( w );
}

#ifdef ENABLE_SDL

void sound_all_objects( UNIVERSE_TYPE *w )
{
  fl_sound_all_objects( w );
}

#endif // ENABLE_SDL

int test_object_for_culling( OBJECT_TYPE *o )
{
  return (fl_test_object_for_culling( o ));
}

void tpc_matrix_multiply( float *in4x4, float *in1x4, float *out1x4 )
{
  fl_tpc_matrix_multiply( in4x4, in1x4, out1x4 );
}

void restructure_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot )
{
  fl_restructure_tree( w, newroot );
}

void restructure_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot)
{
  fl_restructure_subtree (oldroot, newroot);
}

void print_all_objects( UNIVERSE_TYPE *w )
{
  fl_print_all_objects( w );
}

void print_gl_matrix( float *x )
{
  fl_print_gl_matrix( x );
}

void eye_coords_in_my_space(UNIVERSE_TYPE *w, OBJECT_TYPE *o, float *point, float *result)
{
  fl_eye_coords_in_my_space(w, o, point, result);
}

void glutMouse_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y )
{
  fl_glutMouse_all_objects( w, button, state, x, y );
}

void glutMotion_all_objects( UNIVERSE_TYPE *w, int x, int y )
{
  fl_glutMotion_all_objects( w, x, y );
}

void glutPassiveMotion_all_objects( UNIVERSE_TYPE *w, int x, int y )
{
  fl_glutPassiveMotion_all_objects( w, x, y );
}

void glutEntry_all_objects( UNIVERSE_TYPE *w, int state )
{
  fl_glutEntry_all_objects( w, state );
}

void glutKeyboard_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y )
{
  fl_glutKeyboard_all_objects( w, key, x, y );
}

void glutSpecial_all_objects( UNIVERSE_TYPE *w, int key, int x, int y )
{
  fl_glutSpecial_all_objects( w, key, x, y );
}

void glutJoystick_all_objects( UNIVERSE_TYPE *w, unsigned int buttonMask, int x, int y, int z )
{
  fl_glutJoystick_all_objects( w, buttonMask, x, y, z );
}

bool flSendMessage( OBJECT_TYPE *from_object, char *to_object_name, char *message )
{
  return (fl_SendMessage( from_object, to_object_name, message ));
}

bool flSendMessage( OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message )
{
  return (fl_SendMessage( from_object, to_object, message ));
}

void kls_3x3_by_1x3_matrix_multiply (float *in3x3, float *in1x3, float *out1x3)
{
  fl_kls_3x3_by_1x3_matrix_multiply (in3x3, in1x3, out1x3);
}

void kls_3x3_matrix_inverse_almost(float in3x3[3][3], float *out3x3, float *det)
{
  fl_kls_3x3_matrix_inverse_almost(in3x3, out3x3, det);
}

void kls_3x3_matrix_inverse (float in3x3[3][3], float *out3x3)
{
  fl_kls_3x3_matrix_inverse (in3x3, out3x3);
}

void root_coords_in_local_space (OBJECT_TYPE *local_object, float *coords)
{
  fl_root_coords_in_local_space (local_object, coords);
}


/* graphics_funcs.h */

void draw_triangle (float *c1, float *c2, float *c3)
{
  fl_draw_triangle (c1,c2,c3);
}

void draw_wire_triangle (float *c1, float *c2, float *c3)
{
  fl_draw_wire_triangle (c1, c2, c3);
}

void draw_cylinder (float c1, float c2, int c3)
{
  fl_draw_cylinder (c1, c2, c3);
}

void draw_circle (float c1, int c2)
{
  fl_draw_circle (c1, c2);
}

void draw_cube (float c1, float c2, float c3)
{
  fl_draw_cube (c1, c2, c3);
}

void draw_frame(float c1, float c2, float c3, float c4, float c5)
{
  fl_draw_frame(c1, c2, c3, c4, c5);
}

void draw_baseless_pyramid(float c1, float c2, float c3)
{
  fl_draw_baseless_pyramid(c1, c2, c3);
}

void beacon_display (float c1, float c2, float c3)
{
  fl_beacon_display (c1, c2, c3);
}

void draw_beacon(float c1, float c2, float c3)
{
  fl_draw_beacon(c1, c2, c3);
}

void draw_beacon_in_mesh(struct rectangular_mesh_type *c1, int c2, int c3)
{
  fl_draw_beacon_in_mesh(c1, c2, c3);
}

void draw_mesh(struct rectangular_mesh_type *c1)
{
  fl_draw_mesh(c1) ;
}

/* all of these appear to be functions that were prototyped, but not
 * implemented */
/*
GLboolean flIsList (GLuint list)
{
  return (fl_IsList (list) );
}

GLuint flGenLists (GLsizei range, GLuint list)
{
  return (fl_GenLists (range, list) );
}

void flNewList (GLuint list, GLenum mode)
{
  fl_NewList (list, mode) ;
}

void flEndList (void)
{
  fl_EndList () ;
}

void flDeleteLists (GLuint list, GLsizei range)
{
  fl_DeleteLists (list, range) ;
}

void flCallList (GLuint list)
{
  fl_CallList (list) ;
}

void flCallLists (GLsizei n, GLenum type, const GLvoid *lists)
{
  fl_CallLists (n, type, lists) ;
}

void flInitLists (void)
{
  fl_InitLists () ;
}
*/


/* graphics_loop.h */

void *GLUT_main( void *c1 )
{
  return (fl_GLUT_main( c1 ));
}

void FlatlandInit( void )
{
  fl_FlatlandInit( );
}

void FlatlandDisplay( void )
{
  fl_FlatlandDisplay( );
}

void FlatlandReshape( int c1, int c2 )
{
  fl_FlatlandReshape( c1, c2 );
}

void FlatlandMouse( int c1, int c2, int c3, int c4 )
{
  fl_FlatlandMouse( c1, c2, c3, c4 );
}

#ifdef Linux
void FlatlandJoystick( unsigned int buttonMask, int x, int y, int z )
{
  fl_FlatlandJoystick( buttonMask, x, y, z );
}
#endif //Linux

void FlatlandMotion( int c1, int c2 )
{
  fl_FlatlandMotion( c1, c2 );
}

void FlatlandPassiveMotion( int c1, int c2 )
{
  fl_FlatlandPassiveMotion( c1, c2 );
}

void FlatlandEntry( int c1 )
{
  fl_FlatlandEntry( c1 );
}

void FlatlandKey( unsigned char key, int x, int y )
{
  fl_FlatlandKey( key, x, y );
}

void FlatlandSpecial( int key, int x, int y )
{
  fl_FlatlandSpecial( key, x, y );
}

void FlatlandIdle( void )
{
  fl_FlatlandIdle( );
}

bool keyscan( OBJ_CONTROLS_ARRAY_TYPE *object_controls, char key )
{
  return (fl_keyscan( object_controls, key ));
}

bool tbe_invert_homogenious_4x4_matrix( float *in, float *out )
{
  return (fl_tbe_invert_homogenious_4x4_matrix( in, out ));
}


/* intersect.h */

INTERSECTION_EVENT_DATABASE_TYPE *make_intersection_event_database( BV_TYPE *bv, int maxitems )
{
  return (fl_make_intersection_event_database( bv, maxitems ) );
}

void free_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db )
{
  fl_free_intersection_db( db ) ;
}

INTERSECTION_EVENT_DATABASE_TYPE *resize_up_intersection_db( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor )
{
  return (fl_resize_up_intersection_db( db, resize_factor ) );
}

INTERSECTION_EVENT_DATABASE_TYPE *resize_up_intersection_db_locked( INTERSECTION_EVENT_DATABASE_TYPE *db, int resize_factor )
{
  return (fl_resize_up_intersection_db_locked( db, resize_factor ) );
}


/* read_config.h */

APPTYPE *makeapp(int type)
{
  return (fl_makeapp(type) );
}

APPLISTTYPE *makeapplist( int maxapps )
{
  return (fl_makeapplist( maxapps ) );
}

APPLISTTYPE *read_config_file( char *file, bool debug_config )
{
  return (fl_read_config_file( file, debug_config ) );
}


/* shadows.h */

int shadow_init(OBJECT_TYPE *o, int plane_nbr, GLfloat sun [3], GLfloat four_points [4] [3])
{
  return (fl_shadow_init(o, plane_nbr, sun, four_points)); 
}

void draw_shadow_from_stencil(OBJECT_TYPE *o, int shadow_plane)
{
  fl_draw_shadow_from_stencil(o, shadow_plane);
}

void findPlane(GLfloat c1[], GLfloat c2, GLfloat c3, GLfloat c4, GLfloat c5, GLfloat c6, GLfloat c7, GLfloat c8, GLfloat c9, GLfloat c10)
{
  fl_findPlane(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10);
}

void shadowMatrix(GLfloat c1 [4][4], GLfloat c2[], GLfloat c3[])
{
  fl_shadowMatrix(c1, c2, c3);
}

/* appears to have been prototyped but not implemented */
/*
void draw_shadow(void display_func(OBJECT_TYPE *), int plane_nbr, OBJECT_TYPE *object, UNIVERSE_TYPE *w)
{
  fl_draw_shadow(display_func, plane_nbr, object, w);
}
*/

GLfloat shadow_setup (void)
{
  return (fl_shadow_setup ());
}

void shadow_cleanup (void)
{
  fl_shadow_cleanup ();
}


/* sound_funcs.h */

  /* all functions were "sl" functions and did not get the "fl_" prefix */

/* sound_thread.h */

#ifdef ENABLE_SDL

void * sound_thread(void *c1)
{
  return (fl_sound_thread(c1));
}

#endif // ENABLE_SDL

/* usr_mtbuf.h */

int make_usrmtbuf(struct object *o, char * data_type, int num_elems)
{
  return (fl_make_usrmtbuf(o, data_type, num_elems));
}

void delete_usrmtbuf(struct object *o, int idx)
{
  fl_delete_usrmtbuf(o, idx);
}

