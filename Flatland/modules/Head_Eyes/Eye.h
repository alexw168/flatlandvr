//
// Eye.h
//
// Include file for Eye.cc
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

#ifndef EYE_H
#define EYE_H

void EyeInit( APPTYPE *App, char *parent, char *name );

typedef struct EyeDataMembers
{
	char HeadTrackerName[MAX_NAME_SIZE];			// head transformationnode
	OBJECT_TYPE *HeadTrackerObject;
} EYE_DATA_MEMBERS;

#endif //ndef EYE_H