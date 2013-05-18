/*
ButtonClient.cc  An app providing access to external buttons

Copyright (C) 2003, University of New Mexico.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

// 
// Maintenance history
//
// Created: 6-22-00, TBE
//

// include the master Flatland library 
#include "flatland_app.h"
#include "ButtonClient.h"

#define BUTTONS_SERVER "localhost"
#define BUTTONS_PORT 32212
#define POLING_DELAY 10

// Function to read button status 
void *ButtonThread(void *X)
{
	OBJECT_TYPE *ButtonClientObject;						// pointer to the object passed in (this obj) 
	BUTTON_CLIENT_DATA_MEMBERS *ButtonClientDataMembers;	// object data pointer 
	char *Name;												// name of the object
	int byte_count;											// number of bytes recieved
	char *buffer;											// buffer to hold incomming data from button server
	char button_count[12];									// buffer to recieve th number of buttons from server
	int i;
		
#ifdef ENABLE_SDL
	IPaddress serverIP;
	TCPsocket tcpsock = NULL;
	SDLNet_SocketSet socketset = NULL;
#endif
    
	ButtonClientObject = (OBJECT_TYPE *) X;
	ButtonClientDataMembers = (BUTTON_CLIENT_DATA_MEMBERS *) ButtonClientObject->ptr1;
	Name = ButtonClientObject->name;

#ifdef ENABLE_SDL

	// Start up SDL
	if ( SDL_Init(SDL_INIT_TIMER) < 0 )
	{
		fprintf(stderr, "%s: Couldn't initialize SDL: %s\n", Name, SDL_GetError());
		return NULL;
	}
    
	// Register exit handler
	atexit(SDL_Quit);
    
	// Initialize the network 
	if ( SDLNet_Init() < 0 )
	{
		fprintf(stderr, "%s: Couldn't initialize net: %s\n", Name, SDLNet_GetError());
		return NULL;
	}
    
	// Register another exit handler...lantinga loves these even more than I do!
	atexit(SDLNet_Quit);
    
	fprintf(stdout,"%s: Connecting to %s ... \n", Name, ButtonClientDataMembers->ServerName);
	fflush(stdout);

	// Allocate the socket set 
	socketset = SDLNet_AllocSocketSet(1); // just the one outgoing connection
	if (socketset == NULL) 
	{
		fprintf(stderr, "%s: Couldn't create socket set: %s\n",Name, SDLNet_GetError());
		fflush(stderr);
		return NULL;
	}
		
	// Resolve IP and fill out serverIP structure
	SDLNet_ResolveHost(&serverIP, ButtonClientDataMembers->ServerName, BUTTONS_PORT);
   
	if ( serverIP.host == INADDR_NONE )
	{
		SDLNet_FreeSocketSet(socketset);
		fprintf(stderr,"%s: Couldn't resolve hostname!\n", Name);
		fflush(stderr);
		return NULL;
	} 

	// Attempt to connect to IP/socket pair 
	tcpsock = SDLNet_TCP_Open(&serverIP);
       
	if ( tcpsock == NULL )
	{
		SDLNet_FreeSocketSet(socketset);
		fprintf(stderr,"%s: Connect failed.\n", Name);
		fflush(stderr);
		return NULL;
	} 

	SDLNet_TCP_AddSocket(socketset, tcpsock);

	printf("%s: connected to button server.\n", Name);

	// request number of buttons on server
	byte_count = SDLNet_TCP_Send(tcpsock, "?", 1);
	if (byte_count < 1) 
	{
		fprintf(stderr, "%s: connection to button server lost.\n", Name);
		SDLNet_TCP_Close(tcpsock);
		SDLNet_FreeSocketSet(socketset);
		return NULL;
	}

	// recieve response
	byte_count = SDLNet_TCP_Recv(tcpsock, button_count, 10); 
	if (byte_count>0)
	{
		sscanf(button_count,"%d",&(ButtonClientDataMembers->NumberOfButtons));
		if (ButtonClientDataMembers->NumberOfButtons<0)
		{
			ButtonClientDataMembers->NumberOfButtons = 0;
		}
	}

	// generate the Button array
	if ((ButtonClientDataMembers->Button = (bool *) malloc (sizeof(bool)*ButtonClientDataMembers->NumberOfButtons)) == NULL)
	{
		SDLNet_TCP_Close(tcpsock);
		SDLNet_FreeSocketSet(socketset);
		fprintf(stderr, "%s: failed to initialize memory for buttons.\n", Name);
		ButtonClientDataMembers->NumberOfButtons = 0;
		exit(1);
	}
	
	// generate a character buffer
	if ((buffer = (char *) malloc (sizeof(char)*(ButtonClientDataMembers->NumberOfButtons+2))) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for buttons.\n", Name);
		SDLNet_TCP_Close(tcpsock);
		SDLNet_FreeSocketSet(socketset);
		ButtonClientDataMembers->NumberOfButtons = 0;
		free(ButtonClientDataMembers->Button);
		exit(1);
	}
	buffer[ButtonClientDataMembers->NumberOfButtons]=0;
	
	fprintf( stdout, "%s: Button server has %d buttons.\n", Name, ButtonClientDataMembers->NumberOfButtons);

	for(i=0;i<ButtonClientDataMembers->NumberOfButtons;i++)
	{	
		ButtonClientDataMembers->Button[i]=false;
	}

	ButtonClientDataMembers->Ready = true;
	
	while( !do_exit )
	{
		if (SDLNet_TCP_Send(tcpsock, "!", 1) < 1) 
		{
			fprintf(stderr, "%s: connection to button server lost.\n", Name);
			SDLNet_TCP_Close(tcpsock);
			SDLNet_FreeSocketSet(socketset);
			ButtonClientDataMembers->NumberOfButtons = 0;
			free(ButtonClientDataMembers->Button);
			free(buffer);
			return NULL;
		}

		SDLNet_CheckSockets(socketset, 1000); // was ~0

		if (SDLNet_SocketReady(tcpsock))
		{
			byte_count = SDLNet_TCP_Recv(tcpsock, buffer, ButtonClientDataMembers->NumberOfButtons); 
			if (byte_count>0)
			{
				for(i=0;i<ButtonClientDataMembers->NumberOfButtons;i++)
				{
					ButtonClientDataMembers->Button[i] = buffer[i]=='1';
				}
			}
			else
			{
				fprintf(stderr, "%s: connection to button server lost.\n", Name);
				SDLNet_TCP_Close(tcpsock);
				SDLNet_FreeSocketSet(socketset);
				ButtonClientDataMembers->NumberOfButtons = 0;
				free(ButtonClientDataMembers->Button);
				free(buffer);
				return NULL;
			}
		}
		SDL_Delay(POLING_DELAY);
	}
	SDLNet_TCP_Close(tcpsock);
	SDLNet_FreeSocketSet(socketset);
	ButtonClientDataMembers->NumberOfButtons = 0;
	free(ButtonClientDataMembers->Button);
	free(buffer);

#endif

	printf("%s: button thread exiting.\n", Name);
	
	fflush(stdout);
	
	return NULL;
}

void empty(OBJECT_TYPE *o)
{
}

bool GetButtonsReady(OBJECT_TYPE *ButtonClientObject)
{
	BUTTON_CLIENT_DATA_MEMBERS *ButtonClientDataMembers = (BUTTON_CLIENT_DATA_MEMBERS *) ButtonClientObject->ptr1;
	return ButtonClientDataMembers->Ready;
}

int GetNumberOfButtons(OBJECT_TYPE *ButtonClientObject)
{
	BUTTON_CLIENT_DATA_MEMBERS *ButtonClientDataMembers = (BUTTON_CLIENT_DATA_MEMBERS *) ButtonClientObject->ptr1;
	return ButtonClientDataMembers->Ready?ButtonClientDataMembers->NumberOfButtons:0;
}

bool GetButton(OBJECT_TYPE *ButtonClientObject, int button_number, bool *status)
{
	BUTTON_CLIENT_DATA_MEMBERS *ButtonClientDataMembers = (BUTTON_CLIENT_DATA_MEMBERS *) ButtonClientObject->ptr1;
	if (!ButtonClientDataMembers->Ready || button_number<0 || button_number>=ButtonClientDataMembers->NumberOfButtons)
	{
		*status = false;
		return false;
	}
	else
	{
		*status = ButtonClientDataMembers->Button[button_number];
		return true;
	}
}

extern "C"
{

// This is the well known initialization function. Everything comes from here 
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];												// temporary transform storage 
	OBJECT_TYPE *ButtonClientObject;							// This object 
	pthread_t button_read_thread;								// thread handle for data input 
	BUTTON_CLIENT_DATA_MEMBERS *ButtonClientDataMembers;		// pointer to "ButtonClientDataMembers" structure 
	BUTTON_CLIENT_ACCESS_MEMBERS *ButtonClientAccessMembers;	// pointer to "ButtonClientAccessMembers" structure 
	char BUTTON_SERVER[MAX_NAME_SIZE] = {BUTTONS_SERVER};  
 	char *Name = App->appname;
	
	/* generate the "ButtonClientDataMembers" structure & attach it to the object */
	if ((ButtonClientDataMembers = (BUTTON_CLIENT_DATA_MEMBERS *) malloc (sizeof (BUTTON_CLIENT_DATA_MEMBERS))) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory tor ButtonClientDataMembers\n", Name);
		exit(1);
	}

	/* generate the "ButtonClientDataMembers" structure & attach it to the object */
	if ((ButtonClientAccessMembers = (BUTTON_CLIENT_ACCESS_MEMBERS *) malloc (sizeof (BUTTON_CLIENT_ACCESS_MEMBERS))) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory tor ButtonClientAccessMembers\n", Name);
		exit(1);
	}

	ButtonClientAccessMembers->GetButton = GetButton;
	ButtonClientAccessMembers->GetButtonsReady = GetButtonsReady;
	ButtonClientAccessMembers->GetNumberOfButtons = GetNumberOfButtons;

	ButtonClientDataMembers->Button = NULL;
	ButtonClientDataMembers->Ready = false;
	ButtonClientDataMembers->NumberOfButtons = 0;
		
	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		if (!strcmp (App->params[i][0], "server_address"))
		{
			sscanf(App->params[i][2], "%s", BUTTON_SERVER);
		}
	}

	strncpy(ButtonClientDataMembers->ServerName,BUTTON_SERVER,MAX_NAME_SIZE);

	ButtonClientObject = fl_add_child_to_parent_by_name(	App->universe_ptr,
														App->parent_object_name,
														App->appname);

	ButtonClientObject->ptr0 = (void *) ButtonClientAccessMembers;	// attach struct to object 
	ButtonClientObject->ptr1 = (void *) ButtonClientDataMembers;	// attach struct to object 

	/* set up callbacks */
	ButtonClientObject->soundfunc = empty;
	ButtonClientObject->sounding_on_flag = 0;
	ButtonClientObject->platformfunc = empty;
	ButtonClientObject->predrawfunc = empty;
	ButtonClientObject->drawfunc = empty;
	ButtonClientObject->trueShadowFunc = empty;
	ButtonClientObject->cheapShadowFunc = empty;
	ButtonClientObject->drawing_on_flag = 0;

	ButtonClientObject->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	ButtonClientObject->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	ButtonClientObject->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
	ButtonClientObject->bounding_volume_set->bvset[0]->p[0][1]=0.0;
	ButtonClientObject->bounding_volume_set->bvset[0]->p[0][2]=0.0;
	/* -x, -y, -z corner */
	ButtonClientObject->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
	ButtonClientObject->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
	ButtonClientObject->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
	/* +x, -y, -z corner */
	ButtonClientObject->bounding_volume_set->bvset[0]->p[2][0]=0.01;    
	ButtonClientObject->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
	ButtonClientObject->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
	/* -x, +y, -z corner */
	ButtonClientObject->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
	ButtonClientObject->bounding_volume_set->bvset[0]->p[3][1]=0.01;
	ButtonClientObject->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
	/* -x, -y, +z corner */
	ButtonClientObject->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
	ButtonClientObject->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
	ButtonClientObject->bounding_volume_set->bvset[0]->p[4][2]=0.01;

	/* Define bounding sphere for culling */
	ButtonClientObject->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	ButtonClientObject->bounding_volume_set->bvset[1]->p[0][0]=0.0;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[0][1]=0.0;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[0][2]=0.0;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[0][3]=1.0;
	/* Surface point */
	ButtonClientObject->bounding_volume_set->bvset[1]->p[1][0]=0.01;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[1][1]=0.01;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[1][2]=0.01;
	ButtonClientObject->bounding_volume_set->bvset[1]->p[1][3]=1.0;
    
	/* Define a bounding point */
	ButtonClientObject->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	ButtonClientObject->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	ButtonClientObject->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	ButtonClientObject->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	ButtonClientObject->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	/* attach to the console in a default place */
	PTemp[0] = 0.0;
	PTemp[1] = 0.0;
	PTemp[2] = 0.0;
	PTemp[3] = 0.0;
	PTemp[4] = 0.0;
	PTemp[5] = 0.0;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	ButtonClientObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	ButtonClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	ButtonClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	ButtonClientObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	ButtonClientObject->orginal_first_node->MpoParams->P->assign( PTemp );

	/* spawn a thread to read the data file */
	pthread_create(&button_read_thread, NULL, &ButtonThread, (void *) ButtonClientObject);
}

} // extern "C"
