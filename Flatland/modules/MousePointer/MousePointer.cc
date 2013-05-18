/*
MousePointerPointer.cc  An app providing access to position tracking
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
// Modified: 5-13-04, TBE cleaned up code
//

// include the master Flatland library 
#include "flatland_app.h"
#include "MousePointer.h"


void empty(OBJECT_TYPE *o)
{
}

// Platform handler 
void platformfunc(OBJECT_TYPE *MousePointerObject)
{
    // no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *MousePointerObject)
{
    // do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *MousePointerObject)
{
    MOUSE_POINTER_MEMBERS *Data = (MOUSE_POINTER_MEMBERS *)MousePointerObject->ptr0;
    if( Data->DisplayAxes )
    {
	GLboolean lighting_flag = GL_FALSE;
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={0.3,0.3,0.3,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	
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
void trueShadowFunc(OBJECT_TYPE *MousePointerObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *MousePointerObject)
{
	// shadow of what?
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *MousePointerObject)
{
}

// animate callback 
void animatefunc(OBJECT_TYPE *MousePointerObject)
{
    TRANSFORMATION_NODE_ACCESS_MEMBERS *m = 0;
    MOUSE_POINTER_MEMBERS *Data = (MOUSE_POINTER_MEMBERS *)MousePointerObject->ptr0;
    CORDORD pos;
    float PTemp[9];	

    if( Data->DataReady )
    {
	if( !Data->MouseTransformationNode )
	{
	    Data->MouseTransformationNode = fl_find_object_by_name( Universe, Data->MouseTransformationName );
	}
	if( !Data->MouseTransformationNode )
	{
//	    fprintf( stdout, "ERROR finding \"%s\" \n", Data->MouseTransformationName );
	    return;
	}
	
	m = (TRANSFORMATION_NODE_ACCESS_MEMBERS*)Data->MouseTransformationNode->ptr0;

    	m->GetPosition( Data->MouseTransformationNode, &pos );	

//	float yaw = -atan2( pos.z, pos.x ) * 180.0 / 3.14159265;
//	float a = sqrt( pos.x * pos.x + pos.z * pos.z );
//	float pitch = atan2( pos.y, a ) * 180.0 / 3.14159265;
//	float roll = 0.0;
			
	float yaw = atan2( pos.x, -pos.z ) * 180.0 / 3.14159265;
	float a = sqrt( pos.x * pos.x + pos.z * pos.z );
	float pitch = -atan2( pos.y, a ) * 180.0 / 3.14159265;
	float roll = 0.0;
			
	PTemp[0] = 0.0;
	PTemp[1] = 0.0;
	PTemp[2] = 0.0;
	PTemp[3] = -pitch;
	PTemp[4] = -yaw;
	PTemp[5] = -roll;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	MousePointerObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	MousePointerObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	MousePointerObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	MousePointerObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	MousePointerObject->orginal_first_node->MpoParams->P->assign( PTemp );
    }
}

/* menu control callback */
void MenuCallback(OBJECT_TYPE *MousePointerObject, char key)
{
    MOUSE_POINTER_MEMBERS *MousePointerMembers2= (MOUSE_POINTER_MEMBERS *) MousePointerObject->ptr0;

    switch (key)
    {
    case 'a':   // display axes
	MousePointerMembers2->DisplayAxes = true;
	break;

    case 'A':   // hide axes
	MousePointerMembers2->DisplayAxes = false;
	break;
    default:
	break;
    }
}


extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
    int i;
    float PTemp[9];					/* temporary transform storage */
    OBJECT_TYPE *MousePointerObject;				/* This object */
//	pthread_t transducer_read_thread;		/* thread handle for data input */
    MOUSE_POINTER_MEMBERS *MousePointerMembers;			/* pointer to "MousePointerMembers" structure */
    float POINTER_X = 0.0f;
    float POINTER_Y = 0.0f;
    float POINTER_Z = 0.0f;
    float POINTER_ROLL = 0.0f;
    float POINTER_PITCH = 0.0f;
    float POINTER_YAW = 0.0f;
    char *Name = App->appname;
    bool known;
    OBJ_CONTROLS_ARRAY_TYPE *subMenu; /* submenu temp variable */

    /* generate the "MousePointerMembers" structure & attach it to the object */
    if ((MousePointerMembers = (MOUSE_POINTER_MEMBERS *) malloc (sizeof (MOUSE_POINTER_MEMBERS))) == NULL)
    {
	printf ("%s: failed to initialize memory for MousePointerMembers\n", Name);
	exit(1);
    }


    MousePointerMembers->DataReady = false;
    MousePointerMembers->DisplayAxes = false;
    strcpy( MousePointerMembers->MouseTransformationName, "MouseTransformationNode" );
    MousePointerMembers->MouseTransformationNode = 0;
    
    /* parse the parameter list */
    for (i=0; i < App->nparams; i++)
    {
	known = false;

	if (!strcmp (App->params[i][0], "x"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_X);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "y"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_Y);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "z"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_Z);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "roll"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_ROLL);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "pitch"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_PITCH);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "yaw"))
	{
	    sscanf(App->params[i][2], "%f", &POINTER_YAW);
	    known = true;
	}

	if (!strcmp (App->params[i][0], "transformation"))
	{
	    strncpy( MousePointerMembers->MouseTransformationName, App->params[i][2], MAX_NAME_SIZE );
	    known = true;
	}

	if (!known)
	{
	    fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
	}
    }

    MousePointerMembers->Position.x = POINTER_X;
    MousePointerMembers->Position.y = POINTER_Y;
    MousePointerMembers->Position.z = POINTER_Z;
    MousePointerMembers->Position.roll = POINTER_ROLL;
    MousePointerMembers->Position.pitch = POINTER_PITCH;
    MousePointerMembers->Position.yaw = POINTER_YAW;

    MousePointerObject = fl_add_child_to_parent_by_name(	App->universe_ptr,
													    App->parent_object_name,
													    App->appname);

    MousePointerObject->ptr0 = (void *) MousePointerMembers;	/* attach struct to object */

    /* set up callbacks */
    MousePointerObject->soundfunc = soundfunc;
    MousePointerObject->sounding_on_flag = 0;
    MousePointerObject->platformfunc = platformfunc;
    MousePointerObject->predrawfunc = predrawfunc;
    MousePointerObject->drawfunc = drawfunc;
    MousePointerObject->trueShadowFunc = trueShadowFunc;
    MousePointerObject->cheapShadowFunc = cheapShadowFunc;
    MousePointerObject->drawing_on_flag = 1;
    MousePointerObject->pregraphicsfunc = animatefunc;

    //// Menu Setup ////

    /* set up the control structure (three subcontrols under one) */
    /* allows control of the object from the keyboard, menues, and voice */
    MousePointerObject->object_controls = fl_makeControlsSet(1);

    /* Define spin/nospin controls for cube */
    strcpy (MousePointerObject->object_controls->controls[0]->menuname, "Axes");
    strcpy (MousePointerObject->object_controls->controls[0]->voicename, "Axes");
    MousePointerObject->object_controls->controls[0]->children = fl_makeControlsSet(2);
    MousePointerObject->object_controls->controls[0]->control_function = NULL;
    subMenu = MousePointerObject->object_controls->controls[0]->children;

    strcpy (subMenu->controls[0]->menuname, "Enable");
    strcpy (subMenu->controls[0]->voicename, "Enable");
    subMenu->controls[0]->keystroke = 'a';
    subMenu->controls[0]->control_function = MenuCallback;

    strcpy (subMenu->controls[1]->menuname, "Disable");
    strcpy (subMenu->controls[1]->voicename, "Disable");
    subMenu->controls[1]->keystroke = 'A';
    subMenu->controls[1]->control_function = MenuCallback;

    //// Bounding Volume Setup ////

    MousePointerObject->bounding_volume_set = fl_makebvset(3);

    /* define a bounding volume (four corners and a center) */
    MousePointerObject->bounding_volume_set->bvset[0]->npoints = 5;
    /* center point */
    MousePointerObject->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
    MousePointerObject->bounding_volume_set->bvset[0]->p[0][1]=0.0;
    MousePointerObject->bounding_volume_set->bvset[0]->p[0][2]=0.0;
    MousePointerObject->bounding_volume_set->bvset[0]->p[0][3]=1.0;
    /* -x, -y, -z corner */
    MousePointerObject->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
    MousePointerObject->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[1][3]=1.0;
    /* +x, -y, -z corner */
    MousePointerObject->bounding_volume_set->bvset[0]->p[2][0]=+0.01;    
    MousePointerObject->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[2][3]=1.0;
    /* -x, +y, -z corner */
    MousePointerObject->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
    MousePointerObject->bounding_volume_set->bvset[0]->p[3][1]=+0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[3][3]=1.0;
    /* -x, -y, +z corner */
    MousePointerObject->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
    MousePointerObject->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[4][2]=+0.01;
    MousePointerObject->bounding_volume_set->bvset[0]->p[4][3]=1.0;

    /* Define bounding sphere for culling */
    MousePointerObject->bounding_volume_set->bvset[1]->npoints = 2 ;
    /* Center point */
    MousePointerObject->bounding_volume_set->bvset[1]->p[0][0]=0.0;
    MousePointerObject->bounding_volume_set->bvset[1]->p[0][1]=0.0;
    MousePointerObject->bounding_volume_set->bvset[1]->p[0][2]=0.0;
    MousePointerObject->bounding_volume_set->bvset[1]->p[0][3]=1.0;
    /* Surface point */
    MousePointerObject->bounding_volume_set->bvset[1]->p[1][0]=+0.01;
    MousePointerObject->bounding_volume_set->bvset[1]->p[1][1]=+0.01;
    MousePointerObject->bounding_volume_set->bvset[1]->p[1][2]=+0.01;	
    MousePointerObject->bounding_volume_set->bvset[1]->p[1][3]=1.0;

    /* Define a bounding point */
    MousePointerObject->bounding_volume_set->bvset[2]->npoints = 1 ;
    /* Center point */
    MousePointerObject->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
    MousePointerObject->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
    MousePointerObject->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
    MousePointerObject->bounding_volume_set->bvset[2]->p[0][3]= 1.0;

    /* attach to the wessel in a default place */
    PTemp[0] = POINTER_X;
    PTemp[1] = POINTER_Y;
    PTemp[2] = POINTER_Z; 
    PTemp[3] = POINTER_YAW;
    PTemp[4] = POINTER_PITCH;
    PTemp[5] = POINTER_ROLL;
    PTemp[6] = 1.0;
    PTemp[7] = 1.0;
    PTemp[8] = 1.0;
    MousePointerObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
    MousePointerObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
    MousePointerObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
    MousePointerObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
    MousePointerObject->orginal_first_node->MpoParams->P->assign( PTemp );

    MousePointerMembers->DataReady = true;
}

} // extern "C"
