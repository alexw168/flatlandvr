/*
dist_funcs.c.  A collection of random number generators for various
probability distributions.

Copyright (C) 2003, University of New Mexico.

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

/* 
 * Maintenance history
 *
 * Created: 1-17-00
 */

/*
   (Note that several of these algorithms came from Greg Heileman's
   text book on data structures and algorithms.)
   
   void fl_distributions_rantest()
   float fl_distributions_uniform( float a, float b )
   void fl_distributions_uniform_nD( int ndim, float *a , float *b, float *x )
   int fl_distributions_spirals( int nspirals, float gain, float rmax, float *x )
   float fl_distributions_normal(float mean, float sigma )
   void fl_distributions_normal_nD( int ndim, float *mean , float *sigma, float *x )
   float fl_distributions_exponential( float mean )
   float fl_distributions_poisson( float theta )
   void fl_distributions_spherical_angles(float *theta, float *phi ) 

*/

#include "gheader.h"

void fl_distributions_rantest()
{
	/* Call this function first to test if RAND_MAX is set
	   correctly for this machine.
	*/
	
	int i, j ;
	float x,sum,sum2,min,max ;

	sum = sum2 = 0.0 ;
	min = 1.0e35 ;
	max = -1.0e35 ;
	
	j = RAND_MAX ;
	
	printf("fl_distributions_rantest- ranmax: %d\n",j) ;

	printf("fl_distributions_rantest- Calculating...\n") ;
	for( i=0 ; i<10000 ; i++ ) {
		x= fl_distributions_uniform( -1.0, 1.0 ) ;
		if( x>max ) max = x ;
		if( x<min ) min = x ;
		sum += x ;
		sum2 += ( x * x ) ;
	}
	sum /= 10000.0 ;
	sum2 = sqrt( sum2/10000.0 - sum*sum ) ;
	printf("fl_distributions_rantest- n: 10000 max: %e min: %e ave: %e sigma: %e\n",max,min,sum,sum2) ;
	if( max<0.99 || max>1.0 || min<-1.0 || min>-0.99 ) {
		printf("                     - ERROR in random number generator, check RAND_MAX.\n") ;
		printf("fl_distributions_rantest- Good bye!\n") ;
		exit(0) ;
	}
	printf("fl_distributions_rantest- RAND_MAX ok!\n") ;
}

float fl_distributions_uniform( float a, float b)
{
/*  This routine returns a iD random float precision number in
    the range from a to b. tpc
*/
	return(a+(b-a)*((float)rand()/(float)RAND_MAX)) ;
}

void fl_distributions_uniform_nD( int ndim, float *a , float *b, float *x )
{
	/* This routine returns a uniform random distribution within a hyperbox
	   defined by the lower(min) vertix a and the upper(max) vertix b in ndim
	   dimensions.  Returns the random point in the vector x .
	*/
	int i ;
	
	for( i=0 ; i<ndim ; i++ ) {
		x[i] = fl_distributions_uniform(a[i],b[i]) ;
	}
}

int fl_distributions_spirals( int nspirals, float gain, float rmax, float *x )
{
/* This function produces a ndim=2 random vector and classifies it as part of
   a spiral region defined by the area between spirals defined by r = gain * theta.
   There are nspiral regions, the rate of growth determined by gain. Rmax is
   the largest radius of any spiral. x[0],x[1] contain the actual random number
   and the function returns an integer [0,nspiral-1] as the class. tpc
*/
	int i, k, kmax ;
	float rx=1.0e35, rik, deltar, dr, thetax, thetak, dtheta0 ;
	
	/* set up initial values */
	dtheta0 = PI2 / (float)nspirals ;
	kmax = 1+(int)(rmax/(gain*PI2)) ;
	deltar = dtheta0 * gain ;
	
	/* Calculate x such that it falls within the rmax circle and x[0]!=0 */
	x[0] = 1.0 ;
	while(x[0]!=0.0 && rx>rmax) {
		x[0] = fl_distributions_uniform(-rmax,rmax) ;
		x[1] = fl_distributions_uniform(-rmax,rmax) ;
		rx = sqrt( x[0]*x[0] + x[1]*x[1] ) ;
	}
	thetax = (float)atan2((double)x[1],(double)x[0]) ;
	thetax += PI ; 

	/* Start search of spiral arm that contains the point */
	for( k=0 ; k<=kmax ; k++ ) {
		thetak = k * PI2 ;
		for( i=0 ; i<nspirals ; i++ ) {
			rik = gain * ( thetax + thetak - dtheta0 * (float)i ) ;
			dr = rik - rx ;
			if( dr>=0.0 && dr<deltar)
				goto XX ;
		}
	}
	printf("fl_distributions_spirals- ERROR, arm not found.\n") ;
	exit(0) ;
XX:	; 
	return( i ) ;
}

float fl_distributions_normal(float mean, float sigma )
{
	/* This function uses Box-Muller method to return a normally distributed
       random number.
	*/
	float u1, u2, x ;
	
	u1 = fl_distributions_uniform(0.0, 1.0) ;
	u2 = fl_distributions_uniform(0.0, 1.0) ;
	
	x = (float)(cos(PI2*(double)u1)*sqrt(-2.0*log((double)u2))) ;

	return( sigma * x + mean ) ;
}

void fl_distributions_normal_nD( int ndim, float *mean , float *sigma, float *x )
{
	/* This routine returns a noraml random distribution 
	   defined by the mean vector and the sigma vector in ndim
	   dimensions.  Returns the random point in the vector x.
	   Assumes diagonal variance matrix (ie axis aligned elipses)
	*/
	int i ;
	
	for( i=0 ; i<ndim ; i++ ) {
		x[i] = fl_distributions_normal(mean[i],sigma[i]) ;
	}
}

float fl_distributions_exponential(float mean)
{
	/* Exponential distribution: f(x) = lamda * exp( -lamda*x )
	   where 1/lamda = mean 
	*/
	float u, x ;
	
	u = fl_distributions_uniform(0.0, 1.0) ;
	
	x = -(float)( log(1.0-u) * mean ) ;

	return( x ) ;
}

float fl_distributions_poisson( float theta )
{
	/* Poisson distribution: f(x) = exp(-theta) * theta^x / x!
	   where theta is the mean and variance.
	*/
	float x, m, u ;
	
	x = 1 ;
	m = (float)exp((double)theta) ;

	while( m>=1.0 ) {
		u = fl_distributions_uniform(0.0, 1.0) ;
		x = x + 1.0 ;
		m = m * u ;
	}
	return(x) ;
}

void fl_distributions_spherical_angles(float *theta, float *phi ) 
/* Produce the spherical coordinates (angle part only) uniformly
	distributed on the shpere.  Angles are in radians: 
	0<=theta<=2pi, 0<=phi<=pi 
*/
{
	float t,sphi ;
	
	do {
		t = fl_distributions_uniform(0.0,1.0) ;
		*phi = fl_distributions_uniform( 0.0, PI) ;
		sphi = sin(*phi) ;
	} while(sphi==0.0 || t>sphi ) ;
	*theta = PI2 * t / sphi ;
}

void fl_spherical_to_cartesian( float theta, float phi, float R, float *xx, float *yy, float *zz )
{
	float r ;
	
	r = R * sin(phi) ;
	*zz =(float)( R * cos(phi)) ;
	*yy =(float)( r * sin(theta)) ;
	*xx =(float)( r * cos(theta)) ;
	// printf("fl_spherical_to_cartesian-  r: %f *x: %f *y: %f *z: %f\n",r,*xx,*yy,*zz) ;
}
