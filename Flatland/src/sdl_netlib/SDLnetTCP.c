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

/* The network API for TCP sockets */

struct _TCPsocket {
	int ready;
	SOCKET channel;
	IPaddress address;
	int sflag;
};

/* Open a TCP network socket
   If 'remote' is NULL, this creates a local server socket on the given port,
   otherwise a TCP connection to the remote host and port is attempted.
   The newly created socket is returned, or NULL if there was an error.
*/
TCPsocket SDLNet_TCP_Open(IPaddress *ip)
{
	TCPsocket sock;
	struct sockaddr_in sock_addr;

	/* Allocate a TCP socket structure */
	sock = (TCPsocket)malloc(sizeof(*sock));
	if ( sock == NULL ) {
		SDLNet_SetError("Out of memory");
		goto error_return;
	}

	/* Open the socket */
	sock->channel = socket(AF_INET, SOCK_STREAM, 0);
	if ( sock->channel == INVALID_SOCKET ) {
		SDLNet_SetError("Couldn't create socket");
		goto error_return;
	}

	/* Connect to remote, or bind locally, as appropriate */
	if ( ip->host != INADDR_NONE ) {
		memset(&sock_addr, 0, sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = ip->host;
		sock_addr.sin_port = ip->port;

		/* Connect to the remote host */
		if ( connect(sock->channel, (struct sockaddr *)&sock_addr,
				sizeof(sock_addr)) == SOCKET_ERROR ) {
			SDLNet_SetError("Couldn't connect to remote host");
			goto error_return;
		}
		sock->sflag = 0;
	} else {
		memset(&sock_addr, 0, sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = INADDR_ANY;
		sock_addr.sin_port = ip->port;

		/* Bind the socket for listening */
		if ( bind(sock->channel, (struct sockaddr *)&sock_addr,
				sizeof(sock_addr)) == SOCKET_ERROR ) {
			SDLNet_SetError("Couldn't bind to local port");
			goto error_return;
		}
		if ( listen(sock->channel, 5) == SOCKET_ERROR ) {
			SDLNet_SetError("Couldn't listen to local port");
			goto error_return;
		}
		sock->sflag = 1;
	}
	sock->ready = 0;

	/* Fill in the channel host address */
	sock->address.host = sock_addr.sin_addr.s_addr;
	sock->address.port = sock_addr.sin_port;

	/* The socket is ready */
	return(sock);

error_return:
	SDLNet_TCP_Close(sock);
	return(NULL);
}

/* Accept an incoming connection on the given server socket.
   The newly created socket is returned, or NULL if there was an error.
*/
TCPsocket SDLNet_TCP_Accept(TCPsocket server)
{
	TCPsocket sock;
	struct sockaddr_in sock_addr;
	int sock_alen;

	/* Only server sockets can accept */
	if ( ! server->sflag ) {
		SDLNet_SetError("Only server sockets can accept()");
		return(NULL);
	}
	server->ready = 0;

	/* Allocate a TCP socket structure */
	sock = (TCPsocket)malloc(sizeof(*sock));
	if ( sock == NULL ) {
		SDLNet_SetError("Out of memory");
		goto error_return;
	}

	/* Accept a new TCP connection on a server socket */
	sock_alen = sizeof(sock_addr);
	//sock->channel = accept(server->channel, (struct sockaddr *)&sock_addr,
	//					(socklen_t *) &sock_alen);
	sock->channel = accept(server->channel, (struct sockaddr *)&sock_addr,
						 &sock_alen);
	if ( sock->channel == SOCKET_ERROR ) {
		SDLNet_SetError("accept() failed");
		goto error_return;
	}
	sock->sflag = 0;
	sock->ready = 0;

	/* Fill in the channel host address */
	sock->address.host = sock_addr.sin_addr.s_addr;
	sock->address.port = sock_addr.sin_port;

	/* The socket is ready */
	return(sock);

error_return:
	SDLNet_TCP_Close(sock);
	return(NULL);
}

/* Get the IP address of the remote system associated with the socket.
   If the socket is a server socket, this function returns NULL.
*/
IPaddress *SDLNet_TCP_GetPeerAddress(TCPsocket sock)
{
	if ( sock->sflag ) {
		return(NULL);
	}
	return(&sock->address);
}

/* Send 'len' bytes of 'data' over the non-server socket 'sock'
   This function returns the actual amount of data sent.  If the return value
   is less than the amount of data sent, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
int SDLNet_TCP_Send(TCPsocket sock, char *data, int len)
{
	int sent, left;

	/* Server sockets are for accepting connections only */
	if ( sock->sflag ) {
		SDLNet_SetError("Server sockets cannot send");
		return(-1);
	}

	/* Keep sending data until it's sent or an error occurs */
	left = len;
	sent = 0;
	errno = 0;
	do {
		len = send(sock->channel, data, left, 0);
		if ( len > 0 ) {
			sent += len;
			left -= len;
			data += len;
		}
	} while ( (left > 0) && ((len > 0) || (errno == EINTR)) );

	return(sent);
}

/* Receive up to 'maxlen' bytes of data over the non-server socket 'sock',
   and store them in the buffer pointed to by 'data'.
   This function returns the actual amount of data received.  If the return
   value is less than or equal to zero, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
int SDLNet_TCP_Recv(TCPsocket sock, char *data, int maxlen)
{
	int len;

	/* Server sockets are for accepting connections only */
	if ( sock->sflag ) {
		SDLNet_SetError("Server sockets cannot send");
		return(-1);
	}

	errno = 0;
	do {
		len = recv(sock->channel, data, maxlen, 0);
	} while ( errno == EINTR );

	sock->ready = 0;
	return(len);
}

/* Close a TCP network socket */
void SDLNet_TCP_Close(TCPsocket sock)
{
	if ( sock != NULL ) {
		if ( sock->channel != INVALID_SOCKET ) {
			closesocket(sock->channel);
		}
		free(sock);
	}
}
