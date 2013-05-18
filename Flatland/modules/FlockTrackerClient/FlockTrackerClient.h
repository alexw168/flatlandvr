//////////////////////////////////////////////////////////////
//
//  FlockTrackerClient.h
//
//  Header file for FlockTrackerClient.cc
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
//  Timothy B. Eyring - 7/4/00
//  KLS: added bird measurement rate variable - 8/8/00
//  TBE: uses TrackerClient "API" - 8/25/00
//

#ifndef FLOCKTRACKERCLIENT_H
#define FLOCKTRACKERCLIENT_H

typedef struct FlockTrackerClientDataMembers
{
/////// STAY OUT OF THESE ///////////////////////////////////////////////
	float birdMeasRate;					// record of the desired bird measurement rate
	CORDORD *Tracker;					// array of transducer positions
	CORDORD Position;					// tracker base position (where is the trackers origin)
	char ServerName[MAX_NAME_SIZE];		// tracker server
	bool Ready;							// is everything ready
	int NumberOfTransducers;			// how many transducers does the server have?
	bool DisplayAxes;
	bool UseTracking;
        bool first_pass;
} FLOCK_TRACKER_CLIENT_DATA_MEMBERS;

#endif //ndef FLOCKTRACKERCLIENT_H
