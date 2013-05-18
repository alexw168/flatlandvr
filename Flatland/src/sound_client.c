/*
sound_client.c.  SDL sound client

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
 * Created: 11/11/99
 * Brought into new Flatland: 1/26/00, KLS
 * Changed warnings to not be as alarming (no, Flatland is not exiting): 1/16/03, KLS
 */
/*
  SDL sound client
  pfh/tbe 11/11/99

  Uses SDL sockets to connect to sound server and Make It Dance.

  Not too bad for a one-day hack! Works perfectly, sockets unidirectional for now
  but commands works, even linux->windows.

  Eventually this should work the server a bit more realisticlly - better positions,
  more sounds, etc, etc.

  11/12/99
  Notes of Note:
  -- Uses SDL for delays and sockets (SDL-demos/netlib to be exact)
  -- All messages are varible length, single upper-case command letter followed
     by (optional) parameters
  -- All messages end with the newline character, so this will break on an EBCDIC
     box. Not My Problem.
  -- Not designed for fastest operation - uses strings to send floats, etc. This
     gives us portability and readability, and is much easier.

     11/15/99
     Removed pan, pause, resume commands - Tim has A3D working, adn these are not
     required. Adding commands for priority, download and upload.

     11/16/99
     Added Query and Priority commands, increased reply length to three bytes, cleaning
     up the code to split demo from these functions, so that Mere Mortals may use
     our Ghodlike Functionality.

     Note: For our purposes, wolverine is the server. However, to use antoher box, just put
     it in the comand line, eg 'demo ac-2' and voila.

     12/8/99
     Adding code to do subdirectories, A3D mode. Also updated sound_client.h
     SDL released v1.0.0, our code seems fine with the new version.

     This file is now relegated to API status - users can call functions out of here
     when writing their code. demo1.cpp now does just that.

     2/25/00 - KLS
     Adding code to set decay distance

     4/10/00 - KLS
     Adding ifdefs for detection of the presence of SDL
 */

#include <config.h>

#ifdef ENABLE_SDL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <SDL/SDL.h>
#include "SDLnet.h"

#include "sound_client.h"

// ----------------------------------------------------------------------

// Global variables
// Working directory as set by the user
char *working_dir = NULL;

// indicates if same is valid/accessible
bool working_dir_ok = false;

// Socket to/from server
static TCPsocket tcpsock = NULL;

// ----------------------------------------------------------------------
// Cleanup at exit
void cleanup(void)
{
    /* Close the network connections */
    if ( tcpsock != NULL )
    {
        SDLNet_TCP_Close(tcpsock);
        tcpsock = NULL;
    }
}

// ----------------------------------------------------------------------
// Simple message sender with error handling - clean up main code
// Uses strlen for length, so only works with C-style strings!
//
// pfh 11/14 - changed to int return, added code to only send if none
// have failed. 
int send_message(const TCPsocket tcpsock, const char *msg)
{
    static bool got_error;
    
    int num_bytes = strlen(msg);

    // IF previous calls succeded, we have faith that we can prevail upon
    // the path of the Great American Way... oh wait, wrong rant.
    if(got_error == false)
    {
	if(SDLNet_TCP_Send(tcpsock, (char *) msg, num_bytes) < num_bytes)
	{
	    printf("\nError sending %d bytes to server!\n", num_bytes);
	    
	    // Set boolean - we be stuck
	    got_error = true;

	    // Everyone ignores the return value, but set it anyway
	    return(CMD_ERR);
	}
	else
	    return(CMD_OK);
    }
    else
    {
	// In a bad state - previous send failed, so bit-bucket all further
	// messages on the assumption that the server or net has gone PVS.
	return(CMD_ERR);
    }
}

// ----------------------------------------------------------------------
// Function to ramp a volume up or down for a given id
// Linear, should work for ALL_SOURCES as well as individual channels.
void fl_ramp_volume(const int snd_id, const bool ramp_down,
		 const int num_steps, const int delay_time,
		 const float start_volume)
{
    int i;
    snd_channel tmp_snd;
    
    for(i = 0; i < num_steps; i++)
    {
        tmp_snd.id = snd_id;

	if(ramp_down == true)
	    tmp_snd.volume = start_volume - ((float) i / (float) num_steps);
	else
	    tmp_snd.volume = start_volume + ((float) i / (float) num_steps);
	    
        fl_send_command(tmp_snd, CMD_VOL);
	
        SDL_Delay(delay_time);
    }

    return;
}
    
// ----------------------------------------------------------------------
// Another small-but-useful function, get N bytes, but be persistant
// about it. Which means the client hangs if the server freezes. Feature!
// pfh 11/15
// Modified 11/16 - now returns in that is status from server, assumes
// reply is REPLY_LENGTH long
int get_reply(const TCPsocket sock)
{
    int rcvd_count = 0;
    int itmp;
    char buffer[2 * REPLY_LENGTH] = "";

    // Note that we will wait forever as this is written now!!
    while(rcvd_count < REPLY_LENGTH)
    {
	// Try and get the whole message at once
	itmp = SDLNet_TCP_Recv(sock, &buffer[rcvd_count], REPLY_LENGTH);
	
	// Increment byte count that we've gotten
	rcvd_count += itmp;
    }

    // Zero-terminate the string so can use strcmp, etc
    buffer[REPLY_LENGTH] = 0x00;

    return(atoi(buffer));;
}

// ----------------------------------------------------------------------
// Send message, only with an immediate receive following it.
// Trying to do bi-dir comms for file open, etc
// int return is zero if both send and receive are OK.
// Received message returned through the rcv_msg pointer.
//
// Works, extending for file xfers, etc. pfh 11/15/99
// 11/16 - slight mod -- since get_reply now does atoi of server reply, this
// need slight changes to propagate same.
int send_rcv_message(const TCPsocket sock, const char* send_msg)
{
    int return_code;

    // Send outgoing
    return_code = send_message(sock, (char *) send_msg);

    if(return_code == 0) // Sent OK, now do (blocking!) receive
	return_code = get_reply(sock);

    return(return_code);
}

/* ----------------------------------------------------------------------

   Util routine that takes a filename, and returns a malloc'd buffer
   with the filename + working directory, if such is valid.

   Caller must free buffer!
   pfh 12/16/99

*/
char *fn_plus_cwd(const char *filename)
{
    char *retbuf = NULL;
    int buflen;
    
    buflen = strlen(filename);

    if(working_dir_ok == true)
	buflen += strlen(dir_separator) +
	    strlen(working_dir);
    
    retbuf = (char *) malloc(sizeof(char) * (buflen + 1));

    if(working_dir_ok)
	sprintf(retbuf, "%s%s%s",
		working_dir,
		dir_separator,
		filename);
    else
	strcpy(retbuf, filename);

    return(retbuf);
}
	

/* ----------------------------------------------------------------------
   First hack at a file uploader.
   pfh 11/15/99
   11/16 - it works, but it assumes two non-trivial things:
   1) Have enough memory to read the entire file into a malloc'd buffer
   2) No security worries - this could overwrite a file on the server if abused...

   Returns CMD_ERR if error, CMD_OK if OK.

   Revising with great caution, trying to add support for working
   directories.
   pfh 12/16/99
*/
int upload_wavfile(const TCPsocket sock, const snd_channel snd, char *ret_str)
{
    long filesize;
    FILE *fhandle = NULL;
    size_t block_size;
    char* xfer_buf = NULL;
    char send_str[BUFSIZ];
    int retval = CMD_OK;
    char *filename = NULL;

    // Build the complete filename
    filename = fn_plus_cwd(snd.filename);
	
    // Open binary, read-only
    fhandle = fopen(filename, "rb");

    // If we can't open it, bail right away
    if(fhandle == NULL)
    {
	fprintf(stderr, "\nSend-wav: Error opening file '%s'", filename);
	free(filename);
	return(CMD_ERR);
    }
    
    // Get filesize
    fseek(fhandle, 0L, SEEK_END);
    filesize = ftell(fhandle);
    fseek(fhandle, 0L, SEEK_SET);

    // Obscure, but it might happen
    if(filesize <= 0L)
    {
	fprintf(stderr, "\nSend-wav: Error getting filesize of '%s'",
		filename);
	
	fclose(fhandle);
	free(filename);
	return(CMD_ERR);
    }

    // Now that we know the filesize, allocate buffer to hold it
    xfer_buf = (char *) malloc(sizeof(char) * filesize);

    if(xfer_buf == NULL)
    {
	free(filename);
	fclose(fhandle);
	return(CMD_ERR);
    }
    
    // Build transfer request message
    // Note that this does not include the directory name!
    sprintf(send_str, "T %s %ld\n", snd.filename,
	    filesize);

    // Zap it off to server
    retval = send_rcv_message(tcpsock, send_str);

    // Server might deny transfer, obscure but could happen
    if(retval != XFRQ_OK)
    {
	fprintf(stderr, "\nSend-wav: Server rejected upload!");
	
	free(filename);
	fclose(fhandle);
	free(xfer_buf);
	return(CMD_ERR);
    }

    printf("\nSend-wav: Reading file into memory...");
    fflush(stdout);
    
    // Read the whole file into memory
    block_size = fread(xfer_buf, filesize, 1, fhandle);

    // Let's be careful here, people!
    if(block_size != 1)
    {
	fprintf(stderr, "\nSend-wav: Error reading file '%s' into memory!", filename);
	fclose(fhandle);
	free(filename);
	free(xfer_buf);
	return(CMD_ERR);
    }

    printf("\nSend-wav: Sending file to server...");
    fflush(stdout);
    
    // Zap it off!
    retval = SDLNet_TCP_Send(tcpsock, (char *) xfer_buf, filesize);

    // It's not paranoia if it happens!
    if(retval != filesize)
    {
	fprintf(stderr, "\nSend-wav: Error on TCP send of file to server!");
	fclose(fhandle);
	free(xfer_buf);
	free(filename);
	return(CMD_ERR);
    }

    // Done with input file
    fclose(fhandle);

    // Ditto with buffer
    free(xfer_buf);
    
    printf("\nFile sent; waiting for reply from server...");
    fflush(stdout);
    
    // Check return code from server
    retval = get_reply(sock);
    
    if(retval == XFRQ_COMPLETE_OK)
    {
	printf("OK.");
	return(CMD_OK);
    }
    else
    {
	printf("\nSend-wav: Transfer failed, return code '%d'", retval);
	return(CMD_ERR);
    }
}
/* ----------------------------------------------------------------------

   Another hack. Let the user set the head position via six floats,

   Just sticks them into a struct and calls the dispatcher.

   pfh 12/9/99
*/
int fl_set_head_position(const float *pos)
{
  snd_channel snd;
  int i;

  for(i = 0; i < 6; i++)
    snd.pos[i] = pos[i];

  return(fl_send_command(snd, CMD_SET_HEAD_POS));
}

/* ----------------------------------------------------------------------
  Hack - call to set the working subdir on the server. This is designed
  to let users load a wave file set into a directory, and then use it on
  subsequent runs as an organizational feature.
  
  Note that subsequent calls to this function overwrite the directory; ie
  we switch to the new directory. Going down more than one subdirectory could
  be done but is not implemented.
  
  In the dubious interests of reusing code, we stuff the dirname into a
  sound struct, and then call the normal dispatcher. Hack? Yeah, it is.
  pfh 12/99

  After discussions, have revised the concept a bit - the call also sets the
  _local_ subdirectory, so that users have organization on both ends.

  Mechanism is simple - once the dir is created/checked, subsequent calls 
  prepend the directory onto filenames for fopen. 
  pfh 12/16/99

  Returns CMD_OK if all went well, CMD_ERR if error
*/
int fl_set_subdirectory(const char dir_name[])
{
  struct stat statbuf;
  int retval;
  snd_channel snd;
  bool local_ok = false;
  bool remote_ok = false;

  if(dir_name == NULL)
      return(CMD_ERR);

  // If they make a null call, just return, assuming that they want the 
  // current working directory.
  if(strlen(dir_name) <= 0)
  {
      working_dir = NULL;
      working_dir_ok = true;
      return(CMD_OK);
  }
  
  // Save a local copy in the global ptr
  working_dir = (char *) malloc(sizeof(char) * strlen(dir_name));
  strcpy(working_dir, dir_name);

  // See if it exists already locally
  retval = stat(working_dir, &statbuf);

  // If we got a successful return value, check mode flags
  if(retval == 0)
  {
      if(S_ISDIR(statbuf.st_mode))
	  local_ok = true;
  }
  else // Error getting status - assume that it doesn't exist and create
  {
      // DDT 
      fprintf(stderr, "\nDirectory '%s' not found, creating.", working_dir);

      // Create the directory, rwx for user/group, rx for others
      retval = mkdir(working_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      
      if(retval == 0)
	  local_ok = true;
  }

  // Set and check the server's directory
  snd.id = ALL_CHANNELS;
  snd.filename = (char *) dir_name;
  retval = fl_send_command(snd, CMD_SET_SUBDIR);

  if(retval == CMD_OK)
      remote_ok = true;

  // Set the global status based on our results
  if((remote_ok == true) && (local_ok == true))
      working_dir_ok = true;
  else
      working_dir_ok = false;

  // DDT tell the user what up
  if(working_dir_ok != true)
      fprintf(stderr, "\nSet sound dir: Local OK is %d, remote OK is %d, wd_ok is %d\n",
	      local_ok, remote_ok, working_dir_ok);

  if(working_dir_ok)
      return(CMD_OK);
  else
      return(CMD_ERR);

}

/* ----------------------------------------------------------------------
   Yet Another Hack.
   This lets the users set the mode (ie HRTF set) of A3D, depending on
   what playback equipment is in use. Headphone HRTFs over speakers sound
   awful, for example.
   
   The hack comes in from using the fl_send_command function.
   
   pfh 12/99
*/
int fl_set_sound_mode(const speaker_mode mode)
{
  snd_channel snd;

  snd.id = mode;

  return(fl_send_command(snd, CMD_SET_MODE));
}

/* ----------------------------------------------------------------------
   Open the network, get a new socket from the server.

   Allocates a socket, sets up cleanup at program exit, etc.

   Returns the socket used to talk to the server.

   Revised 12/15/99 to return void instead of calling exit, more suitable 
   for multithreaded setup.

*/
TCPsocket startup_sdl_net(char *server_str)
{
    IPaddress serverIP;
    char *server;
    
    // Start up SDL
    if ( SDL_Init(SDL_INIT_TIMER) < 0 )
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n",
                SDL_GetError());
        return(NULL);
    }
    
    // Register exit handler
    atexit(SDL_Quit);
    
    /* Initialize the network */
    if ( SDLNet_Init() < 0 )
    {
        fprintf(stderr, "Couldn't initialize net: %s\n",
                SDLNet_GetError());
        return(NULL);
    }
    
    // Register another exit handler...lantinga loves these even more than I do!
    atexit(SDLNet_Quit);
    
    /* Get ready to initialize all of our data ... by yet another exit function*/
    atexit(cleanup);

    /* This is old-style, used for determining server name when it was standalone.
       Need to modify the server ptr to change which machine to contact.

    if(argc > 1)
        server = argv[1];
    else
        server = (char *) server_str;

*/
    // Point to static global w/server name
    server = (char *) server_str;

    printf("\nConnecting to %s ... ", server);
    fflush(stdout);

    // Resolve IP and fill out serverIP structure
    SDLNet_ResolveHost(&serverIP, server, server_port);
    
    if ( serverIP.host == INADDR_NONE )
    {
        printf("\nCouldn't resolve hostname!\n");
        return(NULL);
    } else
    {
        /* Attempt to connect to IP/socket pair */
        tcpsock = SDLNet_TCP_Open(&serverIP);
        
        if ( tcpsock == NULL )
        {
            printf("\nConnect failed, sound thread exiting.\n\n");
            return(NULL);
        } else
        {
            printf("Connected\n");
        }
    }

    fflush(stdout);
    
    return(tcpsock);
}

/* ----------------------------------------------------------------------
   User-friendly function to drive the remote server based on the
   info in the sound structure and a given command.

   This is intended to be the main user interface, with the exception of
   the utility functions above.
   pfh
   
   Return codes:
   CMD_OK (0) all OK
   CMD_ERR (1) file not found on server
   2 network error, or command not found
   Some commands have different return codes, need to fix this!!!
   For example, CMD_QUERY_CAPS returns the number of channels on the server...
*/
int fl_send_command(const snd_channel snd, const snd_command cmd)
{
    char dest_str[512]; // Space for the message
    char tmp_str[512];  // used for the position message builder
    int  i;

    // No socket, no comms, go away and don't return without
    // A SHRUBBERY! A nice one, too.
    if(tcpsock == NULL)
        return(2);

    switch(cmd)
    {
    case CMD_INIT: // Open the audio system
      strcpy(dest_str, INIT_MSG);
      break;

    case CMD_QUIT:// Close system
      strcpy(dest_str, QUIT_MSG);
      break;


    case CMD_VOL: // Volume
        sprintf(dest_str, "V %5d %f\n",
                snd.id,
                snd.volume);
        break;

    case CMD_POS: // Position
        sprintf(dest_str, "L %5d ",
                snd.id);

        for(i = 0; i < 6; i++)
        {
            sprintf(tmp_str, " %8.4f", snd.pos[i]/SOUND_DISTANCE_ATTENUATION);
            strcat(dest_str, tmp_str);
        }

        // Append EOM character
        strcat(dest_str, EOM_MSG);
        break;

    case CMD_CLOSE: // close file
        sprintf(dest_str, "C %5d\n", snd.id);
        break;
        
    case CMD_STOP: // stop a source
        sprintf(dest_str, "S %5d\n", snd.id);
        break;

    case CMD_REW: // rewind
        sprintf(dest_str, "R %5d\n", snd.id);
        break;
            
    case CMD_BEGIN: // start source(s) playing
        sprintf(dest_str, "B %5d %d\n",
                snd.id,
                snd.num_loops);
        break;

    case CMD_UPLOAD: // Send file to server
	printf("\nRequesting file transfer of '%s'...", snd.filename);
	fflush(stdout);

	// Note that we return directly from here!
	return(upload_wavfile(tcpsock, snd, tmp_str));
	
    case CMD_OPEN: // Open wav file
        sprintf(dest_str, "O %5d %s\n",
                snd.id,
                snd.filename);

	// Send message, wait for reply
	if(send_rcv_message(tcpsock, dest_str) == 1)
	    return(CMD_OK);
	else
	    return(CMD_ERR);

    case CMD_STATUS: // Request the status of a channel
	sprintf(dest_str, "Q %5d\n", snd.id);

	// Returns the status as told by the server!
	return(send_rcv_message(tcpsock, dest_str));

    case CMD_QUERY_CAPS: // Request server capabilities - how many channels possible?
	sprintf(dest_str, "W\n");

	// Direct return again
	return(send_rcv_message(tcpsock, dest_str));

    case CMD_SET_PRIORITY: // Set the priority of a channel, used by A3D only!
	sprintf(dest_str, "P %5d %f\n", snd.id, snd.priority);
	break;

    case CMD_SET_HEAD_POS: // Update head position
	sprintf(dest_str, "U");

	// In this case, the position floats are interpreted as a head position!
        for(i = 0; i < 6; i++)
        {
            sprintf(tmp_str, " %8.4f", snd.pos[i]);
            strcat(dest_str, tmp_str);
        }

        // Append EOM character
        strcat(dest_str, EOM_MSG);
        break;

    case CMD_SET_SUBDIR: // Change subdirectory where server looks for files
      // Reuses filename field in struct
      sprintf(dest_str, "E%s\n", snd.filename);
      break;

    case CMD_SET_MODE: // CHange which HRTF set A3D uses
      // Reuses ID field in struct for mode
      sprintf(dest_str, "M %d\n", snd.id);
      break;

    case CMD_DIST: // Change default sound decay distance
      sprintf(dest_str, "D %5d %f\n",
              snd.id,
              snd.distance);
      break;

    case CMD_BAD:
    default:
        printf("\nUnknown or unimplemented command!");
        return(2);
    }

    // Off you go, then!
    return(send_message(tcpsock, dest_str));
}

#endif // ENABLE_SDL
