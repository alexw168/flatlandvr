/*
Flatland.cpp  Main program for Flatland.  Does init and spawns sound & vis
threads.

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

#include "gheader.h"

#ifndef USE_OLD_CPP_HEADERS
#	include <iostream>
#else
#	include <iostream.h>
#endif


#ifndef USE_OLD_CPP_HEADERS
using namespace std;
#endif

// globals 
Boolean do_exit = FALSE;			// exit now flag 
Boolean exit_now = FALSE;                       // everybody else is gone... 
                                                // you leave too now flag
APPLISTTYPE *loadableApps;			// applications specified in the config file 
// TODO: NOTE:  This is big to cover up a mutex problem when expanding the db
int maxobjects=2000;					// MAX # object in the graph 
int maxxforms=5;					// MAX # of transforms between objects 

UNIVERSE_TYPE *Universe ;			// *THE* universe 
OBJECT_TYPE *CenterOfTheUniverse;

// dummy functions to get the included function calls into the symbol table on
// SGIs (or any other machine that uses a static libglut.a).
// It would probably be a good idea never to call these functions.
void flatland_dummy1 (int a, int b, int c) {}
void flatland_dummy (void)
{
  unsigned char tmpstr [MAX_NAME_SIZE];

  glutSetWindow(1);
  glutGetWindow();
  glutSetIconTitle("");
  glutSetWindowTitle("");
  glutDestroyWindow(0);
  glutShowWindow();
  glutHideWindow();
  glutSetCursor(GLUT_CURSOR_HELP);
  glutGetModifiers();
  glutSolidCube(1.0);
  glutWireCube(1.0);
  glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_10, tmpstr);
  glutChangeToMenuEntry(0,"",0);
  glutChangeToSubMenu(0,"",0);
  glutDetachMenu(0);
  glutDeviceGet((GLenum)GLUT_HAS_SPACEBALL);
  glutDialsFunc(fl_FlatlandReshape);
  glutEntryFunc(fl_menu_select);
  glutEstablishOverlay();
  glutGetColor(0,0);
  glutGetMenu();
  glutHideOverlay();
  glutIconifyWindow();
  glutLayerGet((GLenum)GLUT_OVERLAY_POSSIBLE);
  glutMenuStatusFunc(flatland_dummy1);
  glutOverlayDisplayFunc(fl_FlatlandInit);
  glutPopWindow();
  glutPositionWindow(0,0);
  glutPostOverlayRedisplay();
  glutPushWindow();
  glutRemoveMenuItem(0);
  glutRemoveOverlay();
  glutSetColor(0,0.f,0.f,0.f);
  glutShowOverlay();
  glutSolidCone(0.0,0.0,0,0);
  glutSolidDodecahedron();
  glutSolidIcosahedron();
  glutSolidOctahedron();
  glutSolidSphere(0.0,0,0);
  glutSolidTeapot(0.0);
  glutSolidTetrahedron();
  glutSolidTorus(0.0,0.0,0,0);
  glutSpaceballButtonFunc(fl_FlatlandReshape);
  glutSpaceballMotionFunc(flatland_dummy1);
  glutSpaceballRotateFunc(flatland_dummy1);
  glutSpecialFunc(flatland_dummy1);
  glutStrokeWidth(GLUT_BITMAP_TIMES_ROMAN_10,0);
  glutTabletButtonFunc(fl_FlatlandMouse);
  glutTabletMotionFunc(fl_FlatlandMotion);
  glutTimerFunc(0,fl_menu_select,0);
  glutUseLayer((GLenum)GLUT_NORMAL);
  glutVisibilityFunc(fl_menu_select);
  glutWireCone(0.0,0.0,0,0);
  glutWireDodecahedron();
  glutWireIcosahedron();
  glutWireOctahedron();
  glutWireSphere(0.0,0,0);
  glutWireTeapot(0.0);
  glutWireTetrahedron();
  glutWireTorus(0.0,0.0,0,0);

  cout << "dummy\n";
  cerr << "dummy\n";
}

// prints the usage
// ************** NOTE: if you change the command line parameters, be darn
//                      sure to change the usage!
void print_usage (void)
{
  printf ("Usage:\n");
  printf ("Flatland [--config <file> | -c <file>] [--help | -h]\n");
  printf ("         [--version | -v] [-debug_config | -d]\n");
}


// // an empty GLUT display function
// void empty_display( )
// {
// }


int main(int argc, char *argv[])
{
        bool debug_config = false;      // print the config file as it is read?
	int i;
        char configFile[MAX_NAME_SIZE] = "Flatland.config";//default config file
	args_struct gl_args;            // needed by glut for initialization 
	pthread_t thread_vis;           // visualisation thread handle 
#ifdef ENABLE_SDL
	pthread_t sound_thread_handle;  // sound thread handel 
#endif


	// Set struct pointers
	gl_args.argc = &argc;
	gl_args.argv = argv;

        // parse the arguments
        for (i = 1; i < argc; i++)
        {
          // NOTE: any changes to this section should be reflected in the
          //       print_usage() function.
          if (!strcmp (argv[i], "--version") || !strcmp (argv[i], "-v"))
          {
#ifdef Flatland_version_C
#ifdef Flatland_version_D
            printf ("Flatland version D, release unknown\n");
#else
            printf ("Flatland version C, release unknown\n");
#endif
#endif
            exit(0);
          }
          if (!strcmp (argv[i], "--help") || !strcmp (argv[i], "-h"))
          {
            print_usage();
            exit(0);
          }
          if ((!strcmp (argv[i], "--config") || !strcmp (argv[i], "-c")))
          {
            if (argc > ++i)
            {
              strncpy (configFile, argv[i], MAX_NAME_SIZE);
              continue;
            }
            else
            {
              fprintf (stderr, "ERROR: No config file given.\n\n");
              print_usage();
              exit(1);
            }
          }
          if ((!strcmp (argv[i], "--debug_config") || !strcmp (argv[i], "-d")))
          {
            debug_config = true;
            continue;
          }

          // if we got this far that means it is an unknown option: print usage
          fprintf (stderr, "Unknown option: \"%s\"\n\n", argv[i]);
          print_usage();
          exit(1);
        }

	fprintf( stdout, "main - Starting Flatland...\n" );
	fflush(stdout);
	fflush(stdin);    

	// Define universe 
	Universe = fl_makeuniverse( "Universe", maxobjects, maxxforms ) ;  /* maxobjects and maxxforms should be in config file */

	// setup the graphics control structure, and fill in default values
	Loop_Control = new Loop_Control_Struct;

	// set up display flagss
	Loop_Control->Universe = Universe;
	Loop_Control->UseStereo = false;
	Loop_Control->UsePassiveStereoViaWindow = false;
	Loop_Control->UsePassiveStereoViaViewport = false;
	Loop_Control->FixedViewingPlanes = false;
	Loop_Control->LeftWindow = 0;
	Loop_Control->RightWindow = 0;
	Loop_Control->Frame_Tics = 0;
        Loop_Control->Current_Time = 0;
	Loop_Control->Simulation_Tics = 0;
	
	// no selected object...
	Loop_Control->CurrentControlObject = NULL;

	// set up fog
	Loop_Control->FogDensity = 1.0;
	Loop_Control->FogColor[0] = 0.0;
	Loop_Control->FogColor[1] = 0.0;
	Loop_Control->FogColor[2] = 0.0;
	Loop_Control->FogColor[3] = 0.0;
	Loop_Control->FogMode = GL_EXP;
	Loop_Control->FogScale = 0.8;
	Loop_Control->MinimumFogDensity = 0.00001;

	// initialize graphics flags
	Loop_Control->showBoundingSpheres = false;				//
	Loop_Control->showBoundingCubes = false;				//
	Loop_Control->showBoundingPlanes = false;				//
	Loop_Control->showTrueShadows = false;					//
	Loop_Control->showFramesPerSecond = false;				//
	Loop_Control->showFullScreen = false;
	
	Loop_Control->NormalizeVectors = false;

	Loop_Control->LogMessages = false;
	Loop_Control->OpenMessageLog = true;
	Loop_Control->LogFile = 0;
	strcpy( Loop_Control->MessageLogName, "MessageLog.txt" );
    	Loop_Control->MessageLogFilter = 0;

	// set up default names of expected objects
	strcpy( Loop_Control->LeftEyeName, "LeftPupil" );					// make configurable
	strcpy( Loop_Control->RightEyeName, "RightPupil" );					// make configurable
	strcpy( Loop_Control->CenterEyeName, "CenterPupil" );				// make configurable
	strcpy( Loop_Control->HeadTrackerName, "HeadTransformationNode" );	// make configurable
	strcpy( Loop_Control->TrackerClientName, "TrackerClient" );			// make configurable
	strcpy( Loop_Control->HeadName, "Head" );							// make configurable

	// read the main Flatland configuration file
	fprintf( stdout, "main- Reading Config File.\n" );
	loadableApps = fl_read_config_file( configFile, debug_config ) ;

	// Loop over the data read from the config file & add the universe 
	// pointer to each one 
	if ( loadableApps )
	{
		for( i=0; i < loadableApps->napps; i++ )
		{
			loadableApps->applications[i]->universe_ptr = Universe;
		}
	}

	// read the window definition file
	fl_ReadWindowDefinition( window_definition_file, Loop_Control );

    	    	Loop_Control->Build_selectCurrentObjMenu = true;
    	    	Loop_Control->Build_menuLoadAppMenu = true;
    	    	Loop_Control->Build_menuUnloadAppMenu = true;
    	    	Loop_Control->Build_modCurrentObjMenu = true;

	// spawn the sound, vis, IFC (InterFlatland Comm), and tracker threads 
	try
	{	    
		fprintf( stdout, "main- Starting graphics thread.\n" );
#ifdef Darwin
		// XXXtkh
		// Mac OS X implementation of GLUT doesn't allow the OpenGL
		// thread to run as a child thread. 
		fl_GLUT_main((void *) &gl_args);
#else
		pthread_create(&thread_vis, NULL, &fl_GLUT_main, (void *) &gl_args);
#endif //Darwin

#ifdef ENABLE_SDL
		fprintf( stdout, "main- Starting sound thread.\n" );
		pthread_create(&sound_thread_handle, NULL, &fl_sound_thread, NULL);
#endif

		pthread_detach(thread_vis);
#ifdef ENABLE_SDL
		pthread_detach(sound_thread_handle);
#endif

	}
	catch(general_exception &X)
	{
		fprintf( stderr, "Exception while starting/creating threads!\nEsception: %s\n", X.message() );
	}

	// Busy wait for threads to exit
	while ( !do_exit ) 
	{
		fl_short_sleep(TENTH_SECOND);
	}

	// Give threads time to close gracefully
	fprintf( stdout, "Main: waiting for threads to exit...\n" );

	fflush( stdout );

	fl_short_sleep( HALF_SECOND );
	
	fprintf( stdout, "(main) End Of Line" );

	for( i=0; i<3; i++ )
	{
	    fflush( stdout );
    	    fl_short_sleep( HALF_SECOND );
	    fprintf( stdout, " ." );
    	}    
	fprintf( stdout, "\n" );

        // Let the stragglers know we have left
        exit_now = TRUE;

	return 0;
}
