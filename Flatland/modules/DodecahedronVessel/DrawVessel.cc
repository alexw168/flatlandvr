/*
draw_craft.c.  Contains the draw functions that make a dodec craft.

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
 * Created: 1-13-00
 */

#include <GL/glut.h>
#include <math.h>


#include "DrawVessel.h"


// some file-global constants

const float PENT_ANGLE = 72.0;
const float DEG_PER_RAD = 180.0 / 3.1415926;
const float EDG_VRT_ANG = 20.905158285;
const float FAC_EDG_ANG = 31.717475807;
const float FAC_VRT_ANG = 37.377366705;
const float EDG_LEN_VS_VRT_RAD = 0.713656;
//const float FAC_RAD_VS_VRT_RAD = 0.794654472;
const float Rt2 = sqrt( 2.0 );
const float Rt2By2 = Rt2 / 2.0;
const float Rt3 = sqrt( 3.0 );
const float Rt3By3 = Rt3 / 3.0;
const float TetAxisAlignAngle = -acos( 1.0 / Rt3 ) * DEG_PER_RAD;
const float DodAxisAlignAngle = 90.0 - FAC_VRT_ANG - 2*EDG_VRT_ANG;


// octagonal approximation of a round tube
//  centered on the x axis, left end at origin
void drawTube( float Radius, float Length ) {

   const float Comp45 = Radius * Rt2By2;
      
   glBegin( GL_QUADS );
      
      glNormal3f( 0.0, 0.0, 1.0 );
         glVertex3f( Length, 0.0, Radius );
         glVertex3f( 0.0, 0.0, Radius );
      glNormal3f( 0.0, -Rt2By2, Rt2By2 );
         glVertex3f( 0.0, -Comp45, Comp45 );
         glVertex3f( Length, -Comp45, Comp45 );
	 
         glVertex3f( Length, -Comp45, Comp45 );
         glVertex3f( 0.0, -Comp45, Comp45 );
      glNormal3f( 0.0, -1.0, 0.0 );
         glVertex3f( 0.0, -Radius, 0.0 );
         glVertex3f( Length, -Radius, 0.0 );
	    
         glVertex3f( Length, -Radius, 0.0 );
         glVertex3f( 0.0, -Radius, 0.0 );
      glNormal3f( 0.0, -Rt2By2, -Rt2By2 );
         glVertex3f( 0.0, -Comp45, -Comp45 );
         glVertex3f( Length, -Comp45, -Comp45 );
	 
         glVertex3f( Length, -Comp45, -Comp45 );
         glVertex3f( 0.0, -Comp45, -Comp45 );
      glNormal3f( 0.0, 0.0, -1.0 );
         glVertex3f( 0.0, 0.0, -Radius );
         glVertex3f( Length, 0.0, -Radius );
	    	 	    
         glVertex3f( Length, 0.0, -Radius );
         glVertex3f( 0.0, 0.0, -Radius );
      glNormal3f( 0.0, Rt2By2, -Rt2By2 );
         glVertex3f( 0.0, Comp45, -Comp45 );
         glVertex3f( Length, Comp45, -Comp45 );
	 
         glVertex3f( Length, Comp45, -Comp45 );
         glVertex3f( 0.0, Comp45, -Comp45 );
      glNormal3f( 0.0, 1.0, 0.0 );
         glVertex3f( 0.0, Radius, 0.0 );
         glVertex3f( Length, Radius, 0.0 );
	    	    
         glVertex3f( Length, Radius, 0.0 );
         glVertex3f( 0.0, Radius, 0.0 );
      glNormal3f( 0.0, Rt2By2, Rt2By2 );
         glVertex3f( 0.0, Comp45, Comp45 );
         glVertex3f( Length, Comp45, Comp45 );
	 
         glVertex3f( Length, Comp45, Comp45 );
         glVertex3f( 0.0, Comp45, Comp45 );
      glNormal3f( 0.0, 0.0, 1.0 );
         glVertex3f( 0.0, 0.0, Radius );
         glVertex3f( Length, 0.0, Radius );

   glEnd( );

}


// a tetrahedron with the vertices clipped
void drawClipTet( float Size ) {

   float HalfSize = Size/2.0;
   
   glPushMatrix( );
   
      glRotatef( -45.0, 0.0, 0.0, 1.0 );
      glRotatef( TetAxisAlignAngle, -1.0, 1.0, 0.0 );

      glNormal3f( Rt3By3, -Rt3By3, Rt3By3 );
      glBegin( GL_POLYGON );
         glVertex3f( Size, HalfSize, HalfSize );
         glVertex3f( HalfSize, HalfSize, Size );
         glVertex3f( -HalfSize, -HalfSize, Size );
         glVertex3f( -HalfSize, -Size, HalfSize );
         glVertex3f( HalfSize, -Size, -HalfSize );
         glVertex3f( Size, -HalfSize, -HalfSize );
      glEnd( );
   
      glNormal3f( -Rt3By3, Rt3By3, Rt3By3 );
      glBegin( GL_POLYGON );
         glVertex3f( HalfSize, HalfSize, Size );
         glVertex3f( HalfSize, Size, HalfSize );
         glVertex3f( -HalfSize, Size, -HalfSize );
         glVertex3f( -Size, HalfSize, -HalfSize );
         glVertex3f( -Size, -HalfSize, HalfSize );
         glVertex3f( -HalfSize, -HalfSize, Size );
      glEnd( );
   
      glNormal3f( Rt3By3, Rt3By3, -Rt3By3 );
      glBegin( GL_POLYGON );
         glVertex3f( HalfSize, Size, HalfSize );
         glVertex3f( Size, HalfSize, HalfSize );
         glVertex3f( Size, -HalfSize, -HalfSize );
         glVertex3f( HalfSize, -HalfSize, -Size );
         glVertex3f( -HalfSize, HalfSize, -Size );
         glVertex3f( -HalfSize, Size, -HalfSize );
      glEnd( );
   
      glNormal3f( -Rt3By3, -Rt3By3, -Rt3By3 );
      glBegin( GL_POLYGON );
         glVertex3f( -HalfSize, -Size, HalfSize );
         glVertex3f( -Size, -HalfSize, HalfSize );
         glVertex3f( -Size, HalfSize, -HalfSize );
         glVertex3f( -HalfSize, HalfSize, -Size );
         glVertex3f( HalfSize, -HalfSize, -Size );
         glVertex3f( HalfSize, -Size, -HalfSize );
      glEnd( );
   
      glBegin( GL_TRIANGLES );
   
         glNormal3f( Rt3By3, Rt3By3, Rt3By3 );
         glVertex3f( Size, HalfSize, HalfSize );
         glVertex3f( HalfSize, Size, HalfSize );
         glVertex3f( HalfSize, HalfSize, Size );
   
         glNormal3f( -Rt3By3, -Rt3By3, Rt3By3 );
         glVertex3f( -Size, -HalfSize, HalfSize );
         glVertex3f( -HalfSize, -Size, HalfSize );
         glVertex3f( -HalfSize, -HalfSize, Size );
   
         glNormal3f( Rt3By3, -Rt3By3, -Rt3By3 );
         glVertex3f( Size, -HalfSize, -HalfSize );
         glVertex3f( HalfSize, -Size, -HalfSize );
         glVertex3f( HalfSize, -HalfSize, -Size );
   
         glNormal3f( -Rt3By3, Rt3By3, -Rt3By3 );
         glVertex3f( -Size, HalfSize, -HalfSize );
         glVertex3f( -HalfSize, Size, -HalfSize );
         glVertex3f( -HalfSize, HalfSize, -Size );
      
      glEnd( );

   glPopMatrix( );

}


// build an atomic element of a dodecahedral craft
void drawDodecAtom( float VRadius, float TetSize, float RailRadius )
{

   float RailLength = EDG_LEN_VS_VRT_RAD * VRadius;

   glPushMatrix( );
      
      // rotates into atomic position
      glRotatef( -FAC_EDG_ANG, 1.0, 0.0, 0.0 );
      glRotatef( EDG_VRT_ANG, 0.0, 1.0, 0.0 );
      
      // last rail
      glTranslatef( 0.0, 0.0, -VRadius );
         glPushMatrix( );
            glRotatef( -EDG_VRT_ANG, 0.0, 1.0, 0.0 );
            drawTube( RailRadius, RailLength );
         glPopMatrix( );
	 
      // middle rail and vertex blob
      glTranslatef( 0.0, 0.0, VRadius );
      glRotatef( -2.0 * EDG_VRT_ANG, 0.0, 1.0, 0.0 );
      glRotatef( -60.0, 0.0, 0.0, 1.0 );
      glTranslatef( 0.0, 0.0, -VRadius );
         drawClipTet( TetSize );
         glPushMatrix( );
            glRotatef( -EDG_VRT_ANG, 0.0, 1.0, 0.0 );
            drawTube( RailRadius, RailLength );
         glPopMatrix( );
      
      // first rail and vertex blob
      glTranslatef( 0.0, 0.0, VRadius );
      glRotatef( -2.0 * EDG_VRT_ANG, 0.0, 1.0, 0.0 );
      glRotatef( 60.0, 0.0, 0.0, 1.0 );
      glTranslatef( 0.0, 0.0, -VRadius );
         drawClipTet( TetSize );
         glPushMatrix( );
            glRotatef( -EDG_VRT_ANG, 0.0, 1.0, 0.0 );
            drawTube( RailRadius, RailLength );
         glPopMatrix( );
	     
   glPopMatrix( );
   
}


// a dodecahedral craft
void drawDodecVessel( float VRadius ) {

   int i;
   float TetSize = VRadius / 64.0;
   float RailRadius = VRadius / 98.0;
   
   glPushMatrix( );
   
      // get a vertex pointing straight up
      glRotatef( DodAxisAlignAngle, 1.0, 0.0, 0.0 );
   
      // front half of dodecahedron
      drawDodecAtom( VRadius, TetSize, RailRadius );
      for ( i=0; i<4; i++ ) {
         glRotatef( PENT_ANGLE, 0.0, 0.0, 1.0 );
            drawDodecAtom( VRadius, TetSize, RailRadius );
      }
   
      // back half of dodecahedron
      glScalef( -1.0, 1.0, 1.0 );
      glRotatef( 180.0, 1.0, 0.0, 0.0 );
         drawDodecAtom( VRadius, TetSize, RailRadius );
         for ( i=0; i<4; i++ ) {
            glRotatef( PENT_ANGLE, 0.0, 0.0, 1.0 );
               drawDodecAtom( VRadius, TetSize, RailRadius );
         }
   
   glPopMatrix( );

}


// a dodecahedral craft sized/oriented as a glutWireDodecahedron
void drawDodecVessel( ) {
   drawDodecVessel( Rt3 );
}

