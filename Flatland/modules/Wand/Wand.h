//
// Wand.h
//
// Include file for Wand.cc
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

#ifndef WAND_H
#define WAND_H

typedef struct wand_members
{
	bool (*GetTopButton)(OBJECT_TYPE *WandObject);		// is button pressed? 
	bool (*GetMiddleButton)(struct object *WandObject);	// is button pressed? 
	bool (*GetBottomButton)(struct object *WandObject);	// is button pressed? 
	bool (*GetBeamingObject)(OBJECT_TYPE *WandObject);	// is an object being beamed?
	OBJECT_TYPE *(*GetBeamedObject)(struct object *WandObject);	// return closest beamed object
	int (*SetHand)(OBJECT_TYPE *WandObject, int Hand);	// set which hand to use 
	int (*SetHead)(OBJECT_TYPE *WandObject, int Head);	// set which head to use 
	
/////// STAY OUT OF THESE ///////////////////////////////////////////////
	bool TopButton;								// is button pressed? 
	bool MiddleButton;							// is button pressed? 
	bool BottomButton;							// is button pressed? 
	int TopNumber;								// button number 
	int MiddleNumber;							// button number 
	int BottomNumber;							// button number
	bool DataReady;								// Has the data been read in? 
	int Hand;									// <0=left, 0=none, >0=right
	int Head;									// 0: default
	char LeftHandDataFile[MAX_NAME_SIZE];		// data filename
	char RightHandDataFile[MAX_NAME_SIZE];		// data filename
	char WandHandleDataFile[MAX_NAME_SIZE];		// data filename
	char DefaultHeadDataFile[MAX_NAME_SIZE];	// data filename
	char ButtonClientName[MAX_NAME_SIZE];		// button client
	GLfloat Scale;								// scale wand		
	int BeamColor;								// color of beam (mod 5)
	void *LeftHand;								// wand object
	void *RightHand;							// wand object
	void *WandHandle;							// wand object
	void *DefaultHead;							// wand object
	// deletion of the button client object would break this....
	OBJECT_TYPE *ButtonClient;					// where are the buttons
	CORDORD Position;							// where is the wand located
	GLuint *WandList;							// Wand display lists
} WAND_MEMBERS;

#endif //ndef WAND_H
