/*
LocomotionClient.cc  An app providing access to position tracking

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
// Created: 11-18-00, TBE
//

/* include the master Flatland library */
#ifdef WIN32
#  include "Flatland.h"
#  include "libFlatland.h"
#else
#  include "flatland_app.h"
#endif
#include "locowin.h"
#include "../Wand/Wand.h"
#include "LocomotionClient.h"

#define LOCOMOTION_CLIENT_LEFT_WAND "LeftWand"
#define LOCOMOTION_CLIENT_LEFT_WAND_TRACKER "LeftWandTransformationNode"
#define LOCOMOTION_CLIENT_RIGHT_WAND "RightWand"
#define LOCOMOTION_CLIENT_RIGHT_WAND_TRACKER "RightWandTransformationNode"

// How many raw trackers managed (by default)
#define NUMBER_OF_TRANSDUCERS 1

/* =======================================================================

Description:
    LocomotionClient provides a TrackerClient interface for positioning the
    Vessel in relation to the Earth.  The Vessel can be moved via sliders on
    the created locomotion window, or via the wands.
         
Objects created when application is loaded:
    LocomotionClient only.
        
Object dependencies:
    LocomotionClient depends on the wands and the wand TransformationNodes 
    for driving.
        
Configurable options:
    left_wand               string  name of left wand
    left_wand_tracker       string  name of left wand TransformationNode
    right_wand              string  name of right wand
    right_wand_tracker      string  name of right wand TransformationNode
    x                       float   attachment position
    y                       float   attachment position
    z                       float   attachment position
    roll                    float   attachment position
    pitch                   float   attachment position
    yaw                     float   attachment position
    velocity_scale          float   initial velocity scale
    start_x                 float   initial Vessel position
    start_y                 float   initial Vessel position
    start_z                 float   initial Vessel position
    start_roll              float   initial Vessel position
    start_pitch             float   initial Vessel position
    start_yaw               float   initial Vessel position
    
Threads created:
    none.
    
Library requirements:
    Flatland basic setup

Location requirements:
    usually attached to "Vessel"

Controls:
    Keyboard:
        'a' - toggle axes display
        't' - toggle use of tracking data
        'p' - print position to stdout
    Menu:
        see keyboard input
    Voice:
        none
    Beaming:
        none

  Status:
    15-Sep-2001 - In need of more documentation.  In need of improvement.
    Perhaps extent of Earth should be retrieved from Earth object?
    
======================================================================= */


const float earthxmax = 1000.0;
const float earthxmin = -1000.0;
const float earthymax = -5.0;
const float earthymin = -1000.0;
const float earthzmax = 1000.0;
const float earthzmin = -1000.0;
const float MAX_LOCO_X = earthxmax;
const float MIN_LOCO_X = earthxmin;
const float MAX_LOCO_Y = earthymax;
const float MIN_LOCO_Y = earthymin;
const float MAX_LOCO_Z = earthzmax;
const float MIN_LOCO_Z = earthzmin;

// global so locomotion window can alter velocities
OBJECT_TYPE *LocomotionClient = NULL;

bool IsReady(OBJECT_TYPE *TrackerClientObject)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;
	return LocomotionClientMembers->Ready;
}

int GetNumberOfTransducers(OBJECT_TYPE *TrackerClientObject)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;
	return LocomotionClientMembers->Ready ? LocomotionClientMembers->NumberOfTransducers : 0;
}

bool GetPosition(OBJECT_TYPE *TrackerClientObject, int transducer_number, Position_Struct *coord)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;
	if (!LocomotionClientMembers->Ready || !LocomotionClientMembers->UseTracking || transducer_number<0 || transducer_number>=LocomotionClientMembers->NumberOfTransducers)
	{
		coord->x = 0.0f;
		coord->y = 0.0f;
		coord->z = 0.0f;
		coord->roll = 0.0f;
		coord->pitch = 0.0f;
		coord->yaw = 0.0f;
		return false;
	}
	else
	{
		coord->x = -LocomotionClientMembers->Tracker.x;
		coord->y = -LocomotionClientMembers->Tracker.y;
		coord->z = -LocomotionClientMembers->Tracker.z;
		coord->roll = -LocomotionClientMembers->Tracker.roll;
		coord->pitch = -LocomotionClientMembers->Tracker.pitch;
		coord->yaw = -LocomotionClientMembers->Tracker.yaw;
		return true;
	}
}

bool GetVelocity(OBJECT_TYPE *TrackerClientObject, int transducer_number, Position_Struct *coord)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;
	if (!LocomotionClientMembers->Ready || !LocomotionClientMembers->UseTracking || transducer_number<0 || transducer_number>=LocomotionClientMembers->NumberOfTransducers)
	{
		coord->x = 0.0f;
		coord->y = 0.0f;
		coord->z = 0.0f;
		coord->roll = 0.0f;
		coord->pitch = 0.0f;
		coord->yaw = 0.0f;
		return false;
	}
	else
	{
		coord->x = -LocomotionClientMembers->Velocity.x;
		coord->y = -LocomotionClientMembers->Velocity.y;
		coord->z = -LocomotionClientMembers->Velocity.z;
		coord->roll = -LocomotionClientMembers->Velocity.roll;
		coord->pitch = -LocomotionClientMembers->Velocity.pitch;
		coord->yaw = -LocomotionClientMembers->Velocity.yaw;
		return true;
	}
}

// Platform handler 
void platformfunc(OBJECT_TYPE *TrackerClientObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *TrackerClientObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *TrackerClientObject)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;

	GLboolean lighting_flag = GL_FALSE;
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={0.3,0.3,0.3,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	
	// draw what?

	// draw axes on demand...	
	if ( LocomotionClientMembers->DisplayAxes ) 
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );

		glutSolidCube(0.05);
	
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

		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, black );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, black );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black );
	}
}

// true shadow callback 
void trueShadowFunc(OBJECT_TYPE *TrackerClientObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *TrackerClientObject)
{
	// shadow of what?
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *TrackerClientObject)
{
}

// provides inertia simulation for locomotion 
float inertia_func( float vold, float vdemand, float param )
{
	float vnew ;

	vnew = vold + param * (vdemand - vold ) ;
	return(vnew) ;
}

float mapping1( float in, float outmax ) 
{
// quadratic in [-1,1], assume "in" is bounded by +/-1 

	if( in>=0 )
		return( outmax*in*in ) ;
	else
		return( -outmax*in*in ) ;
}

float mapping2( float in, float outmid ) 
{
// forth power in [0,2], assume "in" is bounded by +/-1 ,
// returns outmid when in=0, returns 0 when in = -1 ,
// and returns 16*outmid when in = +1.  
	
	return( outmid * (in+1.0)*(in+1.0)*(in+1.0)*(in+1.0) ) ;
}

// animate callback 
void animatefunc(OBJECT_TYPE *TrackerClientObject)
{
  if (TrackerClientObject->original_owner_flag == true)
  {
//	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers;			/* pointer to "TrackerClientMembers" structure */
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;

	OBJECT_TYPE *lw = fl_find_object_by_name( TrackerClientObject->universe, LocomotionClientMembers->LeftWandName );
	OBJECT_TYPE *lwt = fl_find_object_by_name( TrackerClientObject->universe, LocomotionClientMembers->LeftWandTrackerName );
	OBJECT_TYPE *rw = fl_find_object_by_name( TrackerClientObject->universe, LocomotionClientMembers->RightWandName );
	OBJECT_TYPE *rwt = fl_find_object_by_name( TrackerClientObject->universe, LocomotionClientMembers->RightWandTrackerName );

	bool lw_drive;
	bool rw_drive;
	CORDORD lw_pos;
	CORDORD rw_pos;
	CORDORD world;
	CORDORD worldv;
	float demandv;
	float alpha = 0.15;
	static bool first_pass = true;
		
	// this is an ugly hack, but what else can I do?
	if (first_pass)
	{
		fprintf( stdout, "Creating Locomotion Window...\n" );
		first_pass = false;

		// create & initialize the locomotion control window 
		init_loco_window();
	}
	
	// retrieve current values
	worldv.x = LocomotionClientMembers->Velocity.x;
	worldv.y = LocomotionClientMembers->Velocity.y;
	worldv.z = LocomotionClientMembers->Velocity.z;
	worldv.roll = LocomotionClientMembers->Velocity.roll;
	worldv.pitch = LocomotionClientMembers->Velocity.pitch;
	worldv.yaw = LocomotionClientMembers->Velocity.yaw;

	world.x = LocomotionClientMembers->Tracker.x;
	world.y = LocomotionClientMembers->Tracker.y;
	world.z = LocomotionClientMembers->Tracker.z;
	world.roll = LocomotionClientMembers->Tracker.roll;
	world.pitch = LocomotionClientMembers->Tracker.pitch;
	world.yaw = LocomotionClientMembers->Tracker.yaw;

	// if the wands are loaded, use them to drive
	if ( lw && lwt && rw && rwt )
	{
		// check the status of the bottom button for the left wand
		lw_drive = ((WAND_MEMBERS*)lw->ptr0)->GetBottomButton( lw );
		if ( lw_drive )
		{
			((TRANSFORMATION_NODE_ACCESS_MEMBERS*)lwt->ptr0)->GetPosition( lwt, &lw_pos );
		}

		// check the status of the bottom button for the right wand
		rw_drive = ((WAND_MEMBERS*)rw->ptr0)->GetBottomButton( rw );
		if ( rw_drive )
		{
			((TRANSFORMATION_NODE_ACCESS_MEMBERS*)rwt->ptr0)->GetPosition( rwt, &rw_pos );
		}

		// Locomotion control using right wand
		if ( !lw_drive && rw_drive ) 
		{
			demandv = mapping1(-rw_pos.roll/90.0, 10.0*LocomotionClientMembers->VelocityScale);
			worldv.x = inertia_func( worldv.x, demandv, alpha ) ;
			demandv = mapping1(-rw_pos.pitch/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.z = inertia_func( worldv.z, demandv, alpha ) ;
			demandv = mapping1(-rw_pos.yaw/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.yaw = inertia_func( worldv.yaw, demandv, alpha ) ;
		}

		// Locomotion control using left wand
		if ( lw_drive && !rw_drive ) 
		{
			demandv = mapping1(-lw_pos.roll/90.0, 10.0*LocomotionClientMembers->VelocityScale);
			worldv.x = inertia_func( worldv.x, demandv, alpha ) ;
			demandv = mapping1(-lw_pos.pitch/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.z = inertia_func( worldv.z, demandv, alpha ) ;
			demandv = mapping1(-lw_pos.yaw/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.yaw = inertia_func( worldv.yaw, demandv, alpha ) ;
		}

		// Locomotion control using both wands
		if ( lw_drive && rw_drive ) 
		{
			demandv = mapping1(-rw_pos.roll/90.0, 10.0*LocomotionClientMembers->VelocityScale);
			worldv.x = inertia_func( worldv.x, demandv, alpha ) ;
			demandv = mapping1(-rw_pos.pitch/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.z = inertia_func( worldv.z, demandv, alpha ) ;
			demandv = mapping1(-rw_pos.yaw/90.0,10.0*LocomotionClientMembers->VelocityScale);
			worldv.yaw = inertia_func( worldv.yaw, demandv, alpha ) ;
			demandv = mapping1( lw_pos.pitch/90.0,5.0*LocomotionClientMembers->VelocityScale );
			worldv.y = inertia_func( worldv.y, demandv, alpha ) ;
		}
	}			

	// figure out the locomotion between us and the craft/head/whatever 
	world.x -= worldv.x * cos( world.yaw*deg2rad ) + worldv.z * sin( world.yaw*deg2rad );
	world.y -= worldv.y ;
	world.z -= worldv.x * sin( world.yaw*deg2rad ) - worldv.z * cos( world.yaw*deg2rad );
	world.yaw += worldv.yaw ;

	// enforce the limits 
	world.x = world.x > MAX_LOCO_X ? MAX_LOCO_X : world.x;
	world.x = world.x < MIN_LOCO_X ? MIN_LOCO_X : world.x;
	world.y = world.y > MAX_LOCO_Y ? MAX_LOCO_Y : world.y;
	world.y = world.y < MIN_LOCO_Y ? MIN_LOCO_Y : world.y;
	world.z = world.z > MAX_LOCO_Z ? MAX_LOCO_Z : world.z;
	world.z = world.z < MIN_LOCO_Z ? MIN_LOCO_Z : world.z;

	// store the new values
	LocomotionClientMembers->Velocity.x = worldv.x;
	LocomotionClientMembers->Velocity.y = worldv.y;
	LocomotionClientMembers->Velocity.z = worldv.z;
	LocomotionClientMembers->Velocity.roll = worldv.roll;
	LocomotionClientMembers->Velocity.pitch = worldv.pitch;
	LocomotionClientMembers->Velocity.yaw = worldv.yaw;

	LocomotionClientMembers->Tracker.x = world.x;
	LocomotionClientMembers->Tracker.y = world.y;
	LocomotionClientMembers->Tracker.z = world.z;
	LocomotionClientMembers->Tracker.roll = world.roll;
	LocomotionClientMembers->Tracker.pitch = world.pitch;
	LocomotionClientMembers->Tracker.yaw = world.yaw;

        // if requested, print position
        if (LocomotionClientMembers->printPosn)
          printf ("x = %f, y = %f, z = %f\n", world.x, world.y, world.z);
  }
}

/* menu control callback */
void MenuCallback(OBJECT_TYPE *TrackerClientObject, char key)
{
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) TrackerClientObject->ptr1;

	switch (key)
	{
    case 'a':   // display axes
		LocomotionClientMembers->DisplayAxes = !LocomotionClientMembers->DisplayAxes;
		break;

    case 't':   // toggle tracking
		LocomotionClientMembers->UseTracking = !LocomotionClientMembers->UseTracking;
		break;
    case 'p':   // print position
		LocomotionClientMembers->printPosn = !LocomotionClientMembers->printPosn;
		break;
	}
}

extern "C"
{

// This is the well known initialization function. Everything comes from here 
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];													// temporary transform storage 
	OBJECT_TYPE *TrackerClientObject;								//This object 
	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers;			// pointer to "TrackerClientMembers" structure 
	LOCOMOTION_CLIENT_MEMBERS *LocomotionClientMembers;				// pointer to "LocomotionClientMembers" structure 
	char TRACKER_CLIENT_LEFT_WAND[MAX_NAME_SIZE] = { LOCOMOTION_CLIENT_LEFT_WAND };  
	char TRACKER_CLIENT_LEFT_WAND_TRACKER[MAX_NAME_SIZE] = { LOCOMOTION_CLIENT_LEFT_WAND_TRACKER };  
	char TRACKER_CLIENT_RIGHT_WAND[MAX_NAME_SIZE] = { LOCOMOTION_CLIENT_RIGHT_WAND };  
	char TRACKER_CLIENT_RIGHT_WAND_TRACKER[MAX_NAME_SIZE] = { LOCOMOTION_CLIENT_RIGHT_WAND_TRACKER };  
	int TRACKER_CLIENT_TRANSDUCERS = NUMBER_OF_TRANSDUCERS;
	float TRACKER_CLIENT_X = 0.0f;
	float TRACKER_CLIENT_Y = 0.0f;
	float TRACKER_CLIENT_Z = 0.0f;
	float TRACKER_CLIENT_ROLL = 0.0f;
	float TRACKER_CLIENT_PITCH = 0.0f;
	float TRACKER_CLIENT_YAW = 0.0f;
	float TRACKER_CLIENT_VELOCITY_SCALE = 5.0f;
	char *Name = App->appname;
	bool known;
	float START_X = 0.0f;
	float START_Y = -15.0f;
	float START_Z = -35.0f;
	float START_ROLL = 0.0f;
	float START_PITCH = 0.0f;
	float START_YAW = 0.0f;
				 
	/* generate the "TrackerClientMembers" structure & attach it to the object */
	if ((TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) malloc (sizeof (TRACKER_CLIENT_ACCESS_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	/* generate the "FlockTrackerClientMembers" structure & attach it to the object */
	if ((LocomotionClientMembers = (LOCOMOTION_CLIENT_MEMBERS *) malloc (sizeof (LOCOMOTION_CLIENT_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	LocomotionClientMembers->Velocity.x = 0;
	LocomotionClientMembers->Velocity.y = 0;
	LocomotionClientMembers->Velocity.z = 0;
	LocomotionClientMembers->Velocity.roll = 0;
	LocomotionClientMembers->Velocity.pitch = 0;
	LocomotionClientMembers->Velocity.yaw = 0;
	TrackerClientMembers->GetPosition = GetPosition;
	TrackerClientMembers->GetVelocity = GetVelocity;
	TrackerClientMembers->GetError = NULL;
	TrackerClientMembers->IsReady = IsReady;
	TrackerClientMembers->GetNumberOfTransducers = GetNumberOfTransducers;
	LocomotionClientMembers->Ready = false;
	LocomotionClientMembers->NumberOfTransducers = 1;
	LocomotionClientMembers->DisplayAxes = false;
	LocomotionClientMembers->UseTracking = true;

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "left_wand"))
		{
			sscanf(App->params[i][2], "%s", TRACKER_CLIENT_LEFT_WAND);
			known = true;
		}
		if (!strcmp (App->params[i][0], "left_wand_tracker"))
		{
			sscanf(App->params[i][2], "%s", TRACKER_CLIENT_LEFT_WAND_TRACKER);
			known = true;
		}
		if (!strcmp (App->params[i][0], "right_wand"))
		{
			sscanf(App->params[i][2], "%s", TRACKER_CLIENT_RIGHT_WAND);
			known = true;
		}
		if (!strcmp (App->params[i][0], "right_wand_tracker"))
		{
			sscanf(App->params[i][2], "%s", TRACKER_CLIENT_RIGHT_WAND_TRACKER);
			known = true;
		}
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "velocity_scale"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_VELOCITY_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_x"))
		{
			sscanf(App->params[i][2], "%f", &START_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_y"))
		{
			sscanf(App->params[i][2], "%f", &START_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_z"))
		{
			sscanf(App->params[i][2], "%f", &START_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_roll"))
		{
			sscanf(App->params[i][2], "%f", &START_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_pitch"))
		{
			sscanf(App->params[i][2], "%f", &START_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "start_yaw"))
		{
			sscanf(App->params[i][2], "%f", &START_YAW);
			known = true;
		}
		
		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

	strncpy(LocomotionClientMembers->LeftWandName,TRACKER_CLIENT_LEFT_WAND,MAX_NAME_SIZE);
	strncpy(LocomotionClientMembers->LeftWandTrackerName,TRACKER_CLIENT_LEFT_WAND_TRACKER,MAX_NAME_SIZE);
	strncpy(LocomotionClientMembers->RightWandName,TRACKER_CLIENT_RIGHT_WAND,MAX_NAME_SIZE);
	strncpy(LocomotionClientMembers->RightWandTrackerName,TRACKER_CLIENT_RIGHT_WAND_TRACKER,MAX_NAME_SIZE);

	LocomotionClientMembers->NumberOfTransducers = TRACKER_CLIENT_TRANSDUCERS;
	LocomotionClientMembers->Position.x = TRACKER_CLIENT_X;
	LocomotionClientMembers->Position.y = TRACKER_CLIENT_Y;
	LocomotionClientMembers->Position.z = TRACKER_CLIENT_Z;
	LocomotionClientMembers->Position.roll = TRACKER_CLIENT_ROLL;
	LocomotionClientMembers->Position.pitch = TRACKER_CLIENT_PITCH;
	LocomotionClientMembers->Position.yaw = TRACKER_CLIENT_YAW;
	LocomotionClientMembers->VelocityScale = TRACKER_CLIENT_VELOCITY_SCALE;

	LocomotionClientMembers->Tracker.x = START_X;
	LocomotionClientMembers->Tracker.y = START_Y;
	LocomotionClientMembers->Tracker.z = START_Z;
	LocomotionClientMembers->Tracker.roll = START_ROLL;
	LocomotionClientMembers->Tracker.pitch = START_PITCH;
	LocomotionClientMembers->Tracker.yaw = START_YAW;

	TrackerClientObject = fl_add_child_to_parent_by_name(	App->universe_ptr,
														App->parent_object_name,
														App->appname);

	TrackerClientObject->ptr0 = (void *) TrackerClientMembers;	/* attach struct to object */
	TrackerClientObject->ptr1 = (void *) LocomotionClientMembers;	/* attach struct to object */

	/* set up callbacks */
	TrackerClientObject->soundfunc = soundfunc;
	TrackerClientObject->sounding_on_flag = 0;
	TrackerClientObject->platformfunc = platformfunc;
	TrackerClientObject->predrawfunc = predrawfunc;
	TrackerClientObject->drawfunc = drawfunc;
	TrackerClientObject->trueShadowFunc = trueShadowFunc;
	TrackerClientObject->cheapShadowFunc = cheapShadowFunc;
	TrackerClientObject->drawing_on_flag = 1;
	TrackerClientObject->pregraphicsfunc = animatefunc;

	//// Menu Setup ////

	/* set up the control structure (three subcontrols under one) */
	/* allows control of the object from the keyboard, menues, and voice */
	TrackerClientObject->object_controls = fl_makeControlsSet(3);

	/* Define controls */
	strcpy (TrackerClientObject->object_controls->controls[0]->menuname, "Toggle Axes Display");
	strcpy (TrackerClientObject->object_controls->controls[0]->voicename, "Axes");
	TrackerClientObject->object_controls->controls[0]->control_function = MenuCallback;
	TrackerClientObject->object_controls->controls[0]->keystroke = 'a';

	strcpy (TrackerClientObject->object_controls->controls[1]->menuname, "Toggle Tracking");
	strcpy (TrackerClientObject->object_controls->controls[1]->voicename, "Tracking");
	TrackerClientObject->object_controls->controls[1]->control_function = MenuCallback;
	TrackerClientObject->object_controls->controls[1]->keystroke = 't';

	strcpy (TrackerClientObject->object_controls->controls[2]->menuname, "Print Position");
	strcpy (TrackerClientObject->object_controls->controls[2]->voicename, "Position");
	TrackerClientObject->object_controls->controls[2]->control_function = MenuCallback;
	TrackerClientObject->object_controls->controls[2]->keystroke = 'p';

	//// Bounding Volume Setup ////

	TrackerClientObject->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	TrackerClientObject->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][1]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][2]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][3]=1.0;
	/* -x, -y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][3]=1.0;
	/* +x, -y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][0]=+0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][3]=1.0;
	/* -x, +y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][1]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][3]=1.0;
	/* -x, -y, +z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][2]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][3]=1.0;

	/* Define bounding sphere for culling */
	TrackerClientObject->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][0]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][1]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][2]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][3]=1.0;
	/* Surface point */
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][0]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][1]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][2]=+0.01;	
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][3]=1.0;
    
	/* Define a bounding point */
	TrackerClientObject->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][3]= 1.0;
  
	/* attach to the wessel in a default place */
	PTemp[0] = TRACKER_CLIENT_X;
	PTemp[1] = TRACKER_CLIENT_Y;
	PTemp[2] = TRACKER_CLIENT_Z; 
	PTemp[3] = TRACKER_CLIENT_YAW;
	PTemp[4] = TRACKER_CLIENT_PITCH;
	PTemp[5] = TRACKER_CLIENT_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	TrackerClientObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	TrackerClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	TrackerClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	TrackerClientObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	TrackerClientObject->orginal_first_node->MpoParams->P->assign( PTemp );

	LocomotionClient = TrackerClientObject;
	
	LocomotionClientMembers->Ready = true;
}

} // extern "C"
