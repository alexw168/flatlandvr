/*
Eye.cc  An app representing the eye

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
// Created: 7-4-00, TBE
//

// include the master Flatland library 
#include "flatland_app.h"
#include "Eye.h"
#include "GL_Object.h"

// Platform handler 
void eye_platformfunc(OBJECT_TYPE *EyeObject)
{
}

// Sound callback 
void eye_soundfunc(OBJECT_TYPE *EyeObject)
{
}

// Draw callback 
void eye_drawfunc(OBJECT_TYPE *EyeObject)
{
}

// true shadow callback 
void eye_trueShadowFunc(OBJECT_TYPE *EyeObject)
{
}

// cheap shadow callback 
void eye_cheapShadowFunc(OBJECT_TYPE *EyeObject)
{
}

// animation callback 
void eye_animatefunc(OBJECT_TYPE *EyeObject)
{
}

// predraw callback 
void eye_predrawfunc(OBJECT_TYPE *EyeObject)
{
	EYE_DATA_MEMBERS *EyeDataMembers = (EYE_DATA_MEMBERS*)EyeObject->ptr1;
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
	EyeObject->orginal_first_node->MpoParams->Po[3] = TRANSLATE ;	
	EyeObject->orginal_first_node->MpoParams->Po[2] = ROTATE010 ;
	EyeObject->orginal_first_node->MpoParams->Po[1] = ROTATE100 ;
	EyeObject->orginal_first_node->MpoParams->Po[0] = ROTATE001 ;
	EyeObject->orginal_first_node->MpoParams->P->assign( PTemp );

	// undo head rotation if using fixed viewing planes (as in a cave setup)
	if ( ctrl->FixedViewingPlanes )
	{	
		if ( EyeObject->universe == ctrl->Universe )
		{
			if ( !EyeDataMembers->HeadTrackerObject )
			{
				EyeDataMembers->HeadTrackerObject = fl_find_object_by_name( ctrl->Universe, EyeDataMembers->HeadTrackerName );
			}
			if ( EyeDataMembers->HeadTrackerObject )
			{
				TRANSFORMATION_NODE_ACCESS_MEMBERS *members = (TRANSFORMATION_NODE_ACCESS_MEMBERS *)(EyeDataMembers->HeadTrackerObject->ptr0);
				if ( members )
				{
					if ( members->GetPosition )
					{
						CORDORD pos;
						members->GetPosition( EyeDataMembers->HeadTrackerObject, &pos );

						PTemp[0] = 0.0;
						PTemp[1] = 0.0;
						PTemp[2] = 0.0;
						PTemp[3] = -pos.pitch;
						PTemp[4] = -pos.yaw;
						PTemp[5] = -pos.roll;
						PTemp[6] = 1.0;
						PTemp[7] = 1.0;
						PTemp[8] = 1.0;
						EyeObject->orginal_first_node->MpoParams->Po[3] = TRANSLATE ;	
						EyeObject->orginal_first_node->MpoParams->Po[2] = ROTATE010 ;
						EyeObject->orginal_first_node->MpoParams->Po[1] = ROTATE100 ;
						EyeObject->orginal_first_node->MpoParams->Po[0] = ROTATE001 ;
						EyeObject->orginal_first_node->MpoParams->P->assign( PTemp );
					}
				}
			}
		}
	}
}


// This is the well known initialization function. Everything comes from here 
void EyeInit( APPTYPE *App, char *parent, char *name )
{
	float PTemp[9];						// temporary transform storage 
	OBJECT_TYPE *Eye;					// This object 
	char HEAD_TRACKER_NAME[MAX_NAME_SIZE] = { "HeadTransformationNode" };			// head transformationnode
	EYE_DATA_MEMBERS *EyeDataMembers = NULL;
	int i;
		
	assert(App);

	// generate the "EyeDataMembers" structure & attach it to the object 
	if ( ( EyeDataMembers = (EYE_DATA_MEMBERS *) malloc( sizeof( EYE_DATA_MEMBERS ) ) ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for EyeDataMembers\n", name);
		fflush(stderr);
		exit(1);
	}

	// parse the parameter list 
	for (i=0; i < App->nparams; i++)
	{
		if ( !strcmp( App->params[i][0], "head_tracker" ) )
		{
			sscanf(App->params[i][2], "%s", HEAD_TRACKER_NAME );
		}
	}

	strncpy( EyeDataMembers->HeadTrackerName, HEAD_TRACKER_NAME, MAX_NAME_SIZE );
	
	Eye = fl_add_child_to_parent_by_name( App->universe_ptr, parent, name);

	Eye->ptr1 = (void*)EyeDataMembers;
	
	// set up callbacks 
	Eye->soundfunc = eye_soundfunc;
	Eye->sounding_on_flag = false;
	Eye->platformfunc = eye_platformfunc;
	Eye->pregraphicsfunc = eye_animatefunc;
	Eye->predrawfunc = eye_predrawfunc;
	Eye->drawfunc = eye_drawfunc;
	Eye->trueShadowFunc = eye_trueShadowFunc;
	Eye->cheapShadowFunc = eye_cheapShadowFunc;
	Eye->drawing_on_flag = false;
	Eye->object_controls = NULL;

	Eye->bounding_volume_set = fl_makebvset(3);

	// define a bounding volume (four corners and a center) 
	Eye->bounding_volume_set->bvset[0]->npoints = 5;
	// center point 
	Eye->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	Eye->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	Eye->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	Eye->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	// -x, -y, -z corner 
	Eye->bounding_volume_set->bvset[0]->p[1][0] = -0.02;    
	Eye->bounding_volume_set->bvset[0]->p[1][1] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[1][2] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	// +x, -y, -z corner 
	Eye->bounding_volume_set->bvset[0]->p[2][0] = 0.02;    
	Eye->bounding_volume_set->bvset[0]->p[2][1] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[2][2] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	// -x, +y, -z corner 
	Eye->bounding_volume_set->bvset[0]->p[3][0] = -0.02;    
	Eye->bounding_volume_set->bvset[0]->p[3][1] = 0.02;
	Eye->bounding_volume_set->bvset[0]->p[3][2] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	// -x, -y, +z corner 
	Eye->bounding_volume_set->bvset[0]->p[4][0] = -0.02;    
	Eye->bounding_volume_set->bvset[0]->p[4][1] = -0.02;
	Eye->bounding_volume_set->bvset[0]->p[4][2] = 0.02;
	Eye->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

	// Define bounding sphere for culling 
	Eye->bounding_volume_set->bvset[1]->npoints = 2;
	// Center point 
	Eye->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	Eye->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	Eye->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	Eye->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	// Surface point 
	Eye->bounding_volume_set->bvset[1]->p[1][0] = 0.02;
	Eye->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	Eye->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	Eye->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	// Define a bounding point 
	Eye->bounding_volume_set->bvset[2]->npoints = 1;
	// Center point 
	Eye->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
	Eye->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
	Eye->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	Eye->bounding_volume_set->bvset[2]->p[0][3]= 1.0;
  
	// attach to the head in a default place 
	PTemp[0] = 0.0;
	PTemp[1] = 0.0;
	PTemp[2] = 0.0;
	PTemp[3] = 0.0;
	PTemp[4] = 0.0;
	PTemp[5] = 0.0;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	Eye->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	Eye->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	Eye->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	Eye->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	Eye->orginal_first_node->MpoParams->P->assign( PTemp );
}
