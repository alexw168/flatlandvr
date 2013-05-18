/*
EyeStalk.cc  An app representing the eye_stalk

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
#include "EyeStalk.h"
#include "GL_Object.h"

// Function to read in a data file 
void *ReadEyeData (void *X)
{
	FILE *inFile;					// input file handle 
	OBJECT_TYPE *EyeStalk;				// pointer to the object passed in (this obj) 
	EYE_STALK_DATA_MEMBERS *EyeStalkDataMembers;			// "global" structure pointer 
	char *Name;
	
	EyeStalk = (OBJECT_TYPE *) X;
	EyeStalkDataMembers = (EYE_STALK_DATA_MEMBERS *) EyeStalk->ptr1;
	Name = EyeStalk->name;

	fprintf(stdout,"%s: reading data file '%s'\n", Name, EyeStalkDataMembers->EyeStalkDataFile);

	if ( strlen( EyeStalkDataMembers->EyeStalkDataFile ) > 0 )	// check for non-zero filename 
	{
		// open the file
		inFile = fopen( EyeStalkDataMembers->EyeStalkDataFile, "rb" );
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, EyeStalkDataMembers->EyeStalkDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SGL_Object*)EyeStalkDataMembers->EyeStalk)->Read(inFile);

		fclose(inFile);
		
		EyeStalkDataMembers->DataReady = true;
	}

	return (NULL);
}

// Platform handler 
void eye_stalk_platformfunc(OBJECT_TYPE *EyeStalkObject)
{
}

// Sound callback 
void eye_stalk_soundfunc(OBJECT_TYPE *EyeStalkObject)
{
}

void DrawEyeStalk(OBJECT_TYPE *EyeStalkObject)
{
	EYE_STALK_DATA_MEMBERS *EyeStalkDataMembers = (EYE_STALK_DATA_MEMBERS *) EyeStalkObject->ptr1;

	glPushMatrix();
	glRotatef(180,0,1,0);
	glScalef(EyeStalkDataMembers->Scale,EyeStalkDataMembers->Scale,EyeStalkDataMembers->Scale);
	((SGL_Object *) EyeStalkDataMembers->EyeStalk)->Draw();
	glPopMatrix();
}

// Draw callback 
void eye_stalk_drawfunc(OBJECT_TYPE *EyeStalkObject)
{
	EYE_STALK_DATA_MEMBERS *EyeStalkDataMembers = (EYE_STALK_DATA_MEMBERS *) EyeStalkObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	int win = ctrl->Window;

	if ( !EyeStalkDataMembers->DataReady ) 
	{
		return;
	}
      
	if ( !glIsList( EyeStalkDataMembers->EyeStalkList[ win ] ) )
	{
		EyeStalkDataMembers->EyeStalkList[ win ] = glGenLists( 1 );
		glNewList( EyeStalkDataMembers->EyeStalkList[ win ], GL_COMPILE );
		DrawEyeStalk( EyeStalkObject );
		glEndList( );
	}

	if ( glIsList( EyeStalkDataMembers->EyeStalkList[ win ] ) )
	{
		glCallList( EyeStalkDataMembers->EyeStalkList[ win ] );
	}
}

// true shadow callback 
void eye_stalk_trueShadowFunc(OBJECT_TYPE *EyeStalkObject)
{
	EYE_STALK_DATA_MEMBERS *EyeStalkDataMembers = (EYE_STALK_DATA_MEMBERS *) EyeStalkObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	int win = ctrl->Window;

	if ( !EyeStalkDataMembers->DataReady ) 
	{
		return;
	}
      
	if ( glIsList( EyeStalkDataMembers->EyeStalkList[ win ] ) )
	{
		glCallList( EyeStalkDataMembers->EyeStalkList[ win ] );
	}
}

// cheap shadow callback 
void eye_stalk_cheapShadowFunc(OBJECT_TYPE *EyeStalkObject)
{
}

// predraw callback 
void eye_stalk_animatefunc(OBJECT_TYPE *EyeStalkObject)
{
}

// predraw callback 
void eye_stalk_predrawfunc(OBJECT_TYPE *EyeStalkObject)
{
}

// This is the well known initialization function. Everything comes from here 
void EyeStalkInit( APPTYPE *App, char *parent, char *name, char *prefix )
{
	int i;
	float PTemp[9];									// temporary transform storage 
	GLfloat EYE_SCALE = 1.0;						// default eye size 
	GLfloat EYE_X = 0.0;							// default eye x location 
	GLfloat EYE_Y = 0.0;							// default eye y location
	GLfloat EYE_Z = 0.0;							// default eye z location 
	GLfloat EYE_ROLL = 0.0;							// default eye roll 
	GLfloat EYE_PITCH = 0.0;						// default eye pitch 
	GLfloat EYE_YAW = 0.0;							// default eye yaw 
	OBJECT_TYPE *EyeStalk;							// This object 
	EYE_STALK_DATA_MEMBERS *EyeStalkDataMembers;
	pthread_t eye_data_read_thread;					// thread handle for data input 
	char EYE_DATA_FILE[MAX_NAME_SIZE] = { "modules/Head_Eyes/EyeBall.dat" };  
	char *Name = App->appname;
	char param_eye_scale[MAX_NAME_SIZE];
	char param_eye_x[MAX_NAME_SIZE];
	char param_eye_y[MAX_NAME_SIZE];
	char param_eye_z[MAX_NAME_SIZE];
	char param_eye_roll[MAX_NAME_SIZE];
	char param_eye_pitch[MAX_NAME_SIZE];
	char param_eye_yaw[MAX_NAME_SIZE];
	char param_eye_data_file[MAX_NAME_SIZE];
	
	assert(App);

	// generate paramater strings for matching	
	sprintf( param_eye_scale, "%s_eye_scale", prefix );
	sprintf( param_eye_x, "%s_eye_x", prefix );
	sprintf( param_eye_y, "%s_eye_y", prefix );
	sprintf( param_eye_z, "%s_eye_z", prefix );
	sprintf( param_eye_roll, "%s_eye_roll", prefix );
	sprintf( param_eye_pitch, "%s_eye_pitch", prefix );
	sprintf( param_eye_yaw, "%s_eye_yaw", prefix );
	sprintf( param_eye_data_file, "%s_eye_data_file", prefix );
			 
	// generate the "EyeStalkMembers" structure & attach it to the object 
	if ( ( EyeStalkDataMembers = (EYE_STALK_DATA_MEMBERS *) malloc( sizeof( EYE_STALK_DATA_MEMBERS ) ) ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for EyeStalkDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	if ( ( EyeStalkDataMembers->EyeStalk = new SGL_Object ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for EyeStalkDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	if ( ( EyeStalkDataMembers->EyeStalkList = new GLuint[ Loop_Control->NumberOfWindows ] ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for EyeStalkDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	for(i=0;i<Loop_Control->NumberOfWindows;i++)
	{
		EyeStalkDataMembers->EyeStalkList[ i ] = 0;
	}
	

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		if (!strcmp (App->params[i][0], param_eye_x))
		{
			sscanf(App->params[i][2], "%f", &EYE_X);
		}
		if (!strcmp (App->params[i][0], param_eye_y))
		{
			sscanf(App->params[i][2], "%f", &EYE_Y);
		}
		if (!strcmp (App->params[i][0], param_eye_z))
		{
			sscanf(App->params[i][2], "%f", &EYE_Z);
		}
		if (!strcmp (App->params[i][0], param_eye_pitch))
		{
			sscanf(App->params[i][2], "%f", &EYE_PITCH);
		}
		if (!strcmp (App->params[i][0], param_eye_yaw))
		{
			sscanf(App->params[i][2], "%f", &EYE_YAW);
		}
		if (!strcmp (App->params[i][0], param_eye_roll))
		{
			sscanf(App->params[i][2], "%f", &EYE_ROLL);
		}
		if (!strcmp (App->params[i][0], param_eye_scale))
		{
			sscanf(App->params[i][2], "%f", &EYE_SCALE);
		}
		if (!strcmp (App->params[i][0], param_eye_data_file))
		{
			sscanf(App->params[i][2], "%s", EYE_DATA_FILE);
		}
	}

	strncpy( EyeStalkDataMembers->EyeStalkDataFile, EYE_DATA_FILE, MAX_NAME_SIZE );
	
	EyeStalkDataMembers->Scale = EYE_SCALE;
	EyeStalkDataMembers->Position.x = EYE_X;
	EyeStalkDataMembers->Position.y = EYE_Y;
	EyeStalkDataMembers->Position.z = EYE_Z;
	EyeStalkDataMembers->Position.roll = EYE_ROLL;
	EyeStalkDataMembers->Position.pitch = EYE_PITCH;
	EyeStalkDataMembers->Position.yaw = EYE_YAW;

	//////////////////////////////////////////////////////////////

	EyeStalk = fl_add_child_to_parent_by_name( App->universe_ptr, parent, name );

	EyeStalk->ptr1 = (void *) EyeStalkDataMembers;	// attach struct to object 

	// set up callbacks 
	EyeStalk->soundfunc = eye_stalk_soundfunc;
	EyeStalk->sounding_on_flag = false;
	EyeStalk->platformfunc = eye_stalk_platformfunc;
	EyeStalk->pregraphicsfunc = eye_stalk_animatefunc;
	EyeStalk->predrawfunc = eye_stalk_predrawfunc;
	EyeStalk->drawfunc = eye_stalk_drawfunc;
	EyeStalk->trueShadowFunc = eye_stalk_trueShadowFunc;
	EyeStalk->cheapShadowFunc = eye_stalk_cheapShadowFunc;
	EyeStalk->drawing_on_flag = true;
	EyeStalk->object_controls = NULL;

	EyeStalk->bounding_volume_set = fl_makebvset(3);

	// define a bounding volume (four corners and a center) 
	EyeStalk->bounding_volume_set->bvset[0]->npoints = 5;
	// center point 
	EyeStalk->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	EyeStalk->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	EyeStalk->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	EyeStalk->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	// -x, -y, -z corner 
	EyeStalk->bounding_volume_set->bvset[0]->p[1][0] = -0.02 * EYE_SCALE;    
	EyeStalk->bounding_volume_set->bvset[0]->p[1][1] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[1][2] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	// +x, -y, -z corner 
	EyeStalk->bounding_volume_set->bvset[0]->p[2][0] = 0.02 * EYE_SCALE;    
	EyeStalk->bounding_volume_set->bvset[0]->p[2][1] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[2][2] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	// -x, +y, -z corner 
	EyeStalk->bounding_volume_set->bvset[0]->p[3][0] = -0.02 * EYE_SCALE;    
	EyeStalk->bounding_volume_set->bvset[0]->p[3][1] = 0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[3][2] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	//* -x, -y, +z corner 
	EyeStalk->bounding_volume_set->bvset[0]->p[4][0] = -0.02 * EYE_SCALE;    
	EyeStalk->bounding_volume_set->bvset[0]->p[4][1] = -0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[4][2] = 0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

	// Define bounding sphere for culling 
	EyeStalk->bounding_volume_set->bvset[1]->npoints = 2 ;
	// Center point 
	EyeStalk->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	EyeStalk->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	EyeStalk->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	EyeStalk->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	// Surface point 
	EyeStalk->bounding_volume_set->bvset[1]->p[1][0] = 0.02 * EYE_SCALE;
	EyeStalk->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	EyeStalk->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	EyeStalk->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	// Define a bounding point
	EyeStalk->bounding_volume_set->bvset[2]->npoints = 1 ;
	// Center point 
	EyeStalk->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	EyeStalk->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	EyeStalk->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	EyeStalk->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	// attach to the head in a default place 
	PTemp[0] = EYE_X;
	PTemp[1] = EYE_Y;
	PTemp[2] = EYE_Z;
	PTemp[3] = EYE_PITCH;
	PTemp[4] = EYE_YAW;
	PTemp[5] = EYE_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	EyeStalk->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	EyeStalk->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	EyeStalk->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	EyeStalk->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	EyeStalk->orginal_first_node->MpoParams->P->assign( PTemp );

	// spawn a thread to read the data file 
	pthread_create( &eye_data_read_thread, NULL, &ReadEyeData, (void *) EyeStalk );
}

