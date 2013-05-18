/*
sound_craft.c.  produce craft sounds (such as engine)

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
 * Created: 1-28-00
 */

#include "flatland_app.h"
#include "SoundVessel.h"

/* Constants */
const char ENG_FILENAMES[NUM_ENG_SOUNDS][20] =
{"snd0-m.wav",
 "snd1-m.wav",
 "snd2-m.wav",
 "snd3-m.wav",
 "snd4-m.wav",
 "snd5-m.wav",
 "snd6-m.wav"};

/* "local" globals */
static GLuint base_eng_list;

void open_eng_sounds(void)
{
    int idx;
    static bool opened;
    float position [4];

    // Guard against multiple calls
    if(opened)
      return;

    // request sound list ids
    base_eng_list = slGenLists (NUM_ENG_SOUNDS);

    // Open/upload all engine sounds
    for(idx = base_eng_list; idx < base_eng_list + NUM_ENG_SOUNDS; idx++)
    {
      // Wav file
      slNewList (idx, (char *) ENG_FILENAMES[idx - base_eng_list]);

      // Initial position is at the origin
      position [0] = 0.0;
      position [1] = 0.0;
      position [2] = 0.0;
      position [3] = 1.0;
      slPositionList (idx, position);

      // Initial volume is zero
      slVolumeList (idx, ZERO_VOLUME);

      // Loop mode
      slLoopList (idx, LOOP_MODE);

      // AC3 priority
      slPriorityList (idx, MED_PRIORITY);

      // Rewind the list (I'm not sure why... historically done this way)
      slRewindList (idx);
    }

    // Start them all playing
    for(idx = base_eng_list; idx < base_eng_list + NUM_ENG_SOUNDS; idx++)
      slCallList (idx);

    return;
}

// Sets all seven gains in one call, expects a ptr to vector of floats
void set_eng_gains(const float *gains)		
{
  int idx;

  for(idx = base_eng_list; idx < base_eng_list + NUM_ENG_SOUNDS; idx++)
  {
    slVolumeList (idx, gains[idx - base_eng_list]);
  }
    
  return;
}	

// Funct to set engine sound location for all of the engine sounds
void set_eng_loc( float *loc)
{
  int idx;

  for(idx = base_eng_list; idx < base_eng_list + NUM_ENG_SOUNDS; idx++)
  {
    slPositionList (idx, loc); 
  }
  
  return;
}	


// new version - open all sounds, mix between them.
int engine_sound(float velocity)
{
  static bool sounds_opened;	// Have the files already been opened
  float	 gains[NUM_ENG_SOUNDS];
  int    idx;

  // Location of sound, in meters, relative to head (below)
  // DDT check if match opengl! (x,y,z,w)
  static float loc[4] = {0.0, -1.5, 0.0, 1.0};

  // On the first time through, open the sound files
  if(sounds_opened == false)
  {
    open_eng_sounds();
    sounds_opened = true;	   
  }

//  // Limit
//  if(velocity < 0.0)
//    velocity = -1.0 * velocity;

	if (velocity<0)
	{
		for(idx = 0; idx < NUM_ENG_SOUNDS; idx++)
		{
			gains[idx] = 0;
		}
	}
	else 
	{  
		if(velocity > 1.0)
		{
			velocity = 1.0;
		}
		
		// Compute gain for each sound
		// This computes a mix that is exponential in dB, and thus resembles a Gaussian mix of all
		// sounds. Works pretty well, courtesy of cpw

		for(idx = 0; idx < NUM_ENG_SOUNDS; idx++)
		{
			// Scale our seven sounds over 0.0 to 1.0
			//    channel_val = idx * (1.0 / (NUM_ENG_SOUNDS - 1.0));

			// exp(x - offset), basically
			// This seems to be unnecessary with new SDL/A3D code...
			// kls: changed from linear dropoff to settable dropoff.  3.0 seems nice
			//gains[idx] = 1.0 - fabs(velocity - idx);
			gains[idx] = ENG_VOLUME * (1.0 - ENG_BLENDING_DROPOFF_RATE * fabs(velocity - (float)idx/(float)(NUM_ENG_SOUNDS-1)));
			if (gains[idx] < 0.0) 
			{
				gains[idx] = 0.0;
			}
		}
	}

	// Set gain and location for all and sundry
	set_eng_loc(loc);	
	set_eng_gains(gains);		

  return(0);
}    

