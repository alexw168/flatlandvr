//
// 
// Bird.cc - Talk to the flock-of-birds
//
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

#ifndef USE_OLD_CPP_HEADERS
#	include <iostream>
#else
#	include <iostream.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
//#include <sys/prctl.h>

#include "Bird.h"

// DS added
#  include "flatland_app.h"

#ifndef USE_OLD_CPP_HEADERS
using namespace std;
#endif

CBird::CBird() :
	m_serialPort(0),
	m_mode(BIRD_POSITION_ANGLES),
	m_Xscale(1.0),
	m_Yscale(1.0),
	m_Zscale(1.0),
	m_XposCent(0.0),
	m_YposCent(0.0),
	m_ZposCent(0.0) 
{	  
	float temp = (float) sysconf( 3 ) / 100.0;
	m_clockTick = (int) (sysconf( 3 ) / 100.0);

	if ((temp - m_clockTick) > 0.0) 
	{
		m_clockTick++;
	}

	if (!m_clockTick) 
	{
		m_clockTick++;
	}
}


CBird::~CBird() 
{
}


bool CBird::getPosAng(float pos[3], float ang[3]) 
{
	if (!sendCommand(BIRD_POINT)) 
	{
		return false;
	}

	unsigned char bufptr[12];
	int total = 0;
	static float values[6];
	
	//  read position and angle from the bird receiver
	while (total < 12) 
	{	
		total += read(m_serialPort, &bufptr[total], 12 - total);
	}

	//  translate strange bird output format to short data
	for (int i = 0; i < 6; i++)
	{
		values[i] = Translate(&(bufptr[i*2])) / (float)SHRT_MAX;
	}
	
	//  scaling 
	pos[0] = values[0] * m_Xscale;
	pos[1] = values[1] * m_Yscale;
	pos[2] = values[2] * m_Zscale;

	//  scaling down from bird format to angels
	ang[0] = values[5] * 180.0;
	ang[1] = values[4] * 180.0;
	ang[2] = values[3] * 180.0;

	return true;
}


bool CBird::getPosMatrix( float pos[3], float matrix[16] ) 
{
	if (!sendCommand(BIRD_POINT))  
	{
		return false;
	}

	// read position and orientation matrix from the bird
	unsigned char bufptr[24];
	int total = 0;
	while (total < 24)
	{
		total+= read(m_serialPort, &bufptr[total], 24-total);
	}

	//  translate strange bird output format to short data
	float values[12];
	for (int i = 0; i < 12; i++)
	{
		values[i] = Translate(&(bufptr[i*2])) / (float)SHRT_MAX;
	}

	// get the matrix
	matrix[0]  = values[3];      
	matrix[1]  = values[4];    
	matrix[2]  = values[5];   
	matrix[3]  = 0.0f;
	matrix[4]  = values[6];   
	matrix[5]  = values[7];   
	matrix[6]  = values[8];   
	matrix[7]  = 0.0f;
	matrix[8]  = values[9];   
	matrix[9]  = values[10];   
	matrix[10] = values[11];   
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;   
	matrix[15] = 1.0f;

	//  get position values
	pos[0] = values[0];
	pos[1] = values[1];
	pos[2] = values[2];

	return true;
}


bool CBird::getMatrix(float matrix[16]) 
{
	if (!sendCommand(BIRD_POINT)) 
	{
		return false;
	}
	
	// Read position and orientation matrix from the bird
	unsigned char bufptr[24];
	int total = 0;
	while (total < 24) 
	{
		total+= read(m_serialPort, &bufptr[total], 24-total);
	}
	
	// translate strange bird output format to short data
	float values[12];
	for (int i = 0; i < 12; i++)
	{
		values[i] = Translate(&(bufptr[i*2])) / (float)SHRT_MAX;
	}

	// get the matrix
	matrix[0]  = values[3];      
	matrix[1]  = values[4];    
	matrix[2]  = values[5];   
	matrix[3]  = 0.0f;
	matrix[4]  = values[6];   
	matrix[5]  = values[7];   
	matrix[6]  = values[8];   
	matrix[7]  = 0.0f;
	matrix[8]  = values[9];   
	matrix[9]  = values[10];   
	matrix[10] = values[11];   
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;   
	matrix[15] = 1.0f;
	matrix[12] = values[1] * m_Xscale - m_XposCent;	// MM(4,1) = Xpos * Sy
	matrix[13] = values[2] * m_Yscale - m_YposCent;	// MM(4,2) = YPos * Sz
	matrix[14] = values[0] * m_Zscale - m_ZposCent;	// MM(4,3) = Zpos * Sx
	matrix[15] = 1.0f;				// MM(4,4) = 1.0

   return true;
}


bool CBird::resetCenterToZero() 
{
	//  set to defaults
	m_XposCent = m_YposCent = m_ZposCent = 0.0;
	
	// set the command byte
	unsigned char RefFrame[13];
	RefFrame[0] = BIRD_REF_FRAME;

	// Sin(z), sin(y),sin(x)
	RefFrame[1] = RefFrame[5] = RefFrame[9] = 0;
	RefFrame[2] = RefFrame[6] = RefFrame[10] = 0;
	
	// Cos(z),cos(y),cos(x)
	RefFrame[3] = RefFrame[7] = RefFrame[11] = 0xFF;
	RefFrame[4] = RefFrame[8] = RefFrame[12] = 0x7F;

	// for RHS - rotate around z axis 90 and x axis -90
	// z: sin(90) and cos(90)
	// x: sin(-90) and cos(-90)
	RefFrame[1] = 0xFF;
	RefFrame[2] = 0x7F;
	RefFrame[9] = 0;
	RefFrame[10] = 0x80;;

	unsigned char cmd[3];
	cmd[0] = BIRD_CHANGE;
	cmd[1] = BIRD_VALUE_XYZREF_FRAME;
	cmd[2] = 1;

	if (!sendCommand(3, cmd)) 
	{
		return false;
	}

	if (!sendCommand(13, RefFrame)) 
	{
   		return false;
	}
	
/************************
	// wait for 40 milliseconds before continuing
#ifdef SGI_SYSTEM
	sginap( 4*m_clockTick );
#else
	usleep(40000);
#endif
**************************/	

	return true;
}


bool CBird::setCenter() 
{
	// this only works in BIRD_POSITION_MATRIX query mode
	if (m_mode != BIRD_POSITION_MATRIX) 
	{
		return false;
	}

	if (!resetCenterToZero()) 
	{
		return false;
	}

	// assign position offsets
	m_XposCent = 0;
	m_YposCent = 0;
	m_ZposCent = 0;

	return true;
}


bool CBird::setQueryMode(char mode) 
{
	if (!sendCommand(mode)) 
	{
		return false;	
	}

	m_mode = mode;
	return true;
}


bool CBird::setHemisphere(const char *hemi) 
{
	unsigned char birdBuffer[3];

	birdBuffer[0] = BIRD_HEMISPHERE;
	birdBuffer[1] = hemi[0];
	birdBuffer[2] = hemi[1];

	if (!sendCommand(3, birdBuffer)) 
	{
		return false;
	}

	return true;
}

void CBird::setScale(float xScale, float yScale, float zScale)
{
	m_Xscale = xScale;
	m_Yscale = yScale;
	m_Zscale = zScale;
}


bool SAloneBird::sendCommand(int n, unsigned char *cmd) 
{
	// since we are stand alone, can just send the command
	int result;

	result = write(m_serialPort, cmd, n);

	if (result < n) 
	{
		return false;
	}

	return true;
}


bool SAloneBird::sendCommand(unsigned char cmd)
{
	// since we are stand alone, can just send the command
	int result;

	result = write(m_serialPort, &cmd, 1);

	if (result < 1) 
	{
		return false;
	}

	return true;
}


//  CFlockBird constructor
CFlockBird::CFlockBird(int add) :
	m_address(add)
{
}


bool CFlockBird::sendCommand(int n, unsigned char *cmd)
{
	int result;
	unsigned char *cmdBuf, *bufPtr;
	
	// since we are in a flock, need to add RS232/FBB command
	cmdBuf = new unsigned char[n+1];
	bzero((unsigned char *)cmdBuf, n+1);
	bufPtr = cmdBuf;
	bufPtr ++;

	cmdBuf[0] = flockPreface();
	// copy over the command
	bcopy(cmd, bufPtr, n);

	result = write(m_serialPort, cmdBuf, n+1);

	if (result < (n+1)) 
	{
		fprintf(stderr, "(CFlockBird::sendCommand) write() failed.\n");
		return false;
	}

	delete [] cmdBuf;

	return true;
}


bool CFlockBird::sendCommand(unsigned char cmd)
{
	int result;

	// since we are in a flock, need to add RS232/FBB command
	unsigned char cmdBuf[2];
	cmdBuf[0] = flockPreface();
	cmdBuf[1] = cmd;

	result = write(m_serialPort, cmdBuf, 2);

	if (result < 2) 
	{
		return false;
	}
   
	return true;
}


unsigned char CFlockBird::flockPreface() 
{
	return BIRD_RS232 + m_address;
}


CFlock::CFlock(int num) :
	m_flocking(false), m_group(false),
	m_numBirds(-1), m_flock(NULL),
	m_serialPort(0), m_mode(BIRD_POSITION_ANGLES),
	m_matrix(NULL), m_ang(NULL), m_pos(NULL),
	m_Xscale(1.0), m_Yscale(1.0), m_Zscale(1.0),
	m_baud(B38400),
	m_commPort(strdup(COMPORT_2)) 
{

	//  set the clock tick for timing
	float temp = (float) sysconf(3) / 100.0;
	m_clockTick = sysconf(3) / 100;
	if ((temp - m_clockTick) > 0.0) 
	{
		m_clockTick++;
	}

	if (!m_clockTick) 
	{
		m_clockTick++;
	}

	if (num == 0) 
	{
		m_numBirds = 1;
		m_flock = (CBird **) (new (SAloneBird (*[m_numBirds])));
		m_flock[0] = new SAloneBird;
		m_matrix[0] = new float[16];
		m_ang[0] = new float [3];
		m_pos[0] = new float [3];
	}
	else 	
	{
		//  running as a flock
		if (num > 0) 
		{
			m_flocking = true;

			//  creat the flock
			m_numBirds = num;
			m_flock = (CBird **) (new (CFlockBird (* [m_numBirds])));
			m_matrix = new (float (* [m_numBirds]));
			m_pos = new (float (*[m_numBirds]));
			m_ang = new (float (*[m_numBirds]));

			for ( int i=0; i<m_numBirds; i++ ) 
			{
				m_flock[i] = new CFlockBird(i+1);
				m_matrix[i] = new float[16];
				m_pos[i] = new float[3];
				m_ang[i] = new float[3];
			}
		}
		else 
		{
			assert(0);
		}
	}
}


CFlock::~CFlock() 
{
	//  free the resources
	if (m_flock) 
	{
		delete [] m_flock;
	}

	for (int i=0; i<m_numBirds; i++) 
	{
		delete [] m_matrix[i];
		delete [] m_pos[i];
		delete [] m_ang[i];
	}

	delete [] m_matrix;
	delete [] m_pos;
	delete [] m_ang;

	// disconnect from the flock
	disconnect();
}


bool CFlock::connect() 
{
	int nResult;
	termios options;

	m_serialPort = open(m_commPort, O_RDWR | O_NONBLOCK | O_NOCTTY, 0666);

	fprintf(stderr, "(CFlock::connect) Opened %s.\n", m_commPort);

	if (m_serialPort < 0) 
	{
		fprintf(stderr, "CFlock: Error connecting to serial port %s\n",
			m_commPort);
		m_serialPort = 0;
		return false;
	}

	tcflush(m_serialPort, TCIOFLUSH);

	if(tcgetattr(m_serialPort, &options) != 0) 
	{
		fprintf(stderr, "CFlock: Couldn't read current settings\n");
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
	cfsetospeed(&options, B38400);
	cfsetispeed(&options, B38400);

	nResult = tcsetattr(m_serialPort, TCSAFLUSH, &options);

	if (nResult < 0) 
	{
		fprintf(stderr, "CFlock: ioctl error establishing serial port\n");
		disconnect();
		return false;
	}

	return true;
}


void CFlock::disconnect() 
{
	if (m_serialPort) 
	{
		close(m_serialPort);
		m_serialPort = 0;
	}
}


bool CFlock::initFlock() 
{
	int i;
	assert(m_serialPort);

	// make sure the flock exists
	if (!m_flock) 
	{
		return false;
	}

	//  need to set Serial Port first in order for sendCommand to work
	//  send the same data mode command to all birds
	for (i = 0; i < m_numBirds; i++) 
	{
		m_flock[i]->setSerialPort(m_serialPort);
		m_flock[i]->setScale(m_Xscale, m_Yscale, m_Zscale);
	}

	if (!autoConfig()) 
	{
		fprintf(stderr, "(CFlock::initFlock) Auto-config failed.\n");
		return false;
	}

	for (i = 0; i < m_numBirds; i++) 
	{
		if (!(m_flock[i]->setQueryMode(m_mode))) 
		{
			fprintf(stderr, "(CFlock::initFlock) setQueryMode() failed.\n");
			return false;
		}
	}

	for (i = 0; i < m_numBirds; i++) 
	{
		if (!(m_flock[i]->setHemisphere(m_hemisphere))) 
		{
			fprintf(stderr, "(CFlock::initFlock) setHemisphere() failed.\n");
			return false;
		}
	}

#if 1
	if (m_group)
	{
		unsigned char changeVal[3];
		changeVal[0] = BIRD_CHANGE;	
		changeVal[1] = 0x23;	//  0x23 -- group mode
		changeVal[2] = 1;	//  1 to enable, 0 to disable
		if (!m_flock[0]->sendCommand(3, changeVal))
		{
			return false;
		}
	}
#endif
	 
	// Query the flock to see if it is turned on
	// This has to happen after the serial ports are set
	if (!queryFlock())
	{
		fprintf(stderr, "CFlock: Flock not responding to query\n");
		return false;
	}

#ifdef DEBUG
    fprintf(stderr, "CFlock::Init - hemis: %s  \n", m_hemisphere);
#endif

#if 1
        // Set the flock measurement rate
	unsigned char changeVal[4];
	changeVal[0] = BIRD_CHANGE;	
	changeVal[1] = BIRD_MEASUREMENT_RATE;
	changeVal[2] = 0xFF & (int) (m_birdMeasRate * 256.0);
	changeVal[3] = 0xFF & (int) m_birdMeasRate;
	if (!m_flock[0]->sendCommand(4, changeVal))
	{
                fprintf (stderr, "Could not set measurement rate\n");
	}
#endif

#if 0
        // Set filter on/off status
	unsigned char filterVal[4];
	filterVal[0] = BIRD_CHANGE;	
	filterVal[1] = 0x04;
	filterVal[2] = 0x00;
	filterVal[3] = 0x04;
	if (!m_flock[0]->sendCommand(4, filterVal))
	{
                fprintf (stderr, "Could not set filter on/off status\n");
	}
#endif


	// DS added a filter
#if 0
        // Change DC filter ALPHA_MIN, set the value less than default 0.02 = 028F Hex
	// greater than 0.008 that is the minimal value not losing mathematical precision
	unsigned char alpha_min_Val[16];
	alpha_min_Val[0] = BIRD_CHANGE;	
	alpha_min_Val[1] = 0x05;
	alpha_min_Val[2] = 0x6F;
	alpha_min_Val[3] = 0xFF;
	alpha_min_Val[4] = 0x6F;
	alpha_min_Val[5] = 0xFF;
	alpha_min_Val[6] = 0x6F;
	alpha_min_Val[7] = 0xFF;
	alpha_min_Val[8] = 0x6F;
	alpha_min_Val[9] = 0xFF;
	alpha_min_Val[10] = 0x6F;
	alpha_min_Val[11] = 0xFF;
	alpha_min_Val[12] = 0x6F;
	alpha_min_Val[13] = 0xFF;
	alpha_min_Val[14] = 0x6F;
	alpha_min_Val[15] = 0xFF;

	fprintf(stderr, "\n CFlock2: DC Filter alpha_min changed \n");
	if (!m_flock[0]->sendCommand(16, alpha_min_Val))
	{
                fprintf (stderr, "Could not set DC Filter ALPHA_MIN value\n");
	}
#endif

	// DS added another filter
#if 0
        // Change DC filter ALPHA_MAX, set the value less than default 0.9 = 07333 Hex
	// but greater than alpha_min required by system
	unsigned char alpha_max_Val[16];
	alpha_max_Val[0] = BIRD_CHANGE;	
	alpha_max_Val[1] = 0xD;
	alpha_max_Val[2] = 0x7F;
	alpha_max_Val[3] = 0xFF;
	alpha_max_Val[4] = 0x7F;
	alpha_max_Val[5] = 0xFF;
	alpha_max_Val[6] = 0x7F;
	alpha_max_Val[7] = 0xFF;
	alpha_max_Val[8] = 0x7F;
	alpha_max_Val[9] = 0xFF;
	alpha_max_Val[10] = 0x7F;
	alpha_max_Val[11] = 0xFF;
	alpha_max_Val[12] = 0x7F;
	alpha_max_Val[13] = 0xFF;
	alpha_max_Val[14] = 0x7F;
	alpha_max_Val[15] = 0xFF;

	fprintf(stderr, "\n CFlock2: DC Filter alpha_max set \n");
	if (!m_flock[0]->sendCommand(16, alpha_max_Val))
	{
                fprintf (stderr, "Could not set DC Filter ALPHA_MAX value\n");
	}
#endif




	//  see if streaming 
	if (m_group)
	{
#if 0
		unsigned char changeVal[3];
		changeVal[0] = BIRD_CHANGE;	
		changeVal[1] = 0x23;		//  0x23 -- group mode
		changeVal[2] = 1;		//  1 to enable, 0 to disable
		
		if (!m_flock[0]->sendCommand(3, changeVal))
		{
			return false;
		}
#endif
	 
		//  get bird data ready
		unsigned char pointVal = BIRD_POINT;

		if (!m_flock[0]->sendCommand(pointVal))
		{
			return false;
		}
	}

	return true;
}

bool CFlock::resetCenterToZero(int add)
{
	// get the block

	// send to only one bird
	if (add > 0)
	{ 
		return m_flock[add - 1]->resetCenterToZero();
	}
	
	// send to all birds
	for (int i = 0; i < m_numBirds; i++)
	{
		if (!(m_flock[i]->resetCenterToZero()))
		{
			return false;
		}
	}

	return true;
}

bool CFlock::setCenter(int add)
{
	// get the block

	// send to only one bird
	if (add > 0) 
	{
		return(m_flock[add - 1]->setCenter());
	}
	
	// send to all birds
	for (int i = 0; i < m_numBirds; i++)
	{
		if (!(m_flock[i]->setCenter())) 
		{
			return false;
		}
	}

	return true;
}

bool CFlock::getPosAng(float **pos, float **ang)
{

	unsigned char *bufptr; // (cpw-3/9/99) more fixes for the 2-bird limit
	unsigned char *cbufptr; // DS added
	short *mybufptr, leftbyte, rightbyte;// DS added
	int total = 0;
	//	float *values; // (cpw - 2/25/99) generalized fix for stack smash problem
	int i,j;


	// (cpw - 2/25/99) fix part two; wouldn't allocate right at declaration
	//	values = new float[6*m_numBirds];
	// (cpw - 3/8/99) buffer fix part two; assume behavior as above
	bufptr = new unsigned char[13*m_numBirds];

	// make sure that we are in the right mode
	assert(m_mode == BIRD_POSITION_ANGLES);

	if (m_mode != BIRD_POSITION_ANGLES)
	{ 
		return false;
	}

	// a point command was already sent from initFlock, so read the bytes
	//	cbufptr = &bufptr[0];
	while (total < 13*m_numBirds)
	{
	  total += read(m_serialPort, &bufptr[total], (13*m_numBirds) - total);
	  //total += read(m_serialPort, cbufptr, (13*m_numBirds) - total);
	}
	
	//  prime the pump for the next read only from the master bird
	unsigned char pointVal = BIRD_POINT;
	if (!m_flock[0]->sendCommand(pointVal))
	{
		return false;
	}

	// Translate strange bird format into interger data
	for (j = 0; j < m_numBirds; j++)
	  {
	    cbufptr = &bufptr[13*j + 0];
	    mybufptr = (short *)(cbufptr);
	    for (i=0;i<6;i++) {
	      rightbyte = (short)(*cbufptr++)&0x7F;
	      leftbyte = (short)(*cbufptr++)<<7;
	      *mybufptr++ = (leftbyte|rightbyte)<<2;
	    }
	    
	    mybufptr = (short *)(&bufptr[13*j + 0]);
	    //	    fprintf(stderr," converted max short %lf \n",(double)SHRT_MAX );
	    
	    for(i=0;i<3;i++) {
	      pos[j][i] = (float)((*mybufptr++) / (double)SHRT_MAX * 3 * 0.3048);
	    }
	    
	    for(i=0;i<3;i++) {
	      ang[j][i] = (float)((double)(*mybufptr++) * (1.0/(double)SHRT_MAX) * 180.0);
	    }
	    
	    //DS added on 3/25/03
	    bcopy(pos[j], m_pos[j], sizeof(float) * 3);
	    bcopy(ang[j], m_ang[j], sizeof(float) * 3); 
	  }
	
	delete [] bufptr;
	//	delete [] values;

	return true;
}

bool CFlock::getPosAng(float pos[3], float ang[3], int add)
{
	// make sure that we are in the right mode
	assert(m_mode == BIRD_POSITION_ANGLES);

	if (m_mode != BIRD_POSITION_ANGLES) 
	{
		return false;
	}
	
	// make sure that we are not past the number of the flock
	assert(add <= m_numBirds);
	if ( add > m_numBirds ) 
	{
		return false;
	}

	if (!(m_flock[add-1]->getPosAng(pos, ang))) 
	{
		return false;
	}

#ifdef DEBUG
	fprintf (stderr, " CFLOCK::getPosAng - position: (%4.2f, %4.2f, %4.2f) angle: (%4.2f, %4.2f, %4.2f)\n\n", pos[0], pos[1], pos[2], ang[0], ang[1], ang[2]);
#endif

	// copy over the values
	bcopy(pos, m_pos[add-1], sizeof(float) * 3);
	bcopy(ang, m_ang[add-1], sizeof(float) * 3);

	return true;
}

bool CFlock::getPosMatrix(float pos[3], float matrix[16], int add)
{
	// make sure that we are in the right mode
	assert(m_mode == BIRD_POSITION_MATRIX);
	if (m_mode != BIRD_POSITION_MATRIX) 
	{
		return false;
	}

	// make sure that we are not past the number of the flock
	assert(add <= m_numBirds);
	if (add > m_numBirds) 
	{
		return false;
	}

	if (!(m_flock[add-1]->getPosMatrix(pos, matrix))) 
	{
		return false;
	}

	bcopy(pos, m_pos[add-1], sizeof(float) * 3);
	bcopy(matrix, m_matrix[add-1], sizeof(float) * 16);

	return true;
}


bool CFlock::getMatrix(float matrix[16], int add)
{
	// make sure that we are not past the number of the flock
	assert(add <= m_numBirds);

	if (add > m_numBirds) 
	{
		return false;
	}

	// copy over the values
	bcopy(m_matrix[add-1], matrix, sizeof(float) * 16);

	return true;
}


void CFlock::setBaud(int baud)
{
	switch(baud) {
	case 2400:
	    m_baud = B2400;
	    break;
	case 4800:
	    m_baud = B4800;
	    break;
	case 9600:
	    m_baud = B9600;
	    break;
	case 19200:
	    m_baud = B19200;
	    break;
	case 38400:
	    m_baud = B38400;
	    break;
	default:
	    fprintf(stdout, "CFlock: Incorrect baud rate has been specified: %d\n",
	    baud);
	    break;
  }
}

bool CFlock::queryFlock(int timeout)
{
	unsigned char examineVal[2];
	examineVal[0] = BIRD_EXAMINE;
	examineVal[1] = 0;

	if (!m_flock[0]->sendCommand(2, examineVal))
	{
		fprintf(stderr, "(CFlock::queryFlock) sendCommand failed.\n");
		return false;
	}

	timeval timeOut;
	timeOut.tv_sec  = timeout;
	timeOut.tv_usec = 0;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_serialPort, &readfds);

	// use select() to see if something came back to the serial port
	int result = select(m_serialPort + 1, &readfds, NULL, NULL, &timeOut);
	cout << "result = " << result << endl;
	//bird_out << "result = " << result << endl;

#if 1
	// was there an error or not responding
	if (result <= 0) 
	{
		return(false);
	}
	cout << "first test passed -  select" << endl;
	//bird_out << "first test passed -  select" << endl;

	if (FD_ISSET(m_serialPort, &readfds) == 0) 
	{
		return(false);
	}
#endif
	cout << "second test passed - ISSET" << endl;
	//bird_out << "second test passed - ISSET" << endl;

	//  read data at port.  There may be garbage form other commands here so
	//  try to read everything and only concern with the last 2 bytes
	unsigned char *bufPointer, bufptr[150];
	int bytes = 0, readBytes = 0, i = 0, maxTries = 5;

	i = 0;
	bzero((unsigned char *) bufptr, 150);
	bufPointer = bufptr;
	while (((bytes = read(m_serialPort, bufPointer+bytes, 150)) > 0) && (i < maxTries))
	{
//#if DEBUG_BIRD
		fprintf(stderr, "bytes = %d\n", bytes);
		fprintf(stderr, "value = %s\n", bufPointer);
//#endif

		i++;
		readBytes = bytes;
	}

	if (readBytes == 0)
	{
		return(false);
	}

//#if DEBUG_BIRD
	if (i < maxTries)
	{
		fprintf(stderr, "   read eof i = %d\n", i);
	}
	else
	{
		fprintf(stderr, "   i = maxTries\n");
	}

	fprintf(stderr, "   readBytes = %d\n", readBytes);

#if 0
	if (readBytes < 2)
	{
		bufPointer = bufptr + readBytes - 1;
	}
	else
	{
		bufPointer = bufptr + readBytes - 2;
	}
#endif

	unsigned short  *dummy;
	dummy =  (unsigned short *)bufPointer;

	fprintf(stderr, "BIRD_EXAME: buf: %x \n", *dummy);
	fprintf(stderr, "BIRD_EXAME: [0]= 0x%x [1]= 0x%x\n", bufPointer[0], bufPointer[1]);

	fprintf(stderr, "\nRaw bytes -> '");
	int num = 8 * 2;
	int loc = 2 - 1;
	int shift = 7;
	int mask;

	for (int j = 1; j <= num; j++)
	{
		mask = 1 << shift;
		if ((bufPointer[loc] & mask) == 0)
		{
			fprintf(stderr, "0");
		}
		else
		{
			fprintf(stderr, "1");
		}
		shift--;
		if ((j % 8 == 0) && (j != num))
		{
			fprintf(stderr, " ");
			loc--;
			shift = 7;
		}
	}
	fprintf(stderr, "' <-\n\n");

	//  Interpret data
	//  bit 0
	int mask1 = 1, value = 0;

	if ((bufPointer[0] & mask1) == 0)
	{
		fprintf(stderr, "0 POINT mode selected\n");
	}
	else
	{
		fprintf(stderr, "1 STREAM mode selected\n");
	}
	
	//  bit 1, 2, 3, 4
	mask1 = 1 << 1;
	if ((bufPointer[0] & mask1) != 0)
	{
		value = 1;
	}
	mask1 = 1 << 2;
	if ((bufPointer[0] & mask1) != 0)
	{
		value += 2;
	}
	mask1 = 1 << 3;
	if ((bufPointer[0] & mask1) != 0)
	{
		value += 4;
	}
	mask1 = 1 << 4;
	if ((bufPointer[0] & mask1) != 0)
	{
		value += 8;
	}
	
	switch (value)
	{
 		case 1:
		  fprintf(stderr, "0001 POSITION outputs selected\n");
		  break;
 		case 2:
		  fprintf(stderr, "0010 ANGLE outputs selected\n");
		  break;
 		case 3:
		  fprintf(stderr, "0011 MATRIX outputs selected\n");
		  break;
 		case 4:
		  fprintf(stderr, "0100 POSITION/ANGLE outputs selected\n");
		  break;
 		case 5:
		  fprintf(stderr, "0101 POSITION/MATRIX outputs selected\n");
		  break;
 		case 6:
		  fprintf(stderr, "0110 factory use only\n");
		  break;
 		case 7:
		  fprintf(stderr, "0111 QUATERNION outputs selected\n");
		  break;
		case 8:
		  fprintf(stderr, "1000 POSITION/QUATERNION outputs selected\n");
		  break;
		default:
		  fprintf(stderr, "%d UNKNOWN type selected\n",value);
	}

	//  bit 5
	mask1 = 1 << 5;
	if ((bufPointer[0] & mask1) == 0)
	{
		fprintf(stderr, "0 the Bird is in RUN mode\n");
	}
	else
	{
		fprintf(stderr, "1 The Bird is in SLEEP mode\n");
	}
	
	//  bit 6
	mask1 = 1 << 6;
	if ((bufPointer[0] & mask1) == 0)
	{
		fprintf(stderr, "0 XON\n");
	}
	else
	{
		fprintf(stderr, "1 XOFF\n");
	}
	
	//  bit 7
	mask1 = 1 << 7;
	if ((bufPointer[0] & mask1) == 0)
	{
		fprintf(stderr, "0 The Bird Commands are enable\n");
	}
	else
	{
		fprintf(stderr, "1 factory test and Bird Commands are enable\n");
	}
	
	//  bit 8
	mask1 = 1;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Sync mode enable\n");
	}
	else
	{
		fprintf(stderr, "1 NO Sync mode enable\n");
	}
	
	//  bit 9
	mask1 = 1 << 1;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Not in CRTSYNC mode \n");
	}
	else
	{
		fprintf(stderr, "1 CRTSYNC mode \n");
	}
	
	//  bit 10
	mask1 = 1 << 2;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Normal address node enable\n");
	}
	else
	{
		fprintf(stderr, "1 Expanded address node enable\n");
	}
	
	//  bit 11
	mask1 = 1 << 3;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 not in HOST SYNC mode\n");
	}
	else
	{
		fprintf(stderr, "1 HOST SYNC mode\n");
	}
	
	//  bit 12
	mask1 = 1 << 4;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Bird is not RUNNING\n");
	}
	else
	{
		fprintf(stderr, "1 Bird is RUNNING\n");
	}
	
	//  bit 13
	mask1 = 1 << 5;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 NO ERROR is detected\n");
	}
	else
	{
		fprintf(stderr, "1 ERROR is detected\n");
	}
	
	//  bit 14
	mask1 = 1 << 6;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Bird has not been initialized\n");
	}
	else
	{
		fprintf(stderr, "1 Bird has been initialized\n");
	}
	
	//  bit 15
    mask1 = 1 << 7;
	if ((bufPointer[1] & mask1) == 0)
	{
		fprintf(stderr, "0 Bird is a SLAVE Bird\n");
	}
	else
	{
		fprintf(stderr, "1 Bird is a MASTER Bird\n");
	}
	
//#endif

	return true;
}

bool CFlock::autoConfig()
{
	// the auto-config command only applies to flock, not standalone
	if (m_flocking)
	{
		// setup the command
		unsigned char params[3];
		params[0] = BIRD_CHANGE;
		params[1] = BIRD_VALUE_AUTOCONFIG; 		//  50
		params[2] = m_numBirds;

		// Must wait > 300ms before sending auto-config to tracker.
		usleep(400000);

		// have the master bird send it
		if ( !m_flock[0]->sendCommand(3, params) ) 
		{
			return false;
		}

		// Must wait > 300ms after sending auto-config to tracker.
		usleep(400000);
	}

	return true;
}

void CFlock::setMeasRate( const float rate )
{
//        cout << "rate = " << rate << ", m_birdMeasRate = " << m_birdMeasRate << endl;
        m_birdMeasRate = rate;
//        cout << "m_birdMeasRate = " << m_birdMeasRate << endl;
}

bool CFlock::setCRTSync(unsigned char sync)
{
	// have the master bird send it
	unsigned char cmd[2];
	cmd[0] = BIRD_SYNC;
	cmd[1] = sync;
	if (!m_flock[0]->sendCommand(2, cmd)) 
	{
		return false;
	}
	
	return true;
}

void CFlock::setScale(float xScale, float yScale,  float zScale)
{
	m_Xscale = xScale;
	m_Yscale = yScale;
	m_Zscale = zScale;
}
