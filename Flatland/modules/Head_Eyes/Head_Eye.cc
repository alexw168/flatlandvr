/*
Head_Eye.cc  An app providing the head and Eyes

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
#include "Head_Eye.h"
#include "GL_Object.h"
#include "Head.h"
#include "EyeStalk.h"
#include "Eye.h"
#include "Pupil.h"


/* =======================================================================

Description:
    Head_Eye provides a single application that sets up the required 
    head and eye objects that Flatland expects to be present.  These
    objects allow Flatland to be configurable for single monitors,
    head mounted displays, or caves.
    
Objects created when application is loaded:
    Head - this object represents the head, and is mostly a common anchor
    point for the eyes.
    
    EyeStalk (left, right, center) - this object positions the eyes to
    produce the correct perspective for stereo (if it is selected)
    
    Eye (left, right, center) - this object reverses the rotations of the
    head when using a cave configuration.  In a cave, tilting the head 
    should not tilt the display on the cave walls.
    
    Pupil (left, right, center) - this object is responsible for facing
    in the direction of the display surface.  It is necesary to face the
    respective display surfaces in order to render the correct image 
    for each display surface.
    
    Total of 10 objects created.
    
Object dependencies:
    Eye depends on the HeadTransformationNode for information on which
    way the head is turned, so that it can be reversed.
    
Configurable options:

    Head_Eye:
    head_name               string  default "Head"
    left_eye_name           string  default "LeftEye"
    right_eye_name          string  default "RightEye"
    center_eye_name         string  default "CenterEye"
    left_eye_stalk_name     string  default "LeftEyeStalk"
    right_eye_stalk_name    string  default "RightEyeStalk"
    center_eye_stalk_name   string  default "CenterEyeStalk"
    left_pupil_name         string  default "LeftPupil"
    right_pupil_name        string  default "RightPupil"
    center_pupil_name       string  default "CenterPupil"

    Head:
    head_x                  float   offset of head from HeadTransformationNode
    head_y                  float   offset of head from HeadTransformationNode
    head_z                  float   offset of head from HeadTransformationNode
    head_pitch              float   offset of head from HeadTransformationNode
    head_yaw                float   offset of head from HeadTransformationNode
    head_roll               float   offset of head from HeadTransformationNode
    head_scale              float   scale of head object
    head_data_file          string  name of data file containing display data

    EyeStalk:
    (left, right, center)_eye_scale     float   offset of eye from center of head
    (left, right, center)_eye_x         float   offset of eye from center of head
    (left, right, center)_eye_y         float   offset of eye from center of head
    (left, right, center)_eye_z         float   offset of eye from center of head
    (left, right, center)_eye_roll      float   offset of eye from center of head
    (left, right, center)_eye_pitch     float   offset of eye from center of head
    (left, right, center)_eye_yaw       float   offset of eye from center of head
    (left, right, center)_eye_data_file string  name of data file containing display data

    Eye:
    head_tracker            string  default "HeadTransformationNode"
    
    
Threads created:
    read_data() - reads data file into memory, for each object

Library requirements:
    Flatland basic setup

Location requirements:
    usually attached to "HeadTransformationNode"

Controls:
    Keyboard:
        none
    Menu:
        see keyboard input
    Voice:
        none
    Beaming:
        none

  Status:
    15-Sep-2001 - In need of more documentation, otherwise done.

======================================================================= */


extern "C"
{

// This is the well known initialization function. Everything comes from here 
void Flatland_init (APPTYPE *App)
{
	int i;
	char HEAD_NAME[MAX_NAME_SIZE] = { "Head" };  
	char LEFT_EYE_NAME[MAX_NAME_SIZE] = { "LeftEye" };  
	char RIGHT_EYE_NAME[MAX_NAME_SIZE] = { "RightEye" };  
	char CENTER_EYE_NAME[MAX_NAME_SIZE] = { "CenterEye" };  
	char LEFT_EYE_STALK_NAME[MAX_NAME_SIZE] = { "LeftEyeStalk" };  
	char RIGHT_EYE_STALK_NAME[MAX_NAME_SIZE] = { "RightEyeStalk" };  
	char CENTER_EYE_STALK_NAME[MAX_NAME_SIZE] = { "CenterEyeStalk" };  
	char LEFT_PUPIL_NAME[MAX_NAME_SIZE] = { "LeftPupil" };  
	char RIGHT_PUPIL_NAME[MAX_NAME_SIZE] = { "RightPupil" };  
	char CENTER_PUPIL_NAME[MAX_NAME_SIZE] = { "CenterPupil" };  

	assert(App);
			 
	// parse the parameter list for the object names
	for (i=0; i < App->nparams; i++)
	{
		if (!strcmp (App->params[i][0], "head_name"))
		{
			sscanf(App->params[i][2], "%s", HEAD_NAME);
		}
		if (!strcmp (App->params[i][0], "left_eye_name"))
		{
			sscanf(App->params[i][2], "%s", LEFT_EYE_NAME);
		}
		if (!strcmp (App->params[i][0], "right_eye_name"))
		{
			sscanf(App->params[i][2], "%s", RIGHT_EYE_NAME);
		}
		if (!strcmp (App->params[i][0], "center_eye_name"))
		{
			sscanf(App->params[i][2], "%s", CENTER_EYE_NAME);
		}
		if (!strcmp (App->params[i][0], "left_eye_stalk_name"))
		{
			sscanf(App->params[i][2], "%s", LEFT_EYE_STALK_NAME);
		}
		if (!strcmp (App->params[i][0], "right_eye_stalk_name"))
		{
			sscanf(App->params[i][2], "%s", RIGHT_EYE_STALK_NAME);
		}
		if (!strcmp (App->params[i][0], "center_eye_stalk_name"))
		{
			sscanf(App->params[i][2], "%s", CENTER_EYE_STALK_NAME);
		}
		if (!strcmp (App->params[i][0], "left_pupil_name"))
		{
			sscanf(App->params[i][2], "%s", LEFT_PUPIL_NAME);
		}
		if (!strcmp (App->params[i][0], "right_pupil_name"))
		{
			sscanf(App->params[i][2], "%s", RIGHT_PUPIL_NAME);
		}
		if (!strcmp (App->params[i][0], "center_pupil_name"))
		{
			sscanf(App->params[i][2], "%s", CENTER_PUPIL_NAME);
		}
	}

	HeadInit( App, App->parent_object_name, HEAD_NAME );
	EyeStalkInit( App, HEAD_NAME, LEFT_EYE_STALK_NAME, "left" );
	EyeStalkInit( App, HEAD_NAME, RIGHT_EYE_STALK_NAME, "right" );
	EyeStalkInit( App, HEAD_NAME, CENTER_EYE_STALK_NAME, "center" );
	EyeInit( App, LEFT_EYE_STALK_NAME, LEFT_EYE_NAME );
	EyeInit( App, RIGHT_EYE_STALK_NAME, RIGHT_EYE_NAME );
	EyeInit( App, CENTER_EYE_STALK_NAME, CENTER_EYE_NAME );
	PupilInit( App, LEFT_EYE_NAME, LEFT_PUPIL_NAME );
	PupilInit( App, RIGHT_EYE_NAME, RIGHT_PUPIL_NAME );
	PupilInit( App, CENTER_EYE_NAME, CENTER_PUPIL_NAME );
}

} // extern "C"
