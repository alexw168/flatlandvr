//
// 
// Bird.cc - Talk to the flock-of-birds
//
/*

Copyright (C) 2003  The University of New Mexico

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

// 
// Maintenance history
//
// Created: 1-13-00
//
// cleaned up code a bit, almost deleted the whole thing! TBE 7/8/00
//
// 9/26/01
// Takeshi Hakamata <tkh@ahpcc.unm.edu>
// Ported serial communication stuff to POSIX termios.
//
// rewrote almost everything except Takeshi's additions! TBE 7/8/2004
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "Bird.h"

#define DEBUG


// ------------------------------------------------------------

#define ANGLE_RECORD_SIZE   	    7
#define POSITION_RECORD_SIZE	    7
#define POSITION_ANGLE_RECORD_SIZE  13

// --------------------------------------------------------------


unsigned char *CFlock::TranslateToBird( unsigned char *dest, short word )
{
    dest[0] = word & 0xff;
    dest[1] = (word >> 8) & 0xff;
    return dest;
}


short CFlock::TranslateFromBird( unsigned char *number ) 
{
//    return (short)(((((short)number[0] & 0x7f) << 1) | (((short)number[1] & 0x7f) << 8)) << 1);
    return (short)( (((short)number[0] & 0x7f) << 2) | (((short)number[1] & 0x7f) << 9) );
}


char *CFlock::TranslateBirdStatus( char *buf, int word )
{
    char *s = buf;
    
    if( word < 0 )
    {
	sprintf( buf, "ERROR: negative word value: %d\n", word );
	return buf;
    }
    
    if( word & 0x8000 )
    {
    	s += sprintf( s, " bit 15: 1 - Master\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 15: 0 - Slave\n" ); 
    }
    
    if( word & 0x4000 )
    {
    	s += sprintf( s, " bit 14: 1 - Initialized\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 14: 0 - Not initialized\n" ); 
    }
    
    if( word & 0x2000 )
    {
    	s += sprintf( s, " bit 13: 1 - Error\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 13: 0 - No error\n" ); 
    }
    
    if( word & 0x1000 )
    {
    	s += sprintf( s, " bit 12: 1 - Bunning\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 12: 0 - Not running\n" ); 
    }
    
    if( word & 0x0800 )
    {
    	s += sprintf( s, " bit 11: 1 - SYNC mode\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 11: 0 - Not in SYNC mode\n" ); 
    }
    
    if( word & 0x0400 )
    {
    	s += sprintf( s, " bit 10: 1 - Expanded address mode\n" ); 
    }
    else
    {
        s += sprintf( s, " bit 10: 0 - Normal address mode\n" ); 
    }
    
    if( word & 0x0200 )
    {
    	s += sprintf( s, " bit  9: 1 - CRT SYNC mode\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  9: 0 - Not in CRT SYNC mode\n" ); 
    }
    
    if( word & 0x0100 )
    {
    	s += sprintf( s, " bit  8: 1 - No sync modes enabled\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  8: 0 - A sync mode enabled\n" ); 
    }
    
    if( word & 0x0080 )
    {
    	s += sprintf( s, " bit  7: 1 - Factory test mode\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  7: 0 - Bird command mode\n" ); 
    }
    
    if( word & 0x0040 )
    {
    	s += sprintf( s, " bit  6: 1 - XOFF\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  6: 0 - XON\n" ); 
    }
    
    if( word & 0x0020 )
    {
    	s += sprintf( s, " bit  5: 1 - Sleep\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  5: 0 - Run\n" ); 
    }

    switch( ( word & 0x001e ) >> 1 )
    {
    case 0x02:
        s += sprintf( s, " bits 1-4: 0010 - Angle\n" ); 
    	break;
	
    case 0x03:
        s += sprintf( s, " bits 1-4: 0011 - Matrix\n" ); 
    	break;
	
    case 0x04:
        s += sprintf( s, " bits 1-4: 0100 - Position/Angle\n" ); 
    	break;
	
    case 0x05:
        s += sprintf( s, " bits 1-4: 0101 - Position/Matrix\n" ); 
    	break;
	
    case 0x07:
        s += sprintf( s, " bits 1-4: 0111 - Quaternion\n" ); 
    	break;
	
    case 0x10:
        s += sprintf( s, " bits 1-4: 1000 - Position/Quaternion\n" ); 
    	break;
	
    default:
        s += sprintf( s, " bits 1-4: ERROR unrecognised pattern\n" ); 
    	break;
    }
        
    if( word & 0x0001 )
    {
    	s += sprintf( s, " bit  0: 1 - Stream mode\n" ); 
    }
    else
    {
        s += sprintf( s, " bit  0: 0 - Point mode\n" ); 
    }

    return buf;
}


// -----------------------------------------------------------------


bool CFlock::MasterSendCommand( unsigned char cmd )
{
#ifdef DEBUG
    fprintf(stdout, "(CFlock::MasterSendCommand) start\n");
#endif

    int result;

    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::MasterSendCommand) port not initialized\n");
#endif
    	return false;
    }

#ifdef DEBUG
    fprintf(stdout, "(CFlock::MasterSendCommand) %02x\n", cmd );
#endif

    result = write( m_serialPort, &cmd, 1 );

    if( result != 1 ) 
    {
        fprintf(stdout, "(CFlockBird::MasterSendCommand) write() failed.\n");
        return false;
    }

#ifdef DEBUG
    fprintf(stdout, "(CFlock::MasterSendCommand) end\n");
#endif
    return true;
}


bool CFlock::MasterSendCommand( unsigned char *cmd, int size )
{
#ifdef DEBUG
    fprintf(stdout, "(CFlock::MasterSendCommand) start\n");
#endif

    int result;

    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::MasterSendCommand) port not initialized\n");
#endif
    	return false;
    }
    
#ifdef DEBUG
    int i;
    fprintf(stdout, "(CFlock::MasterSendCommand)" );
    for( i=0; i<size; i++ )
    {
    	fprintf(stdout, " %02x", cmd[i] );
    }
    fprintf(stdout, "\n" );
#endif

    result = write( m_serialPort, cmd, size );

    if( result != size ) 
    {
        fprintf(stdout, "(CFlockBird::MasterSendCommand) write() failed.\n");
    	return false;
    }

#ifdef DEBUG
    fprintf(stdout, "(CFlock::MasterSendCommand) done\n");
#endif
    return true;
}


bool CFlock::BirdSendCommand( int bird, unsigned char cmd )
{
#ifdef DEBUG
    fprintf(stdout, "(CFlock::BirdSendCommand) start\n");
#endif

    unsigned char cmd_buf[2];
    bool result;
        
    cmd_buf[0] = 0xf0 | (bird & 0x0f);
    cmd_buf[1] = cmd;
    
    result = MasterSendCommand( cmd_buf, 2 );

#ifdef DEBUG
    fprintf(stdout, "(CFlock::BirdSendCommand) end\n");
#endif

    return result;
}


bool CFlock::BirdSendCommand( int bird, unsigned char *cmd, int size )
{
#ifdef DEBUG
    fprintf(stdout, "(CFlock::BirdSendCommand) start\n");
#endif

    unsigned char cmd_buf[200]; // arbitrary large buffer!
    bool result;
        
    cmd_buf[0] = 0xf0 | (bird & 0x0f);
    memcpy( cmd_buf+1, cmd, size );
    
    result = MasterSendCommand( cmd_buf, size+1 );
    
#ifdef DEBUG
    fprintf(stdout, "(CFlock::BirdSendCommand) end\n");
#endif

    return result;
}


int CFlock::Read_Data( unsigned char *buffer, int size, long int timeout )
{
#ifdef DEBUG
    fprintf(stdout, "(CFlock::Read_Data) start\n");
#endif

    int i;
    timeval time;
    fd_set readfds;
    int bytes;
    int result;

    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::Read_Data) port not initialized\n");
#endif
    	return 0;
    }
    
    FD_ZERO( &readfds );
    FD_SET( m_serialPort, &readfds );

    bytes = 0;
    while( bytes<size )
    {
	time.tv_sec  = 0;
	time.tv_usec = timeout;
	result = select( m_serialPort + 1, &readfds, NULL, NULL, &time );
	if( result )
	{
    	    bytes += read( m_serialPort, buffer+bytes, size-bytes );
	}
	else
	{
	    break;
	}
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::Read_Data) %d bytes: ", bytes );
    for( i=0; i<bytes; i++ )
    {
    	fprintf( stdout, "%02x", buffer[i] );
    }
    fprintf( stdout, "\n" );
#endif

#ifdef DEBUG
    fprintf(stdout, "(CFlock::Read_Data) end\n");
#endif

    return bytes;    
}


bool CFlock::FlockAutoConfig( )
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockAutoConfig) begin\n" );
#endif

    unsigned char flock_auto_config_query[3] = { 0x50, 0x32, 0 };

    // setup the command
    flock_auto_config_query[2] = m_flockNumberOfBirds;

    // Must wait > 600ms before sending auto-config to tracker.
    usleep( 750000 );

    if( !MasterSendCommand( flock_auto_config_query, 3 ) )
    {
	fprintf( stdout, "(CFlock::FlockAutoConfig) send command failed.\n" );
	return false;
    }

    // Must wait > 600ms after sending auto-config to tracker.
    usleep( 750000 );

#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockAutoConfig) end\n" );
#endif
    return true;
}


bool CFlock::CheckFlockSystemStatus( )
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::CheckFlockSystemStatus) begin\n" );
#endif
    unsigned char flock_system_status_query[2] = { 0x4f, 0x24 };
    int i;
    timeval timeout;
    fd_set readfds;
    int bytes;
    int result;
    int num_birds;
    int num_transmitters;
    int num_extended_range_transmitters;
    int num_receivers;
//    unsigned char dummy_buffer[1000];
    
    // clear system status buffer    
    for( i=0; i<14; i++ )
    {
    	m_flockSystemStatus[ i ] = 0;
    }
    
    if( !m_serialPort )
    {
    	return false;
    }

    // throw out unused extra data
    ClearSerialPort( );
        
    MasterSendCommand( flock_system_status_query, 2 );

//     timeout.tv_sec  = 0;
//     timeout.tv_usec = 250000;
//     FD_ZERO( &readfds );
//     FD_SET( m_serialPort, &readfds );
// 
//     bytes = 0;
//     while( bytes<14 )
//     {
// 	timeout.tv_sec  = 0;
// 	timeout.tv_usec = 250000;
// 	result = select( m_serialPort + 1, &readfds, NULL, NULL, &timeout );
// 	if( result )
// 	{
//     	    bytes += read( m_serialPort, m_flockSystemStatus+bytes, 14-bytes );
// 	}
// 	else
// 	{
// 	    break;
// 	}
//     }
// 
// #ifdef DEBUG
//     fprintf( stdout, "(CFlock::CheckFlockSystemStatus) %d bytes: ", bytes );
//     for( i=0; i<bytes; i++ )
//     {
//     	fprintf( stdout, "%02x", m_flockSystemStatus[i] );
//     }
//     fprintf( stdout, "\n" );
// #endif

    bytes = Read_Data( m_flockSystemStatus, 14 );
    
    if( bytes<14 )
    {
    	return false;
    }

    if( m_flockSystemStatus[0] & 0x80 )
    {
	num_birds = 0;
	num_transmitters = 0;
	num_extended_range_transmitters = 0;
	num_receivers = 0;

	for( i=0; i<14; i++ )
	{
	    if( m_flockSystemStatus[ i ] & 0x80 ) num_birds++;
	    if( m_flockSystemStatus[ i ] & 0x20 ) num_receivers++;
//	    	if( m_flockSystemStatus[ i ] & 0x10 ) num_birds++;
	    if( m_flockSystemStatus[ i ] & 0x08 ) num_extended_range_transmitters++;
	    if( m_flockSystemStatus[ i ] & 0x04 ) num_extended_range_transmitters++;
	    if( m_flockSystemStatus[ i ] & 0x02 ) num_extended_range_transmitters++;
	    if( m_flockSystemStatus[ i ] & 0x01 )
	    {
    	    	if( m_flockSystemStatus[ i ] & 0x10 ) 
		{	
		    num_extended_range_transmitters++;
		}
		else
		{
		    num_transmitters++;
		}
	    }
	}     

	m_flockNumberOfBirds = num_birds;
	m_flockNumberOfReceivers = num_receivers;
	m_flockNumberOfTransmitters = num_transmitters;
	m_flockNumberOfExtendedRangeTransmitters = num_extended_range_transmitters;

#ifdef DEBUG
	fprintf( stdout, "(CFlock::CheckFlockSystemStatus) %d birds\n", num_birds );
	fprintf( stdout, "(CFlock::CheckFlockSystemStatus) %d transmitters\n", num_transmitters );
	fprintf( stdout, "(CFlock::CheckFlockSystemStatus) %d extended range transmitters\n", num_extended_range_transmitters );
	fprintf( stdout, "(CFlock::CheckFlockSystemStatus) %d receivers\n", num_receivers );	    
#endif
    }    

#ifdef DEBUG
    fprintf( stdout, "(CFlock::CheckFlockSystemStatus) end\n" );
#endif
    return true;
}


/***
bool CFlock::AutomaticallyDetectBaudRate( )
{
    int rate_list[ 5 ] = { 115200, 57600, 38400 , 19200, 9600 };
    int i;
    unsigned char blank[100];
        
    for( i=0; i<100; i++ )
    {
    	blank[i] = 0;
    }

    for( i=0; i<5; i++ )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::AutomaticallyDetectBaudRate) trying %d baud.\n", rate_list[i] );
#endif
        setBaud( rate_list[i] );
      
        if( StartSerialPort( ) )
    	{
//	    m_flockNumberOfBirds = 1;
//	    FlockAutoConfig( );
	    MasterSendCommand( blank, 100 );
	    FBBReset( );
	    usleep( 100000 ); // pause for 0.1 seconds
	    
    	    if( CheckFlockSystemStatus( ) )
	    {
                StopSerialPort( );
	    	return true;
	    }
            StopSerialPort( );
	}
    }
    
    return false;
}
***/


CFlock::CFlock( char *serial_port, int baud_rate )
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::CFlock) begin\n" );
#endif

    int i, j;
    
//    m_mode = BIRD_POSITION_ANGLES;
    m_mode = 0x59;
    m_up = false;    
    m_flockNumberOfBirds = 0;
    m_flockNumberOfReceivers = 0;
    m_flockNumberOfTransmitters = 0;
    m_flockNumberOfExtendedRangeTransmitters = 0;
    m_group = true;
    m_hemisphere[0] = 0;
    m_hemisphere[1] = 0;
    m_beep_on_error = false;
        
    for( i=0; i<14; i++ )
    {
    	for( j=0; j<3; j++ )
	{
   	    m_flockPosition[i][j] = 0;
   	    m_flockAngle[i][j] = 0;
	}
    	for( j=0; j<4; j++ )
	{
    	    m_flockQuaternion[i][j] = 0;
	}
    	for( j=0; j<16; j++ )
	{
   	    m_flockMatrix[i][j] = 0;
	}
    }
    for( i=0; i<14*25; i++ )
    {
    	m_receiveBuffer[i] = 0;
    }
    
    m_serialPortName = strdup( serial_port );
//    if( baud_rate < 0 )
//    {
//    	AutomaticallyDetectBaudRate( );
//    }
//    else
//    {
    	SetBaud( baud_rate );
//    }
      
    if( StartSerialPort( ) )
    {
    	SerialSetRTS( );
	SerialWaitForCTSClear( true );
    	SerialClearRTS( );
	SerialWaitForCTSSet( true );

	RequestData( );
    	ClearSerialPort( );
			
    	if( CheckFlockSystemStatus( ) )
	{
	    FlockAutoConfig( );
	    CheckFlockSystemStatus( );
	    
	    //  create the flock
	    m_numBirds = m_flockNumberOfBirds;
	    
	    if( FlockInitialize( ) )
	    {
	    	m_up = true;
	    }
	}
	else
	{
    	    fprintf( stdout, "(CFlock::Cflock) failed to get system status from flock\n" );
	}
    }
    else
    {
    	// serial port failed to start....
	fprintf( stdout, "(CFlock::Cflock) failed to connect to serial port \"%s\"\n", serial_port );
    }
    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::CFlock) end\n" );
#endif
}


CFlock::~CFlock() 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::~CFlock) begin\n" );
#endif

    if( m_up )
    {
	// attempt to stop sending stream and clear up extra data
	RequestData( );

	ClearSerialPort( );

	FBBReset( );
	SetSleep( );
    }
    
    // disconnect from the flock
    StopSerialPort( );

    //  free the resources
    if( m_serialPortName )
    {
    	free( m_serialPortName );
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::~CFlock) end\n" );
#endif
}


bool CFlock::StartSerialPort( ) 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::StartSerialPort) begin\n" );
#endif

    int nResult;
    termios options;
    
    m_serialPort = open( m_serialPortName, O_RDWR | O_NONBLOCK | O_NOCTTY, 0666 );

#ifdef DEBUG
    fprintf( stdout, "(CFlock::StartSerialPort) Opened %s.\n", m_serialPortName );
#endif

    if( m_serialPort < 0 ) 
    {
        fprintf( stdout, "(CFlock::StartSerialPort) Error connecting to serial port %s\n", m_serialPortName );
        m_serialPort = 0;
    	return false;
    }

    tcflush( m_serialPort, TCIOFLUSH );

    if( tcgetattr( m_serialPort, &options ) != 0 ) 
    {
        fprintf( stderr, "(CFlock::StartSerialPort) Couldn't read current settings\n");
    	m_serialPort = 0;
    	return false;
    }

    options.c_iflag      = IGNBRK | IGNPAR;
    options.c_oflag      = 0;
    options.c_cflag      = CREAD | CS8 | CLOCAL | HUPCL | CSTOPB;
    options.c_lflag      = 0;
    options.c_cc[VEOL]   = '\r';
    options.c_cc[VERASE] = '\000';
    options.c_cc[VKILL]  = '\000';
    options.c_cc[VMIN]   = 0;
    options.c_cc[VTIME]  = 0;
    cfsetospeed( &options, m_baudRate );
    cfsetispeed( &options, m_baudRate );

    nResult = tcsetattr( m_serialPort, TCSAFLUSH, &options );

    if( nResult < 0 ) 
    {
	fprintf( stderr, "(CFlock::StartSerialPort) ioctl error establishing serial port\n");
	StopSerialPort();
	return false;
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::StartSerialPort) end\n" );
#endif
    return true;
}


bool CFlock::SerialClearRTS() 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialClearRTS) begin\n" );
#endif

    int control;
    bool result;
    
    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::SerialClearRTS) port not initialized\n");
#endif
    	return false;
    }
    
    ioctl( m_serialPort, TIOCMGET, &control); // get current control bits
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialClearRTS) read control bits %x\n", control );
#endif
    control &= ~( TIOCM_RTS ); // clear RTS bit
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialClearRTS) write control bits %x\n", control );
#endif
    ioctl( m_serialPort, TIOCMSET, &control); //   ->should be set RTS OFF
    ioctl( m_serialPort, TIOCMGET, &control); //   -> this should say RTS is OFF
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialClearRTS) read contol bits %x\n", control );
#endif
    
    result = control & TIOCM_RTS == 0;
	    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialClearRTS) end\n" );
#endif
    
    return result;
}


bool CFlock::SerialSetRTS() 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialSetRTS) begin\n" );
#endif

    int control;
    bool result;
    
    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::SerialSetRTS) port not initialized\n");
#endif
    	return false;
    }
    
    ioctl( m_serialPort, TIOCMGET, &control); // get current control bits
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialSetRTS) read control bits %x\n", control );
#endif
    control |= ( TIOCM_RTS ); // set RTS bit
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialSetRTS) write control bits %x\n", control );
#endif
    ioctl( m_serialPort, TIOCMSET, &control); //   ->should be set RTS ON
    ioctl( m_serialPort, TIOCMGET, &control); //   -> this should say RTS is ON
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialSetRTS) read contol bits %x\n", control );
#endif
    
    result = control & TIOCM_RTS == TIOCM_RTS;
	    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialSetRTS) end\n" );
#endif
    
    return result;
}


bool CFlock::SerialWaitForCTSSet( bool timeout ) 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialWaitForCTSSet) begin\n" );
#endif

    int control;
    int tries;
    
    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::SerialWaitForCTSSet) port not initialized\n");
#endif
    	return false;
    }
    
    tries = 0;

//    usleep( 500000 ); // wait half a second
    
    ioctl( m_serialPort, TIOCMGET, &control); // check to see if flock is flying
    while( !( control & TIOCM_CTS ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::SerialWaitForCTSSet) flock not flying yet.\n" );
#endif
    	tries++;
	if( timeout && tries > 4 )
	{
#ifdef DEBUG
    	    fprintf( stdout, "(CFlock::SerialWaitForCTSSet) flock not flying, try autoconfig anyway.\n" );
#endif
    	    return false;
	    
	}
	
    	usleep( 500000 ); // wait half a second
        ioctl( m_serialPort, TIOCMGET, &control); 
    }
    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialWaitForCTSSet) end\n" );
#endif
    return true;
}


bool CFlock::SerialWaitForCTSClear( bool timeout ) 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialWaitForCTSClear) begin\n" );
#endif

    int control;
    int tries;
    
    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::SerialWaitForCTSClear) port not initialized\n");
#endif
    	return false;
    }
    
    tries = 0;

//    usleep( 500000 ); // wait half a second
    
    ioctl( m_serialPort, TIOCMGET, &control); // check to see if flock is flying
    while( ( control & TIOCM_CTS ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::SerialWaitForCTSClear) flock not flying yet.\n" );
#endif
    	tries++;
	if( timeout && tries > 4 )
	{
#ifdef DEBUG
    	    fprintf( stdout, "(CFlock::SerialWaitForCTSClear) flock not flying, try autoconfig anyway.\n" );
#endif
    	    return false;
	    
	}
	
    	usleep( 500000 ); // wait half a second
        ioctl( m_serialPort, TIOCMGET, &control); 
    }
    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::SerialWaitForCTSClear) end\n" );
#endif
    return true;
}


void CFlock::StopSerialPort() 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::StopSerialPort) begin\n" );
#endif

    if( m_serialPort ) 
    {

//    	SerialSetRTS( );
//	SerialWaitForCTSClear( true );

	close( m_serialPort );
	m_serialPort = 0;
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::StopSerialPort) end\n" );
#endif
}


bool CFlock::FlockInitialize() 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockInitialize) begin\n" );
#endif

//    int i;

    if( !(SetGroup( m_group )) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::FlockInitialize) group mode failed\n" );
#endif
	return false;
    }

    if( !(SetQueryMode( m_mode )) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::FlockInitialize) query mode failed\n" );
#endif
    	return false;
    }

    if( !(SetHemisphere( m_hemisphere )) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::FlockInitialize) hemisphere failed\n" );
#endif
	return false;
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockInitialize) end\n" );
#endif
    return true;
}


bool CFlock::FBBReset( )
{
    // send a FBB reset request to the master bird
    if( !MasterSendCommand( 0x2f ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::FBBReset) failed\n" );
#endif
        return false;
    }
    
    return true;
}


bool CFlock::RequestData( )
{
    //  prime the pump for the next read only from the master bird
    if( !MasterSendCommand( 0x42 ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::RequestData) failed\n" );
#endif
        return false;
    }
    
    return true;
}


bool CFlock::GetPosAng( float **pos, float **ang )
{
#ifdef DEBUG
//    fprintf( stdout, "(CFlock::getPosAng) begin\n" );
#endif

//    unsigned char *bufptr; // (cpw-3/9/99) more fixes for the 2-bird limit
//    unsigned char *cbufptr; // DS added
    int i; 
    int expected_message_size;
    int receiver_count;
    int bytes;
    timeval timeout;
    fd_set readfds;
    int result;

//////////////////////////////////////////////
#ifdef DEBUG
#if 1
    struct timeval start_time;
    static int count=-100;
    static struct timeval t0;
    float t, rate;
        
    gettimeofday( &start_time, NULL );
    gettimeofday( &start_time, NULL );

    start_time.tv_sec = 5;
    start_time.tv_usec = 5;
    
    if( count==0 )
    {
        gettimeofday( &t0, NULL );
    }
    else
    {
        gettimeofday( &start_time, NULL );
    	t = (float)(start_time.tv_sec-t0.tv_sec) + (float)(start_time.tv_usec-t0.tv_usec) / 1000000.0f;    
	if( t > 5.0f )
	{
	    rate = (float)count / t;
	    fprintf( stdout, "%d samples in %f seconds, at %f samples/second\n", count, t, rate );
	    count = -1;
            gettimeofday( &t0, NULL );
	}
    }
    count++;
#endif    
#endif    
/////////////////////////////////////////////////
            
    FD_ZERO( &readfds );
    FD_SET( m_serialPort, &readfds );
    
    receiver_count = m_flockNumberOfReceivers;
    
    expected_message_size = 13 * receiver_count;

    // make sure that we are in the right mode
//    if( m_mode != BIRD_POSITION_ANGLES )
    if( m_mode != 0x59 )
    { 
    	// switch to position/angler mode 
    	if( !(SetQueryMode( 0x59 )) )
	{
	    fprintf( stdout, "(CFlock::getPosAng) unable to set position/angle mode\n" );
	    return false;
	}
    }

    bytes = 0;
    while( bytes<expected_message_size )
    {
	timeout.tv_sec  = 0;
	timeout.tv_usec = 250000;
	result = select( m_serialPort + 1, &readfds, NULL, NULL, &timeout );
	if( result )
	{
    	    bytes += read( m_serialPort, m_receiveBuffer+bytes, expected_message_size-bytes );
	}
	else
	{
	    break;
	}
    }
    
#ifdef DEBUG
#if 1
    fprintf( stdout, "(CFlock::getPosAng) " );
    for( i=0; i<bytes; i++ )
    {
        fprintf( stdout, " %02x", m_receiveBuffer[i] );
    }
    fprintf( stdout, "\n" );
#endif
#endif


    if( bytes != expected_message_size )
    {
	// short read, use backup
	fprintf( stdout, "(CFlock::getPosAng) incorrect read of %d bytes, expected %d bytes\n", bytes, expected_message_size );
	if( m_beep_on_error )
	{
	    fprintf( stdout, "%c", 0x07 );
	}
    	return false;
    }
    
    for( i=0; i<m_flockNumberOfReceivers; i++ )
    {
	pos[i][0] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13 ) / (double)SHRT_MAX );
	pos[i][1] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+2 ) / (double)SHRT_MAX );
	pos[i][2] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+4 ) / (double)SHRT_MAX );

    	ang[i][0] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+6 ) * 180.0 / (double)SHRT_MAX );
    	ang[i][1] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+8 ) * 180.0 / (double)SHRT_MAX );
    	ang[i][2] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+10 ) * 180.0 / (double)SHRT_MAX );
    }

#ifdef DEBUG
//    fprintf( stdout, "(CFlock::getPosAng) end\n" );
#endif
    return true;
}


bool CFlock::Read_PositionAngle( float **position, float **angle )
{
#ifdef DEBUG
//    fprintf( stdout, "(CFlock::Read_PositionAngle) begin\n" );
#endif

    int i, j; 
    int bytes;
    timeval timeout;
    fd_set readfds;
    int result;
    int k;
    
//////////////////////////////////////////////
#ifdef DEBUG
#if 1
    struct timeval start_time;
    static int count=-100;
    static struct timeval t0;
    float t, rate;
        
    gettimeofday( &start_time, NULL );
    gettimeofday( &start_time, NULL );

    start_time.tv_sec = 5;
    start_time.tv_usec = 5;
    
    if( count==0 )
    {
        gettimeofday( &t0, NULL );
    }
    else
    {
        gettimeofday( &start_time, NULL );
    	t = (float)(start_time.tv_sec-t0.tv_sec) + (float)(start_time.tv_usec-t0.tv_usec) / 1000000.0f;    
	if( t > 5.0f )
	{
	    rate = (float)count / t;
	    fprintf( stdout, "%d samples in %f seconds, at %f samples/second\n", count, t, rate );
	    count = -1;
            gettimeofday( &t0, NULL );
	}
    }
    count++;
#endif    
#endif    
/////////////////////////////////////////////////
            
    for( i=0; i<m_flockNumberOfReceivers; i++ )
    {
    	bytes = Read_Data( m_receiveBuffer+i*POSITION_ANGLE_RECORD_SIZE, POSITION_ANGLE_RECORD_SIZE );
	
	if( bytes != POSITION_ANGLE_RECORD_SIZE )
    	{	
	    // short read, use backup
	    fprintf( stdout, "(CFlock::getPosAng) incorrect read of %d bytes, expected %d bytes\n", bytes, POSITION_ANGLE_RECORD_SIZE );
    	    return false;
	}
    }
        
    for( i=0; i<m_flockNumberOfReceivers; i++ )
    {
	// find bird #
	if( m_flockNumberOfExtendedRangeTransmitters == 0 )
	{
	    j = (int) m_receiveBuffer[ i * POSITION_ANGLE_RECORD_SIZE + 12 ] - 1;
	}
	else
	{
	    j = (int) m_receiveBuffer[ i * POSITION_ANGLE_RECORD_SIZE + 12 ] - 2;
	}
	
	if( j>=0 && j<m_flockNumberOfReceivers )
	{
	    // find position
	    position[j][0] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13 ) / (double)SHRT_MAX );
	    position[j][1] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+2 ) / (double)SHRT_MAX );
	    position[j][2] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+4 ) / (double)SHRT_MAX );

	    // find angle
    	    angle[j][0] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+6 ) * 180.0 / (double)SHRT_MAX );
    	    angle[j][1] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+8 ) * 180.0 / (double)SHRT_MAX );
    	    angle[j][2] = (float)( (double)TranslateFromBird( m_receiveBuffer+i*13+10 ) * 180.0 / (double)SHRT_MAX );
	}
	else
	{
	    fprintf( stdout, "ERROR finding tracker #%d", j );
	    for( k=0; k<13; k++ )
	    {
		fprintf( stdout, " %02x", m_receiveBuffer[i*13+k] );
	    } 
	    fprintf( stdout, "\n" );
	}
    }

#ifdef DEBUG
//    fprintf( stdout, "(CFlock::Read_PositionAngle) end\n" );
#endif
    return true;
}


bool CFlock::StartStream( ) 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::StartSream).\n" );
#endif

//    unsigned char cmd;

    // have the master bird send it
    if( !MasterSendCommand( 0x40 ) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::StartSream) unable to start sreaming\n" );
#endif
        return false;
    }

    return true;
}


bool CFlock::StopStream( ) 
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::StopSream).\n" );
#endif

//    unsigned char cmd;

    // have the master bird send it
    if( !MasterSendCommand( 0x3f ) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::StopSream) unable to stop sreaming\n" );
#endif
        return false;
    }

    return true;
}


void CFlock::SetBaud( int baud_rate )
{
    switch( baud_rate ) 
    {
    case 2400:
	m_baudRate = B2400;
	break;
    case 4800:
	m_baudRate = B4800;
	break;
    case 9600:
	m_baudRate = B9600;
	break;
    case 19200:
	m_baudRate = B19200;
	break;
    case 38400:
	m_baudRate = B38400;
	break;
    case 57600:
	m_baudRate = B57600;
	break;
    case 115200:
	m_baudRate = B115200;
	break;
//    case 230400:
//	m_baudRate = B230400;
//	break;
    default:
	fprintf( stdout, "(CFlock::setBaud) Incorrect baud rate has been specified: %d\n", baud_rate );
	m_baudRate = B0;
	break;
    }
}


bool CFlock::ClearSerialPort( long int timeout )
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::ClearSerialPort) begin\n" );
#endif

    int bytes = 0;
    timeval time;
    fd_set readfds;
    int result;
    unsigned char buffer[1000];
    int tries=0;
      
    if( !m_serialPort )
    {
    	return false;
    }
      
    // try to read serial port until it is empty...
    FD_ZERO( &readfds );
    FD_SET( m_serialPort, &readfds );

    time.tv_sec  = 0;
    time.tv_usec = timeout;
    result = select( m_serialPort + 1, &readfds, NULL, NULL, &time );
    while( result )
    {
//	bytes = read( m_serialPort, buffer, 1000 );
	bytes = Read_Data( buffer, 1000 );

#ifdef DEBUG
// 	int i;
// 	if( bytes > 0 )
// 	{
// 	    fprintf(stdout, "(CFlock::ClearSerialPort)" );
// 	    for( i=0; i<bytes; i++ )
// 	    {
//     		fprintf(stdout, " %02x", buffer[i] );
// 	    }
// 	    fprintf(stdout, "\n" );
// 	}
	fprintf(stdout, "(CFlock::ClearSerialPort) skipped %d bytes\n", bytes );
#endif

	time.tv_sec  = 0;
	time.tv_usec = timeout;
	result = select( m_serialPort + 1, &readfds, NULL, NULL, &time );
	if( tries > 1000 )
	{
#ifdef DEBUG
    	    fprintf( stdout, "(CFlock::ClearSerialPort) failed\n" );
#endif
	    return false;
	}
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::ClearSerialPort) end\n" );
#endif
    return true;
}
    

/*
bool CFlock::FlockQuery(int timeout)
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockQuery) begin\n" );
#endif

    int i;
    int j;
    unsigned char *bufPointer, *old_bufPointer, bufptr[150];
    int bytes = 0, readBytes = 0, maxTries = 5;
    unsigned char examineVal[2];
    int num = 8 * 2;
    int loc = 2 - 1;
    int shift = 7;
    int mask;
    timeval timeOut;
    fd_set readfds;
    int result;
    unsigned short *dummy;
//    unsigned char status[20];
//    int availableBirds;
//    bool lastBirdGood;
    
    ClearSerialPort( );
        
    bytes = 0;
    examineVal[0] = 0x4f;
    examineVal[1] = 0;

    if( !MasterSendCommand( examineVal, 2 ) )
    {
	fprintf( stderr, "(CFlock::FlockQuery) sendCommand failed.\n" );
	return false;
    }

    timeOut.tv_sec  = 0;
    timeOut.tv_usec = 250000;
    FD_ZERO( &readfds );
    FD_SET( m_serialPort, &readfds );

    // use select() to see if something came back to the serial port
    result = select( m_serialPort + 1, &readfds, NULL, NULL, &timeOut );
    fprintf( stdout, "(CFlock::FlockQuery) result = %d\n", result );

    // was there an error or not responding
    if( result <= 0 ) 
    {
        return false;
    }

    if( FD_ISSET( m_serialPort, &readfds ) == 0 ) 
    {
        return false;
    }

    //  read data at port.  There may be garbage form other commands here so
    //  try to read everything and only concern with the last 2 bytes
    i = 0;
//    bzero( (unsigned char *) bufptr, 150 );
    bufPointer = bufptr;
    old_bufPointer = bufptr;
    while( ((bytes = read( m_serialPort, bufPointer+bytes, 150 )) > 0) && (i < maxTries) )
    {
	fprintf( stdout, "(CFlock::FlockQuery) bytes = %d value = ", bytes ); 
    	for( j=0; j<bytes; j++ )
	{
	    fprintf( stdout, "%02x", *(old_bufPointer+j) );
	}
        fprintf( stdout, "\n" );
	
	old_bufPointer = bufPointer+bytes;
	i++;
	readBytes = bytes;
    }
    if( readBytes == 0 )
    {
        return false;
    }

    if( i < maxTries )
    {
        fprintf( stdout, "(CFlock::FlockQuery)    read eof i = %d\n", i );
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery)    i = maxTries\n" );
    }

    fprintf( stdout, "(CFlock::FlockQuery)    readBytes = %d\n", readBytes );

    dummy = (unsigned short *)bufPointer;

    fprintf( stdout, "(CFlock::FlockQuery) BIRD_EXAME: buf: %x \n", *dummy);
    fprintf( stdout, "(CFlock::FlockQuery) BIRD_EXAME: [0]= 0x%x [1]= 0x%x\n", bufPointer[0], bufPointer[1]);

    fprintf( stdout, "(CFlock::FlockQuery) Raw bytes -> '" );
    for( j = 1; j <= num; j++ )
    {
	mask = 1 << shift;
	if( (bufPointer[loc] & mask) == 0 )
	{
	    fprintf( stdout, "0" );
	}
	else
	{
	    fprintf( stdout, "1" );
	}
	shift--;
	if( (j % 8 == 0) && (j != num) )
	{
	    fprintf( stdout, " " );
	    loc--;
	    shift = 7;
	}
    }
    fprintf( stdout, "' <-\n" );

    //  Interpret data
    //  bit 0
    int mask1 = 1, value = 0;


    if( (bufPointer[0] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 POINT mode selected\n" );
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 STREAM mode selected\n" );
    }

    //  bit 1, 2, 3, 4
    mask1 = 1 << 1;
    if( (bufPointer[0] & mask1) != 0 )
    {
        value = 1;
    }
    mask1 = 1 << 2;
    if( (bufPointer[0] & mask1) != 0 )
    {
        value += 2;
    }
    mask1 = 1 << 3;
    if( (bufPointer[0] & mask1) != 0 )
    {
        value += 4;
    }
    mask1 = 1 << 4;
    if( (bufPointer[0] & mask1) != 0 )
    {
        value += 8;
    }

    switch( value )
    {
    case 1:
        fprintf( stdout, "(CFlock::FlockQuery) 0001 POSITION outputs selected\n");
        break;
    case 2:
    	fprintf( stdout, "(CFlock::FlockQuery) 0010 ANGLE outputs selected\n");
    	break;
    case 3:
    	fprintf( stdout, "(CFlock::FlockQuery) 0011 MATRIX outputs selected\n");
        break;
    case 4:
    	fprintf( stdout, "(CFlock::FlockQuery) 0100 POSITION/ANGLE outputs selected\n");
    	break;
    case 5:
        fprintf( stdout, "(CFlock::FlockQuery) 0101 POSITION/MATRIX outputs selected\n");
    	break;
    case 6:
        fprintf( stdout, "(CFlock::FlockQuery) 0110 factory use only\n");
    	break;
    case 7:
        fprintf( stdout, "(CFlock::FlockQuery) 0111 QUATERNION outputs selected\n");
    	break;
    case 8:
        fprintf( stdout, "(CFlock::FlockQuery) 1000 POSITION/QUATERNION outputs selected\n");
    	break;
    default:
        fprintf( stdout, "(CFlock::FlockQuery) %d UNKNOWN type selected\n",value);
    }

    //  bit 5
    mask1 = 1 << 5;
    if( (bufPointer[0] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 the Bird is in RUN mode\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 The Bird is in SLEEP mode\n");
    }

    //  bit 6
    mask1 = 1 << 6;
    if( (bufPointer[0] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 XON\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 XOFF\n");
    }

    //  bit 7
    mask1 = 1 << 7;
    if( (bufPointer[0] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 The Bird Commands are enable\n");
    }
    else
    {
    	fprintf( stdout, "(CFlock::FlockQuery) 1 factory test and Bird Commands are enable\n");
    }

    //  bit 8
    mask1 = 1;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Sync mode enable\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 NO Sync mode enable\n");
    }

    //  bit 9
    mask1 = 1 << 1;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Not in CRTSYNC mode \n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 CRTSYNC mode \n");
    }

    //  bit 10
    mask1 = 1 << 2;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Normal address node enable\n");
    }
    else
    {
    	fprintf( stdout, "(CFlock::FlockQuery) 1 Expanded address node enable\n");
    }

    //  bit 11
    mask1 = 1 << 3;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 not in HOST SYNC mode\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 HOST SYNC mode\n");
    }

    //  bit 12
    mask1 = 1 << 4;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Bird is not RUNNING\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 Bird is RUNNING\n");
    }

    //  bit 13
    mask1 = 1 << 5;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 NO ERROR is detected\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 ERROR is detected\n");
    }

    //  bit 14
    mask1 = 1 << 6;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Bird has not been initialized\n");
    }
    else
    {
    	fprintf( stdout, "(CFlock::FlockQuery) 1 Bird has been initialized\n");
    }

    //  bit 15
    mask1 = 1 << 7;
    if( (bufPointer[1] & mask1) == 0 )
    {
        fprintf( stdout, "(CFlock::FlockQuery) 0 Bird is a SLAVE Bird\n");
    }
    else
    {
        fprintf( stdout, "(CFlock::FlockQuery) 1 Bird is a MASTER Bird\n");
    }

#ifdef DEBUG
    fprintf( stdout, "(CFlock::FlockQuery) end\n" );
#endif
    return true;
}
*/
	
int CFlock::Examine_BirdStatus( int bird )
{
#ifdef DEBUG
    fprintf( stdout, "(CFlock::Examine_BirdStatus) begin\n" );
#endif

    unsigned char command[2] = { 0x4f, 0x00 };
    unsigned char response[2];
    int bytes;
    timeval timeout;
    fd_set readfds;
    int result;
    int expected_message_size = 2;
        
    if( !m_serialPort ) 
    {
#ifdef DEBUG
    	fprintf(stdout, "(CFlock::Examine_BirdStatus) port not initialized\n");
#endif
    	return -1;
    }
    
//    ClearSerialPort( );
        
    if( !BirdSendCommand( bird, command, 2 ) )
    {
        fprintf( stdout, "(CFlock::Examine_BirdStatus) failed to send command.\n" );
	return -1;
    }

    // --- command has been sent, now read response ----------------------
    
//     FD_ZERO( &readfds );
//     FD_SET( m_serialPort, &readfds );
//     
//     bytes = 0;
//     while( bytes<expected_message_size )
//     {
// 	timeout.tv_sec  = 0;
// 	timeout.tv_usec = 250000;
// 	result = select( m_serialPort + 1, &readfds, NULL, NULL, &timeout );
// 	if( result )
// 	{
//     	    bytes += read( m_serialPort, response+bytes, expected_message_size-bytes );
// 	}
// 	else
// 	{
// 	    break;
// 	}
//     }

    bytes = Read_Data( response, 2 );
    
    if( bytes != expected_message_size )
    {
	fprintf( stdout, "(CFlock::Examine_BirdStatus) ERROR: recieved %d bytes, expected %d bytes\n", bytes, expected_message_size );
	return -1;
    }
    
#ifdef DEBUG
    fprintf( stdout, "(CFlock::Examine_BirdStatus) end\n" );
#endif

    return ( response[1] & 0xff ) << 8 | ( response[0] & 0xff );
}


bool CFlock::SetMeasurementRate( const float rate )
{
    unsigned char changeVal[4];

    m_birdMeasRate = rate;

    int temp = (int)( m_birdMeasRate * 256.0 );

    // Set the flock measurement rate
    changeVal[0] = 0x50;	
    changeVal[1] = 0x07;
    changeVal[2] = 0xff & ( temp );
    changeVal[3] = 0xff & ( temp >> 8 );
    if( !MasterSendCommand( changeVal, 4 ) )
    {
#ifdef DEBUG
        fprintf( stdout, "(CFlock::setMeasurementRate) could not set measurement rate\n" );
#endif
    	return false;
    }
    
    return true;
}


void CFlock::SetComPort( const char *port ) 
{ 
    if( m_serialPortName )
    {
    	free( m_serialPortName );
    }
    m_serialPortName = strdup( port ); 
}


bool CFlock::SetQueryMode( char mode ) 
{ 
    m_mode = mode; 

    // have the master bird send it
    if( !MasterSendCommand( m_mode ) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::setQueryMode) unable to set query mode\n" );
#endif
        return false;
    }

    return true;
}


bool CFlock::SetGroup(const int group) 
{ 
    unsigned char changeVal[3];

    m_group = group; 

    changeVal[0] = 0x50;	
    changeVal[1] = 0x23;	//  0x23 -- group mode
    changeVal[2] = (m_group) ? 1 : 0;	//  1 to enable, 0 to disable
    if( !MasterSendCommand( changeVal, 3 ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::setGroup) unable to set group mode\n" );
#endif
    	return false;
    }
    
    return true;
}


bool CFlock::SetHemisphere( unsigned char hemi0, unsigned char hemi1 ) 
{
    unsigned char hemi[2];
    
    hemi[0] = hemi0;
    hemi[1] = hemi1; 

    return SetHemisphere( &(hemi[0]) ); 
}


bool CFlock::SetHemisphere( unsigned char *hemi ) 
{
    unsigned char cmd[4];

    m_hemisphere[0] = hemi[0];
    m_hemisphere[1] = hemi[1]; 

    // have the master bird send it
    cmd[0] = 0x50;
    cmd[1] = 0x16; // hemisphere
    cmd[2] = m_hemisphere[0];
    cmd[3] = m_hemisphere[1];
    if( !MasterSendCommand( cmd, 4 ) ) 
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::setHemisphere) unable to set hemisphere\n" );
#endif
        return false;
    }

    return true;
}


bool CFlock::IsUp( )
{
    return m_up;
}


int CFlock::GetNumberOfBirds( )
{
    return m_flockNumberOfBirds;
}


int CFlock::GetNumberOfReceivers( )
{
    return m_flockNumberOfReceivers;
}


int CFlock::GetNumberOfExtendedRangeTransmitters( )
{
    return m_flockNumberOfExtendedRangeTransmitters;
}


bool CFlock::SetReportRate(const int rate) 
{ 
    unsigned char changeVal[3];

    changeVal[0] = 0x50;	
    changeVal[1] = 0x1d;	//  0x1d -- report rate
    changeVal[2] = rate & 0x7f;	//  1 to 127
    if( !MasterSendCommand( changeVal, 3 ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::setGroup) unable to set report rate\n" );
#endif
    	return false;
    }
    
    return true;
}


bool CFlock::SetSleep( ) 
{ 
    if( !MasterSendCommand( 0x47 ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::SetSleep) unable to set sleep mode\n" );
#endif
    	return false;
    }
    
    return true;
}


bool CFlock::SetReferenceFrame( float yaw, float pitch, float roll )
{
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::SetReferenceFrame) begin\n" );
#endif
    unsigned char cmd[8];

    cmd[0] = 0x50;	
    cmd[1] = 0x18;	
    TranslateToBird( cmd+2, (short)( yaw / 180.0 * (double)SHRT_MAX ) );
    TranslateToBird( cmd+4, (short)( pitch / 180.0 * (double)SHRT_MAX ) );
    TranslateToBird( cmd+6, (short)( roll / 180.0 * (double)SHRT_MAX ) );
    if( !MasterSendCommand( cmd, 8 ) )
    {
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::setGroup) unable to set reference frame\n" );
#endif
    	return false;
    }
    
#ifdef DEBUG
    	fprintf( stdout, "(CFlock::SetReferenceFrame) end\n" );
#endif
    return true;
}


bool CFlock::SetBeepOnError( bool beep )
{
    m_beep_on_error=beep;
    return m_beep_on_error;
}

