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


struct UDP_channel {
  int numbound;
  IPaddress address[SDLNET_MAX_UDPADDRESSES];
};

struct _UDPsocket {
	int ready;
	SOCKET channel;
	IPaddress address;
	int lastchannel;
	struct UDP_channel binding[SDLNET_MAX_UDPCHANNELS];
};


/* Allocate/free a single UDP packet 'size' bytes long.
   The new packet is returned, or NULL if the function ran out of memory.
 */
extern UDPpacket *SDLNet_AllocPacket(int size)
{
	UDPpacket *packet;
	int error;


	error = 1;
	packet = (UDPpacket *)malloc(sizeof(*packet));
	if ( packet != NULL ) {
		packet->maxlen = size;
		packet->data = (char *) malloc(size);
		if ( packet->data != NULL ) {
			error = 0;
		}
	}
	if ( error ) {
		SDLNet_FreePacket(packet);
		packet = NULL;
	}
	return(packet);
}
int SDLNet_ResizePacket(UDPpacket *packet, int newsize)
{
	char *newdata;

	newdata = (char *)malloc(newsize);
	if ( newdata != NULL ) {
		free(packet->data);
		packet->data = newdata;
		packet->maxlen = newsize;
	}
	return(packet->maxlen);
}
extern void SDLNet_FreePacket(UDPpacket *packet)
{
	if ( packet ) {
		if ( packet->data )
			free(packet->data);
		free(packet);
	}
}

/* Allocate/Free a UDP packet vector (array of packets) of 'howmany' packets,
   each 'size' bytes long.
   A pointer to the packet array is returned, or NULL if the function ran out
   of memory.
 */
UDPpacket **SDLNet_AllocPacketV(int howmany, int size)
{
	UDPpacket **packetV;

	packetV = (UDPpacket **)malloc((howmany+1)*sizeof(*packetV));
	if ( packetV != NULL ) {
		int i;
		for ( i=0; i<howmany; ++i ) {
			packetV[i] = SDLNet_AllocPacket(size);
			if ( packetV[i] == NULL ) {
				break;
			}
		}
		packetV[i] = NULL;

		if ( i != howmany ) {
			SDLNet_FreePacketV(packetV);
			packetV = NULL;
		}
	}
	return(packetV);
}
void SDLNet_FreePacketV(UDPpacket **packetV)
{
	if ( packetV ) {
		int i;
		for ( i=0; packetV[i]; ++i ) {
			SDLNet_FreePacket(packetV[i]);
		}
		free(packetV);
	}
}

/* Open a UDP network socket
   If 'port' is non-zero, the UDP socket is bound to a fixed local port.
*/
extern UDPsocket SDLNet_UDP_Open(Uint16 port)
{
	UDPsocket sock;

	/* Allocate a UDP socket structure */
	sock = (UDPsocket)malloc(sizeof(*sock));
	if ( sock == NULL ) {
		SDLNet_SetError("Out of memory");
		goto error_return;
	}
	memset(sock, 0, sizeof(*sock));

	/* Open the socket */
	sock->channel = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock->channel == INVALID_SOCKET ) {
		SDLNet_SetError("Couldn't create socket");
		goto error_return;
	}

	/* Bind locally, if appropriate */
	if ( port ) {
		struct sockaddr_in sock_addr;
		memset(&sock_addr, 0, sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = INADDR_ANY;
		sock_addr.sin_port = SDL_SwapBE16(port);

		/* Bind the socket for listening */
		if ( bind(sock->channel, (struct sockaddr *)&sock_addr,
				sizeof(sock_addr)) == SOCKET_ERROR ) {
			SDLNet_SetError("Couldn't bind to local port");
			goto error_return;
		}
		/* Fill in the channel host address */
		sock->address.host = sock_addr.sin_addr.s_addr;
		sock->address.port = sock_addr.sin_port;
	}

	/* The socket is ready */
	return(sock);

error_return:
	SDLNet_UDP_Close(sock);
	return(NULL);
}

/* Verify that the channel is in the valid range */
static int ValidChannel(int channel)
{
	if ( (channel < 0) || (channel >= SDLNET_MAX_UDPCHANNELS) ) {
		SDLNet_SetError("Invalid channel");
		return(0);
	}
	return(1);
}

/* Bind the address 'address' to the requested channel on the UDP socket.
   If the channel is -1, then the first unbound channel will be bound with
   the given address as it's primary address.
   If the channel is already bound, this new address will be added to the
   list of valid source addresses for packets arriving on the channel.
   If the channel is not already bound, then the address becomes the primary
   address, to which all outbound packets on the channel are sent.
   This function returns the channel which was bound, or -1 on error.
*/
int SDLNet_UDP_Bind(UDPsocket sock, int channel, IPaddress *address)
{
	struct UDP_channel *binding;

	if ( channel == -1 ) 
	{
	    for ( channel=0; channel < SDLNET_MAX_UDPCHANNELS; ++channel ) 
	    {
		binding = (struct UDP_channel *) &sock->binding[channel];

		if ( binding->numbound < SDLNET_MAX_UDPADDRESSES ) 
		{
		    break;
		}
	    }
	} 
	else 
	{
		if ( ! ValidChannel(channel) ) 
		{
		    return(-1);
		}
		binding = (struct UDP_channel *) &sock->binding[channel];
	}
	if ( binding->numbound == SDLNET_MAX_UDPCHANNELS ) 
	{
	    SDLNet_SetError("No room for new addresses");
	    return(-1);
	}
	binding->address[binding->numbound++] = *address;
	return(channel);
}

/* Unbind all addresses from the given channel */
void SDLNet_UDP_Unbind(UDPsocket sock, int channel)
{
	if ( (channel >= 0) && (channel < SDLNET_MAX_UDPCHANNELS) ) {
		sock->binding[channel].numbound = 0;
	}
}

/* Get the primary IP address of the remote system associated with the
   socket and channel.
   If the channel is not bound, this function returns NULL.
 */
IPaddress *SDLNet_UDP_GetPeerAddress(UDPsocket sock, int channel)
{
	IPaddress *address;

	address = NULL;
	switch (channel) {
		case -1:
			/* Return the actual address of the socket */
			address = &sock->address;
			break;
		default:
			/* Return the address of the bound channel */
			if ( ValidChannel(channel) &&
				(sock->binding[channel].numbound > 0) ) {
				address = &sock->binding[channel].address[0];
			}
			break;
	}
	return(address);
}

/* Send a vector of packets to the addresses bound to the channels specified
   within the packet.  Each packet will be updated with the status of the
   packet after it has been sent, -1 if the packet send failed.
   This function returns the number of packets sent.
*/
int SDLNet_UDP_SendV(UDPsocket sock, UDPpacket **packets, int npackets)
{
	int numsent, i, j;
	struct UDP_channel *binding;
	int sock_len, status;
	struct sockaddr_in sock_addr;

	/* Set up the variables to send packets */
	sock_len = sizeof(sock_addr);

	numsent = 0;
	for ( i=0; i<npackets; ++i ) {
		binding = &sock->binding[packets[i]->channel];
		/* Send to each of the bound addresses on the channel */
		for ( j=binding->numbound-1; j>=0; --j ) {
			sock_addr.sin_addr.s_addr = binding->address[j].host;
			sock_addr.sin_port = binding->address[j].port;
			sock_addr.sin_family = AF_INET;
			status = sendto(sock->channel, 
					packets[i]->data, packets[i]->len, 0,
					(struct sockaddr *)&sock_addr,sock_len);
			if ( status >= 0 ) {
				packets[i]->status = status;
				++numsent;
			}
		}
	}
	return(numsent);
}

int SDLNet_UDP_Send(UDPsocket sock, int channel, UDPpacket *packet)
{
	/* This is silly, but... */
	packet->channel = channel;
	return(SDLNet_UDP_SendV(sock, &packet, 1));
}

/* Returns true if a socket is has data available for reading right now */
static int SocketReady(SOCKET sock)
{
	int retval;
	struct timeval tv;
	fd_set mask;

	/* Check the file descriptors for available data */
	do {
		errno = 0;

		/* Set up the mask of file descriptors */
		FD_ZERO(&mask);
		FD_SET(sock, &mask);

		/* Set up the timeout */
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		/* Look! */
		retval = select(sock+1, &mask, NULL, NULL, &tv);
	} while ( errno == EINTR );

	return(retval == 1);
}

/* Receive a vector of pending packets from the UDP socket.
   The returned packets contain the source address and the channel they arrived
   on.  If they did not arrive on a bound channel, the the channel will be set
   to -1.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
extern int SDLNet_UDP_RecvV(UDPsocket sock, UDPpacket **packets)
{
	int numrecv, i, j;
	struct UDP_channel *binding;
	int sock_len;
	struct sockaddr_in sock_addr;

	numrecv = 0;
	while ( packets[numrecv] && SocketReady(sock->channel) ) {
		UDPpacket *packet;

		packet = packets[numrecv];
		sock_len = sizeof(sock_addr);
		//packet->status = recvfrom(sock->channel,
		//		packet->data, packet->maxlen, 0,
		//		(struct sockaddr *)&sock_addr, 
                //                (socklen_t *) &sock_len);
		packet->status = recvfrom(sock->channel,
				packet->data, packet->maxlen, 0,
				(struct sockaddr *)&sock_addr, &sock_len);
		if ( packet->status >= 0 ) {
			packet->len = packet->status;
			packet->src.host = sock_addr.sin_addr.s_addr;
			packet->src.port = sock_addr.sin_port;
			packet->channel = -1;
			for ( i=0; i<SDLNET_MAX_UDPCHANNELS; ++i ) {
				binding = &sock->binding[i];
				for ( j=binding->numbound-1; j>=0; --j ) {
					if ( (packet->src.host ==
					          binding->address[j].host) &&
					     (packet->src.port ==
					          binding->address[j].port) ) {
						packet->channel = i;
						break;
					}
				}
			}
			++numrecv;
		} else {
			packet->len = 0;
		}
	}
	sock->ready = 0;
	return(numrecv);
}

/* Receive a single packet from the UDP socket.
   The returned packet contains the source address and the channel it arrived
   on.  If it did not arrive on a bound channel, the the channel will be set
   to -1.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
int SDLNet_UDP_Recv(UDPsocket sock, UDPpacket *packet)
{
	UDPpacket *packets[2];

	/* Receive a packet array of 1 */
	packets[0] = packet;
	packets[1] = NULL;
	return(SDLNet_UDP_RecvV(sock, packets));
}

/* Close a UDP network socket */
extern void SDLNet_UDP_Close(UDPsocket sock)
{
	if ( sock != NULL ) {
		if ( sock->channel != INVALID_SOCKET ) {
			closesocket(sock->channel);
		}
		free(sock);
	}
}
