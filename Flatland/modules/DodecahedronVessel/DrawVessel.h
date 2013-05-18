// DrawVessel.h
//  
//  Copyright (C) 2003, University of New Mexico.
//  
//      This library is free software; you can redistribute it and/or
//      modify it under the terms of the GNU Lesser General Public
//      License as published by the Free Software Foundation; either
//      version 2.1 of the License, or (at your option) any later version.
//  
//      This library is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//      Lesser General Public License for more details.
//  
//      You should have received a copy of the GNU Lesser General Public
//      License along with this library; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  

#ifndef DRAW_CRAFT_H
#define DRAW_CRAFT_H


// an octagonal approximation of a round bar
void drawTube( float Radius, float Length );

// a tetrahedron with the points clipped
void drawClipTet( float Size );

// build an atomic element of a dodecahedral craft
void drawDodecAtom( float VRadius, float TetSize, float RailRadius );

// a dodecahecral craft
void drawDodecVessel( float VRadius );

// a dodecahecral craft sized/oriented as a glutWireDodecahedron
void drawDodecVessel( void );


#endif
