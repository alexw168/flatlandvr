/* This file is basically same as FlockTrackerClient.cc except removing 
   error analysis part that is used for my thesis
   Modified for three receivers 12-4-02 
*/

// filter were added
/* modified from FlockTrackerClient.cc.9-5
   compare the predicted value with the true values. 
   one comparison happens in Tracker thread, the other one happens in the graphics loop
*/
/*
TrackerClientClient.cc  An app providing access to position tracking

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
// Maintenance history
//
// Created: 7-4-00, TBE
// Added measurement rate input:  8-8-00, KLS
// modified by Dongshan, add prediction
// for compare the result with prediction and without prediction, predictive count should be 
// accumulate correctly

// include the master Flatland library 
#include "flatland_app.h"
#include "FlockDefs.h"
#include "Bird.h"
#include "FlockTrackerClient.h"

//#define POLL_DELAY 10000 // if this value is used, it will be very jittering

#define POLL_DELAY 80000

// Max number of birds to read, used to size buffers
#define MAX_BIRDS 6
#define BIRD_BAUD 38400
// How many raw trackers managed (by default)
//#define NUMBER_OF_TRANSDUCERS 3

// Which serial port
#define TRACKER_CLIENT_SERIAL_PORT "/dev/ttyd2"  
// This can be changed from the Flatland.config file

/* ------ Dongshan add circular queue and regression algorithm ---------*/
/*
void predictedPosAng(OBJECT_TYPE *TrackerClientObject);
void predictedCoeff(OBJECT_TYPE *TrackerClientObject);

static int polyflag = 0;
#define MAXQUEUE 10
#define samplelen 3000

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


//float origin[samplelen][NUMBER_OF_TRANSDUCERS][7];
//float real[samplelen][NUMBER_OF_TRANSDUCERS][6];
//float predicted[samplelen][NUMBER_OF_TRANSDUCERS][7];
//float real_time[samplelen];

float predictedPos[NUMBER_OF_TRANSDUCERS][3];
float predictedAng[NUMBER_OF_TRANSDUCERS][3];
float storedtime[MAXQUEUE];
*/

// operations on queue
void createQ(Queue * q) {
  q->count = 0;
  q->front = 0;
  q->rear = -1;  
}

bool queueFull(Queue * q)
{
  return (q->count >= MAXQUEUE);
}

bool queueEmpty(Queue * q)
{
  return (q->count <= 0); 
}

void append(queueEntry x, Queue * q) 
{
  if (queueFull(q)) {
    q->count = MAXQUEUE;
    q->front = (q->front + 1) % MAXQUEUE;
  }
  else
    q->count++;
  q->rear = (q->rear + 1) % MAXQUEUE;
  q->entry[q->rear] = x;
}

void serve(queueEntry *x, Queue * q)
{
  if(queueEmpty(q))
    fprintf(stderr, " Empty queue, can not serve\n");
  else {
    q->count--;
    *x = q->entry[q->front];
    q->front = (q->front + 1) % MAXQUEUE;
  }
}

queueEntry getFront(Queue * q) {
  queueEntry value;
  if(queueEmpty(q)){
    fprintf(stderr, " Empty Queue \n");
  }
  else {
    value = q->entry[q->front];
  }
  return value;
}

void PrintNode(queueEntry x)
{
  int i;
  for (i = 0; i < NUMBER_OF_TRANSDUCERS; i++) {
    fprintf(stderr," entry %f %f %f %f %f %f %f \n ", x.position[i][0], x.position[i][1], x.position[i][2],  x.angles[i][0], x.angles[i][1], x.angles[i][2], x.time);
  }
  fprintf(stderr, " print one entry time  %f \n", x.time);
}

void traverse(Queue *q, void(*Visit)(queueEntry x)) {
  int pos, entries;
  pos = q->front;
  entries = q->count;
  if(queueEmpty(q))
    fprintf(stderr," empty queue\n");
  else {
    while(entries)
      {
	(*Visit)(q->entry[pos]);
	pos = (pos + 1) % MAXQUEUE;
	entries--;
      }
  }
}

void adjustTime(Queue *q) {
  
  if(queueEmpty(q))
    fprintf(stderr," empty queue\n");
  else {
    q->entry[q->front].time = 0;
  }
}

queueEntry * getStoreData(Queue * q) {
  int pos, entries, j;
  pos = q->front;
  entries = q->count;
  queueEntry * data;
  data = (queueEntry*)malloc(sizeof(queueEntry) * MAXQUEUE);
  if(data == NULL)
    exit(1);
  if(queueEmpty(q))
    fprintf(stderr," empty queue\n");
  else {
    while(entries)
      {
	for (j = 0; j < NUMBER_OF_TRANSDUCERS; j++) {
	  //	  fprintf(stderr," GET STORED DATA \n");
	  data[q->count - entries].position[j][0] = q->entry[pos].position[j][0];
	  data[q->count - entries].position[j][1] = q->entry[pos].position[j][1];
	  data[q->count - entries].position[j][2] = q->entry[pos].position[j][2];
	  data[q->count - entries].angles[j][0] = q->entry[pos].angles[j][0];
	  data[q->count - entries].angles[j][1] = q->entry[pos].angles[j][0];
	  data[q->count - entries].angles[j][0] = q->entry[pos].angles[j][0];
	  //	  fprintf(stderr," GET STORED DATA for j \n");
	}
	data[q->count - entries].time = q->entry[pos].time;
	pos = (pos + 1) % MAXQUEUE;
	entries--;
      }
  }
  return data;
}

/* ---------------------- end of circular queue -------------------------- */

/* ---------------------- Regression Algorithm --------------------------- */
// Gauss Jondon method

void gjswap(float *s1,float *s2)
{
   float temp;

   temp = (*s1);
   (*s1) = (*s2);
   (*s2) = temp;
}

void GaussJordan( float *coefary,float *constary,
                  int numcol,float *solcoef, float *invary,
                  float *det)
{
   int pivlst[50][2];
   char pivchk[50];
   int i;
   int j;
   int k;
   int l;
   int lerow;
   int lecol;
   int l1;
   float piv;
   float t;
   float leval;

   (*det) = 1;
   for ( i = 0; i <= numcol-1; ++i ) {
      pivchk[i] = 0;
      for ( j = 0; j <= numcol-1; ++j ) {
         invary[i*numcol+j] = coefary[i*numcol+j];
               }
   }
   for ( i = 0; i <= numcol-1; ++i ) {
      leval = 0.0;
      for ( j = 0; j <= numcol-1; ++j ) {
         if ( ! (pivchk[j]) ) {
            for ( k = 0; k <= numcol-1; ++k ) {
               if ( ! (pivchk[k]) ) {
                  if ( fabs(invary[j*numcol+k]) > leval ) {
                     lerow = j;
                     lecol = k;
                     leval = fabs(invary[j*numcol+k]);
                  }
               }
            }
         }
      }
      pivchk[lecol] = 1;
      pivlst[i][0] = lerow;
      pivlst[i][1] = lecol;
      if ( lerow != lecol ) {
	(*det) = -(*det);
	for ( l = 0; l <= numcol-1; ++l ) {
	  gjswap(&invary[lerow*numcol+l],&invary[lecol*numcol+l]);
	}
	gjswap(&constary[lerow],&constary[lecol]);
      }
      piv = invary[lecol*numcol+lecol];
      (*det) = (*det) * piv;
      if ( (*det) > 1.0e+20 ) {
	(*det) = 1.0e+30;
      }
      invary[lecol*numcol+lecol] = 1.0;
      for ( l = 0; l <= numcol-1; ++l ) {
	invary[lecol*numcol+l] = invary[lecol*numcol+l] / piv;
      }
      constary[lecol] = constary[lecol] / piv;
      for ( l1 = 0; l1 <= numcol-1; ++l1 ) {
         if ( l1 != lecol ) {
            t = invary[l1*numcol+lecol];
            invary[l1*numcol+lecol] = 0.0;
            for ( l = 0; l <= numcol-1; ++l ) {
               invary[l1*numcol+l] = invary[l1*numcol+l] -
                                     invary[lecol*numcol+l] * t;
            }
            constary[l1] = constary[l1] - constary[lecol] * t;
         }
      }
   }
   for ( i = 0; i <= numcol-1; ++i ) {
     l = numcol - i - 1;
     if ( pivlst[l][0] != pivlst[l][1] ) {
       lerow = pivlst[l][0];
       lecol = pivlst[l][1];
       for ( k = 0; k <= numcol-1; ++k ) {
	 gjswap(&invary[k*numcol+lerow],&invary[k*numcol+lecol]);
       }
     }
   }
   for ( i = 0; i <= numcol-1; ++i ) {
     solcoef[i] = constary[i];
   }
} // end of gauss jordon method 

// start regression algorithm
void  MatTxTiX(float *aryin,int numrow,int numcol,float *aryout)
{
   int i;
   int j;
   int k;

   for ( i = 0; i <= numcol-1; ++i ) {
      for ( j = 0; j <= numcol-1; ++j ) {
         aryout[i*numcol+j] = 0.0;
         for ( k = 0; k <= numrow-1; ++k ) {
            aryout[i*numcol+j] = aryout[i*numcol+j] +
                                 aryin[k*numcol+j] * aryin[k*numcol+i];
         }
      }
   }
}

void MatYTiX(float *aryy,float *aryx,int numrow,
		    int numcol,float *aryout)
{
   int i;
   //   int j;
   int k;

   for ( i = 0; i <= numcol-1; ++i ) {
      aryout[i] = 0.0;
      for ( k = 0; k <= numrow-1; ++k ) {
         aryout[i] = aryout[i] + aryy[k] * aryx[k*numcol+i];
      }
   }
}

void MultipleReg(float *indvardat,float *depvardat,int numiv,int numobs,
		 float *regcoef)
{
   float  *regmat;
   float  *aryinv;
   float  *arya;
   float  *aryg;

   int numcol;
   int i;
   int j;
   float regdet;
   
   numcol = numiv + 1;

   regmat = (float *) calloc((numobs)*numcol,4);
   aryinv = (float *) calloc(numcol*numcol,4);
   arya = (float *) calloc(numcol*numcol,4);
   aryg = (float *) calloc(numcol,4);

   for ( i = 0; i <= numobs-1; ++i ) {
      for ( j = 0; j <= numiv-1; ++j ) {
         regmat[i*numcol + j + 1] = indvardat[i*numiv + j];
      }
      regmat[i*numcol] = 1;
   }
   MatTxTiX(regmat,numobs,numcol,arya);
   MatYTiX(depvardat,regmat,numobs,numcol,aryg);

   GaussJordan(arya,aryg,numcol,regcoef,aryinv,&regdet);
   free(regmat); free(aryinv); free(arya); free(aryg);
}

/* -------------------------- end of regression algorithm ---------------- */

/* --------------------------- help function ------------------------------*/
/*
float stdev(float *indv) {
  int i;
  float value;
  float sum = 0;
  for (i = 0; i< samplelen - 10; i++) {
    sum = sum + indv[i]*indv[i];
  }
  value = sqrt(sum/(samplelen - 10));
  return value;
}

float max_diff(float *indv) {
  int i;
  float value = 0;
  for (i = 0; i< samplelen - 10; i++) {
    if(value < fabsf(indv[i]))
      value = fabsf(indv[i]);
  }
  return value;
}
*/

float get_predicted_time() {
  float interval;

  int i, k, currentFront ;
  float indvar[MAXQUEUE-1];
  float depvar[MAXQUEUE-1];
  float swaptime[MAXQUEUE];
  float beta[2];
  int num = 1;
  int numob = MAXQUEUE - 1;

  currentFront = tq->front;
  
  for(k = 0; k < MAXQUEUE; k++){
    swaptime[k] = tq->entry[currentFront].time;
    currentFront = (currentFront + 1) % MAXQUEUE;
  }
  
  for (i = 0; i < MAXQUEUE - 1; i++) {
    indvar[i] = i;
    depvar[i] = swaptime[i+1] - swaptime[i];
  }
  MultipleReg(indvar, depvar, num, numob,beta);
  interval = beta[0] + beta[1] * (MAXQUEUE - 1);
  //  fprintf(stderr," last interval = %f \n",interval);
  return interval;
}


/* end of help function */

// Function to read transducer status 
void *TrackerClientThread(void *X)
{
  OBJECT_TYPE *TrackerClientObject = (OBJECT_TYPE *) X;
  //	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) TrackerClientObject->ptr0;
  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *fdata = FlockClientDataMembers;
  char *Name = TrackerClientObject->name;
  CFlock *Flock;
  CORDORD *Tracker; // = TrackerClientMembers->Tracker;

  CORDORD *curr_filted; // DS added for filted data;
  CORDORD *prev_filted; // DS added for filted data;
  
  float **pos, **ang;
  bool flock_connected;
  bool flock_initialized;
  float *real_triplets;
  bool bird_on_flag = false;
  int i;
  
  /* ----------------  Dongshan add ------------------------------------ */
  CQ = (Queue *)malloc(sizeof(Queue));
  if(CQ == NULL) {
    fprintf(stderr," can not allocate memory for position data queue\n");
    exit(1);
  }
  createQ(CQ);
  
  tq = (Queue *)malloc(sizeof(Queue));
  if(tq == NULL) {
    fprintf(stderr," can not allocate memory for position data queue\n");
    exit(1);
  }
  createQ(tq);
  /* -------------------------------------------------------------------- */
  
  // Set up raw tracker data spaces
  if ((Tracker = FlockClientDataMembers->Tracker = new CORDORD[FlockClientDataMembers->NumberOfTransducers]) == NULL)
    {
      fprintf (stderr, "%s: Cannot allocate memory for tracker data\n", Name);
      exit (1);
    }

  // DS added for Setting up filtering data spaces
  if ((curr_filted = FlockClientDataMembers->curr_filted = new CORDORD[FlockClientDataMembers->NumberOfTransducers]) == NULL)
    {
      fprintf (stderr, "Cannot allocate memory for filted data\n");
      exit (1);
    }
  
  if ((prev_filted = FlockClientDataMembers->prev_filted = new CORDORD[FlockClientDataMembers->NumberOfTransducers]) == NULL)
    {
      fprintf (stderr, "Cannot allocate memory for more filted data\n");
      exit (1);
    }
  float alpha = FlockClientDataMembers -> alpha;  
  // DS added
  static int firstread = 0;
  
  // init data
  for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
    {
      Tracker[i].x = 0.0;
      Tracker[i].y = 0.0;
      Tracker[i].z = 0.0;
      Tracker[i].roll = 0.0;
      Tracker[i].pitch = 0.0;
      Tracker[i].yaw = 0.0;
    }
  
  // Allocate pointers-to-pointers
  pos = new (float (*[FlockClientDataMembers->NumberOfTransducers]));
  ang = new (float (*[FlockClientDataMembers->NumberOfTransducers]));
  
  // Allocate continuous data blocks that will actually store
  // the data.
  real_triplets = new float[FlockClientDataMembers->NumberOfTransducers*6];
  
  // set pointers into continuous data block
  for(i = 0; i < FlockClientDataMembers->NumberOfTransducers; i++)
    {
      pos[i] = &(real_triplets[3 * i]);
      ang[i] = &(real_triplets[(3 * i) + (3 * FlockClientDataMembers->NumberOfTransducers)]);
    }
  fprintf(stdout, "%s: calling CFlock constructor (%d transducers)\n", Name, FlockClientDataMembers->NumberOfTransducers);
  
  // construct a flock
  Flock = new CFlock(FlockClientDataMembers->NumberOfTransducers);
  
  short_sleep(500000);
  
  // DDT
  fprintf(stdout, "%s: setting RS232 parameters.\n", Name);
  
  // set the serial port information
  Flock->setBaud( BIRD_BAUD );
  Flock->setComPort( FlockClientDataMembers->ServerName );
  Flock->setQueryMode( BIRD_POSITION_ANGLES );
  Flock->setHemisphere( BIRD_LOWER );
  //	Flock->setScale( 36.25, 36.25, 36.25 );
  Flock->setScale( 0.92075, 0.92075, 0.92075 );
  Flock->setGroup( 1 );
  Flock->setMeasRate( FlockClientDataMembers->birdMeasRate );
  
  //	fprintf(stdout,"just set measurement rate\n");
  
  short_sleep(500000);
  
  fprintf(stdout, "%s: connecting to bird\n", Name);
  
  // try to connect to the flock - this is establishing the serial port stuff
  flock_connected = Flock->connect( );
  
  if ( !flock_connected )
    {
      fprintf(stdout, "%s: Failed to connect to flock, using Fake Data (tm) mode\n", Name);
    }
  else
    {
      fprintf(stdout, "%s: Flock connected\n", Name);
      
      short_sleep(HALF_SECOND);
      
      // Now that the serial port is established, initialize the flock
      flock_initialized = Flock->initFlock( );
      
      if ( !flock_initialized )
	{
	  fprintf(stdout, "%s: Flock failed to initialize, using Fake Data (tm) mode\n", Name);
	}
      else
	{
	  fprintf(stdout, "%s: Flock initialized OK.\n", Name);
	}
    }
  
  if ((flock_connected) && (flock_initialized))
    {
      bird_on_flag = TRUE;
      FlockClientDataMembers->Ready = true;
    }
  
  // Main thread loop    
  while( !do_exit )
    {
      if (!bird_on_flag)
	{
	  // Since this takes no I/O time, go to sleep for a bit
	  short_sleep( HALF_SECOND );
	}
      else // Connected OK, use flock
	{
	  // NOTE: this is hard coded for the UNM trackers... customize to
	  // suite 
	  
	  short_sleep( POLL_DELAY );
	  
	  Flock->getPosAng(pos, ang);
	  
	  if ( FlockClientDataMembers->UseTracking ) 
	    {
	      for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
		{
		  FlockClientDataMembers->Tracker[i].x = pos[i][1];
		  FlockClientDataMembers->Tracker[i].y = -pos[i][2];
		  FlockClientDataMembers->Tracker[i].z = -pos[i][0];

          /*
           * Andrei added shift and clamp
           */
          fdata->Tracker[i].x += fdata->shift_x[i];
          fdata->Tracker[i].y += fdata->shift_y[i];
          fdata->Tracker[i].z += fdata->shift_z[i];

          if(fdata->Tracker[i].x < fdata->min_x[i]) 
            fdata->Tracker[i].x = fdata->min_x[i];
          else if(fdata->Tracker[i].x > fdata->max_x[i])
            fdata->Tracker[i].x = fdata->max_x[i];

          if(fdata->Tracker[i].y < fdata->min_y[i]) 
            fdata->Tracker[i].y = fdata->min_y[i];
          else if(fdata->Tracker[i].y > fdata->max_y[i])
            fdata->Tracker[i].y = fdata->max_y[i];

          if(fdata->Tracker[i].z < fdata->min_z[i]) 
            fdata->Tracker[i].z = fdata->min_z[i];
          else if(fdata->Tracker[i].z > fdata->max_z[i])
            fdata->Tracker[i].z = fdata->max_z[i];


		  /*
		    FlockClientDataMembers->Tracker[i].x = 0.0254*pos[i][1];
		    FlockClientDataMembers->Tracker[i].y = -0.0254*pos[i][2];
		    FlockClientDataMembers->Tracker[i].z = -0.0254*pos[i][0];
		  */
		  /*
		    FlockClientDataMembers->Tracker[i].pitch = ang[i][1];
		    FlockClientDataMembers->Tracker[i].yaw = -ang[i][2];
		    FlockClientDataMembers->Tracker[i].roll = -ang[i][0];
		  */
		  FlockClientDataMembers->Tracker[i].pitch = ang[i][1];
		  FlockClientDataMembers->Tracker[i].yaw = -ang[i][0];
		  FlockClientDataMembers->Tracker[i].roll = -ang[i][2];
		}
	      
	      
	      //dongshan Guo
#if 1
	      if (FlockClientDataMembers -> polyflag == 0) {
		//fprintf(stderr,"\n predition off alpha %f \n", alpha);
		//DS added flter 
		if(firstread == 0) {
		  for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++) {
		    FlockClientDataMembers -> curr_filted[i].x = FlockClientDataMembers->Tracker[i].x;
		    FlockClientDataMembers -> curr_filted[i].y = FlockClientDataMembers->Tracker[i].y;
		    FlockClientDataMembers -> curr_filted[i].z = FlockClientDataMembers->Tracker[i].z;
		    FlockClientDataMembers -> curr_filted[i].roll = FlockClientDataMembers->Tracker[i].roll;
		    FlockClientDataMembers -> curr_filted[i].pitch = FlockClientDataMembers->Tracker[i].pitch;
		    FlockClientDataMembers -> curr_filted[i].yaw = FlockClientDataMembers->Tracker[i].yaw;
		    
		    
		    FlockClientDataMembers -> prev_filted[i].x = 0;
		    FlockClientDataMembers -> prev_filted[i].y = 0;
		    FlockClientDataMembers -> prev_filted[i].z = 0;
		    FlockClientDataMembers -> prev_filted[i].roll = 0;
		    FlockClientDataMembers -> prev_filted[i].pitch = 0;
		    FlockClientDataMembers -> prev_filted[i].yaw = 0;
		  }
		  firstread = 1;
		}
		else {
		  for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++) {

		    FlockClientDataMembers -> curr_filted[i].x = FlockClientDataMembers -> prev_filted[i].x + alpha * (FlockClientDataMembers->Tracker[i].x - FlockClientDataMembers -> prev_filted[i].x);
		    FlockClientDataMembers -> curr_filted[i].y = FlockClientDataMembers -> prev_filted[i].y + alpha * (FlockClientDataMembers->Tracker[i].y - FlockClientDataMembers -> prev_filted[i].y);
		    FlockClientDataMembers -> curr_filted[i].z = FlockClientDataMembers -> prev_filted[i].z + alpha * (FlockClientDataMembers->Tracker[i].z - FlockClientDataMembers -> prev_filted[i].z);
		    FlockClientDataMembers -> curr_filted[i].roll = FlockClientDataMembers -> prev_filted[i].roll + alpha * (FlockClientDataMembers->Tracker[i].roll - FlockClientDataMembers -> prev_filted[i].roll);
		    FlockClientDataMembers -> curr_filted[i].pitch = FlockClientDataMembers -> prev_filted[i].pitch + alpha * (FlockClientDataMembers->Tracker[i].pitch - FlockClientDataMembers -> prev_filted[i].pitch);
		    FlockClientDataMembers -> curr_filted[i].yaw = FlockClientDataMembers -> prev_filted[i].yaw + alpha * (FlockClientDataMembers->Tracker[i].yaw - FlockClientDataMembers -> prev_filted[i].yaw);

		    FlockClientDataMembers -> prev_filted[i].x = FlockClientDataMembers -> curr_filted[i].x;
		    FlockClientDataMembers -> prev_filted[i].y = FlockClientDataMembers -> curr_filted[i].y;
		    FlockClientDataMembers -> prev_filted[i].z = FlockClientDataMembers -> curr_filted[i].z;
		    FlockClientDataMembers -> prev_filted[i].roll = FlockClientDataMembers -> curr_filted[i].roll;
		    FlockClientDataMembers -> prev_filted[i].pitch = FlockClientDataMembers -> curr_filted[i].pitch;
		    FlockClientDataMembers -> prev_filted[i].yaw = FlockClientDataMembers -> curr_filted[i].yaw;
		  }
		}
	      }
#endif      
	      
	      /* Dongshan add for new prediction */
	      predictedCoeff(TrackerClientObject);
	      // fprintf(stdout, "\n while loop inside tracker thread\n");
	    }
	  else
	    {
	      for(i=0;i<FlockClientDataMembers->NumberOfTransducers;i++)
		{
		  FlockClientDataMembers->Tracker[i].x = 0;
		  FlockClientDataMembers->Tracker[i].y = 0;
		  FlockClientDataMembers->Tracker[i].z = 0;
		  FlockClientDataMembers->Tracker[i].pitch = 0;
		  FlockClientDataMembers->Tracker[i].yaw = 0;
		  FlockClientDataMembers->Tracker[i].roll = 0;
		}
	    }
	}
    }
  
  // Done, shutdown flock
  if(bird_on_flag)
    {
      Flock->disconnect( );
    }
  
  delete[] real_triplets;
  delete[] pos;
  delete[] ang;
  delete [] curr_filted;
  delete [] prev_filted;
  free(CQ);  
  free(tq);

  // Log same
  fprintf(stdout, "\n%s: Flock thread exiting\n", Name);
  
  return( NULL );
} /* end of track thread */


/* -------------------- Dongshan Guo add ----------------------------- */

void predictedCoeff(OBJECT_TYPE *TrackerClientObject) {
  //  FILE * outFile1;
  int i, j, k;
  int numiv = 2;
  int numobs = MAXQUEUE;
  int NO_OF_TRANSDUCERS;
  float indv[MAXQUEUE][2];
  float currentTime, last_time, interval;
  
  // DS added 8-19-02
  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
  NO_OF_TRANSDUCERS = FlockClientDataMembers->NumberOfTransducers;

  if (NO_OF_TRANSDUCERS > MAX_NO_TRANSDUCERS)
  {
    fprintf (stderr, "ERROR:  Too many transducers:  change MAX_NO_TRANSDUCERS in FlockTrackerClient.h\n");
    exit (1);
  }
  
  float x_beta[MAX_NO_TRANSDUCERS][3];
  float y_beta[MAX_NO_TRANSDUCERS][3];
  float z_beta[MAX_NO_TRANSDUCERS][3];
  float ro_beta[MAX_NO_TRANSDUCERS][3];
  float ph_beta[MAX_NO_TRANSDUCERS][3];
  float yw_beta[MAX_NO_TRANSDUCERS][3];

  float	stored_x[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float stored_y[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float stored_z[MAX_NO_TRANSDUCERS * MAXQUEUE];

  float stored_ro[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float stored_ph[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float stored_yw[MAX_NO_TRANSDUCERS * MAXQUEUE];

  float raw_stored_ro[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float raw_stored_ph[MAX_NO_TRANSDUCERS * MAXQUEUE];
  float raw_stored_yw[MAX_NO_TRANSDUCERS * MAXQUEUE];
  //  float alpha = 1.0;
  int currentFront = 0;
  float bird_response_time = 20.0;

  /*
  static float predicted_roll = 0;
  static float predicted_pitch = 0;
  static float predicted_yaw = 0;
  */
  //  static float previous_yaw = 0;

  static int first_record = 0;
  //  static int entryCount = 0;
  //  static int outputdata = 0;
  //  static int predictedCount = 0;

  //  static  float previousTime;
  //  fprintf(stdout, "\n prediction function called \n");
  
  queueEntry input;
  
  if(first_record == 0) {
    for (j = 0; j < NO_OF_TRANSDUCERS; j++) {
      input.position[j][0] = 0;
      input.position[j][1] = 0;
      input.position[j][2] = 0;
      input.angles[j][0] = 0;
      input.angles[j][1] = 0;
      input.angles[j][2] = 0;
    }
    for (j = 0; j < NO_OF_TRANSDUCERS; j++) {
      for(k = 0; k < 3; k++) {
	x_beta[j][k] = 0;
	y_beta[j][k] = 0;
	z_beta[j][k] = 0;
	ro_beta[j][k] = 0;
	ph_beta[j][k] = 0;
	yw_beta[j][k] = 0;
      }
    }
    
    /*
      for ( i = 0; i < samplelen; i++) 
      for ( j = 0; j < NO_OF_TRANSDUCERS; j++) 
      for (k = 0; k < 7; k++)
      origin[i][j][k] = 0;
    */
    first_record = 1;
  } // end of if(first_record == 0)
  
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  
  // This time is for regression analysis
  input.time = currentTime;
  
  if (FlockClientDataMembers->Ready || FlockClientDataMembers->UseTracking) {
    // only consider one transducer
    
    //for(j = 0; j < 1; j++) {
      for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      input.position[j][0] = FlockClientDataMembers->Tracker[j].x;
      input.position[j][1] = FlockClientDataMembers->Tracker[j].y;
      input.position[j][2] = FlockClientDataMembers->Tracker[j].z;
      
      input.angles[j][0] = FlockClientDataMembers->Tracker[j].roll;
      input.angles[j][1] = FlockClientDataMembers->Tracker[j].pitch;
      input.angles[j][2] = FlockClientDataMembers->Tracker[j].yaw;
      /*
      origin[entryCount][j][0] = input.position[j][0];
      origin[entryCount][j][1] = input.position[j][1];
      origin[entryCount][j][2] = input.position[j][2];
      origin[entryCount][j][3] = input.angles[j][0];
      origin[entryCount][j][4] = input.angles[j][1];
      origin[entryCount][j][5] = input.angles[j][2];
      origin[entryCount][j][6] = currentTime;
      */
    }
      /*
    if(entryCount != samplelen) 
      entryCount++;
      */
  } // end of if
  
  //  if(!queueFull(CQ))  
  append(input, CQ);
  
  
  // The following does prediction  
  
  if(queueFull(CQ)) {
    //    adjustTime(CQ);
    //    traverse(CQ, PrintNode);
    currentFront = CQ->front;
    
    //fprintf(stderr," prepare data \n");
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      for(k = 0; k < MAXQUEUE; k++){
	
	storedtime[k] = CQ->entry[currentFront].time - CQ->entry[CQ->rear].time;
	indv[k][0] = storedtime[k];
	indv[k][1] = storedtime[k] * storedtime[k];  
	
	
	stored_x[k + j * MAXQUEUE] = CQ->entry[currentFront].position[j][0];
	stored_y[k + j * MAXQUEUE] = CQ->entry[currentFront].position[j][1];
	stored_z[k + j * MAXQUEUE] = CQ->entry[currentFront].position[j][2];
	stored_ro[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][0];
	stored_ph[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][1];
	stored_yw[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][2];
	/*
	raw_stored_ro[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][0];
	raw_stored_ph[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][1];
	raw_stored_yw[k + j * MAXQUEUE] = CQ->entry[currentFront].angles[j][2];
	*/
	currentFront = (currentFront + 1) % MAXQUEUE;
      } // end of inner for loop
      // add filter (not used when alpha is set to 1	
      /*
      stored_ro[0] = raw_stored_ro[0];
      stored_ph[0] = raw_stored_ph[0];
      stored_yw[0] = raw_stored_yw[0];
      for(k = 0; k < MAXQUEUE; k++){
	
	stored_ro[k + 1] = stored_ro[k] + alpha * (raw_stored_ro[k + 1] - stored_ro[k]);
	stored_ph[k + 1] = stored_ph[k] + alpha * (raw_stored_ph[k + 1] - stored_ph[k]);
	stored_yw[k + 1] = stored_yw[k] + alpha * (raw_stored_yw[k + 1] - stored_yw[k]);
      }
      */
      
      /*   fprintf(stderr," regression \n"); */
      
      
      MultipleReg(&indv[0][0],&stored_x[j * MAXQUEUE],numiv,numobs,x_beta[j]);
      MultipleReg(&indv[0][0],&stored_y[j * MAXQUEUE],numiv,numobs,y_beta[j]);
      MultipleReg(&indv[0][0],&stored_z[j * MAXQUEUE],numiv,numobs,z_beta[j]);
      
      
      MultipleReg(&indv[0][0],&stored_ro[j * MAXQUEUE],numiv,numobs,ro_beta[j]);
      MultipleReg(&indv[0][0],&stored_ph[j * MAXQUEUE],numiv,numobs,ph_beta[j]);
      MultipleReg(&indv[0][0],&stored_yw[j * MAXQUEUE],numiv,numobs,yw_beta[j]);
    }      
    
    //  fprintf(stdout," sent coeffecences \n")  ;
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      for(k = 0; k < 3; k++) {
	
	FlockClientDataMembers->x_beta[j][k] = x_beta[j][k];
	FlockClientDataMembers->y_beta[j][k] = y_beta[j][k];
	FlockClientDataMembers->z_beta[j][k] = z_beta[j][k];
	
	FlockClientDataMembers->ro_beta[j][k] = ro_beta[j][k];
	FlockClientDataMembers->ph_beta[j][k] = ph_beta[j][k];
	FlockClientDataMembers->yw_beta[j][k] = yw_beta[j][k];
      }
    }  
    
    last_time =   FlockClientDataMembers->readTime;
    FlockClientDataMembers->readTime = glutGet(GLUT_ELAPSED_TIME);
    
    interval = currentTime - last_time;
    if(interval > 40 ) {
      interval = bird_response_time;  
    }
    //  fprintf(stderr," interval of data acquistion   %f \n", interval );
    
    //    j = 0;
    /*
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      predicted_roll  = ro_beta[j][0] + ro_beta[j][1] * interval + ro_beta[j][2] * interval * interval;
      predicted_pitch = ph_beta[j][0] + ph_beta[j][1] * interval + ph_beta[j][2] * interval * interval;
      predicted_yaw   = yw_beta[j][0] + yw_beta[j][1] * interval + yw_beta[j][2] * interval * interval;
    }
    */
  } // end of if(queueFull(CQ)) (doing prediction)
} /* ------- end of predictedPosAng function  ---------------------- */

/* this function do the real job for predicting position */

void predictedPosAng(OBJECT_TYPE *TrackerClientObject) {

  int i, j, k;
  float interval;

  int NO_OF_TRANSDUCERS;
  float pd_time, curr_time, read_time;
  //  static float next_diff[samplelen];
  static int eCount = 0;
  //  static int outputPre = 0;
  static int init_flag = 0;
  //  static int first_print = 0;
  // DS added
  // DS add on 11-26-02
  //  static float previous_yaw_graph;

  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
  NO_OF_TRANSDUCERS = FlockClientDataMembers -> NumberOfTransducers;

  if (NO_OF_TRANSDUCERS > MAX_NO_TRANSDUCERS)
  {
    fprintf (stderr, "ERROR:  Too many transducers:  change MAX_NO_TRANSDUCERS in FlockTrackerClient.h\n");
    exit (1);
  }
  
  float x_beta[MAX_NO_TRANSDUCERS][3];
  float y_beta[MAX_NO_TRANSDUCERS][3];
  float z_beta[MAX_NO_TRANSDUCERS][3];
  float ro_beta[MAX_NO_TRANSDUCERS][3];
  float ph_beta[MAX_NO_TRANSDUCERS][3];
  float yw_beta[MAX_NO_TRANSDUCERS][3];
  float interval_swapbuffer = 50.0;  
  
  queueEntry inputT;
  
  if(init_flag == 0) {
    /*
      for ( i = 0; i < samplelen; i++) { 
      for ( j = 0; j < NO_OF_TRANSDUCERS; j++) { 
      for (k = 0; k < 7; k++) {
      predicted[i][j][k] = 0;
      }
      }
      next_diff[i] = 0;
      }
    */
    for (j = 0; j < NO_OF_TRANSDUCERS; j++) {
      inputT.position[j][0] = 0;
      inputT.position[j][1] = 0;
      inputT.position[j][2] = 0;
      inputT.angles[j][0] = 0;
      inputT.angles[j][1] = 0;
      inputT.angles[j][2] = 0;
      
      predictedPos[j][0] = 0;
      predictedPos[j][1] = 0;
      predictedPos[j][2] = 0;
      predictedAng[j][0] = 0;
      predictedAng[j][1] = 0;
      predictedAng[j][2] = 0;
      
    }
    
    init_flag = 1;
  }
  
  
  
  if (FlockClientDataMembers->Ready || FlockClientDataMembers->UseTracking) {
    //    fprintf(stdout, " received coeffecences \n");
    //    fflush(stdout);
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      for(k = 0; k < 3; k++) {
	
	x_beta[j][k] = FlockClientDataMembers->x_beta[j][k];
	y_beta[j][k] = FlockClientDataMembers->y_beta[j][k];
	z_beta[j][k] = FlockClientDataMembers->z_beta[j][k];
	
	ro_beta[j][k] = FlockClientDataMembers->ro_beta[j][k];
	ph_beta[j][k] = FlockClientDataMembers->ph_beta[j][k];
	yw_beta[j][k] = FlockClientDataMembers->yw_beta[j][k];
      }
    }
  }
  
  read_time = FlockClientDataMembers->readTime;
  
  //  fprintf(stdout, "\n prediction function called \n");

  curr_time = glutGet(GLUT_ELAPSED_TIME);
  inputT.time = curr_time;    

  append(inputT, tq);
  //  fprintf(stderr," ready for prediction  \n");

  //  if(queueFull(tq) && read_time != 0 && first_print == 0) {
  if(queueFull(tq) && read_time != 0) {
    interval = get_predicted_time();
    pd_time = curr_time - read_time + interval;
    if(pd_time > 50)
      pd_time = interval_swapbuffer;
    
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {
      //    for(j = 0; j < 1; j++) { // for only head tracking
     
      predictedPos[j][0] = x_beta[j][0] + x_beta[j][1] * pd_time + x_beta[j][2] * pd_time * pd_time;
      predictedPos[j][1] = y_beta[j][0] + y_beta[j][1] * pd_time + y_beta[j][2] * pd_time * pd_time;
      predictedPos[j][2] = z_beta[j][0] + z_beta[j][1] * pd_time + z_beta[j][2] * pd_time * pd_time;
      predictedAng[j][0] = ro_beta[j][0] + ro_beta[j][1] * pd_time + ro_beta[j][2] * pd_time * pd_time;
      predictedAng[j][1] = ph_beta[j][0] + ph_beta[j][1] * pd_time + ph_beta[j][2] * pd_time * pd_time;
      predictedAng[j][2] = yw_beta[j][0] + yw_beta[j][1] * pd_time + yw_beta[j][2] * pd_time * pd_time;
     
      /*
      predictedPos[j][0] = FlockClientDataMembers->Tracker[j].x;
      predictedPos[j][1] = FlockClientDataMembers->Tracker[j].y;
      predictedPos[j][2] = FlockClientDataMembers->Tracker[j].z;
      predictedAng[j][0] = FlockClientDataMembers->Tracker[j].roll;
      predictedAng[j][1] = FlockClientDataMembers->Tracker[j].pitch;
      predictedAng[j][2] = FlockClientDataMembers->Tracker[j].yaw;
      */
      /*     
      if(eCount < 3000 - 1) {     
	predicted[eCount + 1][j][0] = predictedPos[j][0];
	predicted[eCount + 1][j][1] = predictedPos[j][1];
	predicted[eCount + 1][j][2] = predictedPos[j][2];
	predicted[eCount + 1][j][3] = predictedAng[j][0];
	predicted[eCount + 1][j][4] = predictedAng[j][1];
	predicted[eCount + 1][j][5] = predictedAng[j][2];
	//predicted[eCount + 1][j][6] = curr_time + interval;
	//	predicted[eCount + 1][j][6] = curr_time + next_diff[eCount];
      }
      */
    } // end of for loop
  } // end of if(queueFull(tq)) (doing prediction)
  
  
  // for checking the constrate influence to time interval 
  /*
  if(eCount !=samplelen) {
    //    real_time[eCount] = curr_time;
    for(j = 0; j < NO_OF_TRANSDUCERS; j++) {

      real[eCount][j][0] = FlockClientDataMembers->Tracker[j].x;
      real[eCount][j][1] = FlockClientDataMembers->Tracker[j].y;
      real[eCount][j][2] = FlockClientDataMembers->Tracker[j].z;
      real[eCount][j][3] = FlockClientDataMembers->Tracker[j].roll;
      real[eCount][j][4] = FlockClientDataMembers->Tracker[j].pitch;
      real[eCount][j][5] = FlockClientDataMembers->Tracker[j].yaw;
    }
    eCount++;
  }
  */
} /* ------- end of predictedPosAng function  ---------------------- */

void empty(OBJECT_TYPE *o)
{
}

bool IsReady(OBJECT_TYPE *TrackerClientObject)
{
  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
  return FlockClientDataMembers->Ready;
}

int GetNumberOfTransducers(OBJECT_TYPE *TrackerClientObject)
{
  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;
  return FlockClientDataMembers->Ready ? FlockClientDataMembers->NumberOfTransducers : 0;
}

bool GetPosition(OBJECT_TYPE *TrackerClientObject, int transducer_number, Position_Struct *coord)
{

  FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) TrackerClientObject->ptr1;

  if (!FlockClientDataMembers->Ready || !FlockClientDataMembers->UseTracking || transducer_number<0 || transducer_number>=FlockClientDataMembers->NumberOfTransducers)
    {
      coord->x = 0.0f;
      coord->y = 0.0f;
      coord->z = 0.0f;
      coord->roll = 0.0f;
      coord->pitch = 0.0f;
      coord->yaw = 0.0f;
      //      fprintf(stderr," get all zeros \n");
      return false;
    }
  else
    {
#if 1
      coord->x = FlockClientDataMembers -> curr_filted[transducer_number].x;
      coord->y = FlockClientDataMembers -> curr_filted[transducer_number].y;
      coord->z = FlockClientDataMembers -> curr_filted[transducer_number].z;
      coord->roll = FlockClientDataMembers -> curr_filted[transducer_number].roll;
      coord->pitch = FlockClientDataMembers -> curr_filted[transducer_number].pitch;
      coord->yaw = FlockClientDataMembers -> curr_filted[transducer_number].yaw;
      
      
      
      /*
	else if(FlockClientDataMembers->polyflag == 1) {
	
	fprintf(stderr,"\n predition on alpha %f  \n", alpha);
	
	if(firstread == 0) {
	  FlockClientDataMembers->curr_filted[transducer_number].x = predictedPos[transducer_number][0];
	  FlockClientDataMembers->curr_filted[transducer_number].y = predictedPos[transducer_number][1];
	  FlockClientDataMembers->curr_filted[transducer_number].z = predictedPos[transducer_number][2];
	  FlockClientDataMembers->curr_filted[transducer_number].roll = predictedAng[transducer_number][0];
	  FlockClientDataMembers->curr_filted[transducer_number].pitch = predictedAng[transducer_number][1];
	  FlockClientDataMembers->curr_filted[transducer_number].yaw = predictedAng[transducer_number][2];
	  coord->x = FlockClientDataMembers->curr_filted[transducer_number].x;
	  coord->y = FlockClientDataMembers->curr_filted[transducer_number].y;
	  coord->z = FlockClientDataMembers->curr_filted[transducer_number].z;
	  coord->roll = FlockClientDataMembers->curr_filted[transducer_number].roll;
	  coord->pitch = FlockClientDataMembers->curr_filted[transducer_number].pitch;
	  coord->yaw = FlockClientDataMembers->curr_filted[transducer_number].yaw;
	 	  
	  FlockClientDataMembers->prev_filted[transducer_number].x = FlockClientDataMembers->curr_filted[transducer_number].x;
	  FlockClientDataMembers->prev_filted[transducer_number].y = FlockClientDataMembers->curr_filted[transducer_number].y;
	  FlockClientDataMembers->prev_filted[transducer_number].z = FlockClientDataMembers->curr_filted[transducer_number].z;
	  FlockClientDataMembers->prev_filted[transducer_number].roll = FlockClientDataMembers->curr_filted[transducer_number].roll;
	  FlockClientDataMembers->prev_filted[transducer_number].pitch = FlockClientDataMembers->curr_filted[transducer_number].pitch;
	  FlockClientDataMembers->prev_filted[transducer_number].yaw = FlockClientDataMembers->curr_filted[transducer_number].yaw;

	  firstread = 1;
	}

	else {
		  
	  FlockClientDataMembers->curr_filted[transducer_number].x = FlockClientDataMembers->prev_filted[transducer_number].x + alpha * (predictedPos[transducer_number][0] - FlockClientDataMembers->prev_filted[transducer_number].x);
	  FlockClientDataMembers->curr_filted[transducer_number].y = FlockClientDataMembers->prev_filted[transducer_number].y + alpha * (predictedPos[transducer_number][1] - FlockClientDataMembers->prev_filted[transducer_number].y);
	  FlockClientDataMembers->curr_filted[transducer_number].z = FlockClientDataMembers->prev_filted[transducer_number].z + alpha * (predictedPos[transducer_number][2] - FlockClientDataMembers->prev_filted[transducer_number].z);
	  FlockClientDataMembers->curr_filted[transducer_number].roll = FlockClientDataMembers->prev_filted[transducer_number].roll + alpha * (predictedAng[transducer_number][0] - FlockClientDataMembers->prev_filted[transducer_number].roll);
	  FlockClientDataMembers->curr_filted[transducer_number].pitch = FlockClientDataMembers->prev_filted[transducer_number].pitch + alpha * (predictedAng[transducer_number][1] - FlockClientDataMembers->prev_filted[transducer_number].pitch);
	  FlockClientDataMembers->curr_filted[transducer_number].yaw = FlockClientDataMembers->prev_filted[transducer_number].yaw + alpha * (predictedAng[transducer_number][2] - FlockClientDataMembers->prev_filted[transducer_number].yaw);
	  
	  coord->x = FlockClientDataMembers->curr_filted[transducer_number].x;
	  coord->y = FlockClientDataMembers->curr_filted[transducer_number].y;
	  coord->z = FlockClientDataMembers->curr_filted[transducer_number].z;
	  coord->roll = FlockClientDataMembers->curr_filted[transducer_number].roll;
	  coord->pitch = FlockClientDataMembers->curr_filted[transducer_number].pitch;
	  coord->yaw = FlockClientDataMembers->curr_filted[transducer_number].yaw;
	  
	  FlockClientDataMembers->prev_filted[transducer_number].x = FlockClientDataMembers->curr_filted[transducer_number].x;
	  FlockClientDataMembers->prev_filted[transducer_number].y = FlockClientDataMembers->curr_filted[transducer_number].y;
	  FlockClientDataMembers->prev_filted[transducer_number].z = FlockClientDataMembers->curr_filted[transducer_number].z;
	  FlockClientDataMembers->prev_filted[transducer_number].roll = FlockClientDataMembers->curr_filted[transducer_number].roll;
	  FlockClientDataMembers->prev_filted[transducer_number].pitch = FlockClientDataMembers->curr_filted[transducer_number].pitch;
	  FlockClientDataMembers->prev_filted[transducer_number].yaw = FlockClientDataMembers->curr_filted[transducer_number].yaw;
	  
	}
      } // end get data polynomial prediction
      */
#endif
      
#if 0
      coord->x = FlockClientDataMembers->Tracker[transducer_number].x;
      coord->y = FlockClientDataMembers->Tracker[transducer_number].y;
      coord->z = FlockClientDataMembers->Tracker[transducer_number].z;
      coord->roll = FlockClientDataMembers->Tracker[transducer_number].roll;
      coord->pitch = FlockClientDataMembers->Tracker[transducer_number].pitch;
      coord->yaw = FlockClientDataMembers->Tracker[transducer_number].yaw;
      fprintf(stderr," no prediction, original data  \n");
#endif 
      return true;
    }

}

// Platform handler 
void platformfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// do nothing
}

// Draw callback 
void drawfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) FlockTrackerClientObject->ptr1;

	GLboolean lighting_flag = GL_FALSE;
	GLfloat black[4]={0.0,0.0,0.0,1.0};
	GLfloat ambient[4]={0.3,0.3,0.3,1.0};
	GLfloat diffuse[4]={1.0,1.0,1.0,1.0};
	GLfloat specular[4]={0.1,0.1,0.1,1.0};
	
	// draw what?

	// draw axes on demand...	
	if ( FlockClientDataMembers->DisplayAxes ) 
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );

		glutSolidCube(0.05);
	
		lighting_flag = glIsEnabled(GL_LIGHTING) ;
		if ( lighting_flag )
		{
			glDisable(GL_LIGHTING);
		}
		glColor3f(1,1,1);
  
		glLineWidth(1.0);
		
		glBegin(GL_LINES);
			// x axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.24,0.01,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.24,-0.01,0.0);
			glVertex3f(0.25,0.0,0.0);
			glVertex3f(0.26,0.0,0.0);
			glVertex3f(0.27,0.015,0.0);
			glVertex3f(0.26,0.015,0.0);
			glVertex3f(0.27,0.0,0.0);
			// y axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.24,0.01);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.24,-0.01);
			glVertex3f(0.0,0.25,0.0);
			glVertex3f(0.0,0.265,0.0);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.26,0.015);
			glVertex3f(0.0,0.265,0.0075);
			glVertex3f(0.0,0.27,0.015);
			// z axis
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(0.01,0.0,0.24);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(-0.01,0.0,0.24);
			glVertex3f(0.0,0.0,0.25);
			glVertex3f(0.0,0.0,0.27);
			glVertex3f(0.0,0.0,0.26);
			glVertex3f(0.015,0.0,0.27);
			glVertex3f(0.0,0.0,0.26);
			glVertex3f(0.015,0.0,0.27);
			glVertex3f(0.015,0.0,0.26);
		glEnd();

		if ( lighting_flag )
		{
			glEnable(GL_LIGHTING);
		}  

		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, black );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, black );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black );
	}
}

// true shadow callback 
void trueShadowFunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// what shadows?
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *FlockTrackerClientObject)
{
	// shadow of what?
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *FlockTrackerClientObject)
{
}

/* menu control callback */
void MenuCallback(OBJECT_TYPE *FlockTrackerClientObject, char key)
{
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) FlockTrackerClientObject->ptr1;
	//fprintf(stderr," menucallback called \n");
	switch (key)
	{
    case 'a':   // display axes
		FlockClientDataMembers->DisplayAxes = !FlockClientDataMembers->DisplayAxes;
		break;

    case 't':   // display axes
		FlockClientDataMembers->UseTracking = !FlockClientDataMembers->UseTracking;
		break;
		/*
		  case 'p':   // use polynomial prediction
		  polyflag = 1;
		  fprintf(stderr," polynomial prediction \n");
		  break;
		*/
		/*
		  case 'o':   // don't use polynomial prediction
		  polyflag = 0;
		  fprintf(stderr," no prediction \n");
		  break;
		*/
	}
}

extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];					/* temporary transform storage */
	OBJECT_TYPE *TrackerClientObject;				/* This object */
	pthread_t transducer_read_thread;		/* thread handle for data input */
	/* the next line may not be used */
	TRACKER_CLIENT_ACCESS_MEMBERS *TrackerClientMembers; /* pointer to "TrackerClientMembers" structure */
	FLOCK_TRACKER_CLIENT_DATA_MEMBERS *FlockClientDataMembers; /* pointer to "TrackerClientMembers" structure */
	char TRACKER_CLIENT_PORT[MAX_NAME_SIZE] = {TRACKER_CLIENT_SERIAL_PORT};  
	int TRACKER_CLIENT_TRANSDUCERS = NUMBER_OF_TRANSDUCERS;
	float TRACKER_CLIENT_X = 0.0f;
	float TRACKER_CLIENT_Y = 0.0f;
	float TRACKER_CLIENT_Z = 0.0f;
	float TRACKER_CLIENT_ROLL = 0.0f;
	float TRACKER_CLIENT_PITCH = 0.0f;
	float TRACKER_CLIENT_YAW = 0.0f;
	char *Name = App->appname;
	bool known;
			 
	/* generate the "TrackerClientMembers" structure & attach it to the object */
	if ((TrackerClientMembers = (TRACKER_CLIENT_ACCESS_MEMBERS *) malloc (sizeof (TRACKER_CLIENT_ACCESS_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	/* generate the "FlockTrackerClientMembers" structure & attach it to the object */
      	if ((FlockClientDataMembers = (FLOCK_TRACKER_CLIENT_DATA_MEMBERS *) malloc (sizeof (FLOCK_TRACKER_CLIENT_DATA_MEMBERS))) == NULL)
	{
		printf ("%s: failed to initialize memory for TrackerClientMembers\n", Name);
		exit(1);
	}

	FlockClientDataMembers->Tracker = NULL;
	TrackerClientMembers->GetPosition = GetPosition;
	TrackerClientMembers->GetVelocity = NULL;
	TrackerClientMembers->GetError = NULL;
	TrackerClientMembers->IsReady = IsReady;
	TrackerClientMembers->GetNumberOfTransducers = GetNumberOfTransducers;
	FlockClientDataMembers->Ready = false;
	FlockClientDataMembers->NumberOfTransducers = 0;
	FlockClientDataMembers->birdMeasRate = 103.3; /*default measurement rate*/
	FlockClientDataMembers->DisplayAxes = false;
	FlockClientDataMembers->UseTracking = true;
	// DS added
	FlockClientDataMembers->polyflag = 0; /*default prediction flag*/
	FlockClientDataMembers->alpha = 0.9; /*default filter parameter*/
    /* Andrei added shift and selective tracking */
	for (i=0; i < MAX_NO_TRANSDUCERS; i++) {
	  FlockClientDataMembers->shift_x[i] = 0.0;
	  FlockClientDataMembers->shift_y[i] = 0.0;
	  FlockClientDataMembers->shift_z[i] = 0.0;
	  FlockClientDataMembers->min_x[i] = -1000000.0;
	  FlockClientDataMembers->min_y[i] = -1000000.0;
	  FlockClientDataMembers->min_z[i] = -1000000.0;
	  FlockClientDataMembers->max_x[i] = 1000000.0;
	  FlockClientDataMembers->max_y[i] = 1000000.0;
	  FlockClientDataMembers->max_z[i] = 1000000.0;
    }
	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "port"))
		{
			sscanf(App->params[i][2], "%s", TRACKER_CLIENT_PORT);
			known = true;
		}
		if (!strcmp (App->params[i][0], "transducers"))
		{
			sscanf(App->params[i][2], "%d", &TRACKER_CLIENT_TRANSDUCERS);
			known = true;
		}
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &TRACKER_CLIENT_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "measurement_rate"))
		{
			sscanf(App->params[i][2], "%f", &FlockClientDataMembers->birdMeasRate);
			known = true;
		}
		// DS added
		if (!strcmp (App->params[i][0], "prediction"))
		{
			sscanf(App->params[i][2], "%d", &FlockClientDataMembers->polyflag);
			known = true;
		}
		// DS added filter parameter
		if (!strcmp (App->params[i][0], "alpha"))
		{
			sscanf(App->params[i][2], "%f", &FlockClientDataMembers->alpha);
			known = true;
		}
		
		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

	strncpy(FlockClientDataMembers->ServerName,TRACKER_CLIENT_PORT,MAX_NAME_SIZE);

	FlockClientDataMembers->NumberOfTransducers = TRACKER_CLIENT_TRANSDUCERS;
	FlockClientDataMembers->Position.x = TRACKER_CLIENT_X;
	FlockClientDataMembers->Position.y = TRACKER_CLIENT_Y;
	FlockClientDataMembers->Position.z = TRACKER_CLIENT_Z;
	FlockClientDataMembers->Position.roll = TRACKER_CLIENT_ROLL;
	FlockClientDataMembers->Position.pitch = TRACKER_CLIENT_PITCH;
	FlockClientDataMembers->Position.yaw = TRACKER_CLIENT_YAW;

	TrackerClientObject = add_child_to_parent_by_name(	App->universe_ptr,
														App->parent_object_name,
														App->appname);

	TrackerClientObject->ptr0 = (void *) TrackerClientMembers;	/* attach struct to object */
	TrackerClientObject->ptr1 = (void *) FlockClientDataMembers;	/* attach struct to object */

	/* set up callbacks */
	TrackerClientObject->soundfunc = soundfunc;
	TrackerClientObject->sounding_on_flag = 0;
	TrackerClientObject->platformfunc = platformfunc;
	TrackerClientObject->predrawfunc = predrawfunc;
	TrackerClientObject->drawfunc = drawfunc;
	TrackerClientObject->trueShadowFunc = trueShadowFunc;
	TrackerClientObject->cheapShadowFunc = cheapShadowFunc;
	TrackerClientObject->drawing_on_flag = 1;
	/* Dongshan add */
       	TrackerClientObject->preswapbufferfunc = predictedPosAng;

	//// Menu Setup ////

	/* set up the control structure (three subcontrols under one) */
	/* allows control of the object from the keyboard, menues, and voice */
	TrackerClientObject->object_controls = makeControlsSet(4);

	/* Define spin/nospin controls for cube */
	strcpy (TrackerClientObject->object_controls->controls[0]->menuname, "Toggle Axes Display");
	strcpy (TrackerClientObject->object_controls->controls[0]->voicename, "Axes");
	TrackerClientObject->object_controls->controls[0]->keystroke = 'a';
	TrackerClientObject->object_controls->controls[0]->control_function = MenuCallback;

	strcpy (TrackerClientObject->object_controls->controls[1]->menuname, "Toggle Tracking");
	strcpy (TrackerClientObject->object_controls->controls[1]->voicename, "Tracking");
	TrackerClientObject->object_controls->controls[1]->keystroke = 't';
	TrackerClientObject->object_controls->controls[1]->control_function = MenuCallback;

	/* Dongshan add for prediction control*/

	strcpy (TrackerClientObject->object_controls->controls[2]->menuname, "Toggle prediction");
	strcpy (TrackerClientObject->object_controls->controls[2]->voicename, "Prediction");
	TrackerClientObject->object_controls->controls[2]->keystroke = 'p';
	TrackerClientObject->object_controls->controls[2]->control_function = MenuCallback;

	strcpy (TrackerClientObject->object_controls->controls[3]->menuname, "Turn off prediction");
	strcpy (TrackerClientObject->object_controls->controls[3]->voicename, "Non Prediction");
	TrackerClientObject->object_controls->controls[3]->keystroke = 'o';
	TrackerClientObject->object_controls->controls[3]->control_function = MenuCallback;

	//// Bounding Volume Setup ////

	TrackerClientObject->bounding_volume_set = makebvset(3);

	/* define a bounding volume (four corners and a center) */
	TrackerClientObject->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][0]=0.0;           
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][1]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][2]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[0][3]=1.0;
	/* -x, -y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[1][3]=1.0;
	/* +x, -y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][0]=+0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[2][3]=1.0;
	/* -x, +y, -z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][1]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][2]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[3][3]=1.0;
	/* -x, -y, +z corner */
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][0]=-0.01;    
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][1]=-0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][2]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[0]->p[4][3]=1.0;

	/* Define bounding sphere for culling */
	TrackerClientObject->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][0]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][1]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][2]=0.0;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[0][3]=1.0;
	/* Surface point */
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][0]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][1]=+0.01;
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][2]=+0.01;	
	TrackerClientObject->bounding_volume_set->bvset[1]->p[1][3]=1.0;
    
	/* Define a bounding point */
	TrackerClientObject->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][0]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][1]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	TrackerClientObject->bounding_volume_set->bvset[2]->p[0][3]= 1.0;
  
	/* attach to the wessel in a default place */
	PTemp[0] = TRACKER_CLIENT_X;
	PTemp[1] = TRACKER_CLIENT_Y;
	PTemp[2] = TRACKER_CLIENT_Z; 
	PTemp[3] = TRACKER_CLIENT_YAW;
	PTemp[4] = TRACKER_CLIENT_PITCH;
	PTemp[5] = TRACKER_CLIENT_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	TrackerClientObject->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	TrackerClientObject->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	TrackerClientObject->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	TrackerClientObject->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	TrackerClientObject->orginal_first_node->MpoParams->P->assign( PTemp );

	/* spawn a thread to read the tracker data */
	pthread_create(&transducer_read_thread, NULL, &TrackerClientThread, (void *) TrackerClientObject);
}

} // extern "C"


























