/*
 * Bird.h
 *
 * Header file for Bird.cc
 * 
 * Copyright (C) 2003, University of New Mexico.
 * 
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2.1 of the License, or (at your option) any later version.
 * 
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef BIRD_H
#define BIRD_H

//#include "FlockDefs.h"

//  A Flock of birds
class CFlock 
{
public:
    CFlock( char *serial_port="/dev/ttyS0", int baud_rate=38400 );
    ~CFlock( );

    // --- translation functions -----------------------------------------
    static unsigned char    *TranslateToBird( unsigned char *dest, short word );
    static short   	    TranslateFromBird( unsigned char *number );
    static char     	    *TranslateBirdStatus( char *dest, int word );
         
    // --- get --------------------------------------------------------
    bool    	    IsUp( );
    int     	    GetNumberOfBirds( );
    int     	    GetNumberOfReceivers( );
    int     	    GetNumberOfExtendedRangeTransmitters( );
    int     	    Examine_BirdStatus( int bird );
    
    int    	    Read_Data( unsigned char *buf, int bytes, long int timeout=250000 );
    
    //  before init Flock
    bool	    SetQueryMode( char mode );

    bool	    SetSleep( );
    bool	    SetGroup( const int group );
    bool	    SetHemisphere( unsigned char *hemi );
    bool	    SetHemisphere( unsigned char hemi0, unsigned char hemi1 );

    bool    	    MasterSendCommand( unsigned char cmd );
    bool    	    MasterSendCommand( unsigned char *cmd, int size );
    bool    	    BirdSendCommand( int bird, unsigned char cmd );
    bool    	    BirdSendCommand( int bird, unsigned char *cmd, int size );

    bool      	    SetMeasurementRate( const float rate);

    bool	    RequestData( ); // poll flock for data
    bool	    GetPosAng( float **pos, float **ang );
    bool	    Read_PositionAngle( float **position, float **angle );

//    bool	    FlockQuery( int timeout = 5 );
    bool	    FBBReset( );
    bool    	    ClearSerialPort( long int timeout=250000 );

    // first pass at streaming code
    bool    	    StartStream( );
    bool    	    StopStream( );
    bool      	    SetReportRate( const int rate);

    bool    	    SetReferenceFrame( float yaw, float pitch, float roll );

    bool    	    SetBeepOnError( bool beep );
    
private:

    //  before connect
    void	    SetBaud( int baud );
    void	    SetComPort( const char *port );

    bool	    StartSerialPort( );
    void	    StopSerialPort( );
    bool    	    SerialClearRTS( );
    bool    	    SerialSetRTS( );
    bool    	    SerialWaitForCTSSet( bool timeout );
    bool    	    SerialWaitForCTSClear( bool timeout );

    bool    	    CheckFlockSystemStatus( );
    bool    	    FlockAutoConfig( );
    bool	    FlockInitialize( );	// initialize the flock (resets everything)
//    bool    	    AutomaticallyDetectBaudRate( );
    
    bool      	    m_extendedRangeTransmitter;
    int	    	    m_numBirds;
    int	    	    m_group;
    float     	    m_birdMeasRate;

    //  flock specific info
    unsigned char   m_mode;
    unsigned char   m_hemisphere[2];

    //  communication info
    int	    	    m_baudRate;
    char	    *m_serialPortName;
    int	    	    m_serialPort;

    unsigned char   m_flockSystemStatus[14];
    int     	    m_flockNumberOfBirds;
    int     	    m_flockNumberOfReceivers;
    int     	    m_flockNumberOfTransmitters;
    int     	    m_flockNumberOfExtendedRangeTransmitters;

    float   	    m_flockPosition[14][3];
    float   	    m_flockAngle[14][3];
    float   	    m_flockMatrix[14][16];
    float   	    m_flockQuaternion[14][4];
    
    unsigned char   m_receiveBuffer[14*25];    
    bool    	    m_up;
    bool    	    m_beep_on_error;
};

#endif
