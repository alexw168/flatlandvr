/*
fl_sound_thread.c.  Main sound thread (and sound loop)

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
 * Created: 12-9-99, pfh
 * Added to new Flatland: 1/26/00, KLS
 * 
 * 4-10-00, KLS
 * Added ifdefs to detect presense of SDL
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "gheader.h"
#include "sound_client.h"

#ifdef ENABLE_SDL

#include <SDL/SDL.h>
#include "SDLnet.h"

// ----------------------------------------------------------------------
// Global variables, defines, etc
char sound_server [MAX_NAME_SIZE] = {"localhost"};
int max_oc_sounds = 100;
static int NUM_OPENING_WAVS = 17;
static int NUM_CLOSING_WAVS = 9;
const char SUBDIR_NAME[] = "sounds";

/* --------------------------------------------------------------------
   New (rewritten, anyway) code to open/upload the engine wav files
   with the new A3D/SDL audio server.

   Assumes server already opened, so can stream commands at will.

   pfh 12/16/99
   */

void fl_sound_all_objects( UNIVERSE_TYPE *w )
{
  OBJECT_TYPE *object ;
  int i ;

  /* update the positions of all sounds */
  slUpdatePositions();
  
  for( i=0 ; i<w->nobjects ; i++ ) 
  {
      object = w->database[i].object ;
      if( object!=NULL && object->soundfunc!=NULL) 
      {
	  if( object->sounding_on_flag==1 ) 
	  {
	      slLoadMatrix ( object->Mco );     /* load the matrix for this object */
	      (*object->soundfunc)(object);     /* sound the object */
	  }
      }
  }
}

// ----------------------------------------------------------------------
// New (12/15/99) sound thread, modified from hello_sdl
// pfh
void *fl_sound_thread(void *foo)
{
    TCPsocket tcpsock;
    snd_channel sound;
    int j;
    const float head_pos[6] = {0.0, 0.0, 0.0,
			       0.0, 0.0, 0.0};
    char filename[64];
    int i;

    // Open the net, etc, etc
    tcpsock = startup_sdl_net(sound_server);

    // If no net, cry and go home
    if(tcpsock == NULL)
      return(NULL);
    
    // Send 'init' message to server to get things rolling
    // Note that the server ignores the struct for this message!
    sound.id = ALL_CHANNELS;
    fl_send_command(sound, CMD_INIT);

    // init the sound list package
    slInit();

    /* Set the maximum number of opening and closing sounds */
    NUM_OPENING_WAVS = NUM_OPENING_WAVS > max_oc_sounds ? max_oc_sounds :
                       NUM_OPENING_WAVS;
    NUM_CLOSING_WAVS = NUM_CLOSING_WAVS > max_oc_sounds ? max_oc_sounds :
                       NUM_CLOSING_WAVS;
    /*
      Global (applying to all streams) configuration - head position,
      speaker mode, file subdirectory.
    */
    
    // Set head position to the origin
    fl_set_head_position(head_pos);

    // Set the speaker mode
    fl_set_sound_mode(SPEAKER_QUAD);


    // Set the subdirectory; exit if error
    if(fl_set_subdirectory(SUBDIR_NAME) != CMD_OK)
    {
	fl_send_command(sound, CMD_QUIT);
	return(NULL);
    }

    // Use the index reserved for transient events
    sound.id = IDX_TMP;

    // The filename is one of several opening wav sounds that we will
    // pick from.

    // Seed PRNG
    srandom((unsigned) time(NULL));

    // Pick a random number from 1 to N_O_WAVS
    i = (random() % NUM_OPENING_WAVS) + 1;

    // Build filename, pretty simple
    sprintf(filename, "%s%d.wav", "opening", i);

    sound.filename = filename;

    // Play it once
    sound.num_loops = 1;

    // ...at full volume
    sound.volume = FULL_VOLUME;
    fl_send_command(sound, CMD_VOL);

    // ...and at normal priority
    sound.priority = MED_PRIORITY;
    fl_send_command(sound, CMD_SET_PRIORITY);
    
    // Position initial sound a ways off - this a tweak value!
    sound.pos[0] = 0.0;
    sound.pos[1] = 0.25; // y == elevation
    sound.pos[2] = -1.0; // z = pos backwards, gl style
    for(j = 4; j < 6; j++)
        sound.pos[j] = 0.0;
    
    fl_send_command(sound, CMD_POS);

    /*
      Now we begin real work - try and open the filename on the server.
      If it doesn't exist on the server, use our upload facility to
      send it over.

      Cool, eh?

    */
    if(fl_send_command(sound, CMD_OPEN) == OPEN_ERR)
    {
	printf("\nFilename '%s' not on server - uploading", sound.filename);
	fl_send_command(sound, CMD_UPLOAD);

	if(fl_send_command(sound, CMD_OPEN) != OPEN_OK)
	{
	    fprintf(stderr, "\nUnable to open file '%s' after upload, exiting",
		    sound.filename);
	    return(NULL);
	}
    }
    
    sound.volume = 0.1;
    fl_send_command(sound, CMD_VOL);

    // Play it!
    fl_send_command(sound, CMD_BEGIN);

    // Loop until server tells us it's complete, but be polite about it.
    while(fl_send_command(sound, CMD_STATUS) == STAT_PLAYING)
      SDL_Delay(10);

    // Set up for shutdown sound
    fl_send_command(sound, CMD_CLOSE);

    i = (random() % NUM_CLOSING_WAVS) + 1;

    // Build filename, pretty simple
    sprintf(filename, "%s%d.wav", "closing", i);

    sound.filename = filename;

    if(fl_send_command(sound, CMD_OPEN) == OPEN_ERR)
    {
	printf("\nFilename '%s' not on server - uploading", sound.filename);
	fl_send_command(sound, CMD_UPLOAD);

	if(fl_send_command(sound, CMD_OPEN) != OPEN_OK)
	{
	    fprintf(stderr, "\nUnable to open file '%s' after upload, exiting",
		    sound.filename);
	    return(NULL);
	}
    }
    
    // Wait for shutdown
    while(do_exit == false)
    {
	// Audify everone else
	fl_sound_all_objects(Universe);

	// Politeness
	SDL_Delay(20);
    }

    // Turn off all sounds
    sound.id = ALL_CHANNELS;
    fl_send_command(sound, CMD_STOP);

    sound.id = IDX_TMP;

    sound.volume = 0.1;
    fl_send_command(sound, CMD_VOL);

    // Play shutdown sound
    fl_send_command(sound, CMD_BEGIN);

    // Loop until server tells us it's complete, but be polite about it.
    while(fl_send_command(sound, CMD_STATUS) == STAT_PLAYING)
      SDL_Delay(10);

    // Send 'quit' message
    fl_send_command(sound, CMD_QUIT);

    printf ("Sound thread exiting\n");

    return(NULL);
}

#endif // ENABLE_SDL
