/*
 * sound_client.h
 *
 * Header file for sound_client.c and fl_sound_thread.c
 * pfh/tbe 11/16/99
 * File sound_client.h
 *
 * Common defines and so forth for sound client programs. 
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

#if !defined(SOUND_CLIENT_H)
#define SOUND_CLIENT_H

/* --------------------------------------------------------------------
  The following section is all of the things users should care or need
  to worry about - constants, prototypes, and so forth.
*/

/*
  Struct that defines everything about a sound channel that we care about

  The idea is to have one of these per sound-creating-entity. Other than
  the global parameters such as head position and A3D mode, this should
  hold everything that the system needs to know to use a sound source.

  In our code, we create arrays of these, where the index in the array
  is the sound ID, it seems to make for good resource management.
*/
typedef struct 
{
  int   id;           // Zero to N-1, unique ID/handle
  char  *filename;    // Wave filename on server, filename ONLY!
  float volume;       // [0.0, 1.0]
  int   num_loops;    // Number of times to repeat, where -1 means loop
  float pos[6];       // Position, (x,y,z,roll,pitch,yaw)
  float model_pos[6]; // Position, (x,y,z,roll,pitch,yaw), in model coords
  float priority;     // A3D priority, [0.0, 1.0]  
  float distance;     // Distance from source that volume decay begins (<0.0)
}snd_channel;

// Speaker Modes
typedef enum { SPEAKER_HEADPHONES, SPEAKER_DUAL, SPEAKER_QUAD } speaker_mode;

const float SOUND_DISTANCE_ATTENUATION = 10.0;

const char HEADPHONE_MSG[] = "  1";
const char DUAL_SPEAKER_MSG[] = "  2";
const char QUAD_SPEAKER_MSG[] = "  3";

// Sound indices
const int IDX_TMP = 0;

// Volumes
const float ZERO_VOLUME = 0.0;
const float FULL_VOLUME = 1.0;
const float HALF_VOLUME = 0.5;

// Priorities, currently ignored
const float LOW_PRIORITY = 0.0;
const float MED_PRIORITY = 0.5;
const float HIGH_PRIORITY = 1.0;

// Defines for all channels and looping mode
const int ALL_CHANNELS = -1;
const int LOOP_MODE = -1;

// Return values from status queries
const int STAT_PLAYING = 1;
const int STAT_NOT_OPEN = -1;
const int STAT_STOPPED = 0;

// Return values from file open & upload
const int OPEN_OK = 0;
const int OPEN_ERR = 1;
const int XFER_OK = 0;
const int XFER_ERR = 1;

// Return value for xfer request
const int XFRQ_OK = 1;
const int XFRQ_COMPLETE_OK = 1;

// Return values for other commands
const int CMD_OK = 0;
const int CMD_ERR = 1;

// Directory separator
#ifdef WIN32
 const char dir_separator[] = "\\";
#else // everything else is unix, right? ;)
 const char dir_separator[] = "/";
#endif

// Enumeration of server commands
typedef enum{CMD_INIT, CMD_QUIT, CMD_VOL, CMD_POS, CMD_BEGIN, CMD_REW,
	       CMD_STOP, CMD_CLOSE, CMD_OPEN, CMD_STATUS, CMD_QUERY_CAPS,
	       CMD_UPLOAD, CMD_SET_PRIORITY, CMD_SET_HEAD_POS,
	       CMD_SET_SUBDIR, CMD_SET_MODE, CMD_DIST, CMD_BAD} snd_command;

#ifdef ENABLE_SDL
// Server address may change, but the port is invariant. So Say Us.
const int server_port = 32212;

// Prototypes - user functions
int fl_send_command(const snd_channel snd, const snd_command cmd);
int fl_set_sound_mode(const speaker_mode mode);
int fl_set_subdirectory(const char dir_name[]);
void fl_ramp_volume(const int snd_id, const bool ramp_down,
		 const int num_steps, const int delay_time,
		 const float start_volume);
int fl_set_head_position(const float *);

/* --------------------------------------------------------------------
   From here on out, it's internal details that users should not need
   to know.


  Define the messages sent between client and server.
  
  All of these have associated paramters, so have the \n appended later
*/

const char INIT_CHAR = 'A'; // Open/startup
const char QUIT_CHAR = 'Z'; // Shutdown/close
const char PRI_CHAR = 'P'; // Priority(channel)
const char VOL_CHAR = 'V'; // Volume(channel)
const char POS_CHAR = 'L'; // Localize(channel,x,y,z,roll,pitch,yaw)
const char BEG_CHAR = 'B'; // Play(channel,loops)
const char REW_CHAR = 'R'; // Rewind(channel)
const char STP_CHAR = 'S'; // Stop(channel)
const char CLS_CHAR = 'C'; // Close(channel)
const char OPN_CHAR = 'O'; // Open(channel,filename)
const char ASK_CHAR = 'W'; // DeviceCapability?()
const char STA_CHAR = 'Q'; // Playing?(channel)
const char UPL_CHAR = 'T'; // Upload(filename)     to server
const char DWL_CHAR = 'D'; // Download(filename)   from server
const char SUB_CHAR = 'E'; // SetSubDirectory(path);
const char MOD_CHAR = 'M'; // SpeakerMode(mode);
const char USR_CHAR = 'U'; // Position User(x,y,z,roll,pitch,yaw)
const char DIST_CHAR = 'D'; // Decay distance (channel, distance in meters)

// These two are special - start and stop the server
const char INIT_MSG[] = { INIT_CHAR, '\n', (char) NULL };
const char QUIT_MSG[] = { QUIT_CHAR, '\n', (char) NULL };

const char PRI_MSG[] = { PRI_CHAR, (char) NULL };
const char VOL_MSG[] = { VOL_CHAR, (char) NULL };
const char POS_MSG[] = { POS_CHAR, (char) NULL };
const char BEG_MSG[] = { BEG_CHAR, (char) NULL };
const char REW_MSG[] = { REW_CHAR, (char) NULL };
const char STP_MSG[] = { STP_CHAR, (char) NULL };
const char CLS_MSG[] = { CLS_CHAR, (char) NULL };
const char OPN_MSG[] = { OPN_CHAR, (char) NULL };
const char ASK_MSG[] = { ASK_CHAR, (char) NULL };
const char STA_MSG[] = { STA_CHAR, (char) NULL };
const char UPL_MSG[] = { UPL_CHAR, (char) NULL };
const char DWL_MSG[] = { DWL_CHAR, (char) NULL };
const char SUB_MSG[] = { SUB_CHAR, (char) NULL };
const char MOD_MSG[] = { MOD_CHAR, (char) NULL };
const char USR_MSG[] = { USR_CHAR, (char) NULL };
const char DIST_MSG[] = { DIST_CHAR, (char) NULL };

// End-of-message delimiter
const char EOM_MSG[] = "\n";

// Strings denoting OK and failure on file open
const char OK_STR[] = "  1";
const char BAD_STR[] = " -1";

// Messages denoting channel status
const char ERR_MSG[] = " -1";
const char STOP_MSG[] = "  0";
const char PLAY_MSG[] = "  1";

// Length of standard reply from server
// Is normally " -1" or "  1", pretty simple to manage
const int REPLY_LENGTH = 3; 

// Prototypes - internal functions
void cleanup(void);
int send_message(const TCPsocket, const char*);
int get_reply(const TCPsocket sock);
int send_rcv_message(const TCPsocket sock, const char* send_msg);
int upload_wavfile(const TCPsocket sock, const snd_channel snd, char *ret_str);
TCPsocket startup_sdl_net(char *);
char *fn_plus_cwd(const char *filename);

#endif // ENABLE_SDL

#endif // SOUND_CLIENT_H

