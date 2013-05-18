//////////////////////////////////////////////////////////////
//
//  FlockTrackerClient.h
//
//  Header file for FlockTrackerClient.cc
//
//  Timothy B. Eyring - 7/4/00
//  KLS: added bird measurement rate variable - 8/8/00
//  TBE: uses TrackerClient "API" - 8/25/00
//

#ifndef FLOCKTRACKERCLIENT_H
#define FLOCKTRACKERCLIENT_H

typedef struct FlockTrackerClientDataMembers
{
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
#define MAX_NUMBER_OF_TRANSDUCERS 4

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

