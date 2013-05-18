/*
 * dist_funcs.h
 *
 * Header file for dist_funcs.c
 *
 * Copyright (C) 2003, University of New Mexico.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *           
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef DIST_FUNCS_H
#define DIST_FUNCS_H


/* dist_funcs.h - A collection definitions for random number generators
   for various probability distributions .
   
   Thomas Caudell
   Created: 17 January 2000
*/
   
void fl_distributions_rantest() ;
float fl_distributions_uniform( float a, float b ) ;
void fl_distributions_uniform_nD( int ndim, float *a , float *b, float *x ) ;
int fl_distributions_spirals( int nspirals, float gain, float rmax, float *x ) ;
float fl_distributions_normal(float mean, float sigma ) ;
void fl_distributions_normal_nD( int ndim, float *mean , float *sigma, float *x ) ;   
float fl_distributions_exponential(float mean) ;
float fl_distributions_poisson( float theta ) ;
void fl_distributions_spherical_angles(float *theta, float *phi ) ;
void fl_spherical_to_cartesian( float theta, float phi, float R, float *x, float *y, float *z ) ;


#endif
