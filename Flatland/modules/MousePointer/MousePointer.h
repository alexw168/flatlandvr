//////////////////////////////////////////////////////////////
//
//  MousePointer.h
//
//  Header file for MousePointer.cc
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
//
//  Timothy B. Eyring - 11/2/00
//

#ifndef MOUSEPOINTER_H
#define MOUSEPOINTER_H

typedef struct MousePointerMembers
{
/////// STAY OUT OF THESE ///////////////////////////////////////////////
    bool DataReady;
    CORDORD Position;					// tracker base position (where is the trackers origin)
    bool DisplayAxes;
    OBJECT_TYPE *MouseTransformationNode;
    char MouseTransformationName[ MAX_NAME_SIZE ];
} MOUSE_POINTER_MEMBERS;

#endif //ndef MOUSEPOINTER_H
