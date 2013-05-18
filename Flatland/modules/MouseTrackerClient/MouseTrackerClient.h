//////////////////////////////////////////////////////////////
//
//  MouseTrackerClient.h
//
//  Header file for MouseTrackerClient.cc
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
//  Timothy B. Eyring - 11/1/00
//

#ifndef MOUSETRACKERCLIENT_DATA_H
#define MOUSETRACKERCLIENT_DATA_H

typedef struct MouseTrackerClientDataMembers
{
/////// STAY OUT OF THESE ///////////////////////////////////////////////
	CORDORD Tracker;						// transducer position
	CORDORD Position;						// tracker base position (where is the trackers origin)
	bool Ready;								// is everything ready
	int NumberOfTransducers;				// how many transducers does the server have? (always 1)
	bool DisplayAxes;						//
	bool UseTracking;						//
	int window;								//
	int viewport;							//
	int x;									//
	int y;									//
	int button;								//
	int state[10];							// should be enough...
	float Radius;							// how for out to project mouse position
	OBJECT_TYPE *HeadTransformationNode;	//
	bool MoveIn;
	bool MoveOut;
	float MoveSpeed;
	bool WarpPointerToCenter;
} MOUSE_TRACKER_CLIENT_DATA_MEMBERS;

#endif //ndef MOUSETRACKERCLIENT_DATA_H
