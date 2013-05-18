/*
 * Bird.h
 *
 * Header file for Bird.cc
 */

#ifndef BIRD_H
#define BIRD_H


#ifndef USE_OLD_CPP_HEADERS
#	include <iostream>
#else
#	include <iostream.h>
#endif

#include <string.h>
#include "FlockDefs.h"


// CBird is an abstract class
class CBird {

   public:
   
      CBird( );
      virtual ~CBird( );

      virtual bool sendCommand( int n, unsigned char *command ) = 0;
      virtual bool sendCommand( unsigned char command ) = 0;

      bool	getPosAng( float pos[3], float ang[3] );
      bool	getPosMatrix( float pos[3], float matrix[16] );
      bool	getMatrix( float matrix[16] );

      bool	resetCenterToZero( );
      bool	setCenter( );
      void	setSerialPort( int port ) { m_serialPort = port; };
      bool	setQueryMode( char mode );
      void	setScale( float xScale, float yScale, float zScale );
      bool	setHemisphere( const char *hemi );

   protected:
   
      int	m_serialPort;
      int	m_clockTick;
      char	m_mode;
      float	m_XposCent, m_YposCent, m_ZposCent;
      float	m_Xscale, m_Yscale, m_Zscale;

};


//  Derived class of CBird
class SAloneBird : public CBird {

   public:
   
      bool	sendCommand( int n, unsigned char *cmd );
      bool	sendCommand( unsigned char cmd );

};


//  Derived class of CBird
class CFlockBird : public CBird {

   public:
   
      CFlockBird( int add );
      bool	sendCommand( int n, unsigned char *cmd );
      bool	sendCommand( unsigned char cmd );
		
   protected:
   
      unsigned char flockPreface( );
      int	m_address;

};


//  A Flock of birds
class CFlock {

   public:
   
      CFlock( int num_birds = 0 );
      ~CFlock( );
      bool	connect( );
      void	disconnect( );
      bool	initFlock( );	// initialize the flock (resets everything)
      
      //  before connect
      void	setBaud( int baud );
      void	setComPort( const char *port ) { m_commPort = strdup( port ); };
      
      //  before init Flock
      void	setQueryMode( char mode ) { m_mode = mode; };
      
      //  set grouping
      void	setGroup(const int group) { m_group = group; };
      
      //  set hemisphere
      void	setHemisphere(const char *hemi) {
		   m_hemisphere[0] = hemi[0];
		   m_hemisphere[1] = hemi[1]; };
      
      //  set scale for position values
      void	setScale( float xScale, float yScale, float zScale );
      
      //  after init Flock
      bool	resetCenterToZero( int add = 0 );
      
      //  set center position/rotation of the flock
      bool	setCenter( int add = 0 );
      
      //  set the sync mode: 0-> don't use, 1-> high sync, 2-> normal sync
      bool	setCRTSync( unsigned char sync );
      
      //  set the measurement frequency
      //void      setMeasRate(const float rate) { m_birdMeasRate = rate; };
      //void      setMeasRate(const float rate) { std::cout << "rate = " << rate << ", m_birdMeasRate = " << m_birdMeasRate << endl; };
      void      setMeasRate( const float rate);

      //  get trackers position and angles
      bool	getPosAng( float pos[3], float ang[3], int add = 1 );
      bool	getPosAng( float **pos, float **ang );
      bool	getPosMatrix( float pos[3], float matrix[16], int add = 1 );
      bool	getMatrix( float matrix[16], int add = 1 );

   protected:
   
      bool	queryFlock( int timeout = 5 );
      bool	autoConfig( );
		
   private:
   
      bool	m_flocking;
      int	m_numBirds;
      int	m_group;
      float     m_birdMeasRate;
      CBird	**m_flock;

      //  flock specific info
      char	m_mode;
      char	m_hemisphere[2];
      float	m_Xscale, m_Yscale, m_Zscale;
		
      //  communication info
      int	m_baud;
      char	*m_commPort;
      int	m_serialPort;
      int	m_clockTick;

      float	**m_matrix;
      float	**m_pos;
      float	**m_ang;

};


inline short Translate( unsigned char *number ) {
   return (short)((((short)number[0] << 1) | ((short)number[1] << 8)) << 1);
}


// Absolute value float
inline float ABS( float x ) {
      return ( (x<0) ? -x : x );
}


// Absolute value int
inline int ABS( int x ) {
      return ( (x<0) ? -x : x );
}


#endif
