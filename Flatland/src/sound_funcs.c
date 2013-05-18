/*
sound_funcs.c. OpenGL list-like interface to the sound system.

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

/*
 * NOTE: thruout this code you may notice that sound channel 0 is never used.
 * This is because it is reserved (in sound_client.h) for transient sound
 * events (noteably the intro and ending sounds).
 */

#include "gheader.h"

/* global variables */
int MAX_SOUND_LISTS = 100;
GLfloat current_sound_matrix[16];

/* "local" global variables */
#ifdef ENABLE_SDL
  static GLuint top_of_list = 1;
  static snd_channel **sound_list;
#endif

/* initialize the sound functions. */
void slInit (void)
{
#ifdef ENABLE_SDL
  printf ("Max # of sounds set to %d\n", MAX_SOUND_LISTS);

  /* allocate memory for the list of channels */
  sound_list = (snd_channel **) calloc(sizeof (snd_channel *), MAX_SOUND_LISTS);
  top_of_list = 1;
#endif
}

void slLoadMatrix (GLfloat mtx[16])
{
#ifdef ENABLE_SDL
  int i,j;
  for ( i=0; i<4; i++ )
    for ( j=0; j<4; j++ )
      current_sound_matrix[4 *i + j] = mtx[4 *i + j];
#endif
}

/* set head position */
void slHeadPosition (GLfloat pos[])
{
#ifdef ENABLE_SDL
  snd_channel hp;

  for(int j = 0; j < 6; j++)
    hp.pos[j] = pos[j];

  fl_send_command (hp, CMD_SET_HEAD_POS);
#endif
}

/* generate one or more sound lists */
/* returns the first in a contiguous list of ids "range" long, or 0 on error */
GLuint slGenLists (GLsizei range)
{
#ifdef ENABLE_SDL
  GLuint tl;

  /* keep the TOL around to return later */
  tl = top_of_list;

  /* Check to make sure we haven't exceeded our sound limit */
  if ((int)(tl + range) >= MAX_SOUND_LISTS || range < 1) return (0);

  for (GLuint i=tl; i < tl + range; i++)
  {
    /* allocate the sound_list structure */
    sound_list[i] = (snd_channel *) calloc (sizeof (snd_channel),1);
    if (sound_list[i] == NULL)
    {
      fprintf(stderr, "ERROR: Could not allocate memory for sound list\n");
      exit (1);
    }

    /* set defaults */
    sound_list[i]->id = i;
    sound_list[i]->filename = NULL;
    sound_list[i]->volume = 0.0;
    sound_list[i]->num_loops = 0;
    sound_list[i]->pos[0] = 0.0; // x 
    sound_list[i]->pos[1] = 0.0; // y
    sound_list[i]->pos[2] = 0.0; // z
    sound_list[i]->pos[3] = 0.0; // roll
    sound_list[i]->pos[4] = 0.0; // pitch
    sound_list[i]->pos[5] = 0.0; // yaw
    sound_list[i]->priority = 0.0;

    /* send the appropriate commands to the server */
    fl_send_command(*sound_list[i], CMD_VOL);
    fl_send_command(*sound_list[i], CMD_POS);
    fl_send_command(*sound_list[i], CMD_SET_PRIORITY);
    fl_send_command(*sound_list[i], CMD_REW);
  }
  top_of_list += range;

  return (tl);
#else
  return (0);
#endif
}

/* returns TRUE if the specified list is a valid sound list */
GLboolean slIsList (GLuint list)
{
#ifdef ENABLE_SDL
  return (sound_list [list] ? TRUE : FALSE);
#else
  return (FALSE);
#endif
}

/* deletes the list and range-1 consecutive lists */
void slDeleteLists (GLuint list, GLsizei range)
{
#ifdef ENABLE_SDL
  if (range > 0)
    for (GLuint i=list; 
         i < list + range  && list + range - 1 < (GLuint)MAX_SOUND_LISTS; 
         i++)
    {
      fl_send_command (*sound_list[i], CMD_CLOSE);
      free (sound_list[i]);
      sound_list[i] = NULL;
    }
#endif
}

/* Create a new sound list, given a list number and a wav file name */
void slNewList (GLuint list, char *file)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
  {
    sound_list[list]->filename = file;

    /* open/upload the sound */
    if(fl_send_command(*sound_list[list], CMD_OPEN) == OPEN_ERR)
    {   
      printf("\nFilename '%s' not on server - uploading", 
             sound_list[list]->filename);
      fl_send_command(*sound_list[list], CMD_UPLOAD);

      if(fl_send_command(*sound_list[list], CMD_OPEN) != OPEN_OK)
      {   
        fprintf(stderr,"\nUnable to open file '%s' after upload, exiting",
                sound_list[list]->filename);
        return;
      }
    }
  }
#endif
}

/* Call a sound */
void slCallList (GLuint list)
{
#ifdef ENABLE_SDL
  fl_send_command (*sound_list [list], CMD_BEGIN);
#endif
}

/* set volume */
void slVolumeList (GLuint list, float vol)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
  {
    if (vol > 1.0) vol = 1.0;
    if (vol < 0.0) vol = 0.0;
    sound_list [list]->volume = vol;
    fl_send_command (*sound_list[list], CMD_VOL);
  }
#endif
}

/* set attenuation */
void slAttenuationList (GLuint list, float att)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
  {
    if (att < 0.0) att = 0.0;
    sound_list [list]->distance = att;
    fl_send_command (*sound_list[list], CMD_DIST);
  }
#endif
}

/* set number of loops */
void slLoopList (GLuint list, int loop)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
    sound_list [list]->num_loops = loop;
#endif
}

/* set position */
void slPositionList (GLuint list, GLfloat pos[4])
{
#ifdef ENABLE_SDL
  int j;
  GLfloat temp[4];
  if (slIsList (list))
  {
    fl_tpc_matrix_multiply (current_sound_matrix,
                         pos,
                         temp);
    // Only use x,y,z position elements leave roll, pitch, yaw 0.0
    for(j = 0; j < 3; j++)
    {
      sound_list[list]->model_pos[j] = pos[j];  // save the model coordinates
      sound_list[list]->pos[j] = temp[j];       // save transformed coordinates
    }
    sound_list[list]->model_pos[j] = pos[j];    // save final coord (the 1.0)

    // TODO: HACK: change sign of z, due to problem with server code?
    // Apparently done in the latest server.  KLS, 10-Apr-00
//    sound_list[list]->pos[2] = -temp[2];

    fl_send_command (*sound_list[list], CMD_POS);
  }
#endif
}

void slPriorityList (GLuint list, float pri)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
  {
    if (pri > 1.0) pri = 1.0;
    if (pri < 0.0) pri = 0.0;
    sound_list [list]->priority = pri;
    fl_send_command (*sound_list[list], CMD_SET_PRIORITY);
  }
#endif
}

void slStopList (GLuint list)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
    fl_send_command (*sound_list[list], CMD_STOP);
#endif
}

void slRewindList (GLuint list)
{
#ifdef ENABLE_SDL
  if (slIsList (list))
    fl_send_command (*sound_list[list], CMD_REW);
#endif
}

void slUpdatePositions (void)
{
#ifdef ENABLE_SDL
  GLuint list;

  /* loop through all valid sound lists & position them correctly (must be
   * done evey time through the loop to keep sounds positioned in the right
   * spot in the enviornment) */
  for (list=1; list < top_of_list; list++)
    if (sound_list[list] != NULL)
      slPositionList (list, sound_list[list]->model_pos);
#endif
}

