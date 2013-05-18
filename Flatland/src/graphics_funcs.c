/*
graphics_funcs.c.  Various graphics functions deemed handy to many functions

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

void fl_draw_triangle( float *p0, float *p1, float *p2 ) 
{
        float n[3] ;

    	fl_calnormals(p0,p1,p2,n) ;

        glBegin( GL_TRIANGLES ) ;
                glNormal3fv(n) ;                
                glVertex3fv( p0 ) ;
                glVertex3fv( p1 ) ;
                glVertex3fv( p2 ) ;
        glEnd() ;
        glBegin( GL_LINES ) ;
                glVertex3fv( p0 ) ;
                glVertex3fv( p1 ) ;
                glVertex3fv( p2 ) ;
        glEnd() ;
}

void fl_draw_wire_triangle( float *p0, float *p1, float *p2 ) 
{
        glBegin( GL_LINE_LOOP ) ;
                glVertex3fv( p0 ) ;
                glVertex3fv( p1 ) ;
                glVertex3fv( p2 ) ;
        glEnd() ;
}

void fl_draw_cylinder( float R, float h, int nfacets )
{
/* base of cylinder in x-y plane, axis along the positive z axis */
	float dtheta, theta ;
	float x0,y0,x1,y1 ;
	int i ;
	
	theta = 0.0 ;
	dtheta = PI2 / (float)nfacets ;
        /* kls moved two calculations out of the loop to half calcs/iteration */
	y0 = R*cos(theta) ;
	x0 = R*sin(theta) ;
	for( i=0 ; i<nfacets ; i++ ) {
		theta+=dtheta ;
		y1 = R*cos(theta) ;
		x1 = R*sin(theta) ;
		glBegin(GL_QUADS) ;
			glNormal3f(x0/R,y0/R,0.0) ;
			glVertex3f(x0,y0,0.0) ;
			glNormal3f(x1/R,y1/R,0.0) ;
			glVertex3f(x1,y1,0.0) ;
			glNormal3f(x1/R,y1/R,0.0) ;
			glVertex3f(x1,y1,h) ;
			glNormal3f(x0/R,y0/R,0.0) ;
			glVertex3f(x0,y0,h) ;
		glEnd() ;
		glBegin(GL_TRIANGLES) ;
			glNormal3f(0.0,0.0,-1.0) ;
			glVertex3f(x0,y0,0.0) ;
			glVertex3f(x1,y1,0.0) ;
			glVertex3f(0.0,0.0,0.0) ;
			glNormal3f(0.0,0.0,1.0) ;
			glVertex3f(x0,y0,h) ;
			glVertex3f(x1,y1,h) ;
			glVertex3f(0.0,0.0,h) ;
		glEnd() ;
                /* kls added to cut calcs/iteration */
                y0 = y1;
                x0 = x1;
	}
}		

void fl_draw_circle( float R, int nfacets )
{
/* base of circle in x-y plane */
	float dtheta, theta ;
	int i ;
	
	theta = 0.0 ;
	dtheta = PI2 / (float)nfacets ;
        glBegin(GL_POLYGON);
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(R*sin(theta), R*cos(theta),  0.0); /* first vertex */
	        for( i=0 ; i<nfacets ; i++ ) {
                        theta+=dtheta;
                        glVertex3f(R*sin(theta), R*cos(theta), 0.0);
	        }
        glEnd();
}		

void fl_draw_cube( float xsize, float ysize, float zsize )
{
	/* Centered on origin */
	float x2,y2,z2 ;
	
	x2 = 0.5*xsize ;
	y2 = 0.5*ysize ;
	z2 = 0.5*zsize ;

	// Draw two x faces 
	glBegin(GL_QUADS ) ;
		glNormal3f(1.0,0.0,0.0) ;
		glVertex3f( x2, y2, z2) ;
		glVertex3f( x2,-y2, z2) ;
		glVertex3f( x2,-y2,-z2) ;
		glVertex3f( x2, y2,-z2) ;
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(-1.0,0.0,0.0) ;
		glVertex3f(-x2, y2, z2) ;
		glVertex3f(-x2, y2,-z2) ;
		glVertex3f(-x2,-y2,-z2) ;
		glVertex3f(-x2,-y2, z2) ;
	glEnd() ;
	
	// Draw two y faces 
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( x2, y2, z2) ;
		glVertex3f( x2, y2,-z2) ;
		glVertex3f(-x2, y2,-z2) ;
		glVertex3f(-x2, y2, z2) ;
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,-1.0,0.0) ;
		glVertex3f( x2,-y2, z2) ;
		glVertex3f(-x2,-y2, z2) ;
		glVertex3f(-x2,-y2,-z2) ;
		glVertex3f( x2,-y2,-z2) ;
	glEnd() ;
	
	// Draw two z faces 
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x2, y2, z2) ;
		glVertex3f(-x2, y2, z2) ;
		glVertex3f(-x2,-y2, z2) ;
		glVertex3f( x2,-y2, z2) ;
	glEnd() ;
	glBegin(GL_QUADS);
		glNormal3f(0.0,0.0,-1.0) ;
		glVertex3f( x2, y2,-z2) ;
		glVertex3f( x2,-y2,-z2) ;
		glVertex3f(-x2,-y2,-z2) ;
		glVertex3f(-x2, y2,-z2) ;
	glEnd() ;
}	
	
void fl_draw_frame( float xsize1, float ysize1, float zsize, float xsize2, float ysize2)
{
/* Centered on origin, in x,y plane. size1 is outside, size2 is inside */
	float x21,y21,z2,x22,y22 ;
	
	x21 = 0.5*xsize1 ;
	y21 = 0.5*ysize1 ;
	x22 = 0.5*xsize2 ;
	y22 = 0.5*ysize2 ;
	z2 = 0.5 * zsize ;

	/* Draw four xy faces positive z */
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,y21,z2 ) ;
		glVertex3f( -x21,y21,z2 ) ;
		glVertex3f( -x21,y22,z2 ) ;
		glVertex3f( x21,y22,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,-y21,z2 ) ;
		glVertex3f( -x21,-y21,z2 ) ;
		glVertex3f( -x21,-y22,z2 ) ;
		glVertex3f( x21,-y22,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,y22,z2 ) ;
		glVertex3f( x22,y22,z2 ) ;
		glVertex3f( x22,-y22,z2 ) ;
		glVertex3f( x21,-y22,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( -x21,y22,z2 ) ;
		glVertex3f( -x22,y22,z2 ) ;
		glVertex3f( -x22,-y22,z2 ) ;
		glVertex3f( -x21,-y22,z2 ) ;		
	glEnd() ;
	
	/* Draw four xy faces negative z */
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,y21,-z2 ) ;
		glVertex3f( -x21,y21,-z2 ) ;
		glVertex3f( -x21,y22,-z2 ) ;
		glVertex3f( x21,y22,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,-y21,-z2 ) ;
		glVertex3f( -x21,-y21,-z2 ) ;
		glVertex3f( -x21,-y22,-z2 ) ;
		glVertex3f( x21,-y22,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( x21,y22,-z2 ) ;
		glVertex3f( x22,y22,-z2 ) ;
		glVertex3f( x22,-y22,-z2 ) ;
		glVertex3f( x21,-y22,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,0.0,1.0) ;
		glVertex3f( -x21,y22,-z2 ) ;
		glVertex3f( -x22,y22,-z2 ) ;
		glVertex3f( -x22,-y22,-z2 ) ;
		glVertex3f( -x21,-y22,-z2 ) ;		
	glEnd() ;
	
	/* Draw exterior lateral surfaces */
	glBegin(GL_QUADS ) ;
		glNormal3f(1.0,0.0,0.0) ;
		glVertex3f( x21,y21,z2 ) ;
		glVertex3f( x21,-y21,z2 ) ;
		glVertex3f( x21,-y21,-z2 ) ;
		glVertex3f( x21,y21,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(1.0,0.0,0.0) ;
		glVertex3f( -x21,y21,z2 ) ;
		glVertex3f( -x21,-y21,z2 ) ;
		glVertex3f( -x21,-y21,-z2 ) ;
		glVertex3f( -x21,y21,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( x21,y21,z2 ) ;
		glVertex3f( x21,y21,-z2 ) ;
		glVertex3f( -x21,y21,-z2 ) ;
		glVertex3f( -x21,y21,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( x21,-y21,z2 ) ;
		glVertex3f( x21,-y21,-z2 ) ;
		glVertex3f( -x21,-y21,-z2 ) ;
		glVertex3f( -x21,-y21,z2 ) ;		
	glEnd() ;
	
	/* Draw interior lateral surfaces */
	glBegin(GL_QUADS ) ;
		glNormal3f(1.0,0.0,0.0) ;
		glVertex3f( x22,y22,z2 ) ;
		glVertex3f( x22,y22,-z2 ) ;
		glVertex3f( x22,-y22,-z2 ) ;
		glVertex3f( x22,-y22,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(1.0,0.0,0.0) ;
		glVertex3f( -x22,y22,z2 ) ;
		glVertex3f( -x22,y22,-z2 ) ;
		glVertex3f( -x22,-y22,-z2 ) ;
		glVertex3f( -x22,-y22,z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( x22,y22,z2 ) ;
		glVertex3f( -x22,y22,z2 ) ;
		glVertex3f( -x22,y22,-z2 ) ;
		glVertex3f( x22,y22,-z2 ) ;		
	glEnd() ;
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( x22,-y22,z2 ) ;
		glVertex3f( -x22,-y22,z2 ) ;
		glVertex3f( -x22,-y22,-z2 ) ;
		glVertex3f( x22,-y22,-z2 ) ;		
	glEnd() ;
}	
	
void fl_draw_baseless_pyramid( float xsize, float ysize, float h )
{
/* Centered on origin, in x,y plane */
	float x2,y2 ;
	float p0[3],p1[3],p2[3],n[3] ;
	
	x2 = 0.5*xsize ;
	y2 = 0.5*ysize ;

	/* Draw four xy faces positive z */
	glBegin(GL_TRIANGLES) ;
		p0[0] = x2 ;  p0[1] = y2 ;  p0[2] = 0.0 ;
		p1[0] = 0.0 ; p1[1] = 0.0 ; p1[2] = h ;
		p2[0] = x2 ;  p2[1] = -y2 ; p2[2] = 0.0 ;
		fl_calnormals(p0,p1,p2,n) ;
		glNormal3fv(n) ;
		glVertex3fv(p0) ;
		glVertex3fv(p1) ;
		glVertex3fv(p2) ;
	glEnd() ;
	glBegin(GL_TRIANGLES) ;
		p0[0] = -x2 ; p0[1] = -y2 ; p0[2] = 0.0 ;
		p1[0] = 0.0 ; p1[1] = 0.0 ; p1[2] = h ;
		p2[0] = -x2 ; p2[1] = y2 ;  p2[2] = 0.0 ;
		fl_calnormals(p0,p1,p2,n) ;
		glNormal3fv(n) ;
		glVertex3fv(p0) ;
		glVertex3fv(p1) ;
		glVertex3fv(p2) ;
	glEnd() ;
	glBegin(GL_TRIANGLES) ;
		p0[0] = x2 ;  p0[1] = y2 ;  p0[2] = 0.0 ;
		p1[0] = -x2 ;  p1[1] = y2 ; p1[2] = 0.0 ;
		p2[0] = 0.0 ; p2[1] = 0.0 ; p2[2] = h ;
		fl_calnormals(p0,p1,p2,n) ;
		glNormal3fv(n) ;
		glVertex3fv(p0) ;
		glVertex3fv(p1) ;
		glVertex3fv(p2) ;
	glEnd() ;
	glBegin(GL_TRIANGLES) ;
		p0[0] = -x2 ;  p0[1] = -y2 ;  p0[2] = 0.0 ;
		p1[0] = x2 ;  p1[1] = -y2 ; p1[2] = 0.0 ;
		p2[0] = 0.0 ; p2[1] = 0.0 ; p2[2] = h ;
		fl_calnormals(p0,p1,p2,n) ;
		glNormal3fv(n) ;
		glVertex3fv(p0) ;
		glVertex3fv(p1) ;
		glVertex3fv(p2) ;
	glEnd() ;
}
	
void fl_beacon_display (float xx, float yy, float zz)
{
    float mamb[4],mdif[4],mspec[4] ;
    float R=0.13,h=2.0;
/* TODO: reenable when shadows are working */
    //float beaconzx,beacontheta,sunzx ;
    //GLfloat y;
    int nfac = 3 ;
	
    glPushMatrix() ;
	
    mamb[0] = 0.9 ;
    mamb[1] = 0.9 ;
    mamb[2] = 0.9 ;
    mamb[3] = 1.0 ;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    mdif[0] = 0.5 ;
    mdif[1] = 0.5 ;
    mdif[2] = 0.5 ;
    mdif[3] = 1.0 ;
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
    mspec[0] = 1.0 ;
    mspec[1] = 1.0 ;
    mspec[2] = 1.0 ;
    mspec[3] = 1.0 ;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 10.0) ;  
    glTranslatef(xx,yy,zz) ;
    
    glPushMatrix() ;
		glRotatef(-90.0,1.0,0.0,0.0) ;
		fl_draw_cylinder( R, h, nfac ) ;
    glPopMatrix() ;
	
    glPopMatrix() ;
}

void fl_draw_beacon(float xx, float yy, float zz)
{
  /* draw the object */
  fl_beacon_display(xx, yy, zz);
}	 

void fl_draw_beacon_in_mesh(struct rectangular_mesh_type *m, int ib, int jb)
{
  fl_draw_beacon(m->mesh[ib][jb][0], m->mesh[ib][jb][1], m->mesh[ib][jb][2]);
}

void fl_draw_mesh( struct rectangular_mesh_type *m ) 
{
/* remap axis.. f(x,z) = y instead, for graphics. */
	int i,j ;
	float *p00, *p01, *p10, *p11  ;
	
	for( i=0 ; i<m->nx-1 ; i++ ) {
		for( j=0 ; j<m->ny-1 ; j++ ) {
			p00 = m->mesh[i][j] ;
			p01 = m->mesh[i][j+1] ;
			p10 = m->mesh[i+1][j] ;
			p11 = m->mesh[i+1][j+1] ;
			glBegin(GL_TRIANGLES) ;
				glNormal3f(p00[3],p00[4],p00[5]) ;
				glVertex3f(p00[0],p00[1],p00[2]) ;			
				glNormal3f(p10[3],p10[4],p10[5]) ;
				glVertex3f(p10[0],p10[1],p10[2]) ;			
				glNormal3f(p01[3],p01[4],p01[5]) ;
				glVertex3f(p01[0],p01[1],p01[2]) ;	
			glEnd() ;		
			glBegin(GL_TRIANGLES) ;
				glNormal3f(p11[3],p11[4],p11[5]) ;
				glVertex3f(p11[0],p11[1],p11[2]) ;			
				glNormal3f(p01[3],p01[4],p01[5]) ;
				glVertex3f(p01[0],p01[1],p01[2]) ;	
				glNormal3f(p10[3],p10[4],p10[5]) ;
				glVertex3f(p10[0],p10[1],p10[2]) ;			
			glEnd()	;
		}
	}	
}

/***
// method for passing out the frustum array information 
FRUSTUM_TYPE **get_frustum_array (void)
{
  return FL_frustum_array;
}
***/
