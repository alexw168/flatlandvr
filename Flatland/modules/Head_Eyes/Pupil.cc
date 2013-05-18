/*
Pupil.cc  An app representing the pupil

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
#include "Pupil.h"

// Platform handler 
void pupil_platformfunc(OBJECT_TYPE *PupilObject)
{
}

// Sound callback 
void pupil_soundfunc(OBJECT_TYPE *PupilObject)
{
}

// Draw callback 
void pupil_drawfunc(OBJECT_TYPE *PupilObject)
{
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	GLint win = ctrl->Window;
	Window_Control_Struct *wc = ctrl->WindowControl + win;
	GLint vp = ctrl->Viewport;
//	Veiwport_Struct *view = wc->Extent + vp;
	Frustrum_Struct *fr = wc->Frustrum + vp;
	float x;
	float y;
	float z;
	float sc;
	int l;
	int i;
	char s[ 100 ];
	GLboolean lighting_flag = GL_FALSE;

	// draw FPS
		
	if ( ctrl->showFramesPerSecond ) 
	{
		lighting_flag = glIsEnabled(GL_LIGHTING);
		if ( lighting_flag )
		{
			glDisable(GL_LIGHTING);
		}

		x = fr->Left + ( fr->Right - fr->Left ) * 0.1;
		y = fr->Top + ( fr->Bottom - fr->Top ) * 0.1;
		z = -( fr->Near + ( 0.1 ) * 0.1 );
		sc = fr->Near / ( 10.0 * 160.0 );
		sprintf( s, "FPS: %5.1f", ctrl->FramesPerSecond );
		l = strlen( s );

		glColor3f(1,1,1);
		glLineWidth(1.0);

		glPushMatrix();
		glTranslatef( x, y, z );
		glScalef( sc, sc, sc );
		for( i=0; i<l; i++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, s[ i ] );
		}		
		glPopMatrix();			

		if ( lighting_flag )
		{
			glEnable(GL_LIGHTING);
		}  
	}
}

// true shadow callback 
void pupil_trueShadowFunc(OBJECT_TYPE *PupilObject)
{
}

// cheap shadow callback 
void pupil_cheapShadowFunc(OBJECT_TYPE *PupilObject)
{
}

// predraw callback 
void pupil_animatefunc(OBJECT_TYPE *PupilObject)
{
}

// predraw callback 
void pupil_predrawfunc(OBJECT_TYPE *PupilObject)
{
	float PTemp[9];	
	Loop_Control_Struct *ctrl = Loop_Control;  // unfortunate global
	GLint win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);
	GLint vp = ctrl->Viewport;
	CORDORD *o = &(wc->Orientation[vp]);

	PTemp[0] = 0.0;
	PTemp[1] = 0.0;
	PTemp[2] = 0.0;
	PTemp[3] = o->pitch;
	PTemp[4] = o->yaw;
	PTemp[5] = o->roll;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	PupilObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	PupilObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	PupilObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	PupilObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	PupilObject->orginal_first_node->MpoParams->P->assign( PTemp );
}

// This is the well known initialization function. Everything comes from here 
void PupilInit( APPTYPE *App, char *parent, char *name )
{
	float PTemp[9];						// temporary transform storage 
	OBJECT_TYPE *Pupil;					// This object 

	assert(App);
			 
	Pupil = fl_add_child_to_parent_by_name( App->universe_ptr, parent, name );

	// set up callbacks 
	Pupil->soundfunc = pupil_soundfunc;
	Pupil->sounding_on_flag = false;
	Pupil->platformfunc = pupil_platformfunc;
	Pupil->pregraphicsfunc = pupil_animatefunc;
	Pupil->predrawfunc = pupil_predrawfunc;
	Pupil->drawfunc = pupil_drawfunc;
	Pupil->trueShadowFunc = pupil_trueShadowFunc;
	Pupil->cheapShadowFunc = pupil_cheapShadowFunc;
	Pupil->drawing_on_flag = true;
	Pupil->object_controls = NULL;

	Pupil->bounding_volume_set = fl_makebvset(3);

	// define a bounding volume (four corners and a center) 
	Pupil->bounding_volume_set->bvset[0]->npoints = 5;
	// center point 
	Pupil->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	Pupil->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	Pupil->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	Pupil->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	// -x, -y, -z corner 
	Pupil->bounding_volume_set->bvset[0]->p[1][0] = -0.02;    
	Pupil->bounding_volume_set->bvset[0]->p[1][1] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[1][2] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	// +x, -y, -z corner 
	Pupil->bounding_volume_set->bvset[0]->p[2][0] = 0.02;    
	Pupil->bounding_volume_set->bvset[0]->p[2][1] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[2][2] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	// -x, +y, -z corner 
	Pupil->bounding_volume_set->bvset[0]->p[3][0] = -0.02;    
	Pupil->bounding_volume_set->bvset[0]->p[3][1] = 0.02;
	Pupil->bounding_volume_set->bvset[0]->p[3][2] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	// -x, -y, +z corner 
	Pupil->bounding_volume_set->bvset[0]->p[4][0] = -0.02;    
	Pupil->bounding_volume_set->bvset[0]->p[4][1] = -0.02;
	Pupil->bounding_volume_set->bvset[0]->p[4][2] = 0.02;
	Pupil->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

	// Define bounding sphere for culling 
	Pupil->bounding_volume_set->bvset[1]->npoints = 2;
	// Center point 
	Pupil->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	Pupil->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	Pupil->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	Pupil->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	// Surface point 
	Pupil->bounding_volume_set->bvset[1]->p[1][0] = 0.02;
	Pupil->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	Pupil->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	Pupil->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
    
	// Define a bounding point 
	Pupil->bounding_volume_set->bvset[2]->npoints = 1;
	// Center point 
	Pupil->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
	Pupil->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
	Pupil->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	Pupil->bounding_volume_set->bvset[2]->p[0][3]= 1.0;
  
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
	Pupil->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	Pupil->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	Pupil->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	Pupil->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	Pupil->orginal_first_node->MpoParams->P->assign( PTemp );
}

