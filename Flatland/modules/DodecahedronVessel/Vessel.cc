//
/*
Vessel.c.  Dodecahedral vessel application.

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

/* 
 * Maintenance history
 *
 * Created: 1-13-00
 *
 * ReCreated: 11-15-00
 */

#include "flatland_app.h"
#include "Vessel.h"
#include "DrawVessel.h"
#include "SoundVessel.h"

#define DEFAULT_LOCOMOTION_NAME "LocomotionClient"

// Platform handler 
void platform_handler(OBJECT_TYPE *o)
{
}

// menu control callback 
void toggle_vis (OBJECT_TYPE *o, char key)
{
	VESSEL_DATA_MEMBERS *VesselDataMembers = (VESSEL_DATA_MEMBERS*)o->ptr1;

	switch (key)
	{
	case 'b':   // toggle visuals 
		VesselDataMembers->DrawBars = !VesselDataMembers->DrawBars;
		break;

	case 's':   // toggle sound 
		VesselDataMembers->EngineSound = !VesselDataMembers->EngineSound;
		break;
	}
}

// Sound callback 
void sound_vessel(OBJECT_TYPE *o)
{
	float rpm = 0.0;
	CORDORD velocity;
	VESSEL_DATA_MEMBERS *VesselDataMembers = (VESSEL_DATA_MEMBERS*)o->ptr1;

	if ( VesselDataMembers->EngineSound )
	{	
		if ( !VesselDataMembers->LocomotionClient )
		{
			VesselDataMembers->LocomotionClient = fl_find_object_by_name( o->universe, VesselDataMembers->LocomotionClientName );
		}
		if ( VesselDataMembers->LocomotionClient )
		{
			if ( ((TRACKER_CLIENT_ACCESS_MEMBERS*)VesselDataMembers->LocomotionClient->ptr0)->GetVelocity( VesselDataMembers->LocomotionClient, 0, &velocity ) )
			{
				rpm = sqrt( velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z ) / 5.0;

				// Call function to get it done    
				#ifdef ENABLE_SDL
					engine_sound( rpm );	   
				#endif
			}
		}
	}
	else
	{
		#ifdef ENABLE_SDL
			engine_sound( -1 );	   
		#endif
	}
}

// Draw callback 
void draw_vessel(OBJECT_TYPE *o) 
{
	VESSEL_DATA_MEMBERS *VesselDataMembers = (VESSEL_DATA_MEMBERS*)o->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	GLint win = ctrl->Window;

    const float Scalefactor = 1.0/1.7320 ;
	float scale = Scalefactor * VesselDataMembers->Scale;
		
//    // Material properties values
//    float mamb[4] = { 0.5, 0.3, 1.0, 1.0 };
//    float mdif[4] = { 0.5, 0.3, 1.0, 1.0 };
//    float mspec[4] = { 1.0, 1.0, 1.0, 1.0 };
    // Material properties values
    float mamb[4] = { 0.5, 0.5, 0.5, 1.0 };
    float mdif[4] = { 0.7, 0.7, 0.7, 1.0 };
    float mspec[4] = { 1.0, 1.0, 1.0, 1.0 };
    
    
	if ( VesselDataMembers->DrawBars )
	{	
  		
		// draw vessel shell 
		glPushMatrix();	   
		glScalef( scale, scale, scale );
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec) ;
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 100.0);

		if ( !glIsList( VesselDataMembers->VesselList[win] ) ) // a valid list 
		{
			VesselDataMembers->VesselList[win] = glGenLists( 1 );
			glNewList( VesselDataMembers->VesselList[win], GL_COMPILE );
			drawDodecVessel( );
			glEndList( );
		}
		if ( glIsList( VesselDataMembers->VesselList[win] ) ) // a valid list 
		{
			glCallList( VesselDataMembers->VesselList[win] );
		}

		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 0.0);    
		glPopMatrix() ;
	}
}

void draw_vessel_cheap_shadow (OBJECT_TYPE *o)
{
	VESSEL_DATA_MEMBERS *VesselDataMembers = (VESSEL_DATA_MEMBERS*)o->ptr1;
	int i;
	GLfloat theta, x, z;

	glBegin(GL_POLYGON) ;
	glNormal3f(0.0,1.0,0.0) ;
	for( i=0 ; i<6 ; i++ ) 
	{
		theta = PI2*(float)i/6.0 ;
		x = (VesselDataMembers->Scale*sin(theta)) ;
		z = (VesselDataMembers->Scale*cos(theta)) ;
		glVertex3f(-x,0.0,-z) ;
	}
	glEnd() ;
}

void predraw_vessel (OBJECT_TYPE *o)
{
}

extern "C"
{
// This is the well known initialization function. Everything comes from here
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];                 // temporary transform storage 
	OBJECT_TYPE *Vessel;
	float VESSEL_SCALE = 1.0;			// default head size (overriden from config) 
	float VESSEL_X = 0.0;				// default head x location 
	float VESSEL_Y = 0.0;				// default head y location (also set below) 
	float VESSEL_Z = 0.0;				// default head z location 
	float VESSEL_ROLL = 0.0;			// default head roll 
	float VESSEL_PITCH = 0.0;			// default head pitch 
	float VESSEL_YAW = 0.0;				// default head yaw 
	int VESSEL_SOUND = 0;				// make engine sound??? 
	int VESSEL_BARS = 0;				// draw bars??? 
	char VESSEL_LOCOMOTION_NAME[MAX_NAME_SIZE] = { DEFAULT_LOCOMOTION_NAME };  
	VESSEL_ACCESS_MEMBERS *VesselAccessMembers;
	VESSEL_DATA_MEMBERS *VesselDataMembers;
	char *Name = App->appname;
	bool known;
	int windows = Loop_Control->NumberOfWindows;
	
	// generate the "VesselAccessMembers" structure & attach it to the object 
	if ( ( VesselAccessMembers = (VESSEL_ACCESS_MEMBERS*) malloc( sizeof( VESSEL_ACCESS_MEMBERS ) ) ) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for VesselAccessMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	// generate the "VesselAccessMembers" structure & attach it to the object 
	if ( ( VesselDataMembers = (VESSEL_DATA_MEMBERS*) malloc( sizeof( VESSEL_DATA_MEMBERS ) ) ) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for VesselDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	// generate the "VesselAccessMembers" structure & attach it to the object 
	if ( ( VesselDataMembers->VesselList = (GLuint*) malloc( sizeof( GLuint ) * windows ) ) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for VesselDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "locomotion_client"))
		{
			sscanf(App->params[i][2], "%s", VESSEL_LOCOMOTION_NAME);
			known = true;
		}
		if (!strcmp (App->params[i][0], "engine_sound"))
		{
			sscanf(App->params[i][2], "%d", &VESSEL_SOUND);
			known = true;
		}
		if (!strcmp (App->params[i][0], "display_bars"))
		{
			sscanf(App->params[i][2], "%d", &VESSEL_BARS);
			known = true;
		}

		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}
	
	VesselAccessMembers->SetSound = NULL;
	VesselAccessMembers->SetDisplay = NULL;
	
	strncpy( VesselDataMembers->LocomotionClientName, VESSEL_LOCOMOTION_NAME, MAX_NAME_SIZE );

	VesselDataMembers->LocomotionClient = NULL;
	VesselDataMembers->Scale = VESSEL_SCALE;
	VesselDataMembers->Position.x = VESSEL_X;
	VesselDataMembers->Position.y = VESSEL_Y;
	VesselDataMembers->Position.z = VESSEL_Z;
	VesselDataMembers->Position.roll = VESSEL_ROLL;
	VesselDataMembers->Position.pitch = VESSEL_PITCH;
	VesselDataMembers->Position.yaw = VESSEL_YAW;
	VesselDataMembers->EngineSound = VESSEL_SOUND;
	VesselDataMembers->DrawBars = VESSEL_BARS;

	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		VesselDataMembers->VesselList[ i ] = 0;
	}		
	
	// initialize the object for the vessel 
	// make new object 
	Vessel = fl_add_child_to_parent_by_name( App->universe_ptr, App->parent_object_name, App->appname );

	Vessel->ptr0 = (void*)VesselAccessMembers;
	Vessel->ptr1 = (void*)VesselDataMembers;
	
	// set up callbacks 
	Vessel->soundfunc = sound_vessel ;
	Vessel->predrawfunc = predraw_vessel;
	Vessel->drawfunc =  draw_vessel ;
	Vessel->platformfunc = platform_handler ;
	Vessel->trueShadowFunc = draw_vessel ;  /* same as drawfunc */
	Vessel->cheapShadowFunc = draw_vessel_cheap_shadow ;

	// Finally, turn on flags 
	Vessel->sounding_on_flag = 1;
	Vessel->drawing_on_flag = 1;

	// set up the control structure 
	// allows control of the object from the keyboard, menues, and voice 
	Vessel->object_controls = fl_makeControlsSet(2);

	// Define controls 
	strcpy (Vessel->object_controls->controls[0]->menuname, "Toggle Bars");
	strcpy (Vessel->object_controls->controls[0]->voicename, "Bars");
	Vessel->object_controls->controls[0]->keystroke = 'b';
	Vessel->object_controls->controls[0]->control_function = toggle_vis;

	// TODO:  this is commented out until we can shut off looping sounds on the
	//          sound server 
	strcpy (Vessel->object_controls->controls[1]->menuname, "Toggle Sound");
	strcpy (Vessel->object_controls->controls[1]->voicename, "Sound");
	Vessel->object_controls->controls[1]->keystroke = 's';
	Vessel->object_controls->controls[1]->control_function = toggle_vis;

	//// Bounding Volume Setup ////

	Vessel->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	Vessel->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	Vessel->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	Vessel->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	Vessel->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	Vessel->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	/* -x, -y, -z corner */
	Vessel->bounding_volume_set->bvset[0]->p[1][0] = -1.1 * VESSEL_SCALE;    
	Vessel->bounding_volume_set->bvset[0]->p[1][1] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[1][2] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	/* +x, -y, -z corner */
	Vessel->bounding_volume_set->bvset[0]->p[2][0] = 1.1 * VESSEL_SCALE;    
	Vessel->bounding_volume_set->bvset[0]->p[2][1] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[2][2] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	/* -x, +y, -z corner */
	Vessel->bounding_volume_set->bvset[0]->p[3][0] = -1.1 * VESSEL_SCALE;    
	Vessel->bounding_volume_set->bvset[0]->p[3][1] = 1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[3][2] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	/* -x, -y, +z corner */
	Vessel->bounding_volume_set->bvset[0]->p[4][0] = -1.1 * VESSEL_SCALE;    
	Vessel->bounding_volume_set->bvset[0]->p[4][1] = -1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[4][2] = 1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

	/* Define bounding sphere for culling */
	Vessel->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	Vessel->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	Vessel->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	Vessel->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	Vessel->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	/* Surface point */
	Vessel->bounding_volume_set->bvset[1]->p[1][0] = 1.1 * VESSEL_SCALE;
	Vessel->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	Vessel->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	Vessel->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	/* Define a bounding point */
	Vessel->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	Vessel->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	Vessel->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	Vessel->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	Vessel->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	// attach the vessel in a default place 
	PTemp[0] = VESSEL_X;
	PTemp[1] = VESSEL_Y;
	PTemp[2] = VESSEL_Z;
	PTemp[3] = VESSEL_PITCH;
	PTemp[4] = VESSEL_YAW;
	PTemp[5] = VESSEL_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;

	Vessel->orginal_first_node->MpoParams->P->assign( PTemp );

	Vessel->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;
	Vessel->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	Vessel->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	Vessel->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
}
} // extern "C"
