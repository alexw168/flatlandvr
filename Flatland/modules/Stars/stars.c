/*
stars.c.  Stars that can be attached to a world

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
 * Created: 2-18-00, KLS
 */

/* 
 * General OBJECT_TYPE variables used:
 * object_state0        => display list number
 * object_state1        => number of stars
 * object_param0        => star shell radius
 * object_param1        => maximum star intensity
 */

/* constants */
const int DEFAULT_NBR_OF_STARS = 4000;
const float DEFAULT_SHELL_RADIUS = 6000.0;
const float DEFAULT_MAX_STAR_INTENSITY = 0.3;

#include "flatland_app.h"

/* Platform handler */
void platform_handler_stars (OBJECT_TYPE *o)
{
}

/* Sound callback */
void sound_stars (OBJECT_TYPE *o)
{
}

/* Routine to create the stars disply list */
int create_stars(int n, float R, float MAX_STAR_INTENSITY)
{
  float theta, phi,r ;
  float base ;
  float x,y,z,red,blue,green ;
  int i;
  GLuint StarsList = 0;
  
  base = 0.4 ;
  srand(1) ;

  StarsList = glGenLists (1);
  glNewList( StarsList, GL_COMPILE ) ;
    glPointSize(2.0) ;
    glBegin(GL_POINTS) ;
    
    for( i=0 ; i<n ; i++ ) {
      do {
        fl_distributions_spherical_angles(&theta, &phi) ;
      } while( phi<0.0 ) ; /* Force stars to be only in the upper hemisphere */
      r =(float)( R * sin(phi)) ;
      z =(float)( R * cos(phi)) ;
      y =(float)( r * sin(theta)) ;
      x =(float)( r * cos(theta)) ;
      red = base+(1.0-base)*fl_distributions_uniform(0.0,MAX_STAR_INTENSITY) ;
      green = base+(1.0-base)*fl_distributions_uniform(0.0,MAX_STAR_INTENSITY) ;
      blue = base+(1.0-base)*fl_distributions_uniform(0.0,MAX_STAR_INTENSITY) ;
      glColor3f(red, green, blue) ;
         glVertex3f(x, y, z) ;       
    }
    glEnd() ;
  glEndList() ;

  /* return the list */
  return (StarsList);
}

/* Draw callback */
void draw_stars (OBJECT_TYPE *o)
{
  GLboolean light0_flag ;
  GLuint *StarsListPtr;
//	int current_main_window = Loop_Control->Window;

  if( o->drawing_on_flag==0) 
    return ;
  
  light0_flag = glIsEnabled(GL_LIGHTING) ;
  if( light0_flag==GL_TRUE )
    glDisable(GL_LIGHTING) ;
  
  if (o->ptr0 == NULL)
    o->ptr0 = (void *) calloc ( Loop_Control->NumberOfWindows, sizeof(GLuint));

  StarsListPtr = (GLuint *) o->ptr0;
  if (!glIsList(StarsListPtr [current_main_window]))
    StarsListPtr [current_main_window] = create_stars( o->object_state1,
                                                      o->object_param0,
                                                      o->object_param1 ) ;

  glCallList( StarsListPtr [current_main_window] ) ;
  
  if( light0_flag==GL_TRUE )
    glEnable(GL_LIGHTING) ;  
}

void draw_stars_cheap_shadow (OBJECT_TYPE *o)
{
}

void predraw_stars (OBJECT_TYPE *o)
{
}

extern "C"
{
/* This is the well known initialization function. Everything comes from here*/
void Flatland_init (APPTYPE *App)
{
  float PTemp[9];                 /* temporary transform storage */
  OBJECT_TYPE *Stars;

  /* initialize the object for the stars object */
  /* make new object */
  Stars = fl_add_child_to_parent_by_name(App->universe_ptr,
                                       App->parent_object_name,
                                       App->appname) ;

#ifdef Flatland_version_C
  /* add the app name to the object, so it can be unloaded */
  Stars->creator_app_ptr = App;
#endif

  /* insert default parameters */
  Stars->object_state1 = DEFAULT_NBR_OF_STARS;
  Stars->object_param0 = DEFAULT_SHELL_RADIUS;
  Stars->object_param0 = DEFAULT_MAX_STAR_INTENSITY;

  /* parse the parameter list */
  for (int i=0; i < App->nparams; i++)
  {
    if (!strcmp (App->params[i][0], "nbr_of_stars"))
      sscanf(App->params[i][2], "%d", &Stars->object_state1);
    if (!strcmp (App->params[i][0], "radius"))
      sscanf(App->params[i][2], "%f", &Stars->object_param0);
    if (!strcmp (App->params[i][0], "max_intensity"))
      sscanf(App->params[i][2], "%f", &Stars->object_param1);
  }

  /* set up the bounding volumes (position & culling) */
  Stars->bounding_volume_set = fl_makebvset(2) ;

  /* set up callbacks */
  Stars->soundfunc = NULL ;
  Stars->predrawfunc = NULL ;
  Stars->drawfunc =  draw_stars ;
  Stars->platformfunc = NULL ;
  Stars->trueShadowFunc = NULL ;
  Stars->cheapShadowFunc = NULL ;

  /* Finally, turn on flags */
  Stars->sounding_on_flag = 0 ;
  Stars->drawing_on_flag = 1 ;

  /* Define bounding box */
  Stars->bounding_volume_set->bvset[0]->npoints = 5 ;
  Stars->bounding_volume_set->bvset[0]->p[0][0]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[0][1]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[0][2]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][0]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][1]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][2]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[2][0]=-Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[2][1]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[2][2]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[3][0]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[3][1]=-Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[3][2]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[4][0]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[4][1]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[4][2]=-Stars->object_param0 ;

  Stars->bounding_volume_set->bvset[0]->npoints = 2 ;
  Stars->bounding_volume_set->bvset[0]->p[0][0]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[0][1]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[0][2]= 0.0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][0]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][1]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[0]->p[1][2]= Stars->object_param0 ;

  /* Define bounding sphere for culling */
  Stars->bounding_volume_set->bvset[1]->npoints = 2 ;
  Stars->bounding_volume_set->bvset[1]->p[0][0]= 0.0 ;
  Stars->bounding_volume_set->bvset[1]->p[0][1]= 0.0 ;
  Stars->bounding_volume_set->bvset[1]->p[0][2]= 0.0 ;
  Stars->bounding_volume_set->bvset[1]->p[1][0]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[1]->p[1][1]= Stars->object_param0 ;
  Stars->bounding_volume_set->bvset[1]->p[1][2]= Stars->object_param0 ;
    
  /* attach the stars in a default place */
  PTemp[0] = 0.0;       /* x */
  PTemp[1] = 0.0;       /* y */
  PTemp[2] = 0.0;       /* z */ 
  PTemp[3] = 0.0;       /* pitch */
  PTemp[4] = 0.0;       /* yaw */
  PTemp[5] = 0.0;       /* roll */
  PTemp[6] = 1.0;
  PTemp[7] = 1.0;
  PTemp[8] = 1.0;

  Stars->orginal_first_node->MpoParams->P->assign( PTemp );
  
  /* initialize the stars display list variable */
  Stars->ptr0 = NULL;
}
} // extern "C"
