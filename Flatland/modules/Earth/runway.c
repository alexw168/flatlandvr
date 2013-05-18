/*
runway.c.  routines to initialize and perceptualize the runways on the earth

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
 * Created: 1-18-00
 */

#include "runway.h"
#include "world.h"

/* Constants */

/* "local" globals */
static struct maze_type *Runway_maze;

/* Runway Draw callback */
/* This callback assumes that the stencil buffer has be prepared w/ a stencil
 * of the earth */
void draw_runway(OBJECT_TYPE *o)
{
  int i,j ;
  float mamb[4],mdif[4],mspec[4] ;
  float y = 0 ;
  float *p00, *p01, *p10, *p11  ;
  WORLD_GLOBALS *globals;       /* pointer to "globals" structure */

  globals = (WORLD_GLOBALS *)o->ptr0 ;

  if(  globals->display_runway_flag==false)
    return ;

  /* Only render where the first bit of the stencil is set.  Since stencil
   * buffer contains depth info for earth, disable depth buffer */
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 0x00000001); /* draw if == 1 */
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  mamb[0] = 0.7 ;
  mamb[1] = 0.5 ;
  mamb[2] = 0.3 ;
  mamb[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  mdif[0] = 0.7 ;
  mdif[1] = 0.5 ;
  mdif[2] = 0.3 ;
  mdif[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  mspec[0] = 0.0 ;
  mspec[1] = 0.0 ;
  mspec[2] = 0.0 ;
  mspec[3] = 1.0 ;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;
      
  /* Loop over boxes of maze */
  for( i=0 ; i<Runway_maze->nx-1 ; i++ ) {
    for( j=0 ; j<Runway_maze->ny-1 ; j++ ) {
      if( Runway_maze->maze[i][j]>=0) {
        p00 = Runway_maze->mesh->mesh[i][j] ;
        p01 = Runway_maze->mesh->mesh[i][j+1] ;
        p10 = Runway_maze->mesh->mesh[i+1][j] ;
        p11 = Runway_maze->mesh->mesh[i+1][j+1] ;
        p00[1] = y ;
        p01[1] = y ;
        p10[1] = y ;
        p11[1] = y ;
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
        glEnd() ;  
      }
    }
  }
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;    

  /* Disable the stencil buffer, reenable the depth buffer */
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
}

void init_runway (APPTYPE *App, char *airport_fn)
{
  /* Make runway maze and randomize underlaying mesh normals */
  Runway_maze = fl_readmaze(airport_fn) ;
  fl_mesh_normal_randomize( 0.6,  Runway_maze->mesh ) ; 
}
