/*
aux_funcs.c.  General purpose auxillary routines.  Useful to everybody.

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
 * Created: 1-17-00
 */

#include "gheader.h"

/* globals */

/* System-independant delay subroutine */
void fl_short_sleep(long usec_sleep)
{
        int i;
        int num_loops;
        int remainder;

        if (usec_sleep >= 1000000)
        {
                num_loops = (usec_sleep / 500000);
                remainder = (usec_sleep % 500000);

                for(i = 0; i < num_loops; i++)
                        usleep(500000);

                usleep(remainder);
        }
        else
                usleep(usec_sleep);

        return;
}

void fl_calnormals( float *p0, float *p1, float *p2, float *n )
{
/* assume dim=3 */

	float norm ;
	float v1[3], v2[3];
	
	v1[0] = p1[0]-p0[0] ;
	v1[1] = p1[1]-p0[1] ;
	v1[2] = p1[2]-p0[2] ;
	v2[0] = p2[0]-p1[0] ;
	v2[1] = p2[1]-p1[1] ;
	v2[2] = p2[2]-p1[2] ;
	
	n[0] = v1[1]*v2[2] - v1[2]*v2[1] ;
	n[1] = -( v1[0]*v2[2] - v1[2]*v2[0] ) ;
	n[2] = v1[0]*v2[1] - v1[1]*v2[0] ;
	norm = (float)sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] ) ;
	if( norm==0.0 ) {
		printf("fl_calnormals- ERROR, norm=0.\n") ;
		exit(0) ;
	}
	n[0] /= norm ;
	n[1] /= norm ;
	n[2] /= norm ;
}

/******************** Runway support routines ********************************/

struct maze_type *fl_makemaze(int nx, int ny, float xmin, float xmax, float ymin, float ymax )
{
	int i,j ;
	struct maze_type *m ;
	
	m = (struct maze_type *)malloc(sizeof(struct maze_type)) ;
	if( m==NULL ) {
		printf("fl_makemaze- ERROR mallocing m.\n") ;
		exit(0) ;
	}
	m->nx = nx ;
	m->ny = ny ;
	m->walllower = 1 ;
	m->wallright = 2 ;
	m->wallupper = 4 ;
	m->wallleft = 8 ;
	m->xmin = xmin ;
	m->xmax = xmax ;
	m->deltax = (xmax-xmin)/(float)nx ;
	m->ymin = ymin ;
	m->ymax = ymax ;
	m->deltay = (ymax-ymin)/(float)ny ;
	
	m->maze = (int **)malloc(nx*sizeof(int *)) ;
	if( m->maze==NULL ) {
		printf("fl_makemaze- ERROR mallocing m->maze.\n") ;
		exit(0) ;
	}
	for( i=0 ; i<nx ; i++ ) {
		m->maze[i] = (int *)malloc(ny*sizeof(int)) ;
		if( m->maze[i]==NULL ) {
			printf("fl_makemaze- ERROR mallocing m->maze[%d].\n",i) ;
			exit(0) ;
		}
		for( j=0 ; j<ny ; j++ )
			m->maze[i][j] = 0 ;
	}
	m->mesh = fl_makerecmesh(nx, ny, xmin, xmax, ymin, ymax ) ;
	return(m) ;
}

struct maze_type *fl_readmaze( char *filename ) 
{
	int i,j ;
	int nx,ny ;
	int value ;
	float xmin,xmax,ymin,ymax ;
	FILE *fpin ;
	struct maze_type *m ;
	
	fpin = fopen(filename, "r" ) ;
	if( fpin==NULL ) {
		printf("read_maze- ERROR opening file: %s\n",filename) ;
		exit(0) ;
	}
	
	fscanf(fpin,"%d %d",&nx,&ny) ;
	fscanf(fpin,"%f %f",&xmin,&xmax) ;
	fscanf(fpin,"%f %f",&ymin,&ymax) ;
	
	m=fl_makemaze( nx,ny,xmin,xmax,ymin,ymax) ;
	
	for( i=0 ; i<nx ; i++ )
		for( j=0 ; j<ny ; j++ ) {
			fscanf(fpin,"%d",&value) ;
			m->maze[i][j] = value ;
		}
		
	fclose(fpin) ;
	
	return( m ) ;
}	
	
void fl_printmaze( struct maze_type *m ) 
{
	int i,j ;
	
	printf("printf_maze- \n") ;
	printf("nx: %d ny: %d\n",m->nx,m->ny) ;
	printf("xmim: %f xmax: %f ymin: %f ymax: %f\n",m->xmin,m->xmax,m->ymin,m->ymax) ;
		
	for( i=0 ; i<m->nx ; i++ ) {
		printf("%3d | ",i) ;
		for( j=0 ; j<m->ny ; j++ )
			printf("%2d ",m->maze[i][j] ) ;
		printf("\n") ;
	}
	printf("\n") ;
}	
	
struct rectangular_mesh_type *fl_makerecmesh(int nx, int ny, float xmin, float xmax, float ymin, float ymax )
{
	int i,j,k ;
	int ncomponents = 10 ;
	struct rectangular_mesh_type *m ;
	
	m = (struct rectangular_mesh_type *)malloc(sizeof(struct rectangular_mesh_type)) ;
	if( m==NULL ) {
		printf("fl_makerecmesh- ERROR mallocing m.\n") ;
		exit(0) ;
	}
	m->nx = nx ;
	m->ny = ny ;
	m->xmin = xmin ;
	m->xmax = xmax ;
	m->deltax = (xmax-xmin)/(float)nx ;
	m->ymin = ymin ;
	m->ymax = ymax ;
	m->deltay = (ymax-ymin)/(float)ny ;
	
	m->mesh = (float ***)malloc(nx*sizeof(float **)) ;
	if( m->mesh==NULL ) {
		printf("fl_makerecmesh- ERROR mallocing m->mesh.\n") ;
		exit(0) ;
	}
	for( i=0 ; i<nx ; i++ ) {
		m->mesh[i] = (float **)malloc(ny*sizeof(float *)) ;
		if( m->mesh[i]==NULL ) {
			printf("fl_makerecmesh- ERROR mallocing m->mesh[%d].\n",i) ;
			exit(0) ;
		}
		for( j=0 ; j<ny ; j++ ) {
			m->mesh[i][j] = (float *)malloc(ncomponents*sizeof(float)) ;
			if( m->mesh[i][j]==NULL ) {
				printf("fl_makerecmesh- ERROR mallocing m->mesh[%d][%d].\n",i,ncomponents) ;
				exit(0) ;
			}
			for( k=0 ; k<ncomponents ; k++ )
				m->mesh[i][j][k] = 0.0 ;
			m->mesh[i][j][4] = 1.0 ; /* set normal to point in y direction */
			m->mesh[i][j][0] = xmin + (float)i*m->deltax ;
			m->mesh[i][j][2] = ymin + (float)j*m->deltay ;
			m->mesh[i][j][9] = 1.0 ; /* color initally to black, but alpha=1 */
		}
	}
	
	return(m) ;
}

void fl_mesh_normal_randomize( float mag, struct rectangular_mesh_type *m)
{
	int i,j ;
	float nx,ny,nz;
	
	for( i=0 ; i<m->nx ; i++ ) {
		for( j=0 ; j<m->ny ; j++ ) {
			nx = fl_distributions_uniform(-mag,mag) ;
			ny = fl_distributions_uniform(-mag,mag) ;
			nz = (float)sqrt( (double)(1.0 - nx*nx - ny*ny) ) ;
			//printf("fl_mesh_normal_randomizei: %3d j: %3d nx: %10.5f ny: %10.5f nz: %10.5f\n",i,j,nx,ny,nz) ;
			m->mesh[i][j][3] = nx ;
			m->mesh[i][j][4] = ny ;
			m->mesh[i][j][5] = nz ;
		}
	}
}

void fl_mesh_rim_mountains( int rimwidth, float rimheight, float mag, struct rectangular_mesh_type *m)
{
/* randonizes normals on a mesh and creates a randon mountain range on the
	parameter (rim) of the rectangular grid */
	
	int i,j ;
	float nx,ny,nz;
	float y, norm ;
	
	norm = (float)(rimwidth*rimwidth) ;
	
	for( i=0 ; i<m->nx ; i++ ) {
		for( j=0 ; j<m->ny ; j++ ) {
			nx = fl_distributions_uniform(-mag,mag) ;
			ny = fl_distributions_uniform(-mag,mag) ;
			nz = (float)sqrt( (double)(1.0 - nx*nx - ny*ny) ) ;
			//printf("fl_mesh_normal_randomizei: %3d j: %3d nx: %10.5f ny: %10.5f nz: %10.5f\n",i,j,nx,ny,nz) ;
			m->mesh[i][j][3] = nx ;
			m->mesh[i][j][4] = ny ;
			m->mesh[i][j][5] = nz ;
			if( i<= rimwidth ) {
				y = rimheight * (float)(rimwidth-i)*(float)(rimwidth-i) * fl_distributions_uniform(0.0,1.0) / norm ;
				m->mesh[i][j][1] = y ;
			}
			if( i>=m->nx-1-rimwidth ) {
				y = rimheight * (float)(rimwidth-m->nx+i+1)*(float)(rimwidth-m->nx+i+1) * fl_distributions_uniform(0.0,1.0) / norm ;
				m->mesh[i][j][1] = y ;
			}
			if( j<= rimwidth ) {
				y = rimheight * (float)(rimwidth-j)*(float)(rimwidth-j) * fl_distributions_uniform(0.0,1.0) / norm ;
				m->mesh[i][j][1] = y ;
			}
			if( j>=m->ny-1-rimwidth ) {
				y = rimheight * (float)(rimwidth-m->ny+j+1)*(float)(rimwidth-m->ny+j+1) *
					 fl_distributions_uniform(0.0,1.0) / norm ;
				m->mesh[i][j][1] = y ;
			}
		}
	}
	fl_smooth_mesh_component( m, 1) ;
	fl_smooth_mesh_component( m, 1) ;
}


void fl_smooth_mesh_component( struct rectangular_mesh_type *m, int c)
{
/* Components are:
	x 0
	y 1
	z 2
	nx 3
	ny 4 
	nz 5 
	red 6
	green 7
	blue 8

Height is y = f(x,z), to match the world/model coordinates 
*/
	int i,j, ii, jj,iis,iie,jjs,jje ;
	float count, sum, mask[3][3]={{1,1,1},{1,1,1},{1,1,1}} ; ;
	struct rectangular_mesh_type *tm ;

	tm = fl_makerecmesh(m->nx, m->ny, m->xmin, m->xmax, m->ymin, m->ymax ) ;
	fl_copy_mesh( m, tm ) ;

	/* convolve with mask */
	for( i=0 ; i<m->nx ; i++ ) {
		/* handle edge effects in i direction */
		iis = 0;
		if( i==0 ) 
			iis = 1 ;
		iie = 2;
		if( i==m->nx-1 ) 
			iie = 1 ;
		for( j=0 ; j<m->ny ; j++ ) {
			sum = 0.0 ;
			count = 0.0 ;
			/* handle edge effects in j direction */
			jjs = 0;
			if( j==0 )
				jjs = 1 ;
			jje = 2;
			if( j==m->ny-1 )
				jje = 1 ;
			for( ii= iis ; ii<=iie ; ii++ ) {
				for( jj= jjs ; jj<=jje ; jj++ ) {
					sum += mask[ii][jj]*m->mesh[i-1+ii][j-1+jj][c] ;
					count += 1.0 ;
				}
			}
			tm->mesh[i][j][c] = sum / count ;
		}
	}
	
	fl_copy_mesh( tm, m ) ;
	fl_delete_mesh( tm ) ;
}

void fl_copy_mesh(struct rectangular_mesh_type *from_mesh, struct rectangular_mesh_type *to_mesh )
{
	/* Assuming each have the same nx,ny only */

	int i,j,k ;
	int ncomponents = 6 ;
	
	if( to_mesh->nx != from_mesh->nx || to_mesh->ny != from_mesh->ny ) {
		printf("fl_copy_mesh- ERROR, dimensions do not match.\n") ;
		exit(0) ;
	}
	to_mesh->xmin = from_mesh->xmin ;
	to_mesh->xmax = from_mesh->xmax ;
	to_mesh->deltax = from_mesh->deltax ;
	to_mesh->ymin = from_mesh->ymin ;
	to_mesh->ymax = from_mesh->ymax ;
	to_mesh->deltay = from_mesh->deltay ;

	for( i=0 ; i<from_mesh->nx ; i++ ) {
		for( j=0 ; j<from_mesh->ny ; j++ ) {
			for( k=0 ; k<ncomponents ; k++ )
				to_mesh->mesh[i][j][k] = from_mesh->mesh[i][j][k] ;
		}
	}
}

void fl_delete_mesh(struct rectangular_mesh_type *m )
{

	int i,j ;
	
	for( i=0 ; i<m->nx ; i++ ) {
		for( j=0 ; j<m->ny ; j++ ) {
			free( m->mesh[i][j] ) ;
		}
		free( m->mesh[i] ) ;
	}
	free( m->mesh ) ;
	free( m ) ;
}

void fl_draw_stroke_string( char *name, float height ) 
{
    // Characters are 119.05 high by 104.76 wide initially
	int nchars, i ;
	float scale ;
	
	nchars = strlen(name) ;
	scale = height/119.05 ;
	
	glPushMatrix() ;
	glScalef(scale,scale,scale) ;
	for( i=0 ; i<nchars ; i++ ) {
		glutStrokeCharacter( GLUT_STROKE_ROMAN,(int)name[i]) ;
	}
	glPopMatrix() ;
}

		
float fl_stroke_string_width( char *name, float height ) 
{
    // Characters are 119.05 high by 104.76 wide initially
	int nchars;
	float width, charwidth;
	
	nchars = strlen(name) ;
	charwidth = 104.76*height/119.05 ;
	width = (float)nchars * charwidth ;
//	printf("fl_stroke_string_width- nchars: %d charwidth: %f width: %f\n",nchars, charwidth, width) ;
	return(width) ;
}

void fl_draw_bitmap_string( void *font, char *name ) 
{
	int nchars, i ;
	
	nchars = strlen(name) ;

	for( i=0 ; i<nchars ; i++ ) {
		glutBitmapCharacter( font,(int)name[i]) ;
	}
}

int fl_width_bitmap_string( void *font, char *name ) 
{
	int nchars, i, w = 0;
	
	nchars = strlen(name) ;

	for( i=0 ; i<nchars ; i++ ) {
		w += glutBitmapWidth( font,(int)name[i]) ;
	}
        return w;
}
