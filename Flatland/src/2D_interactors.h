/*
 * 2D_interactors.h
 *
 * header for 2D_interactors.cpp
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

#ifndef TWOD_INTERACTORS_H
#define TWOD_INTERACTORS_H

// structures
struct myslider_type {
	char name[MAX_NAME_SIZE] ;
	VERTEX_TYPE box[4] ;
	double box_color[4] ;
	VERTEX_TYPE stop_box[4] ;
	double stop_box_color[4] ;
	VERTEX_TYPE slider[4] ;
	double slider_color[4] ;
	double slider_max,slider_min ;
	int wyold ;
	double colorold[3] ;
	int moused_state ; /* 1 -> mouse was clicked DOWN in this slider, 0 otherwise */
	double slider_position ; /* position of the slider bar relative to the center of the slider frame*/ 
} ;

struct myslider_array_type {
	int nsliders ;
	struct myslider_type **sliders ;
} ;

struct myMCbutton_type {
	char name[MAX_NAME_SIZE] ;
	VERTEX_TYPE box[4] ;
	double box_on_color[4] ;
	double box_off_color[4] ;
	int state ; /* 1 or 0 */
} ;

struct myMCbutton_array_type {
	int nbuttons ;
	struct myMCbutton_type **buttons ;
} ;

struct myTbutton_type {
	char name[MAX_NAME_SIZE] ;
	VERTEX_TYPE box[4] ;
	double box_on_color[4] ;
	double box_off_color[4] ;
	int state ; /* 1 or 0 */
} ;

struct myTbutton_array_type {
	int nbuttons ;
	struct myTbutton_type **buttons ;
} ;

// Function prototypes
struct myslider_array_type *fl_make_my_sliders(int nsliders) ;
void fl_set_sliders_params(struct myslider_array_type *a, int swidth, int sheight, int salley, double xstart, double ystart,double stoph) ;
void fl_set_slider_mouse_state( int wx, int wy, struct myslider_array_type *a, int state ) ;
void fl_slider_picker( int wx, int wy, struct myslider_array_type *my_sliders, int *slider_index, double *value ) ;
void fl_reset_sliders( struct myslider_array_type *a );
void fl_display_sliders( struct myslider_array_type *a ) ;
struct myMCbutton_array_type *fl_make_my_MCbuttons(int nbuttons) ;
void fl_set_MCbutton_params(struct myMCbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart) ;
void fl_MCbutton_up_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index ) ;
void fl_MCbutton_down_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index ) ;
void fl_display_MCbuttons( struct myMCbutton_array_type *a ) ;
struct myTbutton_array_type *fl_make_my_Tbuttons(int nbuttons) ;
void fl_set_Tbutton_params(struct myTbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart) ;
void fl_Tbutton_down_picker( int wx, int wy, struct  myTbutton_array_type *a, int *index ) ;
void fl_display_Tbuttons( struct myTbutton_array_type *a ) ;
void fl_get_subwindow_location (GLsizei new_win_x,
                             GLsizei new_win_y,
                             GLsizei *suggested_x,
                             GLsizei *suggested_y) ;

#endif
