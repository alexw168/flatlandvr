/*
TransformationNode.cc  An app for a transformation node object

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

/* include the master Flatland library */
#ifdef WIN32
#  include "Flatland.h"
#  include "libFlatland.h"
#else
#  include "flatland_app.h"
#endif
#include "TransformationNodeData.h"

#define TRACKER_CLIENT "TrackerClient"


/* =======================================================================

Description:
    a TransformationNode gets position information from a TrackerClient
    and adjusts its poition to the reported one via its Mpo parameters.
    Anything attached to the TransformationNode will move with it.
        
Objects created when application is loaded:
    TransformationNode - the actual object.
        
Object dependencies:
    TransformationNode depends on a TrackerClient to provide position
    information.
        
Configurable options:
    x                   float       default position
    y                   float       default position
    z                   float       default position
    pitch               float       default position
    yaw                 float       default position
    roll                float       default position
    tracker_client      string      name of object providing position information
    transducer          int         index of transducer in TrackerClient
    move_child          int(bool)   if set false the inverse translations are used
    use_tracking        int(bool)   should tracking be used initially

Threads created:
    none
    
Library requirements:
    Flatland basic setup

Location requirements:
    attached after ascociated TackerClient, but not necesarily to any
    particular point.

Controls:
    Keyboard:
        'a' - toggle display of axes
        't' - toggle use of tracking information
    Menu:
        see keyboard input
    Voice:
        none
    Beaming:
        none

  Status:
    16-Sep-2001 - In need of more documentation, otherwise done.  Well,
    actually it needs cleanup of modifications since creation.

======================================================================= */


int SetTracking(OBJECT_TYPE *TransformationNodeObject, bool tracking)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	TransformationNodeDataMembers->UseTracking = tracking;
	
	return 0;
}	

int GetTracking(OBJECT_TYPE *TransformationNodeObject, bool *tracking)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	 *tracking = TransformationNodeDataMembers->UseTracking;
	
	return 0;
}

int SetTrackerClient(OBJECT_TYPE *TransformationNodeObject, char *TrackerClientName)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	if (TransformationNodeDataMembers->TrackerClientSet)
	{
		strcpy(TransformationNodeDataMembers->TrackerClientName,TrackerClientName);
		TransformationNodeDataMembers->TrackerClient = NULL;
	}
	else
	{
		TransformationNodeDataMembers->TrackerClientSet = true;
		strcpy(TransformationNodeDataMembers->TrackerClientName,TrackerClientName);
		TransformationNodeDataMembers->TrackerClient = NULL;
	}
	
	return 0;
}

int GetTrackerClient(OBJECT_TYPE *TransformationNodeObject, char *TrackerClientName)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	if (TransformationNodeDataMembers->TrackerClientSet)
	{
		strcpy(TrackerClientName,TransformationNodeDataMembers->TrackerClientName);
	}
	else
	{
		strcpy(TrackerClientName,"");
	}
	
	return 0;
}

int SetTransducer(OBJECT_TYPE *TransformationNodeObject, int transducer)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	if (TransformationNodeDataMembers->TransducerSet)
	{
		TransformationNodeDataMembers->TransducerNumber = transducer;
	}

	else
	{
		TransformationNodeDataMembers->TransducerSet = true;
		TransformationNodeDataMembers->TransducerNumber = transducer;
	}
	
	return 0;
}

int GetTransducer(OBJECT_TYPE *TransformationNodeObject, int *transducer)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	
	if (TransformationNodeDataMembers->TransducerSet)
	{
		*transducer = TransformationNodeDataMembers->TransducerNumber;
	}
	else
	{
		*transducer = -1;
	}
	
	return 0;
}

int DisconnectTracking(OBJECT_TYPE *TransformationNodeObject)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	CORDORD *Default = &TransformationNodeDataMembers->DefaultPosition;
	CORDORD *Position = &TransformationNodeDataMembers->Position;

	if (TransformationNodeDataMembers->TrackerClient)
	{
		TransformationNodeDataMembers->TrackerClient = NULL;
		
		Position->x = Default->x;
		Position->y = Default->y;
		Position->z = Default->z;
		Position->roll = Default->roll;
		Position->pitch = Default->pitch;
		Position->yaw = Default->yaw;
	}

	return 0;
}

int SetDefaultPosition(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	CORDORD *DefaultPosition = &TransformationNodeDataMembers->DefaultPosition;
	
	DefaultPosition->x = Position->x;
	DefaultPosition->y = Position->y;
	DefaultPosition->z = Position->z;
	DefaultPosition->roll = Position->roll;
	DefaultPosition->pitch = Position->pitch;
	DefaultPosition->yaw = Position->yaw;
	 
	return 0;
}

int GetDefaultPosition(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	CORDORD *DefaultPosition = &TransformationNodeDataMembers->DefaultPosition;
	
	Position->x = DefaultPosition->x;
	Position->y = DefaultPosition->y;
	Position->z = DefaultPosition->z;
	Position->roll = DefaultPosition->roll;
	Position->pitch = DefaultPosition->pitch;
	Position->yaw = DefaultPosition->yaw;
	 
	return 0;
}

int GetPosition(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	CORDORD *CurrentPosition = &TransformationNodeDataMembers->Position;
	
	Position->x = CurrentPosition->x;
	Position->y = CurrentPosition->y;
	Position->z = CurrentPosition->z;
	Position->roll = CurrentPosition->roll;
	Position->pitch = CurrentPosition->pitch;
	Position->yaw = CurrentPosition->yaw;
	 
	return 0;
}

bool ReadTracker(OBJECT_TYPE *TransformationNodeObject)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	OBJECT_TYPE *TrackerClient;
	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers;
	CORDORD *Default = &TransformationNodeDataMembers->DefaultPosition;
	CORDORD *Position = &TransformationNodeDataMembers->Position;
        CORDORD *TrackerScaling = &TransformationNodeDataMembers->TrackerScaling;
//	char *Name = TransformationNodeObject->name;
	bool retval = true;
	
	if (TransformationNodeDataMembers->UseTracking)
	{
		// get to tracker client
		TrackerClient = TransformationNodeDataMembers->TrackerClient;
		if ( !TrackerClient )
		{
			TrackerClient = TransformationNodeDataMembers->TrackerClient = fl_find_object_by_name(TransformationNodeObject->universe, TransformationNodeDataMembers->TrackerClientName);
		}

		if (TrackerClient)
		{
			TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) TrackerClient->ptr0;
			retval = TrackerClientMembers->GetPosition(TrackerClient,TransformationNodeDataMembers->TransducerNumber,&TransformationNodeDataMembers->Position);
                        // scale
                        Position->x *= TrackerScaling->x;
                        Position->y *= TrackerScaling->y;
                        Position->z *= TrackerScaling->z;
                        Position->roll *= TrackerScaling->roll;
                        Position->pitch *= TrackerScaling->pitch;
                        Position->yaw *= TrackerScaling->yaw;
		} 
		else
		{
			retval = false;
		}
	}
	else 
	{
		retval = false;
	}

	if ( !retval )
	{
		Position->x = Default->x * TrackerScaling->x;
		Position->y = Default->y * TrackerScaling->y;
		Position->z = Default->z * TrackerScaling->z;
		Position->roll = Default->roll * TrackerScaling->roll;
		Position->pitch = Default->pitch * TrackerScaling->pitch;
		Position->yaw = Default->yaw * TrackerScaling->yaw;
	}	
        else
        {
                if (!TransformationNodeDataMembers->UseTranslation)
                {
                        Position->x = Default->x * TrackerScaling->x;
                        Position->y = Default->y * TrackerScaling->y;
                        Position->z = Default->z * TrackerScaling->z;
                }
                if (!TransformationNodeDataMembers->UseRotation)
                {
		        Position->roll = Default->roll * TrackerScaling->roll;
		        Position->pitch = Default->pitch * TrackerScaling->pitch;
		        Position->yaw = Default->yaw * TrackerScaling->yaw;
                }
        }
	return retval;
}

// Platform handler 
void platformfunc(OBJECT_TYPE *TransformationNodeObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *TransformationNodeObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *TransformationNodeObject)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;

	TransformationNodeDataMembers->PreDrawn = false;
	GLboolean lighting_flag = GL_FALSE;
	
	// draw what?

	// draw axes on demand...	
	if ( TransformationNodeDataMembers->DisplayAxes ) 
	{
		lighting_flag = glIsEnabled(GL_LIGHTING) ;
		if ( lighting_flag )
		{
			glDisable(GL_LIGHTING);
		}
		glColor3f(1,1,1);
  
		glLineWidth(1.0);
		
		glBegin(GL_LINES);
			// x axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.24,0.01,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.24,-0.01,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.26,0.0,0.0);
			glVertex3f(0.27,0.015,0.0);
			glVertex3f(0.26,0.015,0.0);
			glVertex3f(0.27,0.0,0.0);
			// y axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.24,0.01);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.24,-0.01);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.265,0.0);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.26,0.015);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.27,0.015);
			// z axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(0.01,0.0,0.24);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(-0.01,0.0,0.24);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(0.0,0.0,0.27);
			glVertex3f(0.0,0.0,0.26);
			glVertex3f(0.015,0.0,0.27);
			glVertex3f(0.0,0.0,0.26);
			glVertex3f(0.015,0.0,0.27);
			glVertex3f(0.015,0.0,0.26);
		glEnd();

		if ( lighting_flag )
		{
			glEnable(GL_LIGHTING);
		}  
	}
    

}

// true shadow callback 
void trueShadowFunc(OBJECT_TYPE *TransformationNodeObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *TransformationNodeObject)
{
	// shadow of what?
}

// cheap shadow callback 
void predrawfunc(OBJECT_TYPE *TransformationNodeObject)
{
	// shadow of what?
}

// predraw callback 
void animatefunc(OBJECT_TYPE *TransformationNodeObject)
{
  if (TransformationNodeObject->original_owner_flag == true)
  {
	float PTemp[9];					// temporary transform storage */
//	TRANSFORMATION_NODE_ACCESS_MEMBERS *TransformationNodeMembers = (TRANSFORMATION_NODE_ACCESS_MEMBERS *) TransformationNodeObject->ptr0;
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;
	CORDORD *Position = &TransformationNodeDataMembers->Position;
//	CORDORD *DefaultPosition = &TransformationNodeDataMembers->DefaultPosition;
//	char *Name = TransformationNodeObject->name;
	
	// only predraw once for the "first" window
//	if (current_main_window == 0)
//	{
//		if (!TransformationNodeDataMembers->PreDrawn)
//		{
			// TransformationNodeDataMembers->Predrawn = true;
			//
			// I need a reliable way to reset this flag, if the 
			// object is culled from the list of displayed 
			// objects drawfunc is not going to work as a 
			// reset point...

			ReadTracker(TransformationNodeObject);

			PTemp[0] = TransformationNodeDataMembers->MoveChild ? Position->x : -Position->x;
			PTemp[1] = TransformationNodeDataMembers->MoveChild ? Position->y : -Position->y;
			PTemp[2] = TransformationNodeDataMembers->MoveChild ? Position->z : -Position->z;
			PTemp[3] = TransformationNodeDataMembers->MoveChild ? Position->pitch : -Position->pitch;
			PTemp[4] = TransformationNodeDataMembers->MoveChild ? Position->yaw : -Position->yaw;
			PTemp[5] = TransformationNodeDataMembers->MoveChild ? Position->roll : -Position->roll;
			PTemp[6] = 1.0;
			PTemp[7] = 1.0;
			PTemp[8] = 1.0;
			TransformationNodeObject->orginal_first_node->MpoParams->P->assign(PTemp,TransformationNodeObject);

			if (TransformationNodeDataMembers->MoveChild) 
			{
				TransformationNodeObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[1] = ROTATE010;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[2] = ROTATE100;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[3] = ROTATE001;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[4] = SCALE;	
			}
			else
			{
				TransformationNodeObject->orginal_first_node->MpoParams->Po[4] = TRANSLATE;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[3] = ROTATE010;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[2] = ROTATE100;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[1] = ROTATE001;	
				TransformationNodeObject->orginal_first_node->MpoParams->Po[0] = SCALE;	
			}
//		}
//	}
  }
}

/* menu control callback */
void MenuCallback(OBJECT_TYPE *TransformationNodeObject, char key)
{
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) TransformationNodeObject->ptr1;

	switch (key)
	{
    case 'a':   // toggle display axes
		TransformationNodeDataMembers->DisplayAxes = !TransformationNodeDataMembers->DisplayAxes;
		break;

    case 't':   // toggle use tracking
		TransformationNodeDataMembers->UseTracking = !TransformationNodeDataMembers->UseTracking;
		break;

    case 'T':   // toggle use traslation tracking
		TransformationNodeDataMembers->UseTranslation = !TransformationNodeDataMembers->UseTranslation;
		break;

    case 'R':   // toggle use rotation tracking
		TransformationNodeDataMembers->UseRotation = !TransformationNodeDataMembers->UseRotation;
		break;

	}
}

extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init(APPTYPE *App)
{
	int i;
	float PTemp[9];						/* temporary transform storage */
	GLfloat TRANSFORMATION_NODE_X = 0.0;				/* default node x location */
	GLfloat TRANSFORMATION_NODE_Y = 0.0;				/* default node y location (also set below) */
	GLfloat TRANSFORMATION_NODE_Z = 0.0;				/* default node z location */
        GLfloat TRANSFORMATION_NODE_X_SCALE = 1.0;              /* default tracker scaling, X direction */
        GLfloat TRANSFORMATION_NODE_Y_SCALE = 1.0;              /* default tracker scaling, Y direction */
        GLfloat TRANSFORMATION_NODE_Z_SCALE = 1.0;              /* default tracker scaling, Z direction */
	GLfloat TRANSFORMATION_NODE_ROLL = 0.0;			/* default node roll */
	GLfloat TRANSFORMATION_NODE_PITCH = 0.0;			/* default node pitch */
	GLfloat TRANSFORMATION_NODE_YAW = 0.0;				/* default node yaw */
        GLfloat TRANSFORMATION_NODE_ROLL_SCALE = 1.0;           /* default tracker scaling, roll direction */
        GLfloat TRANSFORMATION_NODE_PITCH_SCALE = 1.0;          /* default tracker scaling, pitch direction */
        GLfloat TRANSFORMATION_NODE_YAW_SCALE = 1.0;            /* default tracker scaling, yaw direction */
	int TRANSFORMATION_NODE_TRANSDUCER = -1;		  
	int TRANSFORMATION_NODE_MOVE_CHILD = 1;		  
	int TRANSFORMATION_NODE_USE_TRACKING = 0;		  
	int TRANSFORMATION_NODE_USE_TRANSLATION = 1;		  
	int TRANSFORMATION_NODE_USE_ROTATION= 1;		  
	OBJECT_TYPE *TransformationNode;					/* This object */
	TRANSFORMATION_NODE_ACCESS_MEMBERS *TransformationNodeMembers;	/* pointer to "TransformationNodeMembers" structure */
	TRANSFORMATION_NODE_DATA_MEMBERS *TransformationNodeDataMembers;	/* pointer to "TransformationNodeDataMembers" structure */
	char TRANSFORMATION_NODE_TRACKER_CLIENT[MAX_NAME_SIZE] = {TRACKER_CLIENT};  
	CORDORD *Position;
	CORDORD *Default;
        CORDORD *TrackerScaling;
	char *Name = App->appname;
	bool known;
		 
	assert(App);
			 
	/* generate the "TransformationNodeMembers" structure & attach it to the object */
	if ((TransformationNodeMembers = (TRANSFORMATION_NODE_ACCESS_MEMBERS *) malloc (sizeof (TRANSFORMATION_NODE_ACCESS_MEMBERS))) == NULL)
	{
		printf ("ERROR: failed to initialize memory for wand TransformationNodeMembers\n");
		exit(1);
	}

	/* generate the "TransformationNodeDataMembers" structure & attach it to the object */
	if ((TransformationNodeDataMembers = (TRANSFORMATION_NODE_DATA_MEMBERS *) malloc (sizeof (TRANSFORMATION_NODE_DATA_MEMBERS))) == NULL)
	{
		printf ("ERROR: failed to initialize memory for wand TransformationNodeMembers\n");
		exit(1);
	}

	Position = &TransformationNodeDataMembers->Position;
	Default = &TransformationNodeDataMembers->DefaultPosition;
        TrackerScaling = &TransformationNodeDataMembers->TrackerScaling;
	
	TransformationNodeDataMembers->DisplayAxes = false;

	TransformationNodeDataMembers->TrackerClient = NULL;
	TransformationNodeMembers->SetTracking = SetTracking;
	TransformationNodeMembers->GetTracking = GetTracking;
	TransformationNodeMembers->SetTrackerClient = SetTrackerClient;
	TransformationNodeMembers->GetTrackerClient = GetTrackerClient;
	TransformationNodeMembers->SetTransducer = SetTransducer;
	TransformationNodeMembers->GetTransducer = GetTransducer;
	TransformationNodeMembers->DisconnectTracking = DisconnectTracking;
	TransformationNodeMembers->SetDefaultPosition = SetDefaultPosition;
	TransformationNodeMembers->GetDefaultPosition = GetDefaultPosition;
	TransformationNodeMembers->GetPosition = GetPosition;

	TransformationNodeDataMembers->TransducerSet = false;
	TransformationNodeDataMembers->TransducerNumber = -1;
	TransformationNodeDataMembers->MoveChild = false;
	TransformationNodeDataMembers->UseTracking = false;
	TransformationNodeDataMembers->UseTranslation = true;
	TransformationNodeDataMembers->UseRotation = true;
	TransformationNodeDataMembers->PreDrawn = false;
	
	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "comment"))
		{
			known = true;
		}
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_x"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_X_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_y"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_Y_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_z"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_Z_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_pitch"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_PITCH_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_yaw"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_YAW_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale_roll"))
		{
			sscanf(App->params[i][2], "%f", &TRANSFORMATION_NODE_ROLL_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "tracker_client"))
		{
			sscanf(App->params[i][2], "%s", TRANSFORMATION_NODE_TRACKER_CLIENT);
			known = true;
		}
		if (!strcmp (App->params[i][0], "transducer"))
		{
			sscanf(App->params[i][2], "%d", &TRANSFORMATION_NODE_TRANSDUCER);
			TransformationNodeDataMembers->TransducerSet = true;
			known = true;
		}
		if (!strcmp (App->params[i][0], "move_child"))
		{
			sscanf(App->params[i][2], "%d", &TRANSFORMATION_NODE_MOVE_CHILD);
			known = true;
		}
		if (!strcmp (App->params[i][0], "use_tracking"))
		{
			sscanf(App->params[i][2], "%d", &TRANSFORMATION_NODE_USE_TRACKING);
			known = true;
		}
		if (!strcmp (App->params[i][0], "use_translation"))
		{
			sscanf(App->params[i][2], "%d", &TRANSFORMATION_NODE_USE_TRANSLATION);
			known = true;
		}
		if (!strcmp (App->params[i][0], "use_rotation"))
		{
			sscanf(App->params[i][2], "%d", &TRANSFORMATION_NODE_USE_ROTATION);
			known = true;
		}
		
		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

	strncpy(TransformationNodeDataMembers->TrackerClientName,TRANSFORMATION_NODE_TRACKER_CLIENT,MAX_NAME_SIZE);

	TransformationNodeDataMembers->MoveChild = TRANSFORMATION_NODE_MOVE_CHILD != 0;
	TransformationNodeDataMembers->UseTracking = TRANSFORMATION_NODE_USE_TRACKING != 0;
	TransformationNodeDataMembers->UseTranslation = TRANSFORMATION_NODE_USE_TRANSLATION != 0;
	TransformationNodeDataMembers->UseRotation = TRANSFORMATION_NODE_USE_ROTATION != 0;

	Default->x = Position->x = TRANSFORMATION_NODE_X;
	Default->y = Position->y = TRANSFORMATION_NODE_Y;
	Default->z = Position->z = TRANSFORMATION_NODE_Z;
	Default->roll = Position->roll = TRANSFORMATION_NODE_ROLL;
	Default->pitch = Position->pitch = TRANSFORMATION_NODE_PITCH;
	Default->yaw = Position->yaw = TRANSFORMATION_NODE_YAW;
        TrackerScaling->x = TRANSFORMATION_NODE_X_SCALE;
        TrackerScaling->y = TRANSFORMATION_NODE_Y_SCALE;
        TrackerScaling->z = TRANSFORMATION_NODE_Z_SCALE;
        TrackerScaling->roll = TRANSFORMATION_NODE_ROLL_SCALE;
        TrackerScaling->pitch = TRANSFORMATION_NODE_PITCH_SCALE;
        TrackerScaling->yaw = TRANSFORMATION_NODE_YAW_SCALE;
		
	TransformationNodeDataMembers->TransducerNumber = TRANSFORMATION_NODE_TRANSDUCER;

	TransformationNode = fl_add_child_to_parent_by_name(	App->universe_ptr,
										App->parent_object_name,
										App->appname);

	TransformationNode->ptr0 = (void *) TransformationNodeMembers;	/* attach struct to object */
	TransformationNode->ptr1 = (void *) TransformationNodeDataMembers;	/* attach struct to object */

	/* set up callbacks */
	TransformationNode->soundfunc = soundfunc;
	TransformationNode->sounding_on_flag = 0;
	TransformationNode->platformfunc = platformfunc;
	TransformationNode->pregraphicsfunc = animatefunc;
	TransformationNode->predrawfunc = predrawfunc;
	TransformationNode->drawfunc = drawfunc;
	TransformationNode->trueShadowFunc = trueShadowFunc;
	TransformationNode->cheapShadowFunc = cheapShadowFunc;
	TransformationNode->drawing_on_flag = 1 ;

	//// Menu Setup ////

	/* set up the control structure (three subcontrols under one) */
	/* allows control of the object from the keyboard, menues, and voice */
	TransformationNode->object_controls = fl_makeControlsSet(4);

	/* Define spin/nospin controls for cube */
	strcpy (TransformationNode->object_controls->controls[0]->menuname, "Toggle Axes Display");
	strcpy (TransformationNode->object_controls->controls[0]->voicename, "Axes");
	TransformationNode->object_controls->controls[0]->keystroke = 'a';
	TransformationNode->object_controls->controls[0]->control_function = MenuCallback;

	strcpy (TransformationNode->object_controls->controls[1]->menuname, "Toggle Tracking");
	strcpy (TransformationNode->object_controls->controls[1]->voicename, "Tracking");
	TransformationNode->object_controls->controls[1]->keystroke = 't';
	TransformationNode->object_controls->controls[1]->control_function = MenuCallback;

	strcpy (TransformationNode->object_controls->controls[2]->menuname,
        "Toggle Translation Tracking");
	strcpy (TransformationNode->object_controls->controls[2]->voicename,
        "Translation");
	TransformationNode->object_controls->controls[2]->keystroke = 'T';
	TransformationNode->object_controls->controls[2]->control_function = MenuCallback;

	strcpy (TransformationNode->object_controls->controls[3]->menuname,
        "Toggle Rotation Tracking");
	strcpy (TransformationNode->object_controls->controls[3]->voicename,
        "Rotation");
	TransformationNode->object_controls->controls[3]->keystroke = 'R';
	TransformationNode->object_controls->controls[3]->control_function = MenuCallback;

	//// Bounding Volume Setup ////

	TransformationNode->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	TransformationNode->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	TransformationNode->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
	TransformationNode->bounding_volume_set->bvset[0]->p[0][1]=0.0;
	TransformationNode->bounding_volume_set->bvset[0]->p[0][2]=0.0;
	TransformationNode->bounding_volume_set->bvset[0]->p[0][3]=1.0;
	/* -x, -y, -z corner */
	TransformationNode->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
	TransformationNode->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[1][3]=1.0;
	/* +x, -y, -z corner */
	TransformationNode->bounding_volume_set->bvset[0]->p[2][0]=0.01;    
	TransformationNode->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[2][3]=1.0;
	/* -x, +y, -z corner */
	TransformationNode->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
	TransformationNode->bounding_volume_set->bvset[0]->p[3][1]=0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[3][3]=1.0;
	/* -x, -y, +z corner */
	TransformationNode->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
	TransformationNode->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[4][2]=0.01;
	TransformationNode->bounding_volume_set->bvset[0]->p[4][3]=1.0;

	/* Define bounding sphere for culling */
	TransformationNode->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	TransformationNode->bounding_volume_set->bvset[1]->p[0][0]=0.0;
	TransformationNode->bounding_volume_set->bvset[1]->p[0][1]=0.0;
	TransformationNode->bounding_volume_set->bvset[1]->p[0][2]=0.0;
	TransformationNode->bounding_volume_set->bvset[1]->p[0][3]=1.0;
	/* Surface point */
	TransformationNode->bounding_volume_set->bvset[1]->p[1][0]=0.01;
	TransformationNode->bounding_volume_set->bvset[1]->p[1][1]=0.0;
	TransformationNode->bounding_volume_set->bvset[1]->p[1][2]=0.0;
	TransformationNode->bounding_volume_set->bvset[1]->p[1][3]=1.0;
    
	/* Define a bounding point */
	TransformationNode->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	TransformationNode->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	TransformationNode->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	TransformationNode->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	TransformationNode->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	/* attach to the transformation node in a default place */
	PTemp[0] = TRANSFORMATION_NODE_X;
	PTemp[1] = TRANSFORMATION_NODE_Y;
	PTemp[2] = TRANSFORMATION_NODE_Z;
	PTemp[3] = TRANSFORMATION_NODE_PITCH;
	PTemp[4] = TRANSFORMATION_NODE_YAW;
	PTemp[5] = TRANSFORMATION_NODE_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	TransformationNode->orginal_first_node->MpoParams->Po[0] = TRANSLATE;	
	TransformationNode->orginal_first_node->MpoParams->Po[1] = ROTATE001;
	TransformationNode->orginal_first_node->MpoParams->Po[2] = ROTATE100;
	TransformationNode->orginal_first_node->MpoParams->Po[3] = ROTATE010;
	TransformationNode->orginal_first_node->MpoParams->Po[4] = SCALE;
	TransformationNode->orginal_first_node->MpoParams->P->assign( PTemp );
}

} // extern "C"
