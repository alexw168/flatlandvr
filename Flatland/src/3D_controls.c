/*
3D_controls.c.  Default support for various 3D controls (buttons, etc.)

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
 */

#include "gheader.h"

/* constants */
        /* Button dimensions */
const float xl = 0.05 , xl2 = xl*0.5 , /* Outer frame */
            yl = 0.05 , yl2 = yl*0.5 ,
            zl = 0.01 ,
            xb = 0.04 , /* Inner frame */
            yb = 0.04 ,
            zb = 0.01 , zb2 = zb*0.5 ,
            yt = 0.02 , yt2 = yt*0.5 ; /* Height of the text box for lable */

void fl_CreateButtonBoundingVols (OBJECT_TYPE *Button)
{
  /* set up the bounding volumes (position & culling) */
  Button->bounding_volume_set = fl_makebvset(3) ;

  /* Define bounding sphere for culling */
  Button->bounding_volume_set->bvset[1]->npoints = 2 ;
  /* Center point */
  Button->bounding_volume_set->bvset[1]->p[0][0]=0.0 ;
  Button->bounding_volume_set->bvset[1]->p[0][1]=0.0 ;
  Button->bounding_volume_set->bvset[1]->p[0][2]= 0.0 ;
  Button->bounding_volume_set->bvset[1]->p[0][3]= 1.0 ;
  /* Surface point */
  Button->bounding_volume_set->bvset[1]->p[1][0]=0.6*xl ;
  Button->bounding_volume_set->bvset[1]->p[1][1]=0.0 ;
  Button->bounding_volume_set->bvset[1]->p[1][2]=0.0 ;
  Button->bounding_volume_set->bvset[1]->p[1][3]=1.0 ;
  
  /* Define a bounding rectalinear solid */
  Button->bounding_volume_set->bvset[2]->npoints = 5 ;
  /* Center point */
  Button->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
  Button->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
  Button->bounding_volume_set->bvset[2]->p[0][2]= -20.0*zb ;
  Button->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  /* First quadrant vertex point */
  Button->bounding_volume_set->bvset[2]->p[1][0]= 0.5*xb ;
  Button->bounding_volume_set->bvset[2]->p[1][1]= 0.5*yb ;
  Button->bounding_volume_set->bvset[2]->p[1][2]= 2.0*zb ;
  Button->bounding_volume_set->bvset[2]->p[1][3]= 1.0 ;
  /* x Edge vertex point */
  Button->bounding_volume_set->bvset[2]->p[2][0]= -0.5*xb ;
  Button->bounding_volume_set->bvset[2]->p[2][1]= 0.5*yb ;
  Button->bounding_volume_set->bvset[2]->p[2][2]=  2.0*zb ;
  Button->bounding_volume_set->bvset[2]->p[2][3]= 1.0 ;
  /* y Edge vertex point */
  Button->bounding_volume_set->bvset[2]->p[3][0]= 0.5*xb ;
  Button->bounding_volume_set->bvset[2]->p[3][1]= -0.5*yb ;
  Button->bounding_volume_set->bvset[2]->p[3][2]=  2.0*zb ;
  Button->bounding_volume_set->bvset[2]->p[3][3]= 1.0 ;
  /* z Edge vertex point */
  Button->bounding_volume_set->bvset[2]->p[4][0]= 0.5*xb ;
  Button->bounding_volume_set->bvset[2]->p[4][1]= 0.5*yb ;
  Button->bounding_volume_set->bvset[2]->p[4][2]= -39.0*zb ; /* Note deeper on back side */
  Button->bounding_volume_set->bvset[2]->p[4][3]= 1.0 ;
}

void fl_Draw3dButton (OBJECT_TYPE *o)
{
  float mamb[4],mdif[4],mspec[4] ;

  if( o->beam_sphere_hit_flag==0 ) {
    mamb[0] = 0.0 ;
    mamb[1] = 0.3 ;
    mamb[2] = 0.0 ;
    mamb[3] = 1.0 ;
    mdif[0] = 0.0 ;
    mdif[1] = 0.3 ;
    mdif[2] = 0.0 ;
    mdif[3] = 1.0 ;
    mspec[0] = 0.0 ;
    mspec[1] = 0.3 ;
    mspec[2] = 0.0 ;
    mspec[3] = 1.0 ;
  } else {
    mamb[0] = 0.0 ;
    mamb[1] = 0.99 ;
    mamb[2] = 0.0 ;
    mamb[3] = 1.0 ;
    mdif[0] = 0.0 ;
    mdif[1] = 0.99 ;
    mdif[2] = 0.0 ;
    mdif[3] = 1.0 ;
    mspec[0] = 0.0 ;
    mspec[1] = 0.99 ;
    mspec[2] = 0.0 ;
    mspec[3] = 1.0 ;        
  }
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 50.0) ;
    
  fl_draw_frame( xl, yl, zl, xb, yb) ;

  if( o->beam_rect_hit_flag==0 ) {
    mamb[0] = 0.6 ;
    mamb[1] = 0.0 ;
    mamb[2] = 0.0 ;
    mamb[3] = 1.0 ;
    mdif[0] = 0.6 ;
    mdif[1] = 0.0 ;
    mdif[2] = 0.0 ;
    mdif[3] = 1.0 ;
    mspec[0] = 0.6 ;
    mspec[1] = 0.0 ;
    mspec[2] = 0.0 ;
    mspec[3] = 1.0 ;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 50.0) ;
    fl_draw_baseless_pyramid( xb,yb,zb ) ;
  } else {
    mamb[0] = 0.0 ;
    mamb[1] = 0.6 ;
    mamb[2] = 0.0 ;
    mamb[3] = 1.0 ;
    mdif[0] = 0.0 ;
    mdif[1] = 0.6 ;
    mdif[2] = 0.0 ;
    mdif[3] = 1.0 ;
    mspec[0] = 0.0 ;
    mspec[1] = 0.6 ;
    mspec[2] = 0.0 ;
    mspec[3] = 1.0 ;        
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 50.0) ;
    fl_draw_baseless_pyramid( xb,yb,-zb ) ;
  }
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;    
}

void fl_PreDraw3dButton (OBJECT_TYPE *o)
{
  if( o->beam_rect_hit_flag==0 ) {
    if (o->object_state0 == 1)
      o->object_state0 = 0;
  } else {
    if (o->object_state0 == 0)
    {
       o->object_state0 = 1;
       o->object_state2 ^= 1;
    }
  }
}

void fl_Label3dButton (OBJECT_TYPE *o, char *labelString)
{
  float string_height, xoffset, yoffset ; ;
  float mamb[4],mdif[4],mspec[4] ;

  /* Print button lable */
  mamb[0] = 0.0 ;
  mamb[1] = 0.3 ;
  mamb[2] = 0.0 ;
  mamb[3] = 1.0 ;
  mdif[0] = 0.2 ;
  mdif[1] = 0.6 ;
  mdif[2] = 0.2 ;
  mdif[3] = 1.0 ;
  mspec[0] = 0.0 ;
  mspec[1] = 0.0 ;
  mspec[2] = 0.0 ;
  mspec[3] = 1.0 ;        
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;
  glBegin(GL_QUADS) ;
    glVertex3f(xl2,-yl2,-zb2 );
    glVertex3f(-xl2,-yl2,-zb2 );
    glVertex3f(-xl2,-yl2-yt,-zb2 );
    glVertex3f(xl2,-yl2-yt,-zb2 );
  glEnd() ;

  string_height = 0.015 ;
  xoffset = -xl2+0.001 ;
  yoffset = -yl2-yt2-string_height*0.5 ;

  glTranslatef(xoffset,yoffset,-zb2+0.001) ;

  mamb[0] = 1.0 ;
  mamb[1] = 1.0 ;
  mamb[2] = 1.0 ;
  mamb[3] = 1.0 ;
  mdif[0] = 1.0 ;
  mdif[1] = 1.0 ;
  mdif[2] = 1.0 ;
  mdif[3] = 1.0 ;
  mspec[0] = 0.0 ;
  mspec[1] = 0.0 ;
  mspec[2] = 0.0 ;
  mspec[3] = 1.0 ;        
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;

  fl_draw_stroke_string( labelString, string_height ) ;
}
