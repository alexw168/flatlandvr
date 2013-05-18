//////////////////////////////////////////////////////////////
//
//  ButtonClient.h
//
//  Header file for ButtonClient.cc
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

#ifndef BUTTONCLIENT_H
#define BUTTONCLIENT_H

typedef struct ButtonClientAccessMembers
{
	// is the button pressed?
	// returns false if there is some error (like invalid button, or client not ready)
	bool (*GetButton)(	OBJECT_TYPE *ButtonClientObject, 
						int button_number, 
						bool *status);		
	
	// has server been contacted yet?	
	bool (*GetButtonsReady)(struct object *ButtonClientObject);	

	// how many buttons does the clien know about?
	int (*GetNumberOfButtons)(struct object *ButtonClientObject);
} BUTTON_CLIENT_ACCESS_MEMBERS;

typedef struct ButtonClientDataMembers
{
	bool *Button;						// button status array
	char ServerName[MAX_NAME_SIZE];		// button server
	bool Ready;							// is everything ready
	int NumberOfButtons;				// how many buttons does the server have?
} BUTTON_CLIENT_DATA_MEMBERS;

#endif //ndef BUTTONCLIENT
