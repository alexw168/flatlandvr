/*
 * Vessel.h
 *
 * Header file for Vessel.c
 * 
 * Copyright (C) 2003, University of New Mexico.
 * 
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2.1 of the License, or (at your option) any later version.
 * 
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
#ifndef VESSEL_H
#define VESSEL_H

typedef struct vessel_access_members
{
	bool (*SetSound)( OBJECT_TYPE *VesselObject, bool SoundOn );		// make sound? 
	bool (*SetDisplay)( OBJECT_TYPE *VesselObject, bool DisplayOn );	// show vessel? 
} VESSEL_ACCESS_MEMBERS;

typedef struct vessel_data_members
{
	bool DrawBars;									// draw bars???
	bool EngineSound;								// make sound??
	float Scale;									// how big is the vessel
	CORDORD Position;								// where is the wand located
	GLuint *VesselList;								// Wand display lists
	char LocomotionClientName[MAX_NAME_SIZE];		// locomotion client name
	OBJECT_TYPE *LocomotionClient;					// locomotion client object
} VESSEL_DATA_MEMBERS;

#endif //ndef VESSEL_H
