/*
TrackerClientClient.cc  An app providing access to position tracking

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
// Added measurement rate input:  8-8-00, KLS
//

/* include the master Flatland library */
#ifdef WIN32
#  include "Flatland.h"
#  include "libFlatland.h"
#else
#  include "flatland_app.h"
#endif
#include "FlockDefs.h"
#include "Bird.h"
#include "ExtendedRangeFlockTrackerClient.h"

#define DEBUG

//#define POLL_DELAY 100000
#define POLL_DELAY 1000

// Max number of birds to read, used to size buffers
// maximum for normal addressing mode is 14 birds
#define MAX_BIRDS 14 

#define BIRD_BAUD 38400
//#define BIRD_BAUD 115200

// How many raw trackers managed (by default)
#define MAX_NUMBER_OF_TRANSDUCERS 14

// how fast should the flock run at?
#define TRACKER_CLIENT_DEFAULT_MEASUREMENT_RATE 112.9

#ifdef LINUX
// for linux
#define TRACKER_CLIENT_SERIAL_PORT "/dev/ttyS0"
#else
// for SGI
#define TRACKER_CLIENT_SERIAL_PORT "/dev/ttyd2"  
#endif //LINUX

#define TRACKER_CLIENT_DEFAULT_HEMISPHERE "lower"

/* =======================================================================

Description:
    a TrackerClient that gets its poition information from an ascention
    flock-of-birds.
        
Objects created when application is loaded:
    only FlockTrackerClient is created.
        
Object dependencies:
    FlockTrackerClient depends on the flock-of-birds hardware.
        
Configurable options:
    port                string  what port to use to talk to flock-of-birds
    transducers         int     how many transducers to use
    x                   float   attachment position
    y                   float   attachment position
    z                   float   attachment position
    roll                float   attachment position
    pitch               float   attachment position
    yaw                 float   attachment position
    measurement_rate    float   parameter for flock-of-birds
    
Threads created:
    TrackerClientThread() - reads data from flock-of-birds

Library requirements:
    Flatland basic setup

Location requirements:
    none.

Controls:
    Keyboard:
        'a' - toggle display of axes
        't' - toggle use of tracking information
    Menu:
        see keyboard input
    Voice:
        none
    Beaming:
        none

Status:
    15-Sep-2001 - In need of more documentation, otherwise done.

NOTE: Data from thread should probably be buffered, so that all requests
in a cycle of the graphics loop are the same...
    
======================================================================= */


// Function to read transducer status 
void *TrackerClientThread(void *X)
{
	OBJECT_TYPE *TrackerClientObject = (OBJECT_TYPE *) X;
//	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) TrackerClientObject->ptr0;
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	char *Name = TrackerClientObject->name;
	CFlock *Flock = 0;
	CORDORD *Tracker= 0; // = TrackerClientMembers->Tracker;
	float **pos = 0, **ang = 0;
	bool flock_initialized;
	float *real_triplets = 0;
	bool bird_on_flag = false;
	int i;
//    	unsigned char big_ass_buffer[1000];
    	int byte_count;
	int j = 0;
    	CORDORD avg = { 0,0,0, 0,0,0 };
    	int bird_status;
    	char buf[ 1000 ];
				
//fprintf( stdout, "(TrackerClientThread) birds=%d\n", num_birds );
	
	// Set up raw tracker data spaces
	if( !(Tracker = FlockClientDataMembers->Tracker = new CORDORD[MAX_NUMBER_OF_TRANSDUCERS]) )
	{
	    fprintf (stderr, "%s: Cannot allocate memory for tracker data\n", Name);
	    exit (1);
	}
	
	// init data
	for(i=0;i<MAX_NUMBER_OF_TRANSDUCERS;i++)
	{
	    Tracker[i].x = 0.0;
	    Tracker[i].y = 0.0;
	    Tracker[i].z = 0.0;
	    Tracker[i].roll = 0.0;
	    Tracker[i].pitch = 0.0;
	    Tracker[i].yaw = 0.0;
	}
	
    	// ---------------------------------------------------------
	
	// Allocate pointers-to-pointers
	pos = new (float (*[MAX_NUMBER_OF_TRANSDUCERS]));
	ang = new (float (*[MAX_NUMBER_OF_TRANSDUCERS]));

	// Allocate continuous data blocks that will actually store
	// the data.
	real_triplets = new float[MAX_NUMBER_OF_TRANSDUCERS*6];

	if( !pos || !ang || !real_triplets )
	{
	    fprintf (stderr, "%s: Cannot allocate memory for tracker data\n", Name);
	    exit (1);
	}

	// set pointers into continuous data block
	for(i = 0; i < MAX_NUMBER_OF_TRANSDUCERS; i++)
	{
	    pos[i] = &(real_triplets[3 * i]);
	    ang[i] = &(real_triplets[(3 * i) + (3 * MAX_NUMBER_OF_TRANSDUCERS)]);
	}
	
    	// ---------------------------------------------------------
	
    	FlockClientDataMembers->Ready = true;

	// Main thread loop    
	while( !do_exit )
	{
	    	if( FlockClientDataMembers->StartFlock || FlockClientDataMembers->StopFlock )
		{
		    	FlockClientDataMembers->StopFlock = false;
			
		    	if( Flock )
			{
#ifdef DEBUG
    	    	    	    fprintf( stdout, "%s: Calling CFlock destructor.\n", Name );
#endif
			    bird_on_flag = false;
			    delete Flock;
			    Flock = 0;
			}
		}
			
	    	if( FlockClientDataMembers->StartFlock )
		{
		    	FlockClientDataMembers->StartFlock = false;
			
#ifdef DEBUG
			fprintf( stdout, "%s: Calling CFlock constructor.\n", Name );
#endif

    			// construct a flock
			Flock = new CFlock( FlockClientDataMembers->SerialPortName, FlockClientDataMembers->BaudRate );

    			if( !Flock )
			{
			    fprintf( stdout, "%s: Cannot allocate CFlock\n", Name );
			    exit(1);
    			}

			if ( Flock->IsUp( ) )
			{
    			    FlockClientDataMembers->NumberOfTransducers = Flock->GetNumberOfReceivers( );

    			    fprintf( stdout, "%s: %d receivers found.\n", Name, FlockClientDataMembers->NumberOfTransducers );

    			    if( Flock->GetNumberOfExtendedRangeTransmitters( ) == 0 )
			    {
				FlockClientDataMembers->FullScaleInMeters = 0.9144;
			    }
			    else
			    {
				FlockClientDataMembers->FullScaleInMeters = 3.6576;
			    }

    			    if( !strcmp( FlockClientDataMembers->Hemisphere, "upper" ) )
			    {
    				Flock->SetHemisphere( 0x0c, 0x01 );
    #ifdef DEBUG
       				fprintf( stdout, "%s: upper hemisphere set\n", Name );
    #endif
    			    }
    			    if( !strcmp( FlockClientDataMembers->Hemisphere, "lower" ) )
			    {
    				Flock->SetHemisphere( 0x0c, 0x00 );
    #ifdef DEBUG
       				fprintf( stdout, "%s: lower hemisphere set\n", Name );
    #endif
    			    }

			    Flock->SetMeasurementRate( FlockClientDataMembers->BirdMeasurementRate );

//			if ( Flock->IsUp( ) )
//			{
			    fprintf(stdout, "%s: Flock initialized OK.\n", Name);
			    bird_on_flag = true;

//    	    	    	    Flock->SetReferenceFrame( -127.09, 3.47, 2.94 );
   			    Flock->SetReferenceFrame( 0.0, 0.0, 0.0 );

#ifdef DEBUG
//			    Flock->FlockQuery( );
			    for( i=1; i<=Flock->GetNumberOfBirds(); i++ )
			    {
				fprintf( stdout, "bird #%d:\n", i );
				fprintf( stdout, "%s\n", Flock->TranslateBirdStatus( buf, Flock->Examine_BirdStatus( i ) ) );
			    }			    
#endif

			    Flock->StartStream( );
			}
			else
			{
    	    	    	    if( FlockClientDataMembers->BeepOnError )
			    {
				fprintf( stdout, "%c", 0x07 );
			    }
			    fprintf(stdout, "%s: Flock failed to initialize.\n", Name);
			}

		}
		
		if (!bird_on_flag)
		{
			// Since this takes no I/O time, go to sleep for a bit
			fl_short_sleep( HALF_SECOND );
		}
		else // Connected OK, use flock
		{
			// NOTE: this is hard coded for the UNM trackers... customize to
			// suite 
         
	    	    	if( !FlockClientDataMembers->Streaming )
			{
			    // only ask for data if polling....
    	    	    	    Flock->RequestData( );
			}

//    	    	    	if( !Flock->GetPosAng(pos, ang) )
    	    	    	if( !Flock->Read_PositionAngle( pos, ang ) )
			{
    	    	    	    if( FlockClientDataMembers->BeepOnError )
			    {
				fprintf( stdout, "%c", 0x07 );
			    }
			    fprintf( stdout, "error reading data from flock.\n" );
			}
				      
			if ( FlockClientDataMembers->UseTracking ) 
			{
				for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
				{
					FlockClientDataMembers->Tracker[i].x = -pos[i][1] * FlockClientDataMembers->FullScaleInMeters;
					FlockClientDataMembers->Tracker[i].y = pos[i][2] * FlockClientDataMembers->FullScaleInMeters;
					FlockClientDataMembers->Tracker[i].z = pos[i][0] * FlockClientDataMembers->FullScaleInMeters;
					FlockClientDataMembers->Tracker[i].pitch = ang[i][1];
					FlockClientDataMembers->Tracker[i].yaw = -ang[i][0];
					FlockClientDataMembers->Tracker[i].roll = -ang[i][2];
				}
			}
			else
			{
				for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
				{
					FlockClientDataMembers->Tracker[i].x = 0;
					FlockClientDataMembers->Tracker[i].y = 0;
					FlockClientDataMembers->Tracker[i].z = 0;
					FlockClientDataMembers->Tracker[i].pitch = 0;
					FlockClientDataMembers->Tracker[i].yaw = 0;
					FlockClientDataMembers->Tracker[i].roll = 0;
				}
			}
		}
		
#ifdef DEBUG
#if 0
		for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
		{
		    fprintf( stdout, "%2d: (%6.2f, %6.2f, %6.2f) roll:%6.2f pitch:%6.2f yaw:%6.2f\n", i, FlockClientDataMembers->Tracker[i].x, FlockClientDataMembers->Tracker[i].y, FlockClientDataMembers->Tracker[i].z, FlockClientDataMembers->Tracker[i].roll, FlockClientDataMembers->Tracker[i].pitch, FlockClientDataMembers->Tracker[i].yaw );
		}
#endif
#endif

#ifdef DEBUG
		j++;
    	    	if( j>1000 )
		{
		    avg.x /= j;
		    avg.y /= j;
		    avg.z /= j;
		    avg.roll /= j;
		    avg.pitch /= j;
		    avg.yaw /= j;
		    
		    fprintf( stdout, "avg: (%6.2f, %6.2f, %6.2f) roll:%6.2f pitch:%6.2f yaw:%6.2f\n", avg.x, avg.y, avg.z, avg.roll, avg.pitch, avg.yaw );
		    j = 0;
		}		
		else
		{
		    avg.x += FlockClientDataMembers->Tracker[0].x;
		    avg.y += FlockClientDataMembers->Tracker[0].y;
		    avg.z += FlockClientDataMembers->Tracker[0].z;
		    avg.roll += FlockClientDataMembers->Tracker[0].roll;
		    avg.pitch += FlockClientDataMembers->Tracker[0].pitch;
		    avg.yaw += FlockClientDataMembers->Tracker[0].yaw;
		}
#endif
	}

	delete Flock;

	delete[] real_triplets;
	delete[] pos;
	delete[] ang;

	// Log same
	fprintf(stdout, "\n%s: Flock thread exiting\n", Name);

	return NULL;
}


void empty(OBJECT_TYPE *o)
{
}


bool IsReady(OBJECT_TYPE *TrackerClientObject)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	return FlockClientDataMembers->Ready;
}

int GetNumberOfTransducers(OBJECT_TYPE *TrackerClientObject)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	return FlockClientDataMembers->Ready ? FlockClientDataMembers->NumberOfTransducers : 0;
}

bool GetPosition(OBJECT_TYPE *TrackerClientObject, int transducer_number, Position_Struct *coord)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
	if (!FlockClientDataMembers->Ready || !FlockClientDataMembers->UseTracking || transducer_number<0 || transducer_number>=FlockClientDataMembers->NumberOfTransducers)
	{
		coord->x = 0.0f;
		coord->y = 0.0f;
		coord->z = 0.0f;
		coord->roll = 0.0f;
		coord->pitch = 0.0f;
		coord->yaw = 0.0f;
		return false;
	}
	else
	{
		coord->x = FlockClientDataMembers->Tracker[transducer_number].x;
		coord->y = FlockClientDataMembers->Tracker[transducer_number].y;
		coord->z = FlockClientDataMembers->Tracker[transducer_number].z;
		coord->roll = FlockClientDataMembers->Tracker[transducer_number].roll;
		coord->pitch = FlockClientDataMembers->Tracker[transducer_number].pitch;
		coord->yaw = FlockClientDataMembers->Tracker[transducer_number].yaw;
		return true;
	}
}

// Platform handler 
void platformfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) FlockTrackerClientObject->ptr1;

	GLboolean lighting_flag = GL_FALSE;
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={0.3,0.3,0.3,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	
	// draw what?

	// draw axes on demand...	
	if ( FlockClientDataMembers->DisplayAxes ) 
	{
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
void trueShadowFunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// shadow of what?
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
}

// pregraphics callback 
void pregraphicsfunc(OBJECT_TYPE *TrackerClientObject)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;

	if (TrackerClientObject->original_owner_flag == true) 
	{
		if (FlockClientDataMembers->first_pass == true)
		  {
		        pthread_t transducer_read_thread;		/* thread handle for data input */
			/* spawn a thread to read the tracker data */
			pthread_create(&transducer_read_thread, NULL, &TrackerClientThread, (void *) TrackerClientObject);
			FlockClientDataMembers->first_pass = false;
		  }
	}
}

/* menu control callback */
void MenuCallback(OBJECT_TYPE *FlockTrackerClientObject, char key)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) FlockTrackerClientObject->ptr1;

	switch (key)
	{
    case 'a':   // display axes
		FlockClientDataMembers->DisplayAxes = !FlockClientDataMembers->DisplayAxes;
		break;

    case 't':   // display axes
		FlockClientDataMembers->UseTracking = !FlockClientDataMembers->UseTracking;
		break;

    case 'b':   // display axes
		FlockClientDataMembers->BeepOnError = !FlockClientDataMembers->BeepOnError;
		break;

    case 'r':   // display axes
		FlockClientDataMembers->StartFlock = true;
		break;

    case 'R':   // display axes
		FlockClientDataMembers->StopFlock = true;
		break;
	}
}

extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
    int i;
    float PTemp[9];					// temporary transform storage 
    OBJECT_TYPE *TrackerClientObject;				// This object 
    TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers;			// pointer to "TrackerClientMembers" structure 
    FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers;			// pointer to "TrackerClientMembers" structure 
    char TRACKER_CLIENT_PORT[MAX_NAME_SIZE] = {TRACKER_CLIENT_SERIAL_PORT};  
    char TRACKER_CLIENT_HEMISPHERE[MAX_NAME_SIZE] = {TRACKER_CLIENT_DEFAULT_HEMISPHERE};  
    bool TRACKER_CLIENT_USE_TRACKING = true;
    bool TRACKER_CLIENT_DISPLAY_AXES = false;
    float TRACKER_CLIENT_MEASUREMENT_RATE = TRACKER_CLIENT_DEFAULT_MEASUREMENT_RATE;
    float TRACKER_CLIENT_X = 0.0f;
    float TRACKER_CLIENT_Y = 0.0f;
    float TRACKER_CLIENT_Z = 0.0f;
    float TRACKER_CLIENT_ROLL = 0.0f;
    float TRACKER_CLIENT_PITCH = 0.0f;
    float TRACKER_CLIENT_YAW = 0.0f;
    char *Name = App->appname;
    bool known;
    int itemp;
    int TRACKER_CLIENT_BAUD_RATE = BIRD_BAUD;
    bool TRACKER_CLIENT_STREAMING = false;
    bool TRACKER_CLIENT_BEEP_ON_ERROR = false;
    
//    float TRACKER_CLIENT_FULL_SCALE_IN_METERS = 0.9144;
    
    /* generate the "TrackerClientMembers" structure & attach it to the object */
    if ((TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) malloc (sizeof (TRACKER_CLIENT_ACCESS_MEMBERS))) == NULL)
    {
	    printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
	    exit(1);
    }

    /* generate the "FlockTrackerClientMembers" structure & attach it to the object */
    if ((FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) malloc (sizeof (FLOCK_TRACKER_CLIENT_DATA_MEMBERS))) == NULL)
    {
	    printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
	    exit(1);
    }

    FlockClientDataMembers->Tracker = NULL;
    TrackerClientMembers->GetPosition = GetPosition;
    TrackerClientMembers->GetVelocity = NULL;
    TrackerClientMembers->GetError = NULL;
    TrackerClientMembers->IsReady = IsReady;
    TrackerClientMembers->GetNumberOfTransducers = GetNumberOfTransducers;
    FlockClientDataMembers->Ready = false;
    FlockClientDataMembers->NumberOfTransducers = 0;
    FlockClientDataMembers->BirdMeasurementRate = 103.3; /*default measurement rate*/
    FlockClientDataMembers->first_pass = true;
    FlockClientDataMembers->FullScaleInMeters = 0.9144;
    FlockClientDataMembers->StartFlock = true;
    FlockClientDataMembers->StopFlock = false;

    /* parse the parameter list */
    for (i=0; i < App->nparams; i++)
    {
	known = false;

	if (!strcmp (App->params[i][0], "hemisphere"))
	{
	    sscanf(App->params[i][2], "%s", TRACKER_CLIENT_HEMISPHERE);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "port"))
	{
	    sscanf(App->params[i][2], "%s", TRACKER_CLIENT_PORT);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "baud_rate"))
	{
	    sscanf(App->params[i][2], "%d", &TRACKER_CLIENT_BAUD_RATE );
	    known = true;
	}
	if (!strcmp (App->params[i][0], "x"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_X);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "y"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Y);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "z"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Z);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "roll"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_ROLL);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "pitch"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_PITCH);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "yaw"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_YAW);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "display"))
	{
	    sscanf(App->params[i][2], "%d", &itemp);
	    TRACKER_CLIENT_DISPLAY_AXES = (itemp != 0);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "tracking"))
	{
	    sscanf(App->params[i][2], "%d", &itemp);
	    TRACKER_CLIENT_USE_TRACKING = (itemp != 0);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "streaming"))
	{
	    sscanf(App->params[i][2], "%d", &itemp);
	    TRACKER_CLIENT_STREAMING = (itemp != 0);
	    known = true;
	}
	if (!strcmp (App->params[i][0], "baud_rate"))
	{
	    sscanf(App->params[i][2], "%d", &TRACKER_CLIENT_BAUD_RATE );
	    known = true;
	}
	if (!strcmp (App->params[i][0], "measurement_rate"))
	{
	    sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_MEASUREMENT_RATE );
	    known = true;
	}
	if (!strcmp (App->params[i][0], "beep_on_error"))
	{
	    sscanf(App->params[i][2], "%d", &itemp);
	    TRACKER_CLIENT_BEEP_ON_ERROR = (itemp != 0);
	    known = true;
	}

	if (!known)
	{
	    fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
	}
    }

    strncpy( FlockClientDataMembers->SerialPortName, TRACKER_CLIENT_PORT,MAX_NAME_SIZE );
    strncpy( FlockClientDataMembers->Hemisphere, TRACKER_CLIENT_HEMISPHERE,MAX_NAME_SIZE );

    FlockClientDataMembers->BirdMeasurementRate = TRACKER_CLIENT_MEASUREMENT_RATE;
    FlockClientDataMembers->BaudRate = TRACKER_CLIENT_BAUD_RATE;
    FlockClientDataMembers->Streaming = TRACKER_CLIENT_STREAMING;
    FlockClientDataMembers->DisplayAxes = TRACKER_CLIENT_DISPLAY_AXES;
    FlockClientDataMembers->UseTracking = TRACKER_CLIENT_USE_TRACKING;
    FlockClientDataMembers->Position.x = TRACKER_CLIENT_X;
    FlockClientDataMembers->Position.y = TRACKER_CLIENT_Y;
    FlockClientDataMembers->Position.z = TRACKER_CLIENT_Z;
    FlockClientDataMembers->Position.roll = TRACKER_CLIENT_ROLL;
    FlockClientDataMembers->Position.pitch = TRACKER_CLIENT_PITCH;
    FlockClientDataMembers->Position.yaw = TRACKER_CLIENT_YAW;
    FlockClientDataMembers->BeepOnError = TRACKER_CLIENT_BEEP_ON_ERROR;
    
    TrackerClientObject = fl_add_child_to_parent_by_name( App->universe_ptr, App->parent_object_name, App->appname );

    TrackerClientObject->ptr0 = (void *) TrackerClientMembers;	// attach struct to object 
    TrackerClientObject->ptr1 = (void *) FlockClientDataMembers;	// attach struct to object 

    // set up callbacks 
    TrackerClientObject->soundfunc = soundfunc;
    TrackerClientObject->sounding_on_flag = 0;
    TrackerClientObject->platformfunc = platformfunc;
    TrackerClientObject->predrawfunc = predrawfunc;
    TrackerClientObject->drawfunc = drawfunc;
    TrackerClientObject->trueShadowFunc = trueShadowFunc;
    TrackerClientObject->cheapShadowFunc = cheapShadowFunc;
    TrackerClientObject->pregraphicsfunc = pregraphicsfunc;
    TrackerClientObject->drawing_on_flag = 1;

    //// Menu Setup ////

    // set up the control structure (three subcontrols under one) 
    // allows control of the object from the keyboard, menues, and voice 
    TrackerClientObject->object_controls = fl_makeControlsSet(5);

    // Define spin/nospin controls for cube 
    strcpy (TrackerClientObject->object_controls->controls[0]->menuname, "Toggle Axes Display");
    strcpy (TrackerClientObject->object_controls->controls[0]->voicename, "Axes");
    TrackerClientObject->object_controls->controls[0]->keystroke = 'a';
    TrackerClientObject->object_controls->controls[0]->control_function = MenuCallback;

    strcpy (TrackerClientObject->object_controls->controls[1]->menuname, "Toggle Tracking");
    strcpy (TrackerClientObject->object_controls->controls[1]->voicename, "Tracking");
    TrackerClientObject->object_controls->controls[1]->keystroke = 't';
    TrackerClientObject->object_controls->controls[1]->control_function = MenuCallback;

    strcpy (TrackerClientObject->object_controls->controls[2]->menuname, "Toggle beep on Error");
    strcpy (TrackerClientObject->object_controls->controls[2]->voicename, "Beep");
    TrackerClientObject->object_controls->controls[2]->keystroke = 'b';
    TrackerClientObject->object_controls->controls[2]->control_function = MenuCallback;

    strcpy (TrackerClientObject->object_controls->controls[3]->menuname, "Run Flock");
    strcpy (TrackerClientObject->object_controls->controls[3]->voicename, "Run");
    TrackerClientObject->object_controls->controls[3]->keystroke = 'r';
    TrackerClientObject->object_controls->controls[3]->control_function = MenuCallback;

    strcpy (TrackerClientObject->object_controls->controls[4]->menuname, "Stop Flock");
    strcpy (TrackerClientObject->object_controls->controls[4]->voicename, "Stop");
    TrackerClientObject->object_controls->controls[4]->keystroke = 'R';
    TrackerClientObject->object_controls->controls[4]->control_function = MenuCallback;


    //// Bounding Volume Setup ////

    TrackerClientObject->bounding_volume_set = fl_makebvset(3);

    // define a bounding volume (four corners and a center) 
    TrackerClientObject->bounding_volume_set->bvset[0]->npoints = 5;
    // center point 
    TrackerClientObject->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
    TrackerClientObject->bounding_volume_set->bvset[0]->p[0][1]=0.0;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[0][2]=0.0;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[0][3]=1.0;
    // -x, -y, -z corner 
    TrackerClientObject->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
    TrackerClientObject->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[1][3]=1.0;
    // +x, -y, -z corner 
    TrackerClientObject->bounding_volume_set->bvset[0]->p[2][0]=+0.01;    
    TrackerClientObject->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[2][3]=1.0;
    // -x, +y, -z corner 
    TrackerClientObject->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
    TrackerClientObject->bounding_volume_set->bvset[0]->p[3][1]=+0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[3][3]=1.0;
    // -x, -y, +z corner 
    TrackerClientObject->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
    TrackerClientObject->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[4][2]=+0.01;
    TrackerClientObject->bounding_volume_set->bvset[0]->p[4][3]=1.0;

    // Define bounding sphere for culling 
    TrackerClientObject->bounding_volume_set->bvset[1]->npoints = 2 ;
    // Center point 
    TrackerClientObject->bounding_volume_set->bvset[1]->p[0][0]=0.0;
    TrackerClientObject->bounding_volume_set->bvset[1]->p[0][1]=0.0;
    TrackerClientObject->bounding_volume_set->bvset[1]->p[0][2]=0.0;
    TrackerClientObject->bounding_volume_set->bvset[1]->p[0][3]=1.0;
    // Surface point 
    TrackerClientObject->bounding_volume_set->bvset[1]->p[1][0]=+0.01;
    TrackerClientObject->bounding_volume_set->bvset[1]->p[1][1]=+0.01;
    TrackerClientObject->bounding_volume_set->bvset[1]->p[1][2]=+0.01;	
    TrackerClientObject->bounding_volume_set->bvset[1]->p[1][3]=1.0;

    // Define a bounding point 
    TrackerClientObject->bounding_volume_set->bvset[2]->npoints = 1 ;
    // Center point 
    TrackerClientObject->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
    TrackerClientObject->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
    TrackerClientObject->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
    TrackerClientObject->bounding_volume_set->bvset[2]->p[0][3]= 1.0;

    // attach to the wessel in a default place 
    PTemp[0] = TRACKER_CLIENT_X;
    PTemp[1] = TRACKER_CLIENT_Y;
    PTemp[2] = TRACKER_CLIENT_Z; 
    PTemp[3] = TRACKER_CLIENT_YAW;
    PTemp[4] = TRACKER_CLIENT_PITCH;
    PTemp[5] = TRACKER_CLIENT_ROLL;
    PTemp[6] = 1.0;
    PTemp[7] = 1.0;
    PTemp[8] = 1.0;
    TrackerClientObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
    TrackerClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
    TrackerClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
    TrackerClientObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
    TrackerClientObject->orginal_first_node->MpoParams->P->assign( PTemp );
}

} // extern "C"
