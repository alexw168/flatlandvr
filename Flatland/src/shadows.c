/*
shadows.cpp.  All things shadows.

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
 * Created: 1-14-00
 */

#include "gheader.h"

/* constants */

/* globals */
Boolean enableTrueShadows = FALSE;

/* "local" globals */
static GLfloat shadowamb [] = {0.03,0.03,0.03,0.8}, /* shadow mat props */
               shadowdif [] = {0.03,0.03,0.03,0.8},
               shadowspec [] ={0.0 ,0.0 ,0.0 ,1.0};

#ifdef NO_POFFSET
/* this overrides the glPolygonOffset() in GL.  This is because NVIDIA Linux
 * drivers don't support (in fact crash because of) this call.  It is here
 * because this is the only place (at this time) this function is used in the
 * Flatland kernel */
/* TODO:  remove this ASAP */
void glPolygonOffset(GLfloat dummy1, GLfloat dummy2) {};
#endif

/* initialize a shadow plane, place the shadow projection matrix in the given
 * object.  This routine is called by any app wanting to have shadows cast 
 * apon it.  The shadow matrix is stored inside the calling object's object 
 * structure.  The shadow plane number (which shadow plane is accessed) is
 * used by all of the shadow routines, but strictly managed by the app.  The
 * four points mark the corners of the rectangle that will be used to define
 * the shadow plane.  They are used to calculate the projection matrix *and*
 * bound the stencil buffer plane drawn to cover shadows.  Typically these will
 * be the corners of the object having shadows cast apon it. 
 * NOTE: They *must* be specified in CCW order! 
 * Returns 1 on success, 0 on failure */

int fl_shadow_init(OBJECT_TYPE *o, 
                 int plane_nbr,
                 GLfloat sun [3],
                 GLfloat four_points [4] [3])
{
  int i,j;      /* loop vars */
  GLfloat groundPlane   [4];
  GLfloat lightPosition [4];

  if (plane_nbr >= MAX_SHADOW_PLANES)
    return (0);

  /* find the plane using the first 3 points of the four planer points */
  fl_findPlane(groundPlane, /* coords describe the XZ plane */
    four_points [0] [0], four_points [0] [1], four_points [0] [2],
    four_points [1] [0], four_points [1] [1], four_points [1] [2],
    four_points [2] [0], four_points [2] [1], four_points [2] [2]);
#if 0
printf ("%f, %f, %f\n%f, %f, %f\n%f, %f, %f\n\n",four_points [0] [0], four_points [0] [1], four_points [0] [2],four_points [1] [0], four_points [1] [1], four_points [1] [2],four_points [2] [0], four_points [2] [1], four_points [2] [2]);
  fl_findPlane(groundPlane,
     0.0, 0.0, 0.0, 
     0.0, 0.0, 1.0,
     1.0, 0.0, 0.0);
#endif
  lightPosition [0] = sun [0];
  lightPosition [1] = sun [1];
  lightPosition [2] = sun [2];
  lightPosition [3] = 1.0;
  
  /* calculate the shadow matrix and store it in the object */
  fl_shadowMatrix(o->shadowMat [plane_nbr], groundPlane, lightPosition);

  /* copy the four points to the object */
  for (i=0; i < 4; i++)
  {
    for (j=0; j < 3; j++)
      o->shadowPlanePts [plane_nbr] [i] [j] = four_points [i] [j];

    o->shadowPlanePts [plane_nbr] [i] [3] = 1.0;
  }

  /* copy the four light source coordinate points to the object */
  for (i = 0; i < 4; i++)
    o->shadowLightSrc [plane_nbr] [i] = lightPosition [i];

  return (1);
}

void fl_draw_shadow_from_stencil(OBJECT_TYPE *o, int shadow_plane)
{
  /* Draw shadows from the stencil buffer */
  /* turn off the depth mask, enable blending, and set blending type */
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Only render where the first bit of the stencil is set. */
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 0x00000001); /* draw if == 1 */
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

#if 0
  /* had to replace this with the following code because, if one of these is
   * not defined (like, for instance GL_POLYGON_OFFSET_EXT) it won't compile! */
  /* Set the polygon offset for the shadow */
  switch (Loop_Control->polygonOffsetVersion)
  {
    case EXTENSION:
      glEnable(GL_POLYGON_OFFSET_EXT);
      break;

    case ONE_DOT_ONE:
      glEnable(GL_POLYGON_OFFSET_FILL);
      break;

    case MISSING:
/* TODO: may want to put a y(ish) offset here, may not */
      /* Oh well. :) */
      break;
  }
#endif
#ifdef GL_VERSION_1_1
  glEnable(GL_POLYGON_OFFSET_FILL);
#else
  #ifdef GL_EXT_polygon_offset
  glEnable(GL_POLYGON_OFFSET_EXT);
  #endif
#endif
  glPolygonOffset (-SHADOW_FACTOR, -SHADOW_OFFSET);

  /* set the shadow material properties */
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,shadowamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,shadowdif) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,shadowspec) ;

  /* draw the shadow plane at ground level */
  glBegin(GL_POLYGON);
    glNormal3d (0.0, 1.0, 0.0);
    glVertex3f (o->shadowPlanePts [shadow_plane] [0] [0],
                o->shadowPlanePts [shadow_plane] [0] [1],
                o->shadowPlanePts [shadow_plane] [0] [2]);
    glVertex3f (o->shadowPlanePts [shadow_plane] [1] [0],
                o->shadowPlanePts [shadow_plane] [1] [1],
                o->shadowPlanePts [shadow_plane] [1] [2]);
    glVertex3f (o->shadowPlanePts [shadow_plane] [2] [0],
                o->shadowPlanePts [shadow_plane] [2] [1],
                o->shadowPlanePts [shadow_plane] [2] [2]);
    glVertex3f (o->shadowPlanePts [shadow_plane] [3] [0],
                o->shadowPlanePts [shadow_plane] [3] [1],
                o->shadowPlanePts [shadow_plane] [3] [2]);
  glEnd();

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  /* unset the polygon offset */
#if 0
  switch (Loop_Control->polygonOffsetVersion)
  {
    case EXTENSION:
      glDisable(GL_POLYGON_OFFSET_EXT);
      break;

    case ONE_DOT_ONE:
      glDisable(GL_POLYGON_OFFSET_FILL);
      break;

    case MISSING:
      /* oh well. :) */
      break;
  }
#endif
#ifdef GL_VERSION_1_1
  glDisable(GL_POLYGON_OFFSET_FILL);
#else
  #ifdef GL_EXT_polygon_offset
  glDisable(GL_POLYGON_OFFSET_EXT);
  #endif
#endif

  /* Disable the stencil buffer */
  glDisable(GL_STENCIL_TEST);
} 

/* Create a matrix that will project the desired shadow. */
void fl_shadowMatrix(GLfloat shadowMat[4][4], 
                  GLfloat groundplane[4],
                  GLfloat lightpos[4])
{
  GLfloat dot;

  /* Find dot product between light position vector and ground plane normal. */
  dot = groundplane[X] * lightpos[X] +
    groundplane[Y] * lightpos[Y] +
    groundplane[Z] * lightpos[Z] +
    groundplane[W] * lightpos[W];

  shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
  shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
  shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
  shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

  shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
  shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
  shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
  shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

  shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
  shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
  shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
  shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

  shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
  shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
  shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
  shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

}

/* Find the plane equation given 3 points. */
void fl_findPlane(GLfloat plane[4], 
               GLfloat v0X, GLfloat v0Y, GLfloat v0Z,
               GLfloat v1X, GLfloat v1Y, GLfloat v1Z,
               GLfloat v2X, GLfloat v2Y, GLfloat v2Z)
{
  GLfloat vec0[3], vec1[3];

  /* Need 2 vectors to find cross product. */
  vec0[X] = v1X - v0X;
  vec0[Y] = v1Y - v0Y;
  vec0[Z] = v1Z - v0Z;

  vec1[X] = v2X - v0X;
  vec1[Y] = v2Y - v0Y;
  vec1[Z] = v2Z - v0Z;

  /* find cross product to get A, B, and C of plane equation */
  plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
  plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
  plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

  plane[D] = -(plane[A] * v0X + plane[B] * v0Y + plane[C] * v0Z);
}

/* fl_shadow_setup sets up the environment for shadow drawing.  This avoids
 * passing huge amounts of data to application programs to deal with shadows.
 * If shadowing is turned off, or for any other reason shadows for this object
 * are not wanted, fl_shadow_setup returns -1.0 and shadows should not be drawn. 
 * If shadows should be drawn, fl_shadow_setup returns the y offset to be used
 * (before appropriate scaling at the application end).
 */
GLfloat fl_shadow_setup (void)
{
  GLfloat y = 0.f;

  /* Only draw to the stencil buffer.  Replace every bit in the stencil buffer
   * that coresponds to a shadow with a '1'. */
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_GREATER, 1, 0x00000001); /* draw if == 0 */
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

  /* Set the polygon offset for the shadow */
  switch (Loop_Control->polygonOffsetVersion)
  {
    case EXTENSION:
      y = 0.0; /* don't need a y offset when using polygon offset */
      break;

    case ONE_DOT_ONE:
      y = 0.0; /* don't need a y offset when using polygon offset */
      break;

    case MISSING:
      /* Oh well. :) */
      y = SHADOW_OFFSET;
      break;
  }

  /* turn off drawing in the color buffer... the shadow will be drawn later
   * from the stencil buffer */
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  return y;
}

/* fl_shadow_cleanup is called after a shadow has been drawn to reset the
 * envirnoment.  Nothing should be done between calls to fl_shadow_setup and
 * fl_shadow_cleanup except local calculations and the drawing of the actual
 * shadow polygons.  Everything else is taken care of by these two routines.
 */
void fl_shadow_cleanup (void)
{
  glDisable(GL_STENCIL_TEST); /* disable stencil testing */

  /* turn drawing in the color buffer back on */
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

