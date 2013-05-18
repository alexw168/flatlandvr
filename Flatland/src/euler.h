/*
 * euler.h
 *
 * Header file for euler.c
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
#ifndef __EULER_H
#define __EULER_H

#include <math.h>

  /*      Here are the Transformation Functions.   */

  /*      In all cases, matrix is a 4 x 4 matrix of 3D coordinates.  */

void fl_MoveMatrix(float *matrix, float distance, char direction, bool inverse);

void fl_TranslateMatrix(float *matrix, 
                     float Xtrans, float Ytrans, float Ztrans, 
                     bool inverse);

  /*   fl_RotateMatrixR is for radian rotations,
       and angleCoords are:
       'x', 'y', or 'z'.  */

void fl_RotateMatrixR(float *matrix, float new_angle, char angleCoord, bool inverse);

  /*   fl_RotateMatrixD is for degree rotations, 
       and angleCoords are:
       'x', 'y', or 'z'.  */

void fl_RotateMatrixD(float *matrix, float new_angle, char angleCoord, bool inverse);

  /*  This function changes the scale of a marix in the X, Y, and Z directoins.
  */
void fl_ScaleMatrix(float *matrix, 
                 float Xscale, float Yscale, float Zscale,
                 bool inverse);

/*  Basic Object Tree Functions  */
/*  None of these functions delete objects.  They simply extract them from
    the tree, or place them in the tree.  */
/*  When an object is removed from the tree, it retains it's Mco matrix, and
    it's MpoParams, but it's first_node->neighbor_node is set to Null.
    All children remain intact.  */

bool fl_RemoveObject(OBJECT_TYPE *object);
bool fl_PlaceObject(OBJECT_TYPE *object, OBJECT_TYPE *parent);
bool fl_ExtractObject(OBJECT_TYPE *object);
bool fl_InsertObject(OBJECT_TYPE *object, OBJECT_TYPE *marker);

/*  Parent Changing Object Functions  */

bool fl_ChangeParentAll(OBJECT_TYPE *object, OBJECT_TYPE *target);
bool fl_ChangeParentExtract(OBJECT_TYPE *object, OBJECT_TYPE *target);
bool fl_ChangeParentAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);
bool fl_ChangeParentExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);

bool fl_ChangeParentTeleportAll(OBJECT_TYPE *object, OBJECT_TYPE *target);
bool fl_ChangeParentTeleportExtract(OBJECT_TYPE *object, OBJECT_TYPE *target);
bool fl_ChangeParentTeleportAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);
bool fl_ChangeParentTeleportExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker);


  /*    Here are the functions for translating a Matrix from Quaternion to Euler.  */


void fl_GetMatrixTranslations(float *matrix, float &Xtrans, float &Ytrans, float &Ztrans);

  /*   fl_GetMatrixAnglesR is for reading matrix angles is radian format.  
       The angles returned are for transformations done in the order:
       X rotate, then Y rotate, then Z rotate.*/

void fl_GetMatrixAnglesR(float *matrix, float &nXangle, float &nYangle, float &nZangle);

  /*   fl_GetMatrixAnglesD is for reading matrix angles is degree format.  
       The angles returned are for transformations done in the order:
       X rotate, then Y rotate, then Z rotate.*/

void fl_GetMatrixAnglesD(float *matrix, float &nXangle, float &nYangle, float &nZangle);

void fl_GetMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale);

   /*  This function extracts the scale of a matrix, returning the X, Y, and
       Z scales that the matrix had before they were removed.  */
void fl_RemoveMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale);

/*  This Matrix Multiply multiplies the left matrix by the right matrix, and
    returns the result in the left matrix.  */
void fl_MatrixMultiply(float *matrix1, float *matrix2);

/*  This Matrix Multiply multiplies the right matrix by the left matrix, and
    returns the result in the left matrix.  */
void Conversefl_MatrixMultiply(float *matrix1, float *matrix2);

/*  This function is used to get the inverse of a transformation matrix  */
void fl_GetInverse(float *in_matrix, float *out_matrix);

/*  This function is used to change an objects Mpo before they are moved in 
    the tree, so that the visual object doesn't move */
void fl_Stabilize(OBJECT_TYPE *object, OBJECT_TYPE *target);

void fl_Read_Matrix_Ang_W_Scale(float *matrix,
                             float &nXangle, float &nYangle, float &nZangle,
                             float &Xscale, float &Yscale, float &Zscale);

void fl_Read_Matrix_Ang_W_ScaleD(float *matrix,
                              float &nXangle, float &nYangle, float &nZangle,
                              float &Xscale, float &Yscale, float &Zscale);

#endif
