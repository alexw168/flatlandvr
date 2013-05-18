/* Euler.c
   
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
#include "gheader.h"
#include "euler.h"

void _Rotate_Matrix_Vector(float *vector, char angleCoord, float new_angle)
   {
   float rotate_matrix[4][4];
   float temp_vector[4];
   float temp_sum;

   int i, j;
   float sin_ang, cos_ang;
   sin_ang = sin(new_angle);
   cos_ang = cos(new_angle);

   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         rotate_matrix[i][j] = (i==j);
         }
      }

   switch(angleCoord)
      {
      case('x'): case('X'):
         {
         rotate_matrix[1][1] = cos_ang;
         rotate_matrix[2][2] = cos_ang;
         rotate_matrix[2][1] = -sin_ang;
         rotate_matrix[1][2] = sin_ang;
         break;
         }
      case('y'): case('Y'):
         {
         rotate_matrix[0][0] = cos_ang;
         rotate_matrix[2][2] = cos_ang;
         rotate_matrix[2][0] = sin_ang;
         rotate_matrix[0][2] = -sin_ang;
         break;
         }
      case('z'): case('Z'):
         {
         rotate_matrix[0][0] = cos_ang;
         rotate_matrix[1][1] = cos_ang;
         rotate_matrix[1][0] = -sin_ang;
         rotate_matrix[0][1] = sin_ang;
         break;
         }
      default :
         {
         fprintf(stderr, "ERROR : Invalid Rotate Coordinate Given.\n");
         fprintf(stderr, "Proper Coordinates are : 'x', 'y', or 'z'\n");
         return;
         }
      }


   for (i = 0; i < 4; i++)
      {
      temp_sum = 0.0;
      for (j = 0; j < 4; j++)
         {
         temp_sum += rotate_matrix[j][i] * vector[j];
         }
      temp_vector[i] = temp_sum;
      }

   for (i = 0; i < 4; i++)
      {
      vector[i] = temp_vector[i];
      }
   }


void _Rotate_Matrix_Multiply(float *matrix, float new_angle, char angleCoord, bool inverse)
   {
   int i, j;
   float sin_ang, cos_ang;
   sin_ang = sin(new_angle);
   cos_ang = cos(new_angle);

   float rotate_matrix[4][4];

   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         rotate_matrix[i][j] = (i==j);
         }
      }

   switch(angleCoord)
      {
      case('x'): case('X'):
         {
         rotate_matrix[1][1] = cos_ang;
         rotate_matrix[2][2] = cos_ang;
         rotate_matrix[2][1] = -sin_ang;
         rotate_matrix[1][2] = sin_ang;
         break;
         }
      case('y'): case('Y'):
         {
         rotate_matrix[0][0] = cos_ang;
         rotate_matrix[2][2] = cos_ang;
         rotate_matrix[2][0] = sin_ang;
         rotate_matrix[0][2] = -sin_ang;
         break;
         }
      case('z'): case('Z'):
         {
         rotate_matrix[0][0] = cos_ang;
         rotate_matrix[1][1] = cos_ang;
         rotate_matrix[1][0] = -sin_ang;
         rotate_matrix[0][1] = sin_ang;
         break;
         }
      default :
         {
         fprintf(stderr, "ERROR : Invalid Rotate Coordinate Given.\n");
         fprintf(stderr, "Proper Coordinates are : 'x', 'y', or 'z'\n");
         return;
         }
      }
   if (inverse)
      {
      Conversefl_MatrixMultiply(matrix, &rotate_matrix[0][0]);
      }
   else
      {
      fl_MatrixMultiply(matrix, &rotate_matrix[0][0]);
      }
   }



void _Get_Matrix_Vector_Angle(float &angle, float *vector, char angleCoord)
   {
   float temp_sum = 0;
   float magnitude;
   float sin_ang;
   float cos_ang;

   int i;

   if ((vector[0] == 0) && (vector[1] == 0))
      {
      vector[0] = .0000001;
      vector[1] = .0000001;
      }

   for (i = 0; i < 3; i++)
      {
      temp_sum += vector[i] * vector[i];
      }

   magnitude = sqrt(temp_sum);

   switch (angleCoord)
      {
      case 'x': case 'X':
         {
         sin_ang = vector[2] / magnitude;
         cos_ang = vector[1] / magnitude;
         break;
         }
      case 'y': case 'Y':
         {
         sin_ang = (-vector[2]) / magnitude;
         cos_ang = vector[0] / magnitude;
         break;
         }
      case 'z': case 'Z':
         {
         sin_ang = vector[1] / magnitude;
         cos_ang = vector[0] / magnitude;
         break;
         }
      default :
         {
         fprintf(stderr, "ERROR : Angle Translate Coordinate Given.\n");
         fprintf(stderr, "Proper Coordinates are : 'x', 'y', or 'z'\n");
         return;
         }
      }

   angle = atan2(sin_ang, cos_ang);
   }


void _Read_Matrix_Angles(float *matrix, float &nXangle, float &nYangle, float &nZangle)
   {
   int i, j;
  
   float x[4], y[4];

   float nmatrix[16];

   float xparallel[4];

   float Xscale, Yscale, Zscale;

   for (i = 0; i < 3; i++)
      {
      for (j = 0; j < 4; j++)
         {
         nmatrix[i*4+j] = matrix[i*4+j];
         }
      nmatrix[3*4+i] = (i == 3);
      }

   fl_RemoveMatrixScale(nmatrix, Xscale, Yscale, Zscale);

   for (i = 0; i < 2; i++)
      {
      for (j = 0; j < 4; j++)
         {
         switch(i)
            {
            case 0:
               {
               x[j] = nmatrix[i*4+j];
               xparallel[j] = nmatrix[i*4+j];
               break;
               }
            case 1:
               {
               y[j] = nmatrix[i*4+j];
               break;
               }
            }
         }
      }

   xparallel[2] = 0;

   _Get_Matrix_Vector_Angle(nZangle, xparallel, 'z');

   _Rotate_Matrix_Vector(x, 'z', (-nZangle));

   _Get_Matrix_Vector_Angle(nYangle, x, 'y');

   _Rotate_Matrix_Vector(y, 'z', (-nZangle));

   _Rotate_Matrix_Vector(y, 'y', (-nYangle));

   _Get_Matrix_Vector_Angle(nXangle, y, 'x');

   }


void fl_Read_Matrix_Ang_W_Scale(float *matrix, 
                             float &nXangle, float &nYangle, float &nZangle,
                             float &Xscale, float &Yscale, float &Zscale)
  {
  int i, j;
 
  float x[4], y[4];

  float nmatrix[16];

  float xparallel[4];

  for (i = 0; i < 3; i++)
     {
     for (j = 0; j < 4; j++)
        {
        nmatrix[i*4+j] = matrix[i*4+j];
        }
     nmatrix[3*4+i] = (i == 3);
     }

  fl_RemoveMatrixScale(nmatrix, Xscale, Yscale, Zscale);
 
  for (i = 0; i < 2; i++)
     {
     for (j = 0; j < 4; j++)
        {
        switch(i)
           {
           case 0:
              {
              x[j] = nmatrix[i*4+j];
              xparallel[j] = nmatrix[i*4+j];
              break;
              }
           case 1:
              {
              y[j] = nmatrix[i*4+j];
              break;
              }
           }
        }
     }

  xparallel[2] = 0;

  _Get_Matrix_Vector_Angle(nZangle, xparallel, 'z');

  _Rotate_Matrix_Vector(x, 'z', (-nZangle));

  _Get_Matrix_Vector_Angle(nYangle, x, 'y');

  _Rotate_Matrix_Vector(y, 'z', (-nZangle));

  _Rotate_Matrix_Vector(y, 'y', (-nYangle));

  _Get_Matrix_Vector_Angle(nXangle, y, 'x');
  }

void fl_Read_Matrix_Ang_W_ScaleD(float *matrix, 
                              float &nXangle, float &nYangle, float &nZangle,
                              float &Xscale, float &Yscale, float &Zscale)
  {
  fl_Read_Matrix_Ang_W_Scale(matrix, 
                          nXangle, nYangle, nZangle,
                          Xscale, Yscale, Zscale);
  nXangle *= 180 / PI; 
  nYangle *= 180 / PI; 
  nZangle *= 180 / PI; 
  }

void fl_ScaleMatrix(float *matrix, 
                 float scaleX, float scaleY, float scaleZ,
                 bool inverse)
   {
   int i, j;
   float scale_matrix[16];
   if (scaleX == 0)
      {
      scaleX = .00001;
      }
   if (scaleY == 0)
      {
      scaleY = .00001;
      }
   if (scaleZ == 0)
      {
      scaleZ = .00001;
      }
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         scale_matrix[i*4+j] = (i == j);
         }
      }
   scale_matrix[0*4+0] = scaleX;
   scale_matrix[1*4+1] = scaleY;
   scale_matrix[2*4+2] = scaleZ;
   if (inverse)
      {
      Conversefl_MatrixMultiply(matrix, scale_matrix);
      }
   else
      {
      fl_MatrixMultiply(matrix, scale_matrix);
      }
   }


bool fl_RemoveObject(OBJECT_TYPE *object)
   {
   if (object != NULL)
      {
      if (strcmp(object->name, object->universe->name))
         {
         int i;
         OBJECT_TYPE *parent; 
         GRAPH_NODE_TYPE *object_ptr, *tmp_ptr;
         parent = object->first_node->neighbor_node->self;
         int neighbors = parent->nneighbors; 
         tmp_ptr = parent->first_node; 

      /*  Here I test if the object I want is at tmo_ptr  (this can be the
          parent's parent node, but not it's REAL parent, if the original
          parent object is also the root object.  In which case, the original
          parent node's parent is really a child.)  */

         if (tmp_ptr->neighbor_node->self == object)
            {
         /*  If we are at the object's node, and it's a first node, we
             set the right_sibling node as the new first_node, and the last
             sibling node in the list will now point to the new first node.  
             object_ptr is the pointer that points to node whose neighbor_node
             is the object itself's first_node. */
            parent->first_node = tmp_ptr->right_sibling_node;
            object_ptr = tmp_ptr;
            for (i = 0; i < neighbors - 1; i++)
               {
               tmp_ptr = tmp_ptr->right_sibling_node;
               }
            tmp_ptr->right_sibling_node = parent->first_node;
            
            }
         else
            {
            for (i = 0; i < neighbors; i++)
               {
            /* If we find our object somewhere in the middle of the list, we
               can just cut it out, and correct all of our pointers.  */ 
               if (tmp_ptr->right_sibling_node->neighbor_node->self == object) 
                  {
                  object_ptr = tmp_ptr->right_sibling_node;
                  tmp_ptr->right_sibling_node = object_ptr->right_sibling_node;
                  }
               tmp_ptr = tmp_ptr->right_sibling_node;
               }
            }
#if 0
	 fprintf(stderr, "Calling free(), object_ptr is %#x\n", object_ptr);
         free(object_ptr);
	 fprintf(stderr, "Finished free()\n");
#endif
         parent->nneighbors--; 
         object->first_node->neighbor_node = NULL;
         return true;
         }
      else
         {
         fprintf(stderr, "ERROR: Object %s to be removed is the root object.\n",
                 object->name);
         fprintf(stderr, "Root object cannot ever be removed from the tree.\n");
         return false;
         }
      }
   else
      {
      fprintf(stderr, "ERROR: fl_RemoveObject() was passed a NULL object.\n");
      return false;
      }
   }

bool fl_PlaceObject(OBJECT_TYPE *object, OBJECT_TYPE *parent)
   {
   if ((object != NULL) && (parent != NULL) && (object != parent))
      {
      fl_makegraphnode(parent);
      object->first_node->neighbor_node = parent->first_node->right_sibling_node;
      parent->first_node->right_sibling_node->neighbor_node = object->first_node;
      parent->first_node->right_sibling_node->MpoParams =
                          object->first_node->MpoParams;
      return(true);
      }
   else if (object == parent)
      {
      fprintf(stderr, "ERROR: fl_PlaceObject() was passed two identical objects.\n"); 
      return(false);
      }
   else
      {
      fprintf(stderr, "ERROR: fl_PlaceObject() was passed a NULL object.\n");
      return(false);
      }
   }


void HopBackParent(OBJECT_TYPE *object, OBJECT_TYPE *parent)
   {
   fl_Stabilize(object, parent);
   fl_RemoveObject(object);
   fl_PlaceObject(object, parent);
   object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
   }


bool fl_ExtractObject(OBJECT_TYPE *object)
   {
   if (object != false)
      {
      OBJECT_TYPE *parent;
      parent = object->first_node->neighbor_node->self; 
      if (fl_RemoveObject(object))
         {
         int i, neighbors;
         GRAPH_NODE_TYPE *tmp_ptr;  
         OBJECT_TYPE *tmp_object;
         tmp_ptr = object->first_node->right_sibling_node;
         neighbors = object->nneighbors - 1; 
         for (i = 0; i < neighbors; i++)
            {
            tmp_object = tmp_ptr->neighbor_node->self; 
            tmp_ptr = tmp_ptr->right_sibling_node;
            HopBackParent(tmp_object, parent);
            }
         return(true);
         }
      else
         {
         fprintf(stderr, "fl_ExtractObject() cancelled due to previous error.\n");
         return false;
         }
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ExtractObject() was passed a NULL object.\n");
      return false;
      }
   }


bool fl_InsertObject(OBJECT_TYPE *object, OBJECT_TYPE *marker)
   {
   if ((object != NULL) && (marker != NULL) && (object != marker))
      {
      OBJECT_TYPE *parent = marker->first_node->neighbor_node->self;
      if (fl_RemoveObject(marker))
         {
         fl_PlaceObject(object, parent);
         fl_xform_update(object->universe);
         fl_Stabilize(marker, object);
         fl_PlaceObject(marker, object);
         return(true);
         }
      else
         {
         printf("Error:  The Object %s could not be removed for insertion.\n",
                 marker->name);
         return(false);
         }
      }
   else if (object == marker)
      {
      fprintf(stderr, "ERROR: fl_InsertObject() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_InsertObject() was passed a NULL object\n");
      return false;
      }
   }

bool TestTree(OBJECT_TYPE *object, OBJECT_TYPE *parent)
   {
   GRAPH_NODE_TYPE *ptr = object->first_node->right_sibling_node;
   int i = 1;
   int neighbors = object->nneighbors;
   bool test_case1 = true;
   bool test_case2 = false;
   while((test_case1) && (!test_case2) && (i < neighbors) && (ptr))
      {
      if (ptr->neighbor_node)
         {
         test_case1 = strcmp(ptr->neighbor_node->self->name, parent->name);
         }
      if (ptr->neighbor_node != NULL) 
         {
         test_case2 = TestTree(ptr->neighbor_node->self, parent); 
         }
      ptr = ptr->right_sibling_node;
      i++; 
      }
   if ((test_case1) && (!test_case2))
      {
      return(false);
      }
   else
      {
      return(true);
      }
   }


bool fl_ChangeParentAll(OBJECT_TYPE *object, OBJECT_TYPE *target)
   {
   if ((object != NULL) && (target != NULL) && (object != target))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if(fl_RemoveObject(object))
         {
         fl_Stabilize(object, target);
         fl_PlaceObject(object, target);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return(true);
         }
      else
         {
         fl_PlaceObject(object, parent);
         fprintf(stderr, "ERROR: fl_ChangeParentAll() failed due to object errors\n"); 
         return false;
         }
      }
   else if (object == target)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentAll() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentAll() was passed a NULL object.\n"); 
      return false;
      }
   }


bool fl_ChangeParentExtract(OBJECT_TYPE *object, OBJECT_TYPE *target)
   {
   if ((object != NULL) && (target != NULL) && (object != target))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if (fl_ExtractObject(object))
         {
         fl_Stabilize(object, target);
         fl_PlaceObject(object, target);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return(true);
         }
      else
         {
         fl_PlaceObject(object, parent);
         fprintf(stderr, " fl_ChangeParentExtract(%s , %s) cancelled due to previous error.\n", object->name, target->name); 
         return false;
         }
      }
   else if (object == target)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentExtract() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentExtract() was passed a NULL object\n"); 
      return false;
      }
   }

bool fl_ChangeParentAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
   {
   if ((object != NULL) && (marker != NULL) && (object != marker))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if (fl_RemoveObject(object))
         {
         if (!TestTree(object, marker))
            {
            fl_Stabilize(object, marker->first_node->neighbor_node->self);
            fl_InsertObject(object, marker);
            object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
            return true;
            }
         else
            {
            fl_InsertObject(object, parent);
            fprintf(stderr, "ERROR: In fl_ChangeParentAllInsert(%s, %s), %s is a child of %s.\n fl_ChangeParentAllInsert() cancelled.\n", 
                    object->name, marker->name, marker->name, object->name); 
            return false;
            }
         }
      else
         {
         fprintf(stderr, " fl_ChangeParentAllInsert(%s , %s) was cancelled due to previous error.\n", object->name, marker->name); 
         return false;
         }
      }
   else if (object == marker)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentAllInsert() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentAllInsert() was passed a NULL object.\n"); 
      return false;
      }
   }

bool fl_ChangeParentExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
   {
   if ((object != NULL) && (marker != NULL) && (object != marker))
      {
      if (fl_ExtractObject(object))
         {
         fl_Stabilize(object, marker->first_node->neighbor_node->self);
         fl_InsertObject(object, marker);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return true;
         }
      else
         {
         fprintf(stderr, " fl_ChangeParentExtractInsert(%s , %s) cancelled due to previous error.\n", object->name, marker->name); 
         return false;
         }
      }
   else if (object == marker)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentExtractInsert() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentExtractInsert() was passed a NULL object.\n"); 
      return false;
      }
   }


bool fl_ChangeParentTeleportAll(OBJECT_TYPE *object, OBJECT_TYPE *target)
   {
   if ((object != NULL) && (target != NULL) && (object != target))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if(fl_RemoveObject(object))
         {
         fl_PlaceObject(object, target);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return(true);
         }
      else
         {
         fl_PlaceObject(object, parent);
         fprintf(stderr, "ERROR: fl_ChangeParentTeleportAll() failed due to object errors\n"); 
         return false;
         }
      }
   else if (object == target)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportAll() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportAll() was passed a NULL object.\n"); 
      return false;
      }
   }


bool fl_ChangeParentTeleportExtract(OBJECT_TYPE *object, OBJECT_TYPE *target)
   {
   if ((object != NULL) && (target != NULL) && (object != target))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if (fl_ExtractObject(object))
         {
         fl_PlaceObject(object, target);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return(true);
         }
      else
         {
         fl_PlaceObject(object, parent);
         fprintf(stderr, " fl_ChangeParentTeleportExtract(%s , %s) cancelled due to previous error.\n", object->name, target->name); 
         return false;
         }
      }
   else if (object == target)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportExtract() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportExtract() was passed a NULL object\n"); 
      return false;
      }
   }


bool fl_ChangeParentTeleportAllInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
   {
   if ((object != NULL) && (marker != NULL) && (object != marker))
      {
      OBJECT_TYPE *parent = object->first_node->neighbor_node->self;
      if (fl_RemoveObject(object))
         {
         if (!TestTree(object, marker))
            {
            fl_InsertObject(object, marker);
            object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
            return(true);
            }
         else
            {
            fl_InsertObject(object, parent);
            fprintf(stderr, "ERROR: In fl_ChangeParentTeleportAllInsert(%s, %s), %s is a child of %s.\n fl_ChangeParentAllInsert() cancelled.\n", object->name, marker->name, marker->name, object->name); 
            return false;
            }
         }
      else
         {
         fprintf(stderr, " fl_ChangeParentTeleportAllInsert(%s , %s) was cancelled due to previous error.\n", object->name, marker->name); 
         return false;
         }
      }
   else if (object == marker)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportAllInsert() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportAllInsert() was passed a NULL object.\n"); 
      return false;
      }
   }


bool fl_ChangeParentTeleportExtractInsert(OBJECT_TYPE *object, OBJECT_TYPE *marker)
   {
   if ((object != NULL) && (marker != NULL) && (object != marker))
      {
      if (fl_ExtractObject(object))
         {
         fl_InsertObject(object, marker);
         object->first_node->neighbor_node->MpoParams = object->first_node->MpoParams;
         return(true);
         }
      else
         {
         fprintf(stderr, " fl_ChangeParentTeleportExtractInsert(%s , %s) cancelled due to previous error.\n", 
                 object->name, marker->name); 
         return false;
         }
      }
   else if (object == marker)
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportExtractInsert() was passed two identical objects.\n"); 
      return false;
      }
   else
      {
      fprintf(stderr, "ERROR: fl_ChangeParentTeleportExtractInsert() was passed a NULL object.\n"); 
      return false;
      }
   }


void fl_GetMatrixTranslations(float *matrix, float &Xtrans, float &Ytrans, float &Ztrans)
   {
   Xtrans = matrix[3*4+0];
   Ytrans = matrix[3*4+1];
   Ztrans = matrix[3*4+2];
   }


void fl_GetMatrixAnglesR(float *matrix, 
                      float &nXangle, float &nYangle, float &nZangle)
   {
   _Read_Matrix_Angles(matrix, nXangle, nYangle, nZangle);
   }


void fl_GetMatrixAnglesD(float *matrix, 
                      float &nXangle, float &nYangle, float &nZangle)
   {
   _Read_Matrix_Angles(matrix, nXangle, nYangle, nZangle);
   nXangle *= 180 / PI; 
   nYangle *= 180 / PI; 
   nZangle *= 180 / PI; 
   }



void fl_RotateMatrixR(float *matrix, float new_angle, char angleCoord, bool inverse)
   {
   _Rotate_Matrix_Multiply(matrix, new_angle, angleCoord, inverse);
   }



void fl_RotateMatrixD(float *matrix, float new_angle, char angleCoord, bool inverse)
   {
   new_angle *= PI / 180;
   _Rotate_Matrix_Multiply(matrix, new_angle, angleCoord, inverse);
   }


void Conversefl_MatrixMultiply(float *matrix1, float *matrix2)
   {
   int i, j, k;
   float temp_matrix[4][4];
   float temp_sum;

   for(i = 0; i < 4; i++)
      {
      for(j = 0; j < 4; j++)
         {
         temp_sum = 0.0;
         for(k = 0; k < 4; k++)
            {
            temp_sum += (matrix2[k*4+i] * matrix1[j*4+k]);
            }
         temp_matrix[i][j] = temp_sum;
         }
      }
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         matrix1[j*4+i] = temp_matrix[i][j];
         }
      }
   }


void fl_MatrixMultiply(float *matrix, float *rotate_matrix)
   {
   int i, j, k;
   float temp_matrix[4][4];
   float temp_sum;

   for(i = 0; i < 4; i++)
      {
      for(j = 0; j < 4; j++)
         {
         temp_sum = 0.0;
         for(k = 0; k < 4; k++)
            {
            temp_sum += (matrix[k*4+i] * rotate_matrix[j*4+k]);
            }
         temp_matrix[i][j] = temp_sum;
         }
      }
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         matrix[j*4+i] = temp_matrix[i][j];
         }
      }
   }


void fl_TranslateMatrix(float *matrix, 
                     float Xtrans, float Ytrans, float Ztrans, 
                     bool inverse) 
   {
   float temp_matrix[4][4];
   int i, j;
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         temp_matrix[i][j] = (i==j);
         }
      }

   temp_matrix[3][0] = Xtrans;
   temp_matrix[3][1] = Ytrans;
   temp_matrix[3][2] = Ztrans;
  
   if (inverse)
      {
      Conversefl_MatrixMultiply(matrix, &temp_matrix[0][0]);
      }
   else
      {
      fl_MatrixMultiply(matrix, &temp_matrix[0][0]);
      }
   }


void fl_MoveMatrix(float *matrix, float distance, 
                char direction, bool inverse)
   {
   float temp_matrix[4][4];
   int i, j;
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         temp_matrix[i][j] = (i==j);
         }
      }
   switch(direction)
      {
      case 'x' : case 'X':
         {
         temp_matrix[3][0] = distance;
         break;
         }
      case 'y' : case 'Y':
         {
         temp_matrix[3][1] = distance;
         break;
         }
      case 'z' : case 'Z':
         {
         temp_matrix[3][2] = distance;
         break;
         }
      default :
         {
         fprintf(stderr, "ERROR: Invalid Translate Coordinate Given.\n");
         fprintf(stderr, "Proper Coordinates are : 'x', 'y', or 'z'\n");
         return;
         }
      }
   if (inverse)
      {
      Conversefl_MatrixMultiply(matrix, &temp_matrix[0][0]);
      }
   else
      {
      fl_MatrixMultiply(matrix, &temp_matrix[0][0]);
      }
   }


void fl_GetMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale)
   {
   int i;
   Xscale = 0;
   Yscale = 0;
   Zscale = 0;
   for(i = 0; i < 4; i++)
      {
      Xscale += matrix[0*4+i] * matrix[0*4+i];
      Yscale += matrix[1*4+i] * matrix[1*4+i];
      Zscale += matrix[2*4+i] * matrix[2*4+i];
      }
   Xscale = sqrt(Xscale);
   Yscale = sqrt(Yscale);
   Zscale = sqrt(Zscale);
   }


void fl_RemoveMatrixScale(float *matrix, float &Xscale, float &Yscale, float &Zscale)
   {
   int i;
   Xscale = 0;
   Yscale = 0;
   Zscale = 0;
   for(i = 0; i < 4; i++)
      {
      Xscale += matrix[0*4+i] * matrix[0*4+i];
      Yscale += matrix[1*4+i] * matrix[1*4+i];
      Zscale += matrix[2*4+i] * matrix[2*4+i];
      }
   Xscale = sqrt(Xscale);
   Yscale = sqrt(Yscale);
   Zscale = sqrt(Zscale);
   for (i = 0; i < 4; i++)
      {
      matrix[0*4+i] /= Xscale;
      matrix[1*4+i] /= Yscale;
      matrix[2*4+i] /= Zscale;
      }
   }

void fl_Stabilize(OBJECT_TYPE *object, OBJECT_TYPE *target)
   {
   float matrix[16];
   float o_matrix[16];
   float t_matrix[16];
   float t_Xtrans, t_Ytrans, t_Ztrans;
   float t_Xangle, t_Yangle, t_Zangle;
   float t_Xscale, t_Yscale, t_Zscale;
   int i, j;

   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         o_matrix[i*4+j] = object->Mco[i*4+j];
         t_matrix[i*4+j] = target->Mco[i*4+j];
         }
      }
   
   fl_GetInverse(t_matrix, matrix);

   Conversefl_MatrixMultiply(o_matrix, matrix);

   fl_GetMatrixTranslations(o_matrix, t_Xtrans, t_Ytrans, t_Ztrans);
   fl_Read_Matrix_Ang_W_ScaleD(o_matrix, 
                            t_Xangle, t_Yangle, t_Zangle,
                            t_Xscale, t_Yscale, t_Zscale);
   float PTemp[9];

   PTemp[0] = t_Xtrans;
   PTemp[1] = t_Ytrans;
   PTemp[2] = t_Ztrans;
   PTemp[3] = t_Xangle;
   PTemp[4] = t_Yangle;
   PTemp[5] = t_Zangle;
   PTemp[6] = t_Xscale;
   PTemp[7] = t_Yscale;
   PTemp[8] = t_Zscale;

   object->orginal_first_node->MpoParams->P->assign(PTemp);
   object->orginal_first_node->MpoParams->Po[0] = TRANSLATE;
   object->orginal_first_node->MpoParams->Po[1] = ROTATE001;
   object->orginal_first_node->MpoParams->Po[2] = ROTATE010;
   object->orginal_first_node->MpoParams->Po[3] = ROTATE100;
   object->orginal_first_node->MpoParams->Po[4] = SCALE;
   }


void fl_GetInverse(float *in_matrix, float *out_matrix)
   {
   int i, j;

   float in_Xtrans, in_Ytrans, in_Ztrans;
   float in_Xscale, in_Yscale, in_Zscale;
   float in_Xangle, in_Yangle, in_Zangle;

   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         {
         out_matrix[i*4+j] = (i==j);
         }
      }

   fl_GetMatrixTranslations(in_matrix, in_Xtrans, in_Ytrans, in_Ztrans);
   fl_Read_Matrix_Ang_W_Scale(in_matrix, 
                           in_Xangle, in_Yangle, in_Zangle,
                           in_Xscale, in_Yscale, in_Zscale);
   
   fl_TranslateMatrix(out_matrix, -in_Xtrans, -in_Ytrans, -in_Ztrans, 1);
   fl_RotateMatrixR(out_matrix, (-in_Zangle), 'z', 1);
   fl_RotateMatrixR(out_matrix, (-in_Yangle), 'y', 1);
   fl_RotateMatrixR(out_matrix, (-in_Xangle), 'x', 1);
   fl_ScaleMatrix(out_matrix, (1/in_Xscale), (1/in_Yscale), (1/in_Zscale), 1);
   }

void MoveAndRestructureSubtree (OBJECT_TYPE *detach_obj, OBJECT_TYPE *sub_root,
                                OBJECT_TYPE *target)
   {
   fl_RemoveObject (detach_obj);
   fl_restructure_subtree (detach_obj, sub_root);
   fl_PlaceObject (sub_root, target);
   }
