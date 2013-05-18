/*
MouseTrackerClientClient.cc  An app providing access to position tracking
from the mouse

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
// Created: 11-1-00, TBE
//
// Modified: 5-13-04, TBE
//

// include the master Flatland library 
#include "flatland_app.h"
#include "MouseTrackerClient.h"

#define NUMBER_OF_TRANSDUCERS 1


void empty(OBJECT_TYPE *o)
{
}

void ReceiveMessage(OBJECT_TYPE *to, OBJECT_TYPE *from, char *message )
{
    MOUSE_TRACKER_CLIENT_DATA_MEMBERS *Data = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) to->ptr1;
    
//    fprintf( stdout, "got: %s\n", message );
    if( !strcmp( message, "warp" ) )
    {
//	fprintf( stdout, "Warp\n" );
	Data->WarpPointerToCenter = true;
    }
    if( !strcmp( message, "set:warp" ) )
    {
	Data->WarpPointerToCenter = true;
    }
}

bool IsReady(OBJECT_TYPE *TrackerClientObject)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	return MouseClientDataMembers->Ready;
}

int GetNumberOfTransducers(OBJECT_TYPE *TrackerClientObject)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	return MouseClientDataMembers->Ready ? MouseClientDataMembers->NumberOfTransducers : 0;
}

bool GetPosition(OBJECT_TYPE *TrackerClientObject, int transducer_number, Position_Struct *coord)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	if (!MouseClientDataMembers->Ready || transducer_number<0 || transducer_number>=MouseClientDataMembers->NumberOfTransducers || !MouseClientDataMembers->UseTracking )
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
		coord->x = MouseClientDataMembers->Tracker.x;
		coord->y = MouseClientDataMembers->Tracker.y;
		coord->z = MouseClientDataMembers->Tracker.z;
		coord->roll = MouseClientDataMembers->Tracker.roll;
		coord->pitch = MouseClientDataMembers->Tracker.pitch;
		coord->yaw = MouseClientDataMembers->Tracker.yaw;
		return true;
	}
}

// Platform handler 
void platformfunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

	GLboolean lighting_flag = GL_FALSE;
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={0.3,0.3,0.3,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	
	// draw what?

	// draw axes on demand...	
	if ( MouseClientDataMembers->DisplayAxes ) 
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
void trueShadowFunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	// shadow of what?
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	// remove head rotation if in a cave environment
	
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *)MouseTrackerClientObject->ptr1;
	float PTemp[9];	
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global

	PTemp[0] = 0.0;
	PTemp[1] = 0.0;
	PTemp[2] = 0.0;
	PTemp[3] = 0.0;
	PTemp[4] = 0.0;
	PTemp[5] = 0.0;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	MouseTrackerClientObject->orginal_first_node->MpoParams->Po[3] = TRANSLATE ;	
	MouseTrackerClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE010 ;
	MouseTrackerClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE100 ;
	MouseTrackerClientObject->orginal_first_node->MpoParams->Po[0] = ROTATE001 ;
	MouseTrackerClientObject->orginal_first_node->MpoParams->P->assign( PTemp );

	if ( ctrl->FixedViewingPlanes )
	{	
		if ( MouseTrackerClientObject->universe == ctrl->Universe )
		{
			if ( !MouseClientDataMembers->HeadTransformationNode )
			{
				MouseClientDataMembers->HeadTransformationNode = fl_find_object_by_name( ctrl->Universe, ctrl->HeadTrackerName );
			}
			if ( MouseClientDataMembers->HeadTransformationNode )
			{
				TRANSFORMATION_NODE_ACCESS_MEMBERS *members = (TRANSFORMATION_NODE_ACCESS_MEMBERS *)(MouseClientDataMembers->HeadTransformationNode->ptr0);
				if ( members->GetPosition )
				{
					CORDORD pos;
					members->GetPosition( MouseClientDataMembers->HeadTransformationNode, &pos );
			
					PTemp[0] = 0.0;
					PTemp[1] = 0.0;
					PTemp[2] = 0.0;
					PTemp[3] = -pos.pitch;
					PTemp[4] = -pos.yaw;
					PTemp[5] = -pos.roll;
					PTemp[6] = 1.0;
					PTemp[7] = 1.0;
					PTemp[8] = 1.0;
					MouseTrackerClientObject->orginal_first_node->MpoParams->Po[3] = TRANSLATE ;	
					MouseTrackerClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE010 ;
					MouseTrackerClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE100 ;
					MouseTrackerClientObject->orginal_first_node->MpoParams->Po[0] = ROTATE001 ;
					MouseTrackerClientObject->orginal_first_node->MpoParams->P->assign( PTemp );
				}
			}
		}
	}
}

// animate callback 
void animatefunc(OBJECT_TYPE *MouseTrackerClientObject)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

//	fprintf( stdout, "win: %d  vp: %d  button: %d  state: %d  position: (%d,%d)\n", MouseClientDataMembers->window, MouseClientDataMembers->viewport, MouseClientDataMembers->button, MouseClientDataMembers->state, MouseClientDataMembers->x, MouseClientDataMembers->y );

	Loop_Control_Struct *ctrl = Loop_Control;

	ctrl->Window = MouseClientDataMembers->window;
	ctrl->Viewport = MouseClientDataMembers->viewport;

	int win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);
	int vp = ctrl->Viewport;
	Frustrum_Struct *f = wc->Frustrum + vp;
	Viewport_Struct *v = wc->Extent + vp;
	double mv[16];
	double pr[16];
	int vw[4];
	double x=0, y=0, z=0;
	
	/////////////////////////////////////////////////////////////////////
	// set up universe to get mouse position in mouse tracker coordinates
	/////////////////////////////////////////////////////////////////////
		
	ctrl->Eye = CENTER_EYE;

	RestructureTreeToEye( ctrl );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();    

	fl_save_current_GL_modelview_matrix( ctrl->Universe->root_object->Mco);

	// Loop over calling predraw on all universe objects 
	fl_pre_draw_all_objects( ctrl->Universe );

	// Update all Mpo xforms 
	fl_update_Mpo_all_objects( ctrl->Universe );
	
	// Traverse tree to update xform inheritance 
	fl_xform_update( ctrl->Universe );

	FigureFrustrum( ctrl );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );	
	glFrustum( f->Left, f->Right, f->Bottom, f->Top, f->Near, f->Far );
	glGetDoublev( GL_PROJECTION_MATRIX, pr );

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( MouseTrackerClientObject->Mco );
	glGetDoublev( GL_MODELVIEW_MATRIX, mv );

	vw[0] = v->X;
	vw[1] = v->Y;
	vw[2] = v->Width;
	vw[3] = v->Height;

	// hide motion if warping mouse to center of screen....
	if( MouseClientDataMembers->WarpPointerToCenter )
	{
    	    MouseClientDataMembers->WarpPointerToCenter = false;
	    MouseClientDataMembers->x = v->Width/2; 
	    MouseClientDataMembers->y = v->Height/2; 
	}
	
	gluUnProject( (double)MouseClientDataMembers->x, (double)(v->Height - MouseClientDataMembers->y), 0.0, mv, pr, vw, &x, &y, &z );

	if( MouseClientDataMembers->MoveIn )
	{
	    MouseClientDataMembers->Radius -= MouseClientDataMembers->MoveSpeed;
	    if( MouseClientDataMembers->Radius < 0 )
	    {
		MouseClientDataMembers->Radius = 0;
	    }
	}
	
	if( MouseClientDataMembers->MoveOut )
	{
	    MouseClientDataMembers->Radius += MouseClientDataMembers->MoveSpeed;
	    // for now, no limit
	}
	
	// project onto sphere 
	double r = sqrt( x * x + y * y + z * z );
	x = MouseClientDataMembers->Radius * x / r;
	y = MouseClientDataMembers->Radius * y / r;
	z = MouseClientDataMembers->Radius * z / r;
		
	MouseClientDataMembers->Tracker.x = (float)x;
	MouseClientDataMembers->Tracker.y = (float)y;
	MouseClientDataMembers->Tracker.z = (float)z;
	
//	fprintf( stdout, "mouse: (%f,%f,%f)\n", (float)x, (float)y, (float)z );
}


/* menu control callback */
void MenuCallback(OBJECT_TYPE *MouseTrackerClientObject, char key)
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers= (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

	switch (key)
	{
    case 'a':   // display axes
		MouseClientDataMembers->DisplayAxes = true;
		break;

	case 'A':   // hide axes
		MouseClientDataMembers->DisplayAxes = false;
		break;

    case 't':   // display axes
		MouseClientDataMembers->UseTracking = true;
		break;

    case 'T':   // display axes
		MouseClientDataMembers->UseTracking = false;
		break;
	}
}


void KeyDownCallback( OBJECT_TYPE *MouseTrackerClientObject, unsigned char key, int x, int y )
{
    MOUSE_TRACKER_CLIENT_DATA_MEMBERS *Data = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

    switch( key )
    {
    case 0x7f:
	Data->MoveIn = true;
	break;
    default:
	break;
    }
}

    
void KeyUpCallback( OBJECT_TYPE *MouseTrackerClientObject, unsigned char key, int x, int y )
{
    MOUSE_TRACKER_CLIENT_DATA_MEMBERS *Data = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

    switch( key )
    {
    case 0x7f:
	Data->MoveIn = false;
	break;
    default:
	break;
    }
}

    
void SpecialDownCallback( OBJECT_TYPE *MouseTrackerClientObject, int key, int x, int y )
{
    MOUSE_TRACKER_CLIENT_DATA_MEMBERS *Data = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

    switch( key )
    {
    case GLUT_KEY_INSERT:
	Data->MoveOut = true;
	break;
    default:
	break;
    }
}

    
void SpecialUpCallback( OBJECT_TYPE *MouseTrackerClientObject, int key, int x, int y )
{
    MOUSE_TRACKER_CLIENT_DATA_MEMBERS *Data = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

    switch( key )
    {
    case GLUT_KEY_INSERT:
	Data->MoveOut = false;
	break;
    default:
	break;
    }
}

    
bool GetButton( OBJECT_TYPE *MouseTrackerClientObject, int button, int *state )
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;

	if ( button >= 0 && button < 10 )
	{
		*state = MouseClientDataMembers->state[ button ];
		return true;
	}
	*state = 0;
	return false;
}


//bool SetMouse( OBJECT_TYPE *MouseTrackerClientObject, int window, int viewport, int button, int state, int x, int y )
void SetMouse( OBJECT_TYPE *MouseTrackerClientObject, int button, int state, int x, int y )
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;
	int window = ctrl->Window;
	int viewport = ctrl->Viewport;

	// odd hack goes here...
	// pretend that the left mouse button is the trigger of the joystick...	
	if( button == GLUT_LEFT_BUTTON )
	{
	    if( state == GLUT_DOWN )
	    {
    	    	fl_glutJoystick_all_objects( Universe, 0x01, 0, 0, 0);
    	    	fl_glutJoystick_all_objects( Universe, 0x05, 0, 0, 0);
	    }
	    if( state == GLUT_UP )
	    {
    	    	fl_glutJoystick_all_objects( Universe, 0x01, 0, 0, 0);
    	    	fl_glutJoystick_all_objects( Universe, 0x00, 0, 0, 0);
	    }
	}

	if( button == GLUT_MIDDLE_BUTTON )
	{
	    if( state == GLUT_DOWN )
	    {
    	    	fl_glutJoystick_all_objects( Universe, 0x08, 0, 0, 0);
	    }
	    if( state == GLUT_UP )
	    {
    	    	fl_glutJoystick_all_objects( Universe, 0x00, 0, 0, 0);
	    }
	}

	MouseClientDataMembers->window = window;
	MouseClientDataMembers->viewport = viewport;
	MouseClientDataMembers->x = x;
	MouseClientDataMembers->y = y;
	if ( button >= 0 && button < 10 )
	{
		MouseClientDataMembers->button = button;
		MouseClientDataMembers->state[ button ] = state;
	}
}
		
	
//bool SetMousePosition( OBJECT_TYPE *MouseTrackerClientObject, int window, int viewport, int x, int y )
void SetMousePosition( OBJECT_TYPE *MouseTrackerClientObject, int x, int y )
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;
	int window = ctrl->Window;
	int viewport = ctrl->Viewport;

	MouseClientDataMembers->window = window;
	MouseClientDataMembers->viewport = viewport;
	MouseClientDataMembers->x = x;
	MouseClientDataMembers->y = y;
}
	

//bool SetPassiveMousePosition( OBJECT_TYPE *MouseTrackerClientObject, int window, int viewport, int x, int y )
void SetPassiveMousePosition( OBJECT_TYPE *MouseTrackerClientObject, int x, int y )
{
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientDataMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) MouseTrackerClientObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;
	int window = ctrl->Window;
	int viewport = ctrl->Viewport;

	MouseClientDataMembers->window = window;
	MouseClientDataMembers->viewport = viewport;
	MouseClientDataMembers->x = x;
	MouseClientDataMembers->y = y;
}    


extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];					/* temporary transform storage */
	OBJECT_TYPE *TrackerClientObject;				/* This object */
//	pthread_t transducer_read_thread;		/* thread handle for data input */
	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers;			/* pointer to "TrackerClientMembers" structure */
	MOUSE_TRACKER_CLIENT_DATA_MEMBERS *MouseClientMembers;			/* pointer to "TrackerClientMembers" structure */
	int TRACKER_CLIENT_TRANSDUCERS = NUMBER_OF_TRANSDUCERS;
	float TRACKER_CLIENT_X = 0.0f;
	float TRACKER_CLIENT_Y = 0.0f;
	float TRACKER_CLIENT_Z = 0.0f;
	float TRACKER_CLIENT_ROLL = 0.0f;
	float TRACKER_CLIENT_PITCH = 0.0f;
	float TRACKER_CLIENT_YAW = 0.0f;
	float TRACKER_CLIENT_RADIUS = 1.0f;
	char *Name = App->appname;
	bool known;
	OBJ_CONTROLS_ARRAY_TYPE *subMenu; /* submenu temp variable */
				 
	/* generate the "TrackerClientMembers" structure & attach it to the object */
	if ((TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) malloc (sizeof (TRACKER_CLIENT_ACCESS_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	/* generate the "MouseTrackerClientMembers" structure & attach it to the object */
	if ((MouseClientMembers = (MOUSE_TRACKER_CLIENT_DATA_MEMBERS *) malloc (sizeof (MOUSE_TRACKER_CLIENT_DATA_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	MouseClientMembers->Tracker.x = 0;
	MouseClientMembers->Tracker.y = 0;
	MouseClientMembers->Tracker.z = 0;
	MouseClientMembers->Tracker.roll = 0;
	MouseClientMembers->Tracker.pitch = 0;
	MouseClientMembers->Tracker.yaw = 0;
	TrackerClientMembers->GetPosition = GetPosition;
	TrackerClientMembers->GetVelocity = NULL;
	TrackerClientMembers->GetError = NULL;
	TrackerClientMembers->IsReady = IsReady;
	TrackerClientMembers->GetNumberOfTransducers = GetNumberOfTransducers;
	MouseClientMembers->Ready = false;
	MouseClientMembers->NumberOfTransducers = 1;
	MouseClientMembers->DisplayAxes = false;
	MouseClientMembers->UseTracking = true;
//	TrackerClientMembers->SetMouse = SetMouse;
//	TrackerClientMembers->SetMousePosition = SetMousePosition;
//	TrackerClientMembers->SetPassiveMousePosition = SetPassiveMousePosition;
	MouseClientMembers->HeadTransformationNode = NULL;
	MouseClientMembers->MoveIn = false;
	MouseClientMembers->MoveOut = false;
	MouseClientMembers->MoveSpeed = 0.01;
	MouseClientMembers->WarpPointerToCenter = false;
	MouseClientMembers->viewport = 0;
	MouseClientMembers->window = 0;

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
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
		if (!strcmp (App->params[i][0], "radius"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_RADIUS);
			known = true;
		}
		
		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

//	MouseClientMembers->NumberOfTransducers = TRACKER_CLIENT_TRANSDUCERS;
	MouseClientMembers->Position.x = TRACKER_CLIENT_X;
	MouseClientMembers->Position.y = TRACKER_CLIENT_Y;
	MouseClientMembers->Position.z = TRACKER_CLIENT_Z;
	MouseClientMembers->Position.roll = TRACKER_CLIENT_ROLL;
	MouseClientMembers->Position.pitch = TRACKER_CLIENT_PITCH;
	MouseClientMembers->Position.yaw = TRACKER_CLIENT_YAW;
	MouseClientMembers->Radius = TRACKER_CLIENT_RADIUS;

	TrackerClientObject = fl_add_child_to_parent_by_name(	App->universe_ptr,
														App->parent_object_name,
														App->appname);

	TrackerClientObject->ptr0 = (void *) TrackerClientMembers;	/* attach struct to object */
	TrackerClientObject->ptr1 = (void *) MouseClientMembers;	/* attach struct to object */

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
	TrackerClientObject->glutMousefunc = SetMouse;
	TrackerClientObject->glutMotionfunc = SetMousePosition;
	TrackerClientObject->glutPassiveMotionfunc = SetPassiveMousePosition;

	TrackerClientObject->glutKeyboardfunc = KeyDownCallback;
	TrackerClientObject->glutKeyboardUpfunc = KeyUpCallback;
	TrackerClientObject->glutSpecialfunc = SpecialDownCallback;
	TrackerClientObject->glutSpecialUpfunc = SpecialUpCallback;

	TrackerClientObject->recievemessagefunc = ReceiveMessage;
	
	//// Menu Setup ////

	/* set up the control structure (three subcontrols under one) */
	/* allows control of the object from the keyboard, menues, and voice */
	TrackerClientObject->object_controls = fl_makeControlsSet(2);

	/* Define spin/nospin controls for cube */
	strcpy (TrackerClientObject->object_controls->controls[0]->menuname, "Axes");
	strcpy (TrackerClientObject->object_controls->controls[0]->voicename, "Axes");
	TrackerClientObject->object_controls->controls[0]->children = fl_makeControlsSet(2);
	TrackerClientObject->object_controls->controls[0]->control_function = NULL;
	subMenu = TrackerClientObject->object_controls->controls[0]->children;

	strcpy (subMenu->controls[0]->menuname, "Enable");
	strcpy (subMenu->controls[0]->voicename, "Enable");
	subMenu->controls[0]->keystroke = 'a';
	subMenu->controls[0]->control_function = MenuCallback;

	strcpy (subMenu->controls[1]->menuname, "Disable");
	strcpy (subMenu->controls[1]->voicename, "Disable");
	subMenu->controls[1]->keystroke = 'A';
	subMenu->controls[1]->control_function = MenuCallback;

	strcpy (TrackerClientObject->object_controls->controls[1]->menuname, "Tracking");
	strcpy (TrackerClientObject->object_controls->controls[1]->voicename, "Tracking");
	TrackerClientObject->object_controls->controls[1]->children = fl_makeControlsSet(2);
	TrackerClientObject->object_controls->controls[1]->control_function = NULL;
	subMenu = TrackerClientObject->object_controls->controls[1]->children;

	strcpy (subMenu->controls[0]->menuname, "Enable");
	strcpy (subMenu->controls[0]->voicename, "Enable");
	subMenu->controls[0]->keystroke = 't';
	subMenu->controls[0]->control_function = MenuCallback;

	strcpy (subMenu->controls[1]->menuname, "Disable");
	strcpy (subMenu->controls[1]->voicename, "Disable");
	subMenu->controls[1]->keystroke = 'T';
	subMenu->controls[1]->control_function = MenuCallback;

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

	MouseClientMembers->Ready = true;
}

} // extern "C"
