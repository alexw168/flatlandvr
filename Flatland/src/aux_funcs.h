/*
 * aux_funcs.h
 *
 * header for aux_funcs.c
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

#ifndef AUX_FUNCS_H
#define AUX_FUNCS_H

/* times used by fl_short_sleep */
const int MILLISECOND = 1000;
const int TENTH_SECOND = 100000;
const int QUARTER_SECOND = 250000;
const int HALF_SECOND = 5 * TENTH_SECOND;
const int ONE_SECOND = 2 * HALF_SECOND;

/* globals */

// Structures
struct rectangular_mesh_type {
	int nx ; // x direction
	int ny ; // z direction
	float xmax, xmin, deltax ;
	float ymax, ymin, deltay ;
	float ***mesh ; /* mesh point contains x,y,z,nx,ny,nz int this order, f(x,z)=y is function value */
			/* the last four components are color, r,g,b,a, makeing a total of 10 components */
			/*
				0	x
				1	y
				2	z
				3	nx
				4	ny
				5	nz
				6	red
				7	green
				8	blue
				9	alpha
			*/
} ;

struct maze_type {
	struct rectangular_mesh_type *mesh ;
	int nx ; // x direction
	int ny ; // z direction
	float xmax, xmin, deltax ;
	float ymax, ymin, deltay ;
	unsigned char walllower, wallright, wallupper, wallleft; /* bit masks */
	int **maze ; /* 4 bit coding, 1=> wall, 0=> no wall, ordered ccwise from lower x side */
	/* Below are the bit labels for the sides of a cell, 
		     2
		  ------
		 |	|
		3| cell	|1      x
		 |	|	^
		  ------	|
	 	    0	        + -> z
			
	Example codes:	
		0	0000	no walls
		1	0001	bottom wall only
		2	0010	right wall only
		4	0100	top wall only
		8	1000	left wall only
		
	Note that interior boxes that should be drawn, but not collided with are coded as a "9". */
} ;

// Function prototypes
void fl_short_sleep(long) ;
void fl_draw_stroke_string( char *name, float height ) ;
float fl_stroke_string_width( char *name, float height ) ;
int fl_width_bitmap_string( void *font, char *name );
void fl_draw_bitmap_string( void *font, char *name ) ;
void fl_calnormals( float *p0, float *p1, float *p2, float *n ) ;
struct maze_type *fl_makemaze(int nx, int ny, float xmin, float xmax, float ymin, float ymax ) ;
struct maze_type *fl_readmaze( char *filename ) ;
void fl_printmaze( struct maze_type *m ) ;
struct rectangular_mesh_type *fl_makerecmesh(int nx, int ny, float xmin, float xmax, float ymin, float ymax ) ;
void fl_mesh_normal_randomize( float mag, struct rectangular_mesh_type *m) ;
void fl_mesh_rim_mountains( int rimwidth, float rimheight, float mag, struct rectangular_mesh_type *m) ;
void fl_smooth_mesh_component( struct rectangular_mesh_type *m, int c) ;
void fl_copy_mesh(struct rectangular_mesh_type *from_mesh, struct rectangular_mesh_type *to_mesh ) ;
void fl_delete_mesh(struct rectangular_mesh_type *m ) ;

#endif
