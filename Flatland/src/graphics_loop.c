/*
graphics_loop.c.  Main glut loop and associated graphics functions

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
 * Created: 1-14-00
 * Destroyed: 10-5-00, TBE
 * Mostly rebuilt: 12/1/00, TBE
 * Added "wrappers" for collaboration: 3-15-02, Moad Mowafi
 * 
 */


/*********************************************************************

How the graphics loop is structured:

The graphics loop begins with GLUT calling fl_FlatlandIdle()...

	GLUT event loop
		fl_FlatlandIdle()
			LeftRightLoop()
				WindowLoop()
					SetWindowAndRedisplay()
			
Control returns to GLUT. next fl_FlatlandDisplay() is called and the 
graphics loop continues.

						GLUT event loop
							fl_FlatlandDisplay()
								InnerLeftRightLoop()
									ClearBuffers()
									ViewportLoop()
										SetViewport()
										BufferLoop()
											RestructureTreeToEye()
											DrawUniverse()
									glutSwapBuffers()					
							
Now the process repeats...

*********************************************************************/


#include "gheader.h"

// globals, YUCK!

// unfortunat global, Hopefully more organaized than lots of seperate globals!
Loop_Control_Struct *Loop_Control = NULL;

char window_definition_file[MAX_NAME_SIZE] = {""};	// window definition file 

char glut_library[MAX_NAME_SIZE]    = "libglut.so.3";
char joystick_device[MAX_NAME_SIZE] = "/dev/input/js0";

// for flatland logo start up...
extern void fl_logo_randomize(void);
extern void def_logo(void);

void Logo_Key( unsigned char key, int x, int y );
void Logo_Init( void );
void Logo_Idle( void );
void Logo_Display( void );


// an empty callback function for the CenterOfTheUniverse object
void EmptyCallback( OBJECT_TYPE *Object )
{
}


// the animation callback for the CenterOfTheUniverse object
void CenterOfUniverseAnimationCallback( OBJECT_TYPE *Object )
{
    Loop_Control_Struct *lc = Loop_Control;
    char buf[100];
    
    if ( lc->CurrentControlObject )
    {
        snprintf( buf, 100, "right00%s", lc->CurrentControlObject->name );
    }
    else
    {
        snprintf( buf, 100, "right00" );
    }
    fl_SendMessage( Object, "HUD", buf );
}


// the menu callback for the CenterOfTheUniverse object
void CenterOfUniverseMenuCallback( OBJECT_TYPE *Object, char key )
{
	// these "globals" do not actually belong to the center of the universe, 
	// however they do need somewhere to be attached to.  The center of the
	// universe provides a reasonable place to put global options, since it 
	// is not a deleteable object.
	
	switch (key)
	{
    case 'q':   // quit Flatland
                do_exit = TRUE;
                break;
    case 's':   // toggle boundiong spheres
		Loop_Control->showBoundingSpheres = !Loop_Control->showBoundingSpheres;
		break;
    case 'c':   // toggle boundiong cubes
		Loop_Control->showBoundingCubes = !Loop_Control->showBoundingCubes;
		break;
    case 'p':   // toggle boundiong planes (??? this option does not seem to do anything...)
		Loop_Control->showBoundingPlanes = !Loop_Control->showBoundingPlanes;
		break;
    case 't':   // toggle display of true shadows
		Loop_Control->showTrueShadows = !Loop_Control->showTrueShadows;
		break;
    case 'f':   // toggle display of frame rate
		Loop_Control->showFramesPerSecond = !Loop_Control->showFramesPerSecond;
		break;
    case 'r':   // toggle full screen display
		if ( Loop_Control->NumberOfWindows == 1 ) 
		{
//			if ( Loop_Control->WindowControl[0].NumberOfViewports == 1 )
//			{
				Loop_Control->showFullScreen = !Loop_Control->showFullScreen;
				if ( Loop_Control->showFullScreen )
				{
					glutFullScreen( );
				}
				else 
				{
					glutReshapeWindow( Loop_Control->WindowControl[0].LastPosition.Width, Loop_Control->WindowControl[0].LastPosition.Height );
				}
//			}
		}
		break;
	}
}


// this function creates and stes up the CenterOfTheUniverse object
void CreateTheCenterOfTheUniverse( void )
{
	// Create root object for universe 
	CenterOfTheUniverse = fl_makeobject(Universe,"CenterOfTheUniverse");
	CenterOfTheUniverse->drawfunc = EmptyCallback;
	CenterOfTheUniverse->pregraphicsfunc = CenterOfUniverseAnimationCallback;
	CenterOfTheUniverse->drawing_on_flag = false;
	CenterOfTheUniverse->bounding_volume_set = fl_makebvset(2);
	CenterOfTheUniverse->soundfunc = EmptyCallback;
	CenterOfTheUniverse->sounding_on_flag = false;
	CenterOfTheUniverse->object_controls = NULL;
	Universe->root_object = CenterOfTheUniverse;

        // add object to the hash table
        InsertElement(CenterOfTheUniverse, Universe->HashTable);

	//// Menu Setup ////

	// set up the control structure (four subcontrols under one) 
	// allows control of global options from the keyboard, menues, and voice 
	CenterOfTheUniverse->object_controls = fl_makeControlsSet(7);

	// Define bounding sphere controls
	strcpy( CenterOfTheUniverse->object_controls->controls[0]->menuname, "Toggle bounding spheres" );
	strcpy( CenterOfTheUniverse->object_controls->controls[0]->voicename, "Toggle bounding spheres" );
	CenterOfTheUniverse->object_controls->controls[0]->keystroke = 's';
	CenterOfTheUniverse->object_controls->controls[0]->control_function = CenterOfUniverseMenuCallback;

	// Define bounding cube controls
	strcpy( CenterOfTheUniverse->object_controls->controls[1]->menuname, "Toggle bounding cubes" );
	strcpy( CenterOfTheUniverse->object_controls->controls[1]->voicename, "Toggle bounding cubes" );
	CenterOfTheUniverse->object_controls->controls[1]->keystroke = 'c';
	CenterOfTheUniverse->object_controls->controls[1]->control_function = CenterOfUniverseMenuCallback;

	// Define bounding plane controls
	strcpy( CenterOfTheUniverse->object_controls->controls[2]->menuname, "Toggle bounding planes" );
	strcpy( CenterOfTheUniverse->object_controls->controls[2]->voicename, "Toggle bounding planes" );
	CenterOfTheUniverse->object_controls->controls[2]->keystroke = 'p';
	CenterOfTheUniverse->object_controls->controls[2]->control_function = CenterOfUniverseMenuCallback;

	// Define true shadow controls
	strcpy( CenterOfTheUniverse->object_controls->controls[3]->menuname, "Toggle true shadows" );
	strcpy( CenterOfTheUniverse->object_controls->controls[3]->voicename, "Toggle true shadows" );
	CenterOfTheUniverse->object_controls->controls[3]->keystroke = 't';
	CenterOfTheUniverse->object_controls->controls[3]->control_function = CenterOfUniverseMenuCallback;

	// Define frame rate controls
	strcpy( CenterOfTheUniverse->object_controls->controls[4]->menuname, "Toggle frame rate" );
	strcpy( CenterOfTheUniverse->object_controls->controls[4]->voicename, "Toggle frame rate" );
	CenterOfTheUniverse->object_controls->controls[4]->keystroke = 'f';
	CenterOfTheUniverse->object_controls->controls[4]->control_function = CenterOfUniverseMenuCallback;

	// Define full screen controls
	strcpy( CenterOfTheUniverse->object_controls->controls[5]->menuname, "Toggle full screen" );
	strcpy( CenterOfTheUniverse->object_controls->controls[5]->voicename, "Toggle full screen" );
	CenterOfTheUniverse->object_controls->controls[5]->keystroke = 'r';
	CenterOfTheUniverse->object_controls->controls[5]->control_function = CenterOfUniverseMenuCallback;

	// Define quit control
	strcpy( CenterOfTheUniverse->object_controls->controls[6]->menuname, "Quit" );
	strcpy( CenterOfTheUniverse->object_controls->controls[6]->voicename, "Quit" );
	CenterOfTheUniverse->object_controls->controls[6]->keystroke = 'q';
	CenterOfTheUniverse->object_controls->controls[6]->control_function = CenterOfUniverseMenuCallback;
}

#ifdef Linux
static void InitJoystick(const char *joystick_device)
{
        //typedef (*glutInitJoystickPtr)(const char *);
	void* dso;
	void  (*glutInitJoystick)(const char *);
	char* error;

	if ((dso = dlopen(glut_library, RTLD_NOW)) == NULL)
	{
		fprintf(stderr, "%s\n", dlerror());
	}
	else
	{
		glutInitJoystick = (void (*)(const char*)) dlsym(dso, "glutInitJoystick");

		if ((error = dlerror()) != NULL)
		{
			fprintf(stderr, "%s\n", error);
			fprintf(stderr, "Failed to initialize joystick device %s. Continuing without it.\n", joystick_device);
		}
		else
		{
	 		(*glutInitJoystick)(joystick_device);
			dlclose(dso);
			fprintf(stderr, "Initialized joystick device %s.\n", joystick_device);
		}

	}
}
#endif //Linux

// Graphics initialization and main glut loop 
void *fl_GLUT_main(void *X)
{
	int i;
	char window_title[MAX_NAME_SIZE] = "";

	// window initializations
	args_struct *gl_args = (args_struct *) X;
	glutInit( gl_args->argc, gl_args->argv );
  	  	
	// Find out what kind of polygon offset is supported 
#ifdef GL_VERSION_1_1
	if (TRUE)
	{
		Loop_Control->polygonOffsetVersion = ONE_DOT_ONE;
	}
	else
#endif
	{
#ifdef GL_EXT_polygon_offset
		if (glutExtensionSupported("GL_EXT_polygon_offset"))
		{
			Loop_Control->polygonOffsetVersion = EXTENSION;
		}
		else
#endif
		{
			Loop_Control->polygonOffsetVersion = MISSING;
			printf("\nMissing polygon offset.\n");
			printf("  Expect shadow depth aliasing artifacts.\n\n");
		}
	}

	// setup windows as defined in Loop_Control
	for (i = 0; i < Loop_Control->NumberOfWindows; i++)
	{
		if ( Loop_Control->UseStereo && !Loop_Control->UsePassiveStereoViaWindow && !Loop_Control->UsePassiveStereoViaViewport )
		{
			glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_STEREO);
		}
		else
		{
			glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
		}

		glutInitWindowPosition( Loop_Control->WindowControl[ i ].Position.X, Loop_Control->WindowControl[ i ].Position.Y );
		glutInitWindowSize( Loop_Control->WindowControl[ i ].Position.Width, Loop_Control->WindowControl[ i ].Position.Height );
 
		sprintf( window_title, "FLATLAND, University of New Mexico, Main Viewer, window %d", i );
		Loop_Control->WindowControl[ i ].glutWindow = glutCreateWindow( window_title );
		glutReshapeWindow( Loop_Control->WindowControl[ i ].Position.Width, Loop_Control->WindowControl[ i ].Position.Height );
		
		// set up logo display for each window
		glutDisplayFunc( Logo_Display );
		glutReshapeFunc( fl_FlatlandReshape );
		glutKeyboardFunc( Logo_Key );

		glEnable(GL_DEPTH_TEST );

		Loop_Control->WindowControl[ i ].MainMenu = 0;
		Loop_Control->WindowControl[ i ].selectCurrentObjMenu = 0;
		Loop_Control->WindowControl[ i ].modCurrentObjMenu = 0;
		Loop_Control->WindowControl[ i ].menuLoadAppMenu = 0;
		Loop_Control->WindowControl[ i ].menuUnloadAppMenu = 0;

		Loop_Control->WindowControl[ i ].NumberOfSubMenus = 0;
	}

	Loop_Control->Build_MainMenu = true;
	Loop_Control->Build_selectCurrentObjMenu = true;
	Loop_Control->Build_modCurrentObjMenu = true;
	Loop_Control->Build_menuLoadAppMenu = true;
	Loop_Control->Build_menuUnloadAppMenu = true;

#ifdef Linux
	// Initialize the joystick device, if there is one.
	InitJoystick( joystick_device );
#endif //Linux

  	// Register idle func 
	glutIdleFunc( Logo_Idle );

	// windows are ready...

	CreateTheCenterOfTheUniverse();

	// set the currently selected object to the CenterOfTheUniverse 
	currentControlObject = CenterOfTheUniverse;

	// Load all non-optional applications now 
	fl_load_all_base_apps( loadableApps );

	// find all the objects needed for a "properly" set up head tracking system
	Loop_Control->LeftEyeObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->LeftEyeName );
	Loop_Control->RightEyeObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->RightEyeName );
	Loop_Control->CenterEyeObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->CenterEyeName );
	Loop_Control->HeadTrackerObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->HeadTrackerName );
	Loop_Control->TrackerClientObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->TrackerClientName );
	Loop_Control->HeadObject = fl_find_object_by_name( Loop_Control->Universe, Loop_Control->HeadName );

	fprintf( stdout, "Base applications loaded...\n" );
	glutMainLoop();
	
	return ( NULL );             
}


// initialize OpenGL to display the flatland logo
void Logo_Init( void )
{
	int i;
	GLfloat lightpos[4] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat lightamb[4] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };
	
	Loop_Control->SkipFog = false;
	Loop_Control->FogDensity = 1.0;
	Loop_Control->FogColor[0] = 0.0;
	Loop_Control->FogColor[1] = 0.0;
	Loop_Control->FogColor[2] = 0.0;
	Loop_Control->FogColor[3] = 0.0;
	Loop_Control->FogMode = GL_EXP;
	Loop_Control->FogScale = 0.8;
	Loop_Control->MinimumFogDensity = 0.00001;

	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
//		fl_glutMenu( Loop_Control->Universe );  // TODO: Don't assume the global universe 
		glShadeModel( GL_SMOOTH );
		glEnable( GL_DEPTH_TEST );
		glLightfv( GL_LIGHT0, GL_POSITION, lightpos );
		glLightfv( GL_LIGHT0, GL_AMBIENT, lightamb );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, lightdif );
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		glColor3f( 1.0, 1.0, 1.0 );
		glClearColor( 0.0, 0.0, 0.0, 1.0 );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glEnable( GL_NORMALIZE );
	}
	def_logo( );
	fl_logo_randomize( );
}


// glut Idle callback for flatland logo display
void Logo_Idle( void )
{
	static bool first = true;
	int i;

	// nasty way to get something done on the forst pass...		
	if ( first ) 
	{
		first = false;
		Logo_Init( );
		if ( full_screen_flag )
		{
			glutFullScreen( );
		}
	}

	// is the logo still going?	
	if ( !logo_done )
	{
		fl_Animate( );
		for( i=0; i<Loop_Control->NumberOfWindows; i++ )
		{
			glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
			glutPostRedisplay( );
		}
		if ( logo_done )
		{
			for( i=0; i<Loop_Control->NumberOfWindows; i++ )
			{
				glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
				glFogf( GL_FOG_DENSITY, 1.0 - Loop_Control->FogDensity );
				glFogfv( GL_FOG_COLOR, Loop_Control->FogColor );
				glFogi( GL_FOG_MODE, Loop_Control->FogMode );
				glEnable( GL_FOG );
			}
		}
	}
	else // logo is done, transition to Flatland
	{
		// is display fade still in progress
		if ( Loop_Control->FogDensity > 0 )
		{
			Loop_Control->FogDensity *= Loop_Control->FogScale;
			if ( Loop_Control->FogDensity < Loop_Control->MinimumFogDensity )
			{
				Loop_Control->FogDensity = 0.0;
			}
			for( i=0; i<Loop_Control->NumberOfWindows; i++ )
			{
				glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
				glFogf( GL_FOG_DENSITY, 1 - Loop_Control->FogDensity );
			}
		}
		else // fade done, switch to Flatland
		{
			// setup windows as defined in Loop_Control
			for( i = 0; i < Loop_Control->NumberOfWindows; i++ )
			{
				// Register Flatlands's GLUT callbacks for each window. 
				Loop_Control->Window = i;
				glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
				glutDisplayFunc( fl_FlatlandDisplay );
				glutReshapeFunc( fl_FlatlandReshape );
				glutMouseFunc( fl_FlatlandMouse );
#ifdef Linux
				glutJoystickFunc( fl_FlatlandJoystick, 50 );
#endif //Linux
				glutMotionFunc( fl_FlatlandMotion );
				glutPassiveMotionFunc( fl_FlatlandPassiveMotion );
				glutEntryFunc( fl_FlatlandEntry );
				glutKeyboardFunc( fl_FlatlandKey );
				glutKeyboardUpFunc( fl_FlatlandKeyUp );
				glutSpecialFunc( fl_FlatlandSpecial );
                                glutSpecialUpFunc( fl_FlatlandSpecialUp );
                                glutSpaceballMotionFunc( fl_FlatlandSpaceballMotion );
                                glutSpaceballRotateFunc( fl_FlatlandSpaceballMotion );
                                glutSpaceballButtonFunc( fl_FlatlandSpaceballButton );
                                glutTabletMotionFunc( fl_FlatlandTabletMotion );
                                glutTabletButtonFunc( fl_FlatlandTabletButton );
				if ( full_screen_flag )
				{
					glutFullScreen( );
				}
//				fl_glutMenu( Loop_Control->Universe );  // TODO: Don't assume the global universe 
    	    	    	    	glutMenuStatusFunc( fl_FlatlandMenuStatus );
			}

  			// Register idle func 
			glutIdleFunc( fl_FlatlandIdle );
		}
	}
}


// glut display callback for logo
void Logo_Display( void )
{
	int glutWindow = glutGetWindow();
	int i, win = -1;

	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			win = i;
		}
	}
	
	if ( win >= 0 )
	{
		if ( Loop_Control->UseStereo && !Loop_Control->UsePassiveStereoViaWindow && !Loop_Control->UsePassiveStereoViaViewport )
		{
			glDrawBuffer( GL_BACK_RIGHT );
			glClear( GL_COLOR_BUFFER_BIT );
			glDrawBuffer( GL_BACK_LEFT );
			glClear( GL_COLOR_BUFFER_BIT );
		}
		else 
		{
			glDrawBuffer( GL_BACK );
			glClear( GL_COLOR_BUFFER_BIT );
		}
		for( i=0; i<Loop_Control->WindowControl[ win ].NumberOfViewports; i++ )
		{	
			if ( Loop_Control->UseStereo && !Loop_Control->UsePassiveStereoViaWindow && !Loop_Control->UsePassiveStereoViaViewport )
			{
				glDrawBuffer( GL_BACK_RIGHT );
				glViewport( Loop_Control->WindowControl[ win ].Extent[i].X, Loop_Control->WindowControl[ win ].Extent[i].Y, Loop_Control->WindowControl[ win ].Extent[i].Width, Loop_Control->WindowControl[ win ].Extent[i].Height );
				glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
				fl_logo_display( );
				glDrawBuffer( GL_BACK_LEFT );
				glViewport( Loop_Control->WindowControl[ win ].Extent[i].X, Loop_Control->WindowControl[ win ].Extent[i].Y, Loop_Control->WindowControl[ win ].Extent[i].Width, Loop_Control->WindowControl[ win ].Extent[i].Height );
				glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
				fl_logo_display( );
			}
			else 
			{
				glDrawBuffer( GL_BACK );
				glViewport( Loop_Control->WindowControl[ win ].Extent[i].X, Loop_Control->WindowControl[ win ].Extent[i].Y, Loop_Control->WindowControl[ win ].Extent[i].Width, Loop_Control->WindowControl[ win ].Extent[i].Height );
				glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
				fl_logo_display( );
			}
		}
		glutSwapBuffers( );
	}
}


// glut keyboard callback during flatland logo display
void Logo_Key( unsigned char key, int x, int y )
{
	switch( key )
	{
	// if ecape is pressed, skip directly to flatland
	case 27:
		logo_done = true;
		Loop_Control->FogDensity = 0.0;
		Loop_Control->SkipFog = true;
	}		
	fl_FlatlandKey (key, x, y);
}


// an empty GLUT display function
void empty_display( )
{
}


// glut idle callback for flatland, beginning of the graphics loop
void fl_FlatlandIdle(void)
{ 
	int i;
	static bool first = true;
	int last_time, current_time;
		
        // if main() has exited, we should too
        // NOTE:  this was added because the NVIDIA drivers would not halt
        // when the main thread exited.  Shouldn't hurt anything on non-NVIDIA
        // systems.
        if (exit_now) exit(0);

	if (first)
	{
		Loop_Control->FramesPerSecond = 0.0;
		Loop_Control->AverageOverFrames = true;
		Loop_Control->NumberOfFrames = 10;
		Loop_Control->FrameCount = 0;
		glDisable(GL_LIGHT0);     // disable light left over from logo
		fl_FlatlandInit();                // then init flatland
		first = false;
	}

	if ( Loop_Control->AverageOverFrames )
	{
		if ( ++Loop_Control->FrameCount >= Loop_Control->NumberOfFrames )
		{
			last_time = Loop_Control->LastElapsedTime;
			current_time = Loop_Control->LastElapsedTime = glutGet( (GLenum) GLUT_ELAPSED_TIME );
			if ( current_time == last_time )
			{
				current_time++;
			}
//fprintf( stdout, "count: %d  time: %f\n", Loop_Control->FrameCount, 0.001 * ( current_time - last_time ) );
//fprintf( stdout, "last: %f  ", Loop_Control->FramesPerSecond );
			Loop_Control->FramesPerSecond = ( 0.5 * Loop_Control->FramesPerSecond + 0.5 * Loop_Control->FrameCount / ( 0.001 * ( current_time - last_time ) ) );
//fprintf( stdout, "now: %f\n", Loop_Control->FramesPerSecond );
			Loop_Control->NumberOfFrames = (int)floor( Loop_Control->FramesPerSecond );
			Loop_Control->FrameCount = 0;

			if ( Loop_Control->FramesPerSecond < 15.0 )
			{
				Loop_Control->AverageOverFrames = false;
			}
		}
	}
	else
	{
		last_time = Loop_Control->LastElapsedTime;
		current_time = Loop_Control->LastElapsedTime = glutGet( (GLenum) GLUT_ELAPSED_TIME );
		if ( current_time == last_time )
		{
			current_time++;
		}
		Loop_Control->FramesPerSecond = ( 0.95 * Loop_Control->FramesPerSecond + 0.05 * 1000.0 / ( current_time - last_time ) );

		if ( Loop_Control->FramesPerSecond > 20.0 )
		{
			Loop_Control->AverageOverFrames = true;
			Loop_Control->NumberOfFrames = (int)floor( Loop_Control->FramesPerSecond );
			Loop_Control->FrameCount = 0;
		}
	}
		
	if ( Loop_Control->FogDensity > 0.0 )
	{
		Loop_Control->FogDensity *= Loop_Control->FogScale;
		if ( Loop_Control->FogDensity < Loop_Control->MinimumFogDensity || Loop_Control->SkipFog )
		{
			Loop_Control->FogDensity = 0.0;
			for( i=0; i<Loop_Control->NumberOfWindows; i++ )
			{
				glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
				glDisable( GL_FOG );
			}
		}
		for( i=0; i<Loop_Control->NumberOfWindows; i++ )
		{
			glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
			glFogf( GL_FOG_DENSITY, Loop_Control->FogDensity );
		}
	}
	
    	// build changed menus
	fl_Build_Menus();
/*
	if ( Loop_Control->Build_selectCurrentObjMenu )
	{
	    Loop_Control->Build_MainMenu  = true;
	    fl_Build_selectCurrentObjMenu();
	    Loop_Control->Build_selectCurrentObjMenu = false;
	}
	
	if ( Loop_Control->Build_modCurrentObjMenu )
	{
	    Loop_Control->Build_MainMenu  = true;
	    fl_Build_modCurrentObjMenu();
	    Loop_Control->Build_modCurrentObjMenu = false;
	}
	
	if ( Loop_Control->Build_menuLoadAppMenu )
	{
	    Loop_Control->Build_MainMenu  = true;
	    fl_Build_menuLoadAppMenu();
	    Loop_Control->Build_menuLoadAppMenu = false;
	}
	
	if ( Loop_Control->Build_menuUnloadAppMenu )
	{
	    Loop_Control->Build_MainMenu  = true;
	    fl_Build_menuUnloadAppMenu();
	    Loop_Control->Build_menuUnloadAppMenu  = false;
	}
	
	if ( Loop_Control->Build_MainMenu )
	{
	    fl_Build_MainMenu();
	    Loop_Control->Build_MainMenu  = false;
	}
*/	
	

	// the graphics loop continues here!
	LeftRightLoop( Loop_Control );

    	if( do_exit )
	{
	    	// 8/19/04 TBE - added to reduce segfaults on exit
		// attempt tp sut down GLUT
	    	// clear all used GLUT callbacks
		for( i = 0; i < Loop_Control->NumberOfWindows; i++ )
		{
			// Register Flatlands's GLUT callbacks for each window. 
			Loop_Control->Window = i;
			glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

    			glutDisplayFunc( empty_display );
			glutIdleFunc( 0 );
			glutReshapeFunc( 0 );
			glutMouseFunc( 0 );
			glutMotionFunc( 0 );
			glutPassiveMotionFunc( 0 );
			glutKeyboardFunc( 0 );
			glutEntryFunc( 0 );
			glutKeyboardUpFunc( 0 );
			glutSpecialFunc( 0 );
        		glutSpecialUpFunc( 0 );
        		glutSpaceballMotionFunc( 0 );
        		glutSpaceballRotateFunc( 0 );
        		glutSpaceballButtonFunc( 0 );
        		glutTabletMotionFunc( 0 );
        		glutTabletButtonFunc( 0 );
			glutMenuStatusFunc( 0 );
#ifdef Linux
			glutJoystickFunc( 0, 100 );
#endif //Linux
		}
	}
}


// Initialize OpenGL for flatland
void fl_FlatlandInit(void)
{
	int i;
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_POINT_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	if ( Loop_Control->NormalizeVectors )
	{
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_NORMALIZE);
	}

	// make sure fog is set up 
	Loop_Control->FogDensity = 1.0;

	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );
		glFogf( GL_FOG_DENSITY, Loop_Control->FogDensity );
		glFogfv( GL_FOG_COLOR, Loop_Control->FogColor );
		glFogi( GL_FOG_MODE, Loop_Control->FogMode );
		glEnable( GL_FOG );
	}
	
	Loop_Control->LastElapsedTime = glutGet( (GLenum) GLUT_ELAPSED_TIME );
	Loop_Control->FramesPerSecond = 0;
}


// glut display callback for flatland, the graphics loop continues here
void fl_FlatlandDisplay( void )
{
	int glutWindow = glutGetWindow();
	int i;

	// find the current window...
	Loop_Control->Window = -1;
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
		}
	}
	
	if ( Loop_Control->Window >= 0 )
	{
		// continue graphics loop
		InnerLeftRightLoop( Loop_Control );
	}
}


// glut display callback for flatland, the graphics loop continues here
void fl_FlatlandMenuStatus( int status, int x, int y )
{
    int glutWindow = glutGetWindow();
    int i;

    // find the current window...
    Loop_Control->Window = -1;
    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
	if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
	{
	    Loop_Control->Window = i;
	}
    }

    if ( Loop_Control->Window >= 0 )
    {
	// continue graphics loop
	fl_glutMenuStatus( status, x, y );
    }
}


bool fl_tbe_invert_homogenious_4x4_matrix( float *in, float *out )
{
	float det = in[0]*in[10]*in[5] - in[0]*in[6]*in[9] - in[1]*in[10]*in[4] + in[1]*in[6]*in[8] + in[2]*in[4]*in[9] - in[2]*in[5]*in[8];

//fl_print_gl_matrix( in );
//fprintf( stdout, "det: %f\n", det );
//fflush(stdout);

//	if ( det == 0 || in[12] != 0 || in[13] !=0 || in[14] !=0 || in[15] != 1 )
	if ( det == 0 )
	{
		return false;
	}

	out[0] = ( in[10]*in[5] - in[6]*in[9] ) / det;
	out[1] = ( -in[1]*in[10] + in[2]*in[9] ) / det;
	out[2] = ( in[1]*in[6] - in[2]*in[5] ) / det;
	out[3] = ( in[1]*in[10]*in[7] - in[1]*in[11]*in[6] - in[10]*in[3]*in[5] + in[11]*in[2]*in[5] - in[2]*in[7]*in[9] + in[3]*in[6]*in[9] ) / det;
	out[4] = ( -in[10]*in[4] + in[6]*in[8] ) / det;
	out[5] = ( in[0]*in[10] - in[2]*in[8] ) / det;
	out[6] = ( -in[0]*in[6] + in[2]*in[4] ) / det;
	out[7] = ( -in[0]*in[10]*in[7] + in[0]*in[11]*in[6] + in[10]*in[3]*in[4] - in[11]*in[2]*in[4] + in[2]*in[7]*in[8] - in[3]*in[6]*in[8] ) / det;
	out[8] = ( in[4]*in[9] - in[5]*in[8] ) / det;
	out[9] = ( -in[0]*in[9] + in[1]*in[8] ) / det;
	out[10] = ( in[0]*in[5] - in[1]*in[4] ) / det;
	out[11] = ( -in[0]*in[11]*in[5] + in[0]*in[7]*in[9] + in[1]*in[11]*in[4] - in[1]*in[7]*in[8] - in[3]*in[4]*in[9] + in[3]*in[5]*in[8] ) / det;
	out[12] = 0;
	out[13] = 0;
	out[14] = 0;
	out[15] = 1;

//fl_print_gl_matrix( out );

	return true;
}

// glut reshape callback for flatland
void fl_FlatlandReshape(int w, int h)
{
	int glutWindow = glutGetWindow();
	Loop_Control_Struct *lc = Loop_Control;
	int win;
	Window_Control_Struct *wc;
	int vp;
	Viewport_Struct *e;
	Viewport_Struct *es;
	int i;
	Wall_Struct *wa;
	OBJECT_TYPE *o = NULL;
	float p0[4], p1[4], p2[4], p3[4];
	float width, height, mid_x; // mid_y;
	static bool first_call = true;
	float McoInv[16];

//fprintf( stdout, "W: %d, h: %d\n", w, h );
		
	if ( first_call )
	{
		first_call = false;
//		return;
	}
			
	// find the current window...
	Loop_Control->Window = -1;
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
		}
	}
	
	win = lc->Window;
	wc = lc->WindowControl + win;
	
	for( vp = 0; vp < wc->NumberOfViewports; vp++ )
	{
		e = wc->Extent + vp;
		es = wc->StartExtent + vp;
		
		e->X = w * es->X / wc->StartPosition.Width;
		e->Width = w * es->Width / wc->StartPosition.Width;
		e->Y = h * es->Y / wc->StartPosition.Height;
		e->Height = h * es->Height / wc->StartPosition.Height;
	}
	
	// if ther is only one window with one viewport
	if ( Loop_Control->NumberOfWindows == 1 && Loop_Control->Window == 0 && Loop_Control->WindowControl[ Loop_Control->Window ].NumberOfViewports == 1 )
	{
		// in this case the Mco's in the graph are already configued for the old screen
		vp = lc->Viewport = 0;
		wa = wc->Wall + vp;
		o = NULL;
		
		if ( lc->FixedViewingPlanes )
		{
			o = fl_find_object_by_name( lc->Universe, lc->TrackerClientName );
		}
		else
		{
			o = fl_find_object_by_name( lc->Universe, lc->HeadName );
		}

		if ( !o )
		{
			fprintf( stderr, "No base object for resize... now what?\n" );
//			exit( -1 );
		}
		else
		{
//fprintf( stdout, "llc: (%f,%f,%f)\n", wa->Corner[0][0], wa->Corner[0][1], wa->Corner[0][2] );
//fprintf( stdout, "lrc: (%f,%f,%f)\n", wa->Corner[1][0], wa->Corner[1][1], wa->Corner[1][2] );
//fprintf( stdout, "urc: (%f,%f,%f)\n", wa->Corner[2][0], wa->Corner[2][1], wa->Corner[2][2] );

			fl_tpc_matrix_multiply( o->Mco,wa->Corner[0], p0 );
			fl_tpc_matrix_multiply( o->Mco,wa->Corner[1], p1 );
			fl_tpc_matrix_multiply( o->Mco,wa->Corner[2], p2 );
			fl_tpc_matrix_multiply( o->Mco,wa->Corner[3], p3 );

//fprintf( stdout, "p0: (%f,%f,%f)\n", p0[0], p0[1], p0[2] );
//fprintf( stdout, "p1: (%f,%f,%f)\n", p1[0], p1[1], p1[2] );
//fprintf( stdout, "p2: (%f,%f,%f)\n", p2[0], p2[1], p2[2] );
			
			width = p1[0] - p0[0];
			height = p2[1] - p1[1];
			mid_x = ( p1[0] + p0[0] ) / 2.0;
//			mid_y = ( p2[1] + p1[1] ) / 2.0;

			width = height * (float)w / (float)h;
			
			p0[0] = p3[0] = mid_x - width / 2.0;
			p1[0] = p2[0] = mid_x + width / 2.0;

//fprintf( stdout, "p0': (%f,%f,%f)\n", p0[0], p0[1], p0[2] );
//fprintf( stdout, "p1': (%f,%f,%f)\n", p1[0], p1[1], p1[2] );
//fprintf( stdout, "p2': (%f,%f,%f)\n", p2[0], p2[1], p2[2] );
			
			if ( !fl_tbe_invert_homogenious_4x4_matrix( o->Mco, McoInv ) )
			{
				fprintf( stderr, "fl_tbe_invert_homogenious_4x4_matrix( o->Mco, McoInv ) FAILED!\n" );
				exit( -1 );
			}
			fl_tpc_matrix_multiply( McoInv, p0, wa->Corner[0] );
			fl_tpc_matrix_multiply( McoInv, p1, wa->Corner[1] );
			fl_tpc_matrix_multiply( McoInv, p2, wa->Corner[2] );
			fl_tpc_matrix_multiply( McoInv, p3, wa->Corner[3] );
			
//fprintf( stdout, "llc': (%f,%f,%f)\n", wa->Corner[0][0], wa->Corner[0][1], wa->Corner[0][2] );
//fprintf( stdout, "lrc': (%f,%f,%f)\n", wa->Corner[1][0], wa->Corner[1][1], wa->Corner[1][2] );
//fprintf( stdout, "urc': (%f,%f,%f)\n", wa->Corner[2][0], wa->Corner[2][1], wa->Corner[2][2] );
		}		
	}

	if ( Loop_Control->Window >= 0 )
	{
		Loop_Control->WindowControl[ Loop_Control->Window ].LastPosition.Width = Loop_Control->WindowControl[ Loop_Control->Window ].Position.Width;
		Loop_Control->WindowControl[ Loop_Control->Window ].LastPosition.Height = Loop_Control->WindowControl[ Loop_Control->Window ].Position.Height;
		Loop_Control->WindowControl[ Loop_Control->Window ].Position.Width = w;
		Loop_Control->WindowControl[ Loop_Control->Window ].Position.Height = h;
	}
}


// glut mouse callback for flatland
void fl_FlatlandMouse(int button, int state, int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <=y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
			fl_glutMouse_all_objects( Loop_Control->Universe, button, state, x, y );
		}
	}
}

#ifdef Linux
void fl_FlatlandJoystick(unsigned int buttonMask, int x, int y, int z)
{
	fl_glutJoystick_all_objects( Loop_Control->Universe, buttonMask, x, y, z );
}
#endif //Linux

// glut mouse callback for flatland
void fl_FlatlandMotion(int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <=y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
			fl_glutMotion_all_objects( Loop_Control->Universe, x, y );
		}
	}
}


// glut mouse callback for flatland
void fl_FlatlandPassiveMotion( int x, int y )
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
//fprintf( stdout, "Window: %d, Viewport: %d, pos: (%d,%d)\n", Loop_Control->Window, Loop_Control->Viewport, x, y );
			fl_glutMotion_all_objects( Loop_Control->Universe, x, y );
		}
	}
}


// glut mouse callback for flatland
void fl_FlatlandEntry( int state )
{
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	fl_glutEntry_all_objects( Loop_Control->Universe, state );
}


// glut keyboard special callback for flatland
void fl_FlatlandSpecial( int key, int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
//fprintf( stdout, "Window: %d, Viewport: %d, pos: (%d,%d)\n", Loop_Control->Window, Loop_Control->Viewport, x, y );
			fl_glutSpecial_all_objects( Loop_Control->Universe, key, x, y );
		}
	}
}

// glut keyboard special up callback for flatland
void fl_FlatlandSpecialUp( int key, int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
//fprintf( stdout, "Window: %d, Viewport: %d, pos: (%d,%d)\n", Loop_Control->Window, Loop_Control->Viewport, x, y );
			fl_glutSpecialUp_all_objects( Loop_Control->Universe, key, x, y );
		}
	}
}

// glut spaceball motion callback for flatland
void fl_FlatlandSpaceballMotion( int x, int y, int z )
{
	fl_glutSpaceballMotion_all_objects( Loop_Control->Universe, x, y, z );
}

// glut spaceball rotate callback for flatland
void fl_FlatlandSpaceballRotate( int x, int y, int z )
{
	fl_glutSpaceballRotate_all_objects( Loop_Control->Universe, x, y, z );
}

// glut spaceball button callback for flatland
void fl_FlatlandSpaceballButton( int button, int state )
{
	fl_glutSpaceballButton_all_objects( Loop_Control->Universe, button, state );
}

// glut tablet motion callback for flatland
void fl_FlatlandTabletMotion( int x, int y )
{
	fl_glutTabletMotion_all_objects( Loop_Control->Universe, x, y );
}

// glut tablet button callback for flatland
void fl_FlatlandTabletButton( int button, int state, int x, int y )
{
	fl_glutTabletButton_all_objects( Loop_Control->Universe, button, state, x, y );
}


// glut keyboard callback for flatland
void fl_FlatlandKey(unsigned char key, int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
//fprintf( stdout, "Window: %d, Viewport: %d, pos: (%d,%d)\n", Loop_Control->Window, Loop_Control->Viewport, x, y );
			fl_glutKeyboard_all_objects( Loop_Control->Universe, key, x, y );
		}
	}

	// scan current object for keystroke. call handler if found 
	if (currentControlObject != NULL && currentControlObject->object_controls != NULL)
	{
	        /* check to see if the current object is shared */
	        if (currentControlObject->shared_flag == true)
		{
		        fl_collab_send_ctrl_msg(currentControlObject, key);
		}
		else    /* the current object is not shared */
		{
		        fl_keyscan (currentControlObject->object_controls, key);
		}
	}
}

// glut keyboard up callback for flatland
void fl_FlatlandKeyUp(unsigned char key, int x, int y)
{
	int win;
	Window_Control_Struct *wcs;
	int i;
	Viewport_Struct *e;
	int glutWindow = glutGetWindow();
	int y2;
	
	Loop_Control->Window = -1;
	Loop_Control->Viewport = -1;

	// find the window recieving message
	for( i=0; i<Loop_Control->NumberOfWindows; i++ )
	{
		if ( glutWindow == Loop_Control->WindowControl[ i ].glutWindow )
		{
			Loop_Control->Window = i;
			break;
		}
	}
	
	// find viewport recieving message
	if ( Loop_Control->Window >= 0 )
	{
		win = Loop_Control->Window;
		wcs = Loop_Control->WindowControl + win;
		y2 = wcs->Position.Height - y;
		for(i=0; i<wcs->NumberOfViewports; i++)
		{
			e = wcs->Extent + i;
//			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y && e->Y+e->Height >= y )
			if ( e->X <= x && e->X+e->Width >= x && e->Y <= y2 && e->Y+e->Height >= y2 )
			{
				Loop_Control->Viewport = i;
				break;
			}
		}

		// respond to message
		if ( Loop_Control->Viewport >= 0 )
		{
//fprintf( stdout, "Window: %d, Viewport: %d, pos: (%d,%d)\n", Loop_Control->Window, Loop_Control->Viewport, x, y );
			fl_glutKeyboardUp_all_objects( Loop_Control->Universe, key, x, y );
		}
	}
}


// check current object's controls for match to key pressed
bool fl_keyscan (OBJ_CONTROLS_ARRAY_TYPE *object_controls, char key)
{
	int i;

	for(i = 0; i < object_controls->ncontrols; i++)
	{
		if (object_controls->controls[i]->children != NULL)
		{
			// we have a child... follow recursively 
			if (fl_keyscan(object_controls->controls[i]->children, key))
			{
				return TRUE;    // the key has been found... no need to keep looking 
			}
		}
		else 
		{
			if (object_controls->controls[i]->keystroke == key)
			{
				// we have found the key 
				object_controls->controls[i]->control_function (currentControlObject,key);
				return TRUE;
			}
		}
	}
	return FALSE;         // didn't find the key 
}


////////////////////////////////////////////////////////////////////////


// calculate the current frustum values
void FigureFrustrum( Loop_Control_Struct *ctrl )
{
	GLint win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);
	GLint vp = ctrl->Viewport;
	Frustrum_Struct *f = &(wc->Frustrum[vp]);
	Wall_Struct *w = &(wc->Wall[vp]);
	OBJECT_TYPE *o = NULL;
	float p0[4], p1[4], p2[4];
		
	if ( ctrl->FixedViewingPlanes )
	{
		o = fl_find_object_by_name( ctrl->Universe, ctrl->TrackerClientName );
	}
	else
	{
		o = fl_find_object_by_name( ctrl->Universe, ctrl->HeadName );
	}
	
	if ( !o )
	{
		// head missing! use some default values
		f->Left = -0.124348;
		f->Right = 0.124348;
		f->Top = 0.093261;
		f->Bottom = -0.093261;
	}
	else
	{
		fl_tpc_matrix_multiply( o->Mco,w->Corner[0], p0 );
		fl_tpc_matrix_multiply( o->Mco,w->Corner[1], p1 );
		fl_tpc_matrix_multiply( o->Mco,w->Corner[2], p2 );

		f->Left = f->Near * p0[0] / fabs( p0[2] );
		f->Right = f->Near * p1[0] / fabs( p1[2] );
		f->Bottom = f->Near * p1[1] / fabs( p1[2] );
		f->Top = f->Near * p2[1] / fabs( p2[2] );  

/***
		fprintf( stdout, "\n" );
		fprintf( stdout, "window: %d\n", win );
		fprintf( stdout, "viewport: %d\n", vp );
		fprintf( stdout, "c0: (%f,%f,%f)\n", w->Corner[0][0], w->Corner[0][1], w->Corner[0][2] );
		fprintf( stdout, "c1: (%f,%f,%f)\n", w->Corner[1][0], w->Corner[1][1], w->Corner[1][2] );
		fprintf( stdout, "c2: (%f,%f,%f)\n", w->Corner[2][0], w->Corner[2][1], w->Corner[2][2] );
		fprintf( stdout, "p0: (%f,%f,%f)\n", p0[0], p0[1], p0[2] );
		fprintf( stdout, "p1: (%f,%f,%f)\n", p1[0], p1[1], p1[2] );
		fprintf( stdout, "p2: (%f,%f,%f)\n", p2[0], p2[1], p2[2] );
		fprintf( stdout, "left:   %f\n", f->Left );
		fprintf( stdout, "right:  %f\n", f->Right );
		fprintf( stdout, "bottom: %f\n", f->Bottom );
		fprintf( stdout, "top:    %f\n", f->Top );
***/
	}
}


// restructure tree so that correct eye is the root
bool RestructureTreeToEye( Loop_Control_Struct *ctrl )
{
	OBJECT_TYPE *o = NULL;
	
	if ( ctrl->Eye == LEFT_EYE )
	{
//		o = fl_find_object_by_name( ctrl->Universe, ctrl->LeftEyeName );
		o = ctrl->LeftEyeObject;
	}
	if ( ctrl->Eye == RIGHT_EYE )
	{
//		o = fl_find_object_by_name( ctrl->Universe, ctrl->RightEyeName );
		o = ctrl->RightEyeObject;
	}
	if ( ctrl->Eye == CENTER_EYE )
	{
//		o = fl_find_object_by_name( ctrl->Universe, ctrl->CenterEyeName );
		o = ctrl->CenterEyeObject;
	}
//	if ( !o ) // use first object in database
//	{
//		if ( ctrl->Universe->nobjects )
//		{
//			o = ctrl->Universe->database[0].object;
//		}
//	}
	if ( o )
	{
		fl_restructure_tree( ctrl->Universe, o );
	}
//	else
//	{
//		// now what, there is no "eye" to display freom ???
//	}
	return o != NULL;
}


// window, viewport, and buffer are set, draw the environment
void DrawUniverse( Loop_Control_Struct *ctrl )
{
	int win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);
	int vp = ctrl->Viewport;
	Frustrum_Struct *f = &(wc->Frustrum[vp]);

	// initialize the matricies
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();    

	fl_save_current_GL_modelview_matrix( ctrl->Universe->root_object->Mco);

	// initalize beam intersection flags 
//	removed 4-Nov-02, to move beam intersection to a service.  KLS
//	init_beam_intersect( ctrl->Universe );

	//==============================================================
	// Update the relationships between the objects then display them
	//==============================================================

	// Loop over calling predraw on all universe objects 
	fl_pre_draw_all_objects( ctrl->Universe );

	// Update all Mpo xforms 
	fl_update_Mpo_all_objects( ctrl->Universe );
	
	// Traverse tree to update xform inheritance 
	fl_xform_update( ctrl->Universe );

	FigureFrustrum( ctrl );

	// set up projection matrix
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity( );	
	glFrustum( f->Left, f->Right, f->Bottom, f->Top, f->Near, f->Far );
	glMatrixMode( GL_MODELVIEW );
	
	// Loop over lighting universe objects 
	fl_light_all_objects( ctrl->Universe );

	// Loop over drawing universe objects 
	fl_draw_all_objects( ctrl->Universe );

	// if required, loop over drawing all bounding spheres 
	if ( Loop_Control->showBoundingSpheres )
	{
		fl_draw_all_bounding_spheres( ctrl->Universe );
	}
	
	// if required, loop over drawing all bounding boxes
	if ( Loop_Control->showBoundingCubes )
	{
		fl_draw_all_bounding_cubes( ctrl->Universe );
	}
	          
	// if required, loop over drawing all bounding planes
	if( Loop_Control->showBoundingPlanes )
	{
		fl_draw_all_bounding_planes( ctrl->Universe );
	}

	// Loop over drawing shadows universe objects 
	fl_draw_all_shadows( ctrl->Universe );

	// Loop over objects drawing transparent portions 
	transparent_fl_draw_all_objects( ctrl->Universe );

	// Flush into pipe 
	glFlush( );
}


// select the correct buffer to draw to
void BufferLoop( Loop_Control_Struct *ctrl )
{
	if ( ctrl->UseStereo )
	{
		if ( !ctrl->UsePassiveStereoViaViewport && !ctrl->UsePassiveStereoViaWindow )
		{
			if ( ctrl->Eye == LEFT_EYE )
			{
				glDrawBuffer( GL_BACK_LEFT );
			}
			if ( ctrl->Eye == RIGHT_EYE )
			{
				glDrawBuffer( GL_BACK_RIGHT );
			}
			// center eye should never be active when using stereo
		}
		else
		{
			// stereo is not being generated using OpenGL stereo
			glDrawBuffer( GL_BACK );
		}
	}
	else
	{
		// not using stereo
		glDrawBuffer( GL_BACK );
	}

	glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// find eye to use
	if ( RestructureTreeToEye( ctrl ) )
	{
		DrawUniverse( ctrl );
	}
}


// set up OpenGL to use the right part of the window
void SetViewport( Loop_Control_Struct *ctrl )
{
	GLint win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);
	int vp = ctrl->Viewport;
	Viewport_Struct *e = &(wc->Extent[vp]);

//fprintf( stdout, "\nWindow:%d  Viewport: %d\n", win, vp );
	glViewport( e->X , e->Y , e->Width , e->Height );
}


// draw to eack viewport in the current window
void ViewportLoop( Loop_Control_Struct *ctrl )
{
	int Viewport;
	GLint win = ctrl->Window;
	Window_Control_Struct *wc = &(ctrl->WindowControl[win]);

	if ( ctrl->UsePassiveStereoViaViewport )	
	{
		if ( ctrl->Eye == LEFT_EYE )
		{
			ctrl->Viewport = wc->LeftViewport;
		}
		if ( ctrl->Eye == RIGHT_EYE )
		{
			ctrl->Viewport = wc->RightViewport;
		}
		// center eye should never be active when using stereo
		SetViewport( ctrl );
		BufferLoop( ctrl );
	}
	else
	{
		for( Viewport = 0; Viewport < wc->NumberOfViewports; Viewport++ )
		{
			ctrl->Viewport = Viewport;
			SetViewport( ctrl );
			BufferLoop( ctrl );
		}
	}
}


// clear the color buffers
void ClearBuffers( Loop_Control_Struct *ctrl )
{
	if ( ctrl->UseStereo && !ctrl->UsePassiveStereoViaWindow && !ctrl->UsePassiveStereoViaViewport )
	{
		glDrawBuffer( GL_BACK_RIGHT );
		glClear( GL_COLOR_BUFFER_BIT );
		glDrawBuffer( GL_BACK_LEFT );
		glClear( GL_COLOR_BUFFER_BIT );
	}
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT );
}


// make sure that stereo is handled if it is being used
void InnerLeftRightLoop( Loop_Control_Struct *ctrl )
{
	ClearBuffers( ctrl );

	if ( ctrl->UseStereo && !ctrl->UsePassiveStereoViaWindow )
	{
		// here stereo is in use, either active, or passive via viewport,
		// draw for each eye
		ctrl->Eye = LEFT_EYE;
		ViewportLoop( ctrl );
		ctrl->Eye = RIGHT_EYE;
		ViewportLoop( ctrl );
	}
	else
	{
		// either stereo is not in use, or passive stereo via windows
		// is being used.  if passive stereo is being used the eye depends
		// on the window being updated, otherwise the center eye is used
		if ( !ctrl->UseStereo )
		{
			ctrl->Eye = CENTER_EYE;
		}
		else
		{
			// match eye to window if using passive stereo via windows
			if ( ctrl->Window == ctrl->LeftWindow )
			{
				ctrl->Eye = LEFT_EYE;
			}
			if ( ctrl->Window == ctrl->RightWindow )
			{
				ctrl->Eye = RIGHT_EYE;
			}
		}
		ViewportLoop( ctrl );
	}

    fl_preswapbuffer_all_objects( ctrl->Universe );
	
	// swap buffers on a per-window basis... after all viewports (and/or stereo) are drawn
	glutSwapBuffers();
}


////////////////////////////////////////////////////////////////////////


// select the correct window and post redisplay
void SetWindowAndRedisplay( Loop_Control_Struct *ctrl )
{
	glutSetWindow( ctrl->WindowControl[ ctrl->Window ].glutWindow );
//fprintf( stdout, "\nPostRedisplay( %d )\n", ctrl->Window );
	glutPostRedisplay( );
}


// redisplay all windows
void WindowLoop( Loop_Control_Struct *ctrl )
{
	int Window;

	if ( ctrl->UsePassiveStereoViaWindow )	
	{
		if ( ctrl->Eye == LEFT_EYE )
		{
			ctrl->Window = ctrl->LeftWindow;
			SetWindowAndRedisplay( ctrl );
		}
		if ( ctrl->Eye == RIGHT_EYE )
		{
			ctrl->Window = ctrl->RightWindow;
			SetWindowAndRedisplay( ctrl );
		}
		// center eye should never be active when using stereo
	}
	else
	{
		for( Window = 0; Window < ctrl->NumberOfWindows; Window++ )
		{
			ctrl->Window = Window;
			SetWindowAndRedisplay( ctrl );
		}
	}
}


// start stereo, especially for passive stereo via windows
void LeftRightLoop( Loop_Control_Struct *ctrl )
{
        // update current time
        ctrl->Current_Time = glutGet( (GLenum) GLUT_ELAPSED_TIME );

	fl_pregraphics_all_objects( ctrl->Universe );

	if ( ctrl->UseStereo && ctrl->UsePassiveStereoViaWindow )
	{
		ctrl->Eye = LEFT_EYE;
		WindowLoop( ctrl );
		ctrl->Eye = RIGHT_EYE;
		WindowLoop( ctrl );
	
	}
	else
	{
		ctrl->Eye = CENTER_EYE;
		WindowLoop( ctrl );
	}

	// update frame count	
	ctrl->Frame_Tics++;
}

