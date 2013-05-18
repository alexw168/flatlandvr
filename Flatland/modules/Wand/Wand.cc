/*
Wand.cc  An app representing a interactive wand

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
#ifdef WIN32
#  include "Flatland.h"
#  include "libFlatland.h"
#else
#  include "flatland_app.h"
#endif
#include <BasicIntersect.h>
#include "Wand.h"
#include "../ButtonClient/ButtonClient.h"
#include "SObject.h"

#define BUTTON_CLIENT "ButtonClient"
#define POLING_DELAY 10
#define LEFT_HAND "modules/Wand/LeftHand.dat"
#define RIGHT_HAND "modules/Wand/RightHand.dat"
#define WAND_HANDLE "modules/Wand/WandHandle.dat"
#define WAND_DEFAULT "modules/Wand/DefaultHead.dat"


/* =======================================================================

Description:
    Wand provides an object that represents the wand in Flatland
         
Objects created when application is loaded:
    Wand only.
        
Object dependencies:
    TransformationNode for movement.  Wand depends on ButtonClient for
    information about the buttons.
        
Configurable options:
    top                 int     index of top button
    middle              int     index of middle button
    bottom              int     index of bottom button
    hand                int     -1 left, 0 none, 1 right
    x                   float   attachment position
    y                   float   attachment position
    z                   float   attachment position
    pitch               float   attachment position
    yaw                 float   attachment position
    roll                float   attachment position
    scale               float   size adjustment
    button_client       string  name of ButtonClient
    left_hand_data      string  data file for display of left hand
    right_hand_data     string  data file for display of right hand
    handle_data         string  data file for display of wand handle
    head_data           string  data file for display of wand head
    head                int     what head to display
    
Threads created:
    ReadWandData() - read data files.
    Sphere_interact_loop() - beamer thread.
        
Library requirements:
    Flatland basic setup

Location requirements:
    usually attached to TransformationNode.

Controls:
    Keyboard:
        none
    Menu:
        none
    Voice:
        none
    Beaming:
        none, but can be used for beaming

  Status:
    15-Sep-2001 - In need of more documentation.  Working.
    
======================================================================= */


int ReadButtons(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;
	OBJECT_TYPE *ButtonClient;
	BUTTON_CLIENT_ACCESS_MEMBERS *ButtonClientAccessMembers;
	
	// get to button client
	ButtonClient = WandMembers->ButtonClient;
	if (ButtonClient)
	{
		ButtonClientAccessMembers = (BUTTON_CLIENT_ACCESS_MEMBERS *) ButtonClient->ptr0;
		ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->BottomNumber,&(WandMembers->BottomButton));
		ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->MiddleNumber,&(WandMembers->MiddleButton));
		ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->TopNumber,&(WandMembers->TopButton));
	} 
	else
	{
		ButtonClient = WandMembers->ButtonClient = fl_find_object_by_name(WandObject->universe, WandMembers->ButtonClientName);
		if (ButtonClient)
		{
			ButtonClientAccessMembers = (BUTTON_CLIENT_ACCESS_MEMBERS *)ButtonClient->ptr0;
			ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->BottomNumber,&(WandMembers->BottomButton));
			ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->MiddleNumber,&(WandMembers->MiddleButton));
			ButtonClientAccessMembers->GetButton(ButtonClient,WandMembers->TopNumber,&(WandMembers->TopButton));
		}
		else 
		{
			WandMembers->BottomButton = false;
			WandMembers->MiddleButton = false;
			WandMembers->TopButton = false;
			return 1;
		}
	}
	return 0;
}

bool GetTopButton(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;
	ReadButtons(WandObject);
	return WandMembers->TopButton;
}

bool GetMiddleButton(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;
	ReadButtons(WandObject);
	return WandMembers->MiddleButton;
}

bool GetBottomButton(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;
	ReadButtons(WandObject);
	return WandMembers->BottomButton;
}

// Function to read in a data file 
void *ReadWandData (void *X)
{
	FILE *inFile;					// input file handle 
	OBJECT_TYPE *Wand;				// pointer to the object passed in (this obj) 
	WAND_MEMBERS *WandMembers;			// "global" structure pointer 
	char *Name;
	bool ok = true;
		
	Wand = (OBJECT_TYPE *) X;
	WandMembers = (WAND_MEMBERS *) Wand->ptr0;
	Name = Wand->name;

	fprintf(stdout,"%s: reading data file '%s'\n", Name, WandMembers->LeftHandDataFile);

	if (strlen (WandMembers->LeftHandDataFile) > 0)	// check for non-zero filename 
	{
		// open the file
		inFile = fopen (WandMembers->LeftHandDataFile, "rb");
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, WandMembers->LeftHandDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SObject *) WandMembers->LeftHand)->Read(inFile);
	}

	fprintf(stdout,"%s: reading data file '%s'\n", Name, WandMembers->RightHandDataFile);

	if (strlen (WandMembers->RightHandDataFile) > 0)	// check for non-zero filename 
	{
		// open the file
		inFile = fopen (WandMembers->RightHandDataFile, "rb");
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, WandMembers->RightHandDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SObject *) WandMembers->RightHand)->Read(inFile);
	}

	fprintf(stdout,"%s: reading data file '%s'\n", Name, WandMembers->WandHandleDataFile);

	if (strlen (WandMembers->WandHandleDataFile) > 0)	// check for non-zero filename 
	{
		// open the file
		inFile = fopen (WandMembers->WandHandleDataFile, "rb");
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, WandMembers->WandHandleDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SObject *) WandMembers->WandHandle)->Read(inFile);
	}

	fprintf(stdout,"%s: reading data file '%s'\n", Name, WandMembers->DefaultHeadDataFile);

	if (strlen (WandMembers->DefaultHeadDataFile) > 0)	// check for non-zero filename 
	{
		// open the file
		inFile = fopen (WandMembers->DefaultHeadDataFile, "rb");
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, WandMembers->DefaultHeadDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SObject *) WandMembers->DefaultHead)->Read(inFile);
	}

	WandMembers->DataReady = ok;

	return (NULL);
}

// Beam/collision (beamer) thread
void *Sphere_interact_loop (void *X)
{
        const int Beamer_vol = 2;       // wand bounding sphere
        OBJECT_TYPE *Wand;

	Wand = (OBJECT_TYPE *) X;

        while (!do_exit)
        {
          fl_short_sleep(TENTH_SECOND);
          sphere_interaction_test_all_objects(Wand->universe, Wand,
                                              Beamer_vol, GetTopButton (Wand));
        }

        return (NULL);
}

// Platform handler 
void platformfunc(OBJECT_TYPE *WandObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *WandObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *WandObject)
{
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={1.0,1.0,1.0,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	GLfloat emission[5][4]={	{1.0,0.0,0.0,1.0},
								{0.0,1.0,0.0,1.0},
								{0.0,0.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{0.0,0.0,0.0,1.0} };
	GLboolean lighting_flag = GL_FALSE;

	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;

	if ( !WandObject->drawing_on_flag || !WandObject->viewable_flag || !WandMembers->DataReady ) 
	{
		return;
	}
      
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, black );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, black );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, black );

	if (!glIsList(WandMembers->WandList[ Loop_Control->Window ]))
	{
		WandMembers->WandList[ Loop_Control->Window ] = glGenLists( 4 );
		if (glIsList(WandMembers->WandList[ Loop_Control->Window ]))
		{
			glNewList( WandMembers->WandList[ Loop_Control->Window ], GL_COMPILE );
			glPushMatrix();
			glRotated(180.0,0.0,1.0,0.0);
			((SObject *) WandMembers->WandHandle)->Draw();
			glPopMatrix();
			glEndList( );

			glNewList( WandMembers->WandList[ Loop_Control->Window ]+1, GL_COMPILE );
			glPushMatrix();
			glRotated(180.0,0.0,1.0,0.0);
			((SObject *) WandMembers->LeftHand)->Draw();
			glPopMatrix();
			glEndList( );

			glNewList( WandMembers->WandList[ Loop_Control->Window ]+2, GL_COMPILE );
			glPushMatrix();
			glRotated(180.0,0.0,1.0,0.0);
			((SObject *) WandMembers->RightHand)->Draw();
			glPopMatrix();
			glEndList( );

			glNewList( WandMembers->WandList[ Loop_Control->Window ]+3, GL_COMPILE );
			glPushMatrix();
			glRotated(180.0,0.0,1.0,0.0);
			((SObject *) WandMembers->DefaultHead)->Draw();
			glPopMatrix();
			glEndList( );

			// WandMembers->Wand could be deallocated now...
		}
	}

	if (glIsList(WandMembers->WandList[ Loop_Control->Window ]))
	{
		glCallList( WandMembers->WandList[ Loop_Control->Window ] );
		if (WandMembers->Hand<0) {
			glCallList( WandMembers->WandList[ Loop_Control->Window ]+1 );
		}		
		if (WandMembers->Hand>0) {
			glCallList( WandMembers->WandList[ Loop_Control->Window ]+2 );
		}		
		glCallList( WandMembers->WandList[ Loop_Control->Window ]+3+WandMembers->Head );
	}

	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ambient);
		glTranslated(0,0,-0.05);
		if (WandMembers->TopButton) 
		{
			glutSolidCube(0.01);
		}
		glTranslated(0,-0.03,0);
		if (WandMembers->MiddleButton) 
		{
			glutSolidCube(0.01);
		}
		glTranslated(0,-0.03,0);
		if (WandMembers->BottomButton) 
		{
			glutSolidCube(0.01);
		}
	glPopMatrix();
	
	// draw the beam */
	if ( WandObject->beam_on_flag ) 
	{
		lighting_flag = glIsEnabled(GL_LIGHTING) ;
		if ( lighting_flag )
		{
			glDisable(GL_LIGHTING);
		}
		glColor3f(emission[WandMembers->BeamColor][0],emission[WandMembers->BeamColor][1],emission[WandMembers->BeamColor][2]);
  
		glLineWidth(3.0);
		glBegin(GL_LINES) ;
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,-0.5);
		glEnd();
		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,-0.5);
			glVertex3f(0.0,0.0,-2.0);
		glEnd();
		glLineWidth(1.0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,-2.0);
			glVertex3f(0.0,0.0,-2000.0);
		glEnd();
		if ( lighting_flag )
		{
			glEnable(GL_LIGHTING);
		}  
	}
    
	glColor3f(0,0,0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission[4]) ;
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS,0.0);
}

// true shadow callback 
void trueShadowFunc(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers;			// "global" structure pointer 
	WandMembers = (WAND_MEMBERS *) WandObject->ptr0;

	if ( !WandObject->drawing_on_flag || !WandObject->viewable_flag || !WandMembers->DataReady ) 
	{
		return;
	}
      
	if (glIsList(WandMembers->WandList[ Loop_Control->Window ]))
	{
		glCallList( WandMembers->WandList[ Loop_Control->Window ] );
		if (WandMembers->Hand<0) {
			glCallList( WandMembers->WandList[ Loop_Control->Window ]+1 );
		}		
		if (WandMembers->Hand>0) {
			glCallList( WandMembers->WandList[ Loop_Control->Window ]+2 );
		}		
		glCallList( WandMembers->WandList[ Loop_Control->Window ]+3+WandMembers->Head );
	}
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *WandObject)
{
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *WandObject)
{
}

// predraw callback 
void animatefunc(OBJECT_TYPE *WandObject)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;
	
	ReadButtons( WandObject );

	WandObject->beam_on_flag = 0;
	WandMembers->BeamColor = ( WandMembers->BeamColor + 1 ) % 5;
}

int SetHand(OBJECT_TYPE *WandObject, int hand)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;

	if (hand<0) 
	{
		WandMembers->Hand = -1;
	}
	else
	{
		if (hand>0)
		{
			WandMembers->Hand = 1;
		}
		else
		{
			WandMembers->Hand = 0;
		}
	}
		
	return 0;
}

int SetHead(OBJECT_TYPE *WandObject, int head)
{
	WAND_MEMBERS *WandMembers = (WAND_MEMBERS *) WandObject->ptr0;

	if (head<0) 
	{
		WandMembers->Head = 0;
		return 1;
	}
	else
	{
		if (head>0)
		{
			WandMembers->Head = 0;
			return 1;
		}
		else
		{
			WandMembers->Head = head;
		}
	}
		
	return 0;
}

extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];						/* temporary transform storage */
	GLfloat WAND_SCALE = 1.0;			/* default wand size (overriden from config) */
	GLfloat WAND_X = 0.0;				/* default wand x location */
	GLfloat WAND_Y = 0.0;				/* default wand y location (also set below) */
	GLfloat WAND_Z = 0.0;				/* default wand z location */
	GLfloat WAND_ROLL = 0.0;			/* default wand roll */
	GLfloat WAND_PITCH = 0.0;			/* default wand pitch */
	GLfloat WAND_YAW = 0.0;				/* default wand yaw */
	int WAND_HAND = 0;		  
	int WAND_HEAD = 0;		  
	int WAND_TOP = -1;		  
	int WAND_MIDDLE = -1;		  
	int WAND_BOTTOM = -1;
	OBJECT_TYPE *Wand;				/* This object */
	pthread_t data_read_thread;		/* thread handle for data input */
	pthread_t wand_beam_thread;		/* thread handle for beaming */
	WAND_MEMBERS *WandMembers;			/* pointer to "WandMembers" structure */
	char LEFT_FILE[MAX_NAME_SIZE] = {LEFT_HAND};
	char RIGHT_FILE[MAX_NAME_SIZE] = {RIGHT_HAND};
	char HANDLE_FILE[MAX_NAME_SIZE] = {WAND_HANDLE};
	char HEAD_FILE[MAX_NAME_SIZE] = {WAND_DEFAULT};
	char WAND_BUTTON_CLIENT[MAX_NAME_SIZE] = {BUTTON_CLIENT};  
	CORDORD *Position;
	char *Name = App->appname;
	bool known;

	assert(App);
			 
	/* generate the "WandMembers" structure & attach it to the object */
	if ((WandMembers = (WAND_MEMBERS *) malloc (sizeof (WAND_MEMBERS))) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	if ((WandMembers->LeftHand = new SObject) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	if ((WandMembers->RightHand = new SObject) == NULL)
	{		 	 
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	if ((WandMembers->WandHandle = new SObject) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	if ((WandMembers->DefaultHead = new SObject) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	if ((WandMembers->WandList = new GLuint[ Loop_Control->NumberOfWindows ] ) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for WandMembers\n", Name);
		exit(1);
	}

	Position = &WandMembers->Position;
	WandMembers->ButtonClient = NULL;
	WandMembers->DataReady = false;
	WandMembers->Hand = 0;
	WandMembers->Head = 0;
	WandMembers->Scale = 1.0f;
	WandMembers->GetTopButton = GetTopButton;
	WandMembers->GetMiddleButton = GetMiddleButton;
	WandMembers->GetBottomButton = GetBottomButton;
	WandMembers->DataReady = false;		/* no data has been read yet */
	WandMembers->TopButton = false;
	WandMembers->MiddleButton = false;
	WandMembers->BottomButton = false;
	WandMembers->BeamColor = 0;

	for( i=0; i<Loop_Control->NumberOfWindows; i++)
	{
		WandMembers->WandList[i]=0;
	}

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "comment"))
		{
			known = true;
		}
		if (!strcmp (App->params[i][0], "top"))
		{
			sscanf(App->params[i][2], "%d", &WAND_TOP);
			known = true;
		}
		if (!strcmp (App->params[i][0], "middle"))
		{
			sscanf(App->params[i][2], "%d", &WAND_MIDDLE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "bottom"))
		{
			sscanf(App->params[i][2], "%d", &WAND_BOTTOM);
			known = true;
		}
		if (!strcmp (App->params[i][0], "hand"))
		{
			sscanf(App->params[i][2], "%d", &WAND_HAND);
			known = true;
		}
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &WAND_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &WAND_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &WAND_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &WAND_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &WAND_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &WAND_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale"))
		{
			sscanf(App->params[i][2], "%f", &WAND_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "button_client"))
		{
			sscanf(App->params[i][2], "%s", WAND_BUTTON_CLIENT);
			known = true;
		}
		if (!strcmp (App->params[i][0], "left_hand_data"))
		{
			sscanf(App->params[i][2], "%s", LEFT_FILE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "right_hand_data"))
		{
			sscanf(App->params[i][2], "%s", RIGHT_FILE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "handle_data"))
		{
			sscanf(App->params[i][2], "%s", HANDLE_FILE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "head_data"))
		{
			sscanf(App->params[i][2], "%s", HEAD_FILE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "head"))
		{
			sscanf(App->params[i][2], "%d", &WAND_HEAD);
			known = true;
		}
		
		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

	strncpy(WandMembers->ButtonClientName,WAND_BUTTON_CLIENT,MAX_NAME_SIZE);
	strncpy(WandMembers->LeftHandDataFile,LEFT_FILE,MAX_NAME_SIZE);
	strncpy(WandMembers->RightHandDataFile,RIGHT_FILE,MAX_NAME_SIZE);
	strncpy(WandMembers->WandHandleDataFile,HANDLE_FILE,MAX_NAME_SIZE);
	strncpy(WandMembers->DefaultHeadDataFile,HEAD_FILE,MAX_NAME_SIZE);

	Position->x = WAND_X;
	Position->y = WAND_Y;
	Position->z = WAND_Z;
	Position->roll = WAND_ROLL;
	Position->pitch = WAND_PITCH;
	Position->yaw = WAND_YAW;
		
	WandMembers->Scale = WAND_SCALE;
	WandMembers->Hand = WAND_HAND;
	WandMembers->Head = WAND_HEAD;
	WandMembers->TopNumber = WAND_TOP;
	WandMembers->MiddleNumber = WAND_MIDDLE;
	WandMembers->BottomNumber = WAND_BOTTOM;

	Wand = fl_add_child_to_parent_by_name(	App->universe_ptr,
										App->parent_object_name,
										App->appname);

	Wand->ptr0 = (void *) WandMembers;	/* attach struct to object */

	/* set up callbacks */
	Wand->soundfunc = soundfunc;
	Wand->sounding_on_flag = 0;
	Wand->platformfunc = platformfunc;
	Wand->predrawfunc = predrawfunc;
	Wand->drawfunc = drawfunc;
	Wand->trueShadowFunc = trueShadowFunc;
	Wand->cheapShadowFunc = cheapShadowFunc;
	Wand->drawing_on_flag = 1 ;
	Wand->pregraphicsfunc = animatefunc;

	Wand->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	Wand->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	Wand->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	Wand->bounding_volume_set->bvset[0]->p[0][1] = -0.07 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[0][2] = 0.03 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[0][3] =  1.0;
	/* -x, -y, -z corner */
	Wand->bounding_volume_set->bvset[0]->p[1][0] = -0.05 * WAND_SCALE;    
	Wand->bounding_volume_set->bvset[0]->p[1][1] = -0.17 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[1][2] = -0.02 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[1][3] =  1.0;
	/* +x, -y, -z corner */
	Wand->bounding_volume_set->bvset[0]->p[2][0] = 0.05 * WAND_SCALE;    
	Wand->bounding_volume_set->bvset[0]->p[2][1] = -0.17 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[2][2] = -0.02 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[2][3] =  1.0;
	/* -x, +y, -z corner */
	Wand->bounding_volume_set->bvset[0]->p[3][0] = -0.05 * WAND_SCALE;    
	Wand->bounding_volume_set->bvset[0]->p[3][1] = 0.03 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[3][2] = -0.02 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[3][3] =  1.0;
	/* -x, -y, +z corner */
	Wand->bounding_volume_set->bvset[0]->p[4][0] = -0.05 * WAND_SCALE;    
	Wand->bounding_volume_set->bvset[0]->p[4][1] = -0.17 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[4][2] = 0.08 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[0]->p[4][3] =  1.0;

	/* Define bounding sphere for culling */
	Wand->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	Wand->bounding_volume_set->bvset[1]->p[0][0] =  0.0;
	Wand->bounding_volume_set->bvset[1]->p[0][1] = -0.07 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[1]->p[0][2] =  0.03 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[1]->p[0][3] =  1.0;
	/* Surface point */
	Wand->bounding_volume_set->bvset[1]->p[1][0] = 0.07 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[1]->p[1][1] = 0.00;
	Wand->bounding_volume_set->bvset[1]->p[1][2] = 0.10 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	/* Define a bounding sphere for beaming */
	Wand->bounding_volume_set->bvset[2]->npoints = 2 ;
	/* Center point */
	Wand->bounding_volume_set->bvset[2]->p[0][0] =  0.0 ;
	Wand->bounding_volume_set->bvset[2]->p[0][1] =  0.0 ;
	Wand->bounding_volume_set->bvset[2]->p[0][2] =  0.0;
	Wand->bounding_volume_set->bvset[2]->p[0][3] =  1.0 ;
	/* Surface point */
	Wand->bounding_volume_set->bvset[2]->p[1][0] = 0.00;
	Wand->bounding_volume_set->bvset[2]->p[1][1] = 0.00;
	Wand->bounding_volume_set->bvset[2]->p[1][2] =-0.10 * WAND_SCALE;
	Wand->bounding_volume_set->bvset[2]->p[1][3] = 1.0;
  
	/* attach to the console in a default place */
	PTemp[0] = WAND_X;
	PTemp[1] = WAND_Y;
	PTemp[2] = WAND_Z;
	PTemp[3] = WAND_PITCH;
	PTemp[4] = WAND_YAW;
	PTemp[5] = WAND_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	Wand->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	Wand->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	Wand->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	Wand->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	Wand->orginal_first_node->MpoParams->P->assign( PTemp );

	/* spawn a thread to read the data file */
	pthread_create(&data_read_thread, NULL, &ReadWandData, (void *) Wand);
        pthread_create(&wand_beam_thread, NULL, &Sphere_interact_loop, (void *) Wand);
}

} // extern "C"
