/*
    NETLIB:  An example cross-platform network library for use with SDL
    Copyright (C) 1997-1999  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

#include "SDLnetsys.h"
#include "SDLnet.h"


/* Initialize/Cleanup the network API */
int  SDLNet_Init(void)
{
#ifdef Win32_Winsock
	/* Start up the windows networking */
	WORD version_wanted = MAKEWORD(1,1);
	WSADATA wsaData;

	if ( WSAStartup(version_wanted, &wsaData) != 0 ) {
		SDLNet_SetError("Couldn't initialize Winsock 1.1\n");
		return(-1);
	}
#endif
	return(0);
}
void SDLNet_Quit(void)
{
#ifdef Win32_Winsock
	/* Clean up windows networking */
	if ( WSACleanup() == SOCKET_ERROR ) {
		if ( WSAGetLastError() == WSAEINPROGRESS ) {
			WSACancelBlockingCall();
			WSACleanup();
		}
	}
#endif
}

/* Resolve a host name and port to an IP address in network form */
int SDLNet_ResolveHost(IPaddress *address, char *host, Uint16 port)
{
	int resolved;

	/* Start off with resolved set to 'true' */
	resolved = 1;

	/* Perform the actual host resolution */
	if ( host == NULL ) {
		address->host = INADDR_NONE;
	} else {
		address->host = inet_addr(host);
		if ( address->host == INADDR_NONE ) {
			struct hostent *hp;

			hp = gethostbyname(host);
			if ( hp ) {
				memcpy(&address->host,hp->h_addr,hp->h_length);
			} else {
				resolved = 0;
			}
		}
	}
	address->port = SDL_SwapBE16(port);

	/* Return the status */
	return(resolved);
}

/* Write a 16/32 bit value to network packet buffer */
void SDLNet_Write16(Uint16 value, Uint8 *area)
{
	area[0] = (value>>8)&0xFF;
	area[1] = value&0xFF;
}
void SDLNet_Write32(Uint32 value, Uint8 *area)
{
	area[0] = (value>>24)&0xFF;
	area[1] = (value>>16)&0xFF;
	area[2] = (value>>8)&0xFF;
	area[3] = value&0xFF;
}

/* Read a 16/32 bit value from network packet buffer */
Uint16 SDLNet_Read16(Uint8 *area)
{
	return((area[0]<<8)|area[1]);
}
Uint32 SDLNet_Read32(Uint8 *area)
{
	return((area[0]<<24)|(area[1]<<16)|(area[2]<<8)|area[3]);
}
