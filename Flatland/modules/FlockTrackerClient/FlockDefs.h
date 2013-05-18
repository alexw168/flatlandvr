/*
 * flockdefs.h
 *
 * Second header file for tracker.h
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

#ifndef FLOCKDEFS_H
#define FLOCKDEFS_H


#ifdef LINUX
// Standard PC only has two serial ports.
static const char *COMPORT_1	= "/dev/ttyS0";
static const char *COMPORT_2	= "/dev/ttyS1";
#else
static const char *COMPORT_1	= "/dev/ttyd1";
static const char *COMPORT_2	= "/dev/ttyd2";
static const char *COMPORT_3	= "/dev/ttyd3";
#endif //LINUX

const unsigned char XON		= 0x11;
const unsigned char XOFF	= 0x13;

// hemispheres
static const char *BIRD_FORWARD	= "\x00\x00";
static const char *BIRD_AFT	= "\x00\x01";
static const char *BIRD_UPPER	= "\x0C\x01";
static const char *BIRD_LOWER	= "\x0C\x00";
static const char *BIRD_LEFT	= "\x06\x01";
static const char *BIRD_RIGHT	= "\x06\x00";

// change/examine values
const char BIRD_VALUE_XYZREF_FRAME = 0x11;
const char BIRD_VALUE_AUTOCONFIG = 0x32;
const char BIRD_MEASUREMENT_RATE = 0x07;

// command codes (as ASCII characters)
const char BIRD_SYNC		= 'A';
const char BIRD_POINT		= 'B';
const char BIRD_REF_FRAME	= 'H';
const char BIRD_ANG_ALIGN	= 'J';
const char BIRD_HEMISPHERE	= 'L';
const char BIRD_CHANGE		= 'P';
const char BIRD_EXAMINE		= 'O';
const char BIRD_POSITION	= 'V';
const char BIRD_MATRIX		= 'X';
const char BIRD_POSITION_ANGLES	= 'Y';
const char BIRD_POSITION_MATRIX	= 'Z';
const char BIRD_RS232		= 0xF0;

// for 1.0 bird unit = 1.0 ft real world
// set default scaling = 3.0/32768.0 ( 1/(12*32768/36) )
const float DEFAULT_SCALING 	= 9.15527344e-5;

const float DEG_PER_RAD     	= 57.29578f;

#endif //FLOCKDEFS_H
