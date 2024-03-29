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

#ifndef _SDLnet_h
#define _SDLnet_h

#include "SDL/SDL.h"
#include "SDL/SDL_endian.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Initialize/Cleanup the network API
   SDL must be initialized before calls to functions in this library,
   because this library uses utility functions from the SDL library.
*/
extern int  SDLNet_Init(void);
extern void SDLNet_Quit(void);

/***********************************************************************/
/* IPv4 hostname resolution API                                        */
/***********************************************************************/

typedef struct {
	Uint32 host;			/* 32-bit IPv4 host address */
	Uint16 port;			/* 16-bit protocol port */
} IPaddress;

/* Resolve a host name and port to an IP address in network form.
   If the host couldn't be resolved, the host portion of the returned
   address will be INADDR_NONE.
 */
#ifndef INADDR_NONE
#define INADDR_NONE	0xFFFFFFFF
#endif
extern int SDLNet_ResolveHost(IPaddress *address, char *host, Uint16 port);

/* Resolve an ip address to a host name in canonical form.
   If the ip couldn't be resolved, this function returns NULL,
   otherwise a pointer to a static buffer containing the hostname
   is returned.  Note that this function is not thread-safe.
*/
extern char *SDLNet_ResolveIP(IPaddress *ip);


/***********************************************************************/
/* TCP network API                                                     */
/***********************************************************************/

typedef struct _TCPsocket *TCPsocket;

/* Open a TCP network socket
   If ip.host is INADDR_NONE, this creates a local server socket on the 
   given port, otherwise a TCP connection to the remote host and port is
   attempted.
   The newly created socket is returned, or NULL if there was an error.
*/
extern TCPsocket SDLNet_TCP_Open(IPaddress *ip);

/* Accept an incoming connection on the given server socket.
   The newly created socket is returned, or NULL if there was an error.
*/
extern TCPsocket SDLNet_TCP_Accept(TCPsocket server);

/* Get the IP address of the remote system associated with the socket.
   If the socket is a server socket, this function returns NULL.
*/
extern IPaddress *SDLNet_TCP_GetPeerAddress(TCPsocket sock);

/* Send 'len' bytes of 'data' over the non-server socket 'sock'
   This function returns the actual amount of data sent.  If the return value
   is less than the amount of data sent, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
extern int SDLNet_TCP_Send(TCPsocket sock, char *data, int len);

/* Receive up to 'maxlen' bytes of data over the non-server socket 'sock',
   and store them in the buffer pointed to by 'data'.
   This function returns the actual amount of data received.  If the return
   value is less than or equal to zero, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
extern int SDLNet_TCP_Recv(TCPsocket sock, char *data, int maxlen);

/* Close a TCP network socket */
extern void SDLNet_TCP_Close(TCPsocket sock);


/***********************************************************************/
/* UDP network API                                                     */
/***********************************************************************/

/* The maximum channels on a a UDP socket */
#define SDLNET_MAX_UDPCHANNELS	32
/* The maximum addresses bound to a single UDP socket channel */
#define SDLNET_MAX_UDPADDRESSES	4

typedef struct _UDPsocket *UDPsocket;
typedef struct {
	int channel;		/* The src/dst channel of the packet */
	char *data;		/* The packet data */
	int len;		/* The length of the packet data */
	int maxlen;		/* The size of the data buffer */
	int status;		/* packet status after sending */
	IPaddress src;		/* The source address of an incoming packet */
} UDPpacket;

/* Allocate/resize/free a single UDP packet 'size' bytes long.
   The new packet is returned, or NULL if the function ran out of memory.
 */
extern UDPpacket *SDLNet_AllocPacket(int size);
extern int SDLNet_ResizePacket(UDPpacket *packet, int newsize);
extern void SDLNet_FreePacket(UDPpacket *packet);

/* Allocate/Free a UDP packet vector (array of packets) of 'howmany' packets,
   each 'size' bytes long.
   A pointer to the first packet in the array is returned, or NULL if the
   function ran out of memory.
 */
extern UDPpacket **SDLNet_AllocPacketV(int howmany, int size);
extern void SDLNet_FreePacketV(UDPpacket **packetV);


/* Open a UDP network socket
   If 'port' is non-zero, the UDP socket is bound to a local port.
   This allows other systems to send to this socket via a known port.
*/
extern UDPsocket SDLNet_UDP_Open(Uint16 port);

/* Bind the address 'address' to the requested channel on the UDP socket.
   If the channel is -1, then the first unbound channel will be bound with
   the given address as it's primary address.
   If the channel is already bound, this new address will be added to the
   list of valid source addresses for packets arriving on the channel.
   If the channel is not already bound, then the address becomes the primary
   address, to which all outbound packets on the channel are sent.
   This function returns the channel which was bound, or -1 on error.
*/
extern int SDLNet_UDP_Bind(UDPsocket sock, int channel, IPaddress *address);

/* Unbind all addresses from the given channel */
extern void SDLNet_UDP_Unbind(UDPsocket sock, int channel);

/* Get the primary IP address of the remote system associated with the 
   socket and channel.  If the channel is -1, then the primary IP port
   of the UDP socket is returned -- this is only meaningful for sockets
   opened with a specific port.
   If the channel is not bound and not -1, this function returns NULL.
 */
extern IPaddress *SDLNet_UDP_GetPeerAddress(UDPsocket sock, int channel);

/* Send a vector of packets to the the channels specified within the packet.
   Each packet will be updated with the status of the packet after it has 
   been sent, -1 if the packet send failed.
   This function returns the number of packets sent.
*/
extern int SDLNet_UDP_SendV(UDPsocket sock, UDPpacket **packets, int npackets);

/* Send a single packet to the specified channel.
   The packet will be updated with the status of the packet after it has
   been sent.
   This function returns 1 if the packet was sent, or 0 on error.
*/
extern int SDLNet_UDP_Send(UDPsocket sock, int channel, UDPpacket *packet);

/* Receive a vector of pending packets from the UDP socket.
   The returned packets contain the source address and the channel they arrived
   on.  If they did not arrive on a bound channel, the the channel will be set
   to -1.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
extern int SDLNet_UDP_RecvV(UDPsocket sock, UDPpacket **packets);

/* Receive a single packet from the UDP socket.
   The returned packet contains the source address and the channel it arrived
   on.  If it did not arrive on a bound channel, the the channel will be set
   to -1.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
extern int SDLNet_UDP_Recv(UDPsocket sock, UDPpacket *packet);

/* Close a UDP network socket */
extern void SDLNet_UDP_Close(UDPsocket sock);


/***********************************************************************/
/* Hooks for checking sockets for available data                       */
/***********************************************************************/

typedef struct _SDLNet_SocketSet *SDLNet_SocketSet;

/* Any network socket can be safely cast to this socket type */
typedef struct sdlnet_genericsocket {
	int ready;
} *SDLNet_GenericSocket;

/* Allocate a socket set for use with SDLNet_CheckSockets()
   This returns a socket set for up to 'maxsockets' sockets, or NULL if
   the function ran out of memory.
 */
extern SDLNet_SocketSet SDLNet_AllocSocketSet(int maxsockets);

/* Add a socket to a set of sockets to be checked for available data */
#define SDLNet_TCP_AddSocket(set, sock) \
			SDLNet_AddSocket(set, (SDLNet_GenericSocket)sock)
#define SDLNet_UDP_AddSocket(set, sock) \
			SDLNet_AddSocket(set, (SDLNet_GenericSocket)sock)
extern int SDLNet_AddSocket(SDLNet_SocketSet set, SDLNet_GenericSocket sock);

/* Remove a socket from a set of sockets to be checked for available data */
#define SDLNet_TCP_DelSocket(set, sock) \
			SDLNet_DelSocket(set, (SDLNet_GenericSocket)sock)
#define SDLNet_UDP_DelSocket(set, sock) \
			SDLNet_DelSocket(set, (SDLNet_GenericSocket)sock)
extern int SDLNet_DelSocket(SDLNet_SocketSet set, SDLNet_GenericSocket sock);

/* This function checks to see if data is available for reading on the
   given set of sockets.  If 'timeout' is 0, it performs a quick poll,
   otherwise the function returns when either data is available for
   reading, or the timeout in milliseconds has elapsed, which ever occurs
   first.  This function returns the number of sockets ready for reading, 
   or -1 if there was an error with the select() system call.
*/
extern int SDLNet_CheckSockets(SDLNet_SocketSet set, Uint32 timeout);

/* After calling SDLNet_CheckSockets(), you can use this function on a
   socket that was in the socket set, to find out if data is available
   for reading.
*/
#define SDLNet_SocketReady(sock) \
		((sock != NULL) && ((SDLNet_GenericSocket)sock)->ready)

/* Free a set of sockets allocated by SDL_NetAllocSocketSet() */
extern void SDLNet_FreeSocketSet(SDLNet_SocketSet set);


/***********************************************************************/
/* Platform-independent data conversion functions                      */
/***********************************************************************/

/* Write a 16/32 bit value to network packet buffer */
extern void SDLNet_Write16(Uint16 value, Uint8 *area);
extern void SDLNet_Write32(Uint32 value, Uint8 *area);

/* Read a 16/32 bit value from network packet buffer */
extern Uint16 SDLNet_Read16(Uint8 *area);
extern Uint32 SDLNet_Read32(Uint8 *area);

/***********************************************************************/
/* Error reporting functions                                           */
/***********************************************************************/

/* We'll use SDL's functions for error reporting */
#define SDLNet_SetError	SDL_SetError
#define SDLNet_GetError	SDL_GetError


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif
#endif /* _SDLnet_h */
