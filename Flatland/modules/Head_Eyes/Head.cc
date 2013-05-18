/*
Head.cc  An app representing the head

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
#include "Head.h"
#include "GL_Object.h"

#define DEFAULT_DATA_FILE "modules/Head_Eyes/HumanHead.dat"

// Function to read in a data file 
void *ReadHeadData (void *X)
{
	FILE *inFile;					// input file handle 
	OBJECT_TYPE *Head;				// pointer to the object passed in (this obj) 
	HEAD_DATA_MEMBERS *HeadDataMembers;			// "global" structure pointer 
	char *Name;
	
	Head = (OBJECT_TYPE *) X;
	HeadDataMembers = (HEAD_DATA_MEMBERS *) Head->ptr1;
	Name = Head->name;

	fprintf(stdout,"%s: reading data file '%s'\n", Name, HeadDataMembers->HeadDataFile);

	if (strlen (HeadDataMembers->HeadDataFile) > 0)	// check for non-zero filename 
	{
		// open the file
		inFile = fopen (HeadDataMembers->HeadDataFile, "rb");
		if (inFile == NULL)
		{
			fprintf (stderr, "%s: could not open input data file '%s'\n", Name, HeadDataMembers->HeadDataFile);
			fprintf (stderr, "    %s will not display\n", Name);
			return (NULL);
		}

	 	// Read in the wand image data
		((SGL_Object *) HeadDataMembers->Head)->Read(inFile);

		fclose(inFile);
		
		HeadDataMembers->DataReady = true;
	}

	return (NULL);
}

// Platform handler 
void head_platformfunc(OBJECT_TYPE *HeadObject)
{
	// no platform handling for this object 
}

// Sound callback 
void head_soundfunc(OBJECT_TYPE *HeadObject)
{
	// do nothing
}

void DrawHead(OBJECT_TYPE *HeadObject)
{
	HEAD_DATA_MEMBERS *HeadDataMembers = (HEAD_DATA_MEMBERS *) HeadObject->ptr1;
	glPushMatrix();
	glRotatef(180,0,1,0);
	glScalef(HeadDataMembers->Scale,HeadDataMembers->Scale,HeadDataMembers->Scale);
	((SGL_Object *) HeadDataMembers->Head)->Draw();
	glPopMatrix();
}

// Draw callback 
void head_drawfunc(OBJECT_TYPE *HeadObject)
{
	HEAD_DATA_MEMBERS *HeadDataMembers = (HEAD_DATA_MEMBERS *) HeadObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	int win = ctrl->Window;

	if ( !HeadDataMembers->DataReady ) 
	{
		return;
	}
      
	if ( !glIsList( HeadDataMembers->HeadList[ win ] ) )
	{
		HeadDataMembers->HeadList[ win ] = glGenLists( 1 );
		glNewList( HeadDataMembers->HeadList[ win ], GL_COMPILE );
		DrawHead( HeadObject );
		glEndList( );
		// HeadDataMembers->Head could be deallocated now...
	}

	if ( glIsList( HeadDataMembers->HeadList[win ] ) )
	{
		glCallList( HeadDataMembers->HeadList[ win ] );
	}
}

// true shadow callback 
void head_trueShadowFunc(OBJECT_TYPE *HeadObject)
{
	HEAD_DATA_MEMBERS *HeadDataMembers = (HEAD_DATA_MEMBERS *) HeadObject->ptr1;
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	int win = ctrl->Window;

	if ( !HeadDataMembers->DataReady ) 
	{
		return;
	}
      
	if ( glIsList( HeadDataMembers->HeadList[win ] ) )
	{
		glCallList( HeadDataMembers->HeadList[ win ] );
	}
}

// cheap shadow callback 
void head_cheapShadowFunc(OBJECT_TYPE *HeadObject)
{
}

// predraw callback 
void head_animatefunc(OBJECT_TYPE *HeadObject)
{
}

// predraw callback 
void head_predrawfunc(OBJECT_TYPE *HeadObject)
{
}

// This is the well known initialization function. Everything comes from here 
void HeadInit( APPTYPE *App, char *parent, char *name )
{
	int i;
	float PTemp[9];						/* temporary transform storage */
	GLfloat HEAD_SCALE = 1.0;			/* default head size (overriden from config) */
	GLfloat HEAD_X = 0.0;				/* default head x location */
	GLfloat HEAD_Y = 0.0;				/* default head y location (also set below) */
	GLfloat HEAD_Z = 0.0;				/* default head z location */
	GLfloat HEAD_ROLL = 0.0;			/* default head roll */
	GLfloat HEAD_PITCH = 0.0;			/* default head pitch */
	GLfloat HEAD_YAW = 0.0;				/* default head yaw */
	OBJECT_TYPE *Head;					/* This object */
	HEAD_DATA_MEMBERS *HeadDataMembers;			/* pointer to "HeadDataMembers" structure */
	pthread_t head_data_read_thread;			/* thread handle for data input */
	char HEAD_DATA_FILE[MAX_NAME_SIZE] = { "modules/Head_Eyes/HumanHead.dat" };  
	char *Name = App->appname;

	assert(App);
			 
	// generate the "HeadDataMembers" structure & attach it to the object 
	if ( ( HeadDataMembers = (HEAD_DATA_MEMBERS *) malloc( sizeof( HEAD_DATA_MEMBERS ) ) ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for HeadDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	if ( ( HeadDataMembers->Head = new SGL_Object ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for HeadDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	if ( ( HeadDataMembers->HeadList = new GLuint[ Loop_Control->NumberOfWindows ] ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for HeadDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	for(i=0;i<Loop_Control->NumberOfWindows;i++)
	{
		HeadDataMembers->HeadList[ i ] = 0;
	}
	
	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		if (!strcmp (App->params[i][0], "head_x"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_X);
		}
		if (!strcmp (App->params[i][0], "head_y"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_Y);
		}
		if (!strcmp (App->params[i][0], "head_z"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_Z);
		}
		if (!strcmp (App->params[i][0], "head_pitch"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_PITCH);
		}
		if (!strcmp (App->params[i][0], "head_yaw"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_YAW);
		}
		if (!strcmp (App->params[i][0], "head_roll"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_ROLL);
		}
		if (!strcmp (App->params[i][0], "head_scale"))
		{
			sscanf(App->params[i][2], "%f", &HEAD_SCALE);
		}
		if (!strcmp (App->params[i][0], "head_data_file"))
		{
			sscanf(App->params[i][2], "%s", HEAD_DATA_FILE);
		}
	}

	strncpy( HeadDataMembers->HeadDataFile, HEAD_DATA_FILE, MAX_NAME_SIZE );
	
	HeadDataMembers->DataReady = false;
	HeadDataMembers->Scale = HEAD_SCALE;
	HeadDataMembers->Position.x = HEAD_X;
	HeadDataMembers->Position.y = HEAD_Y;
	HeadDataMembers->Position.z = HEAD_Z;
	HeadDataMembers->Position.roll = HEAD_ROLL;
	HeadDataMembers->Position.pitch = HEAD_PITCH;
	HeadDataMembers->Position.yaw = HEAD_YAW;

	//////////////////////////////////////////////////////////////

	Head = fl_add_child_to_parent_by_name( App->universe_ptr, parent, name );

	Head->ptr1 = (void *) HeadDataMembers;	// attach struct to object 

	// set up callbacks 
	Head->soundfunc = head_soundfunc;
	Head->sounding_on_flag = 0;
	Head->platformfunc = head_platformfunc;
	Head->pregraphicsfunc = head_animatefunc;
	Head->predrawfunc = head_predrawfunc;
	Head->drawfunc = head_drawfunc;
	Head->trueShadowFunc = head_trueShadowFunc;
	Head->cheapShadowFunc = head_cheapShadowFunc;
	Head->drawing_on_flag = 1 ;
	Head->object_controls = NULL;
	
	Head->bounding_volume_set = fl_makebvset(3);

	// define a bounding volume (four corners and a center) 
	Head->bounding_volume_set->bvset[0]->npoints = 5;
	// center point 
	Head->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	Head->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	Head->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	Head->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	// -x, -y, -z corner 
	Head->bounding_volume_set->bvset[0]->p[1][0] = -0.2 * HEAD_SCALE;    
	Head->bounding_volume_set->bvset[0]->p[1][1] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[1][2] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	// +x, -y, -z corner 
	Head->bounding_volume_set->bvset[0]->p[2][0] = 0.2 * HEAD_SCALE;    
	Head->bounding_volume_set->bvset[0]->p[2][1] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[2][2] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	// -x, +y, -z corner 
	Head->bounding_volume_set->bvset[0]->p[3][0] = -0.2 * HEAD_SCALE;    
	Head->bounding_volume_set->bvset[0]->p[3][1] = 0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[3][2] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	// -x, -y, +z corner 
	Head->bounding_volume_set->bvset[0]->p[4][0] = -0.2 * HEAD_SCALE;    
	Head->bounding_volume_set->bvset[0]->p[4][1] = -0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[4][2] = 0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

	// Define bounding sphere for culling 
	Head->bounding_volume_set->bvset[1]->npoints = 2 ;
	// Center point 
	Head->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	Head->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	Head->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	Head->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	// Surface point 
	Head->bounding_volume_set->bvset[1]->p[1][0] = 0.2 * HEAD_SCALE;
	Head->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	Head->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	Head->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	// Define a bounding point 
	Head->bounding_volume_set->bvset[2]->npoints = 1 ;
	// Center point 
	Head->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	Head->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	Head->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	Head->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	// attach to the universe in a default place 
	PTemp[0] = HEAD_X;
	PTemp[1] = HEAD_Y;
	PTemp[2] = HEAD_Z;
	PTemp[3] = HEAD_PITCH;
	PTemp[4] = HEAD_YAW;
	PTemp[5] = HEAD_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	Head->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	Head->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	Head->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	Head->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	Head->orginal_first_node->MpoParams->P->assign( PTemp );

	// spawn a thread to read the data file 
	pthread_create( &head_data_read_thread, NULL, &ReadHeadData, (void *) Head );
}

