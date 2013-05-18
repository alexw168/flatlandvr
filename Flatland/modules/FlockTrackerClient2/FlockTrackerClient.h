//////////////////////////////////////////////////////////////
//
//  FlockTrackerClient.h
//
//  Header file for FlockTrackerClient.cc
//  
//  Copyright (C) 2003, University of New Mexico.
//  
//      This library is free software; you can redistribute it and/or
//      modify it under the terms of the GNU Lesser General Public
//      License as published by the Free Software Foundation; either
//      version 2.1 of the License, or (at your option) any later version.
//  
//      This library is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//      Lesser General Public License for more details.
//  
//      You should have received a copy of the GNU Lesser General Public
//      License along with this library; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//
//  Timothy B. Eyring - 7/4/00
//  KLS: added bird measurement rate variable - 8/8/00
//  TBE: uses TrackerClient "API" - 8/25/00
//

#ifndef FLOCKTRACKERCLIENT_H
#define FLOCKTRACKERCLIENT_H

#define MAX_NO_TRANSDUCERS 5

typedef struct FlockTrackerClientDataMembers
{
  // added by Andrei to clamp and shift values
  float max_x[MAX_NO_TRANSDUCERS];
  float max_y[MAX_NO_TRANSDUCERS];
  float max_z[MAX_NO_TRANSDUCERS];
  float min_x[MAX_NO_TRANSDUCERS];
  float min_y[MAX_NO_TRANSDUCERS];
  float min_z[MAX_NO_TRANSDUCERS];
  float shift_x[MAX_NO_TRANSDUCERS];
  float shift_y[MAX_NO_TRANSDUCERS];
  float shift_z[MAX_NO_TRANSDUCERS];

  /////// STAY OUT OF THESE ///////////////////////////////////////////////
  float birdMeasRate;       // record of the desired bird measurement rate
  CORDORD *Tracker;	    // array of transducer positions
  CORDORD Position;	    // tracker base position (where is the trackers origin)
  char ServerName[MAX_NAME_SIZE];		// tracker server
  bool Ready;					// is everything ready
  int NumberOfTransducers;			// how many transducers does the server have?
  bool DisplayAxes;
  bool UseTracking;
  
  int polyflag;
  float alpha;
  float x_beta[3][3];
  float y_beta[3][3];
  float z_beta[3][3];
  float ro_beta[3][3];
  float ph_beta[3][3];
  float yw_beta[3][3];
  float readTime;

  CORDORD * curr_filted;
  CORDORD * prev_filted;
  
} FLOCK_TRACKER_CLIENT_DATA_MEMBERS;

// DS added for prediction purpose

void predictedPosAng(OBJECT_TYPE *TrackerClientObject);
void predictedCoeff(OBJECT_TYPE *TrackerClientObject);

//static int polyflag = 0;
#define MAXQUEUE 10
#define NUMBER_OF_TRANSDUCERS 3

typedef struct samplePoint {
  float position[NUMBER_OF_TRANSDUCERS][3];
  float angles[NUMBER_OF_TRANSDUCERS][3];
  float time;
} SamplePoint;

typedef SamplePoint queueEntry;;
typedef struct queue {
  int count;
  int front;
  int rear;
  queueEntry entry[MAXQUEUE];
} Queue;

Queue *CQ;
Queue *tq;

float predictedPos[NUMBER_OF_TRANSDUCERS][3];
float predictedAng[NUMBER_OF_TRANSDUCERS][3];
float storedtime[MAXQUEUE];


#endif //ndef FLOCKTRACKERCLIENT_H

