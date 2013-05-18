/*
 * graphics_loop.h
 *
 * Header for graphics_loop.cpp
 *
 * Copyright (C) 2003, University of New Mexico.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *           
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GRAPHICS_LOOP_H
#define GRAPHICS_LOOP_H

// define a log file filter function...
//typedef bool MESSAGE_FILTER_FUNCTION( OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message );


// what type of poygon offset does OpenGL support?
typedef enum { MISSING, EXTENSION, ONE_DOT_ONE } OFFSET;


// which eye position is in use?
typedef enum { CENTER_EYE, LEFT_EYE, RIGHT_EYE } EYE_ENUM;


// where is the display physically located?
// start with the position of the lower left corner, in homogenious 
// coordinates.  Then the lower right. upper right amd, finally the 
// upper left corner.
struct Wall_Struct
{
/// Fill in these values /////////////////////
											//
	float Corner[4][4];						// [ll/lr/ur/ul][x/y/z/w]
											//
//////////////////////////////////////////////
};	


// Where is the vieport located.
struct Viewport_Struct
{
/// Fill in these values /////////////////////
											//
	GLsizei X;								//
	GLsizei Y;								//
	GLsizei Width;							//
	GLsizei Height;							//
											//
//////////////////////////////////////////////
};	


// The parameters defining the Frustum (note spelling!)
struct Frustrum_Struct
{
/// Fill in these values /////////////////////
											//
	GLfloat Left;							//
	GLfloat Right;							//
	GLfloat Top;							//
	GLfloat Bottom;							//
	GLfloat Near;							//
	GLfloat Far;							//
											//
//////////////////////////////////////////////
};	


// This information defines a window.
// A window has a posiotion, and size, and a glut handle.
// A window can have an arbitrary number of viewports asociated with it.
// Each viewport has the following:
//   Extent - the position and size of a viewport (in pixels) within a window.
//   Frustrum - the frustum to use for that viewport. (recalculated at varios 
//     times,depending on the graphics display options)
//   Wall - the physical position of the display in the virtual environment.
//   Orientation - the orientation to take to face the display. (relative to
//     facing in the -Z direction with +Y up.)
// If passive stereo is being used via viewports, then the viewports to use 
// are specified using LeftViewport and RightViewport.
//
// Note: to allow more than one passive stereo display per window, LeftViewport 
// and RightViewport could be chanded to arrays.
struct Window_Control_Struct
{
/// Fill in these values /////////////////////
											//
	int NumberOfViewports;					// how many viewports
	int LeftViewport;						// specify viewport to use for left eye (passive stereo)
	int RightViewport;						// specify viewport to use for right eye (passive stereo)
	Wall_Struct *Wall;						// physical display position (relative to tracker)
	Wall_Struct *StartWall;						// physical display position (relative to tracker)
	Viewport_Struct *Extent;				// viewport extent
	Viewport_Struct *StartExtent;			// starting viewport extent
	Frustrum_Struct *Frustrum;				// frustrum for each viewport
	CORDORD *Orientation;					// head orientation (what is the direction of the cave wall)
	GLint glutWindow;						// glut window handle
	Viewport_Struct Position;				// window position 
	Viewport_Struct LastPosition;			// last window position 
	Viewport_Struct StartPosition;			// starting window position 
											//
	int MainMenu;				    	    	// main menu
	int selectCurrentObjMenu;				// sub menu handle of objects that can be selected
	int modCurrentObjMenu;					// sub menu handle to control current selected object
	int menuLoadAppMenu;					// sub menu handle for loadable apps
	int menuUnloadAppMenu;					// sub menu handle for unloadable apps

    	int SubMenuList[ 1000 ];    	    	    	    	// sub menus need to be destroyed
	int NumberOfSubMenus;											//

    	bool MenuInUse;
	
//////////////////////////////////////////////
};

// The Loop_Control_Struct defines all the windows to be used with Flatland
struct Loop_Control_Struct
{
/// Fill in these values /////////////////////
											//
	struct universe_type *Universe;			// the universe to display
	bool UseStereo;							// is the display in stereo
	bool FixedViewingPlanes;				// is the display a cave
	int NumberOfWindows;					// how many windows
	bool UsePassiveStereoViaWindow;			// use passive stereo, one window per eye
	int LeftWindow;							// specify window to use for left eye (passive stereo)
	int RightWindow;						// specify window to use for right eye (passive stereo)
	bool UsePassiveStereoViaViewport;		// use passive stereo, one window per eye
	Window_Control_Struct *WindowControl;	// Window cotrol structure
											//
	char LeftEyeName[MAX_NAME_SIZE];		// name of left eye object
	char RightEyeName[MAX_NAME_SIZE];		// name of right eye object
	char CenterEyeName[MAX_NAME_SIZE];		// name of center eye object
	char HeadTrackerName[MAX_NAME_SIZE];	// name of head transformation node object
	char TrackerClientName[MAX_NAME_SIZE];	// name of tracker client object
	char HeadName[MAX_NAME_SIZE];			// name of head object
											//
/// not these ////////////////////////////////
											//
	EYE_ENUM Eye;							// is the current eye the center one, the left one, or the right one?
	int Window;								// the current window
	int Viewport;							// the current viewport
	OBJECT_TYPE *CurrentControlObject;		// the currently selected obnect
											//
	OBJECT_TYPE *LeftEyeObject;				// left eye object
	OBJECT_TYPE *RightEyeObject;			// right eye object
	OBJECT_TYPE *CenterEyeObject;			// center eye object
	OBJECT_TYPE *HeadTrackerObject;			// head tracker object
	OBJECT_TYPE *TrackerClientObject;		// tracker client object
	OBJECT_TYPE *HeadObject;				// head object
											//
/// extra fluff (but needed) /////////////////
											//
	float FogDensity;						// the current fog density
	float FogColor[4];						// fog color
	int FogMode;							// fog mode
	float FogScale;							// scale fog density by this each step
	float MinimumFogDensity;				// the smallest fog density
	bool SkipFog;							// skip fog fade in of flatland
											//
/// other global values //////////////////////
											//
	bool showBoundingSpheres;				// show bounding spheres?
	bool showBoundingCubes;					// show bounding cubes?
	bool showBoundingPlanes;				// show bounding planes?
	bool showTrueShadows;					// show true shadows?
	bool showFullScreen;					// use full screen mobe?
											//
	OFFSET polygonOffsetVersion;			// how are offsets implemented?
											//
	int LastElapsedTime;					// the last elapsed time (for frame rate)
	float FramesPerSecond;					// frame rate
	bool showFramesPerSecond;				// display frame rate
	bool AverageOverFrames;					// averager time over a number of frames?
	int NumberOfFrames;						// average over how manty frames
	int FrameCount;							// how many frames so far
											//
	long int Frame_Tics;					// how many frames so far
	long int Simulation_Tics;				// how many simulation steps so far
        int Current_Time;                                       // how many milliseconds since glutInit()
											//
	bool NormalizeVectors;					// enable GL_NORMALIZE?
						
	char MessageLogName[MAX_NAME_SIZE];			// name of head object
	bool LogMessages;
	bool OpenMessageLog;
	FILE *LogFile;
	bool (*MessageLogFilter)(OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message ); // a filter for logging messages, logs when returns true...

	bool Build_MainMenu;				// sub menu handle of objects that can be selected
	bool Build_selectCurrentObjMenu;				// sub menu handle of objects that can be selected
	bool Build_modCurrentObjMenu;					// sub menu handle to control current selected object
	bool Build_menuLoadAppMenu;					// sub menu handle for loadable apps
	bool Build_menuUnloadAppMenu;					// sub menu handle for unloadable apps
											//
};


// the one and only Loop_Control object
extern Loop_Control_Struct *Loop_Control;

// unfortunate globals
extern char window_definition_file[MAX_NAME_SIZE];	// window definition file 

#ifdef Linux
// Name of the GLUT library. Usually it's libglut.so.3.
extern char glut_library[MAX_NAME_SIZE];

// Path to the joystick device.
extern char joystick_device[MAX_NAME_SIZE];
#endif //Linux

// the graphics loop starts here, called by glutIdel function
void LeftRightLoop( Loop_Control_Struct *ctrl );

// called by LeftRightLoop
void WindowLoop( Loop_Control_Struct *ctrl );
// called by WindowLoop
void SetWindowAndRedisplay( Loop_Control_Struct *ctrl );

// continues graphics loop, called bu glutDisplay function
void InnerLeftRightLoop( Loop_Control_Struct *ctrl );

// called by InnerLeftRightLoop
void ViewportLoop( Loop_Control_Struct *ctrl );
//called by ViewportLoop
void SetViewport( Loop_Control_Struct *ctrl );

// callsed by ViewportLoop
void BufferLoop( Loop_Control_Struct *ctrl );
// called by BufferLoop
bool RestructureTreeToEye( Loop_Control_Struct *ctrl );
// called by BufferLoop
void DrawUniverse( Loop_Control_Struct *ctrl );

// called by DrawUniverse
void FigureFrustrum( Loop_Control_Struct *ctrl );


// these two structs should probably be somewhere else....
typedef struct {
	double *point;
	double *color;	
	double *normal;
} VERTEX_TYPE;

typedef struct {
	int nvertices;
	VERTEX_TYPE **vertices;
} POLYGON_TYPE;


// function prototypes 
void *fl_GLUT_main( void * );
void fl_FlatlandInit( void );
void fl_FlatlandDisplay( void );
void fl_FlatlandMenuStatus( int status, int x, int y );
void fl_FlatlandReshape( int, int );
void fl_FlatlandMouse( int, int, int, int );
void fl_FlatlandJoystick( unsigned int buttonMask, int x, int y, int z );
void fl_FlatlandMotion( int, int );
void fl_FlatlandPassiveMotion( int, int );
void fl_FlatlandEntry( int );
void fl_FlatlandKey( unsigned char key, int x, int y );
void fl_FlatlandKeyUp( unsigned char key, int x, int y );
void fl_FlatlandSpecial( int key, int x, int y );
void fl_FlatlandSpecialUp( int key, int x, int y );
void fl_FlatlandSpaceballMotion( int x, int y, int z );
void fl_FlatlandSpaceballRotate( int x, int y, int z );
void fl_FlatlandSpaceballButton( int button, int state );
void fl_FlatlandTabletMotion( int x, int y );
void fl_FlatlandTabletButton( int button, int state, int x, int y );
void fl_FlatlandIdle( void );
bool fl_keyscan( OBJ_CONTROLS_ARRAY_TYPE *object_controls, char key );
bool fl_tbe_invert_homogenious_4x4_matrix( float *in, float *out );

#define currentControlObject	Loop_Control->CurrentControlObject
#define current_main_window	Loop_Control->Window
#define full_screen_flag	Loop_Control->showFullScreen
#define sim_time		Loop_Control->Frame_Tics

#endif
