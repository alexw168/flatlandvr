//////////////////////////////////////////////////////////////
//
//  TrackerClient.h
//
//  Header file for TrackerClient "API"
//
// Copyright (C) 2003, University of New Mexico.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//           
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
//  Timothy B. Eyring - 7/4/00
//  KLS: added bird measurement rate variable - 8/8/00
//

#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

typedef struct Position_Struct 
{
	float x;
	float y;
	float z;
	float pitch;
	float yaw;
	float roll;
} CORDORD;

typedef struct TrackerClientAccessMembers
{
	// where is the trandsucer?
	bool (*GetPosition)(	OBJECT_TYPE *TrackerObject, 
							int transducer_number, 
							CORDORD *coord);		
	
	// where is the trandsucer?
	bool (*GetVelocity)(	OBJECT_TYPE *TrackerObject, 
							int transducer_number, 
							CORDORD *coord);		
	
	// where is the trandsucer?
	bool (*GetError)(	OBJECT_TYPE *TrackerObject, 
						int transducer_number, 
						CORDORD *coord);		
	
	// has server been contacted yet?	
	bool (*IsReady)(struct object *TrackerObject);	

	// how many transducers does the client know about?
	int (*GetNumberOfTransducers)(struct object *TrackerObject);

} TRACKER_CLIENT_ACCESS_MEMBERS;

#endif //ndef TRACKERCLIENT_H
