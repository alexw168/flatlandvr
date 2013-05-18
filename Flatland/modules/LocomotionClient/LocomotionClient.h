//////////////////////////////////////////////////////////////
//
//  LocomotionClient.h
//
//  Header file for LocomotionClient.cc
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
//  Timothy B. Eyring - 11/8/00
//

#ifndef LOCOMOTIONCLIENT_H
#define LOCOMOTIONCLIENT_H

extern OBJECT_TYPE *LocomotionClient;

typedef struct LocomotionClientMembers
{
/////// STAY OUT OF THESE ///////////////////////////////////////////////
	CORDORD Tracker;							// tracker position
	CORDORD Velocity;							// tracker velocity
	CORDORD Position;							// tracker base position (where is the trackers origin)
	char LeftWandName[MAX_NAME_SIZE];			// left wand
	char LeftWandTrackerName[MAX_NAME_SIZE];	// left transformation node
	char RightWandName[MAX_NAME_SIZE];			// right wand
	char RightWandTrackerName[MAX_NAME_SIZE];	// right transformation node
	bool Ready;									// is everything ready
	int NumberOfTransducers;					// how many transducers does the server have?
	float VelocityScale;						// how granular are the sliders?
	bool DisplayAxes;
	bool UseTracking;
        bool printPosn;
} LOCOMOTION_CLIENT_MEMBERS;

#endif //ndef LOCOMOTIONCLIENT_H
