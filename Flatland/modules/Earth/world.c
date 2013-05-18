/*
world.c.  World application.

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
 * Created: 1-18-00
 */

#include "flatland_app.h"
#include "world.h"
#include "sun.h"
#include "runway.h"
#include "logoart.h"

/* Globals */

/* "local" globals */
//static GLuint *PlanetList = NULL;

//static rectangular_mesh_type *Earth_mesh;

/* Constants */
const int MAX_NBR_OF_PLATFORMS = 10;
const GLfloat PLATFORM_BUFFER_DISTANCE = 5.0;

void platform_display(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
                      OBJECT_TYPE *object ) 
{
	float xw,zw ;
	float x,z,dx,dz;
	int i,nbeacons = 2 ;

        xw = xmax - xmin ;
        zw = zmax - zmin ;

	dx = xw / (float)nbeacons ;
	dz = zw / (float)nbeacons ;

	/* top face */
	glBegin(GL_QUADS ) ;
		glNormal3f(0.0,1.0,0.0) ;
		glVertex3f( xmax, ymax, zmax) ;
		glVertex3f( xmax, ymax, zmin) ;
		glVertex3f( xmin, ymax, zmin) ;
		glVertex3f( xmin, ymax, zmax) ;
	glEnd() ;

	for( i=0 ; i<=nbeacons ; i++ ) {
		z = zmax - (float)i * dz ;
		fl_beacon_display(xmax,ymax,z) ;
	}
	for( i=0 ; i<=nbeacons ; i++ ) {
		z = zmax - (float)i * dz ;
		fl_beacon_display(xmin,ymax,z) ;
	}
	for( i=1 ; i<nbeacons ; i++ ) {
		x = xmax - (float)i * dx ;
		fl_beacon_display(x,ymax,zmax) ;
	}
	for( i=1 ; i<nbeacons ; i++ ) {
		x = xmax - (float)i * dx ;
		fl_beacon_display(x,ymax,zmin) ;
	}
}

/* platform draw function for all platforms */
void draw_platform( OBJECT_TYPE *o ) {
        float y;

	char *tmpstr;
        char *tmpstr2;
        char *tmpstr3;
	float mamb[4],mdif[4],mspec[4] ;
	float lable_height = 3.0 ;

	if( o->drawing_on_flag==0) 
		return ;
	
  	if( o->viewable_flag==0 ) {
  		return ;
	}

    	mamb[0] = 0.5 ;
    	mamb[1] = 0.3 ;
  	mamb[2] = 0.1 ;
  	mamb[3] = 1.0 ;
 	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
 	mdif[0] = 0.7 ;
 	mdif[1] = 0.5 ;
    	mdif[2] = 0.3 ;
    	mdif[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
    	mspec[0] = 0.0 ;
    	mspec[1] = 0.0 ;
    	mspec[2] = 0.0 ;
    	mspec[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec) ;    
    	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 100.0);    

        // set the polygon offset for the platform
#if 0
  /* had to replace this with the following code because, if one of these is
   * not defined (like, for instance GL_POLYGON_OFFSET_EXT) it won't compile! */
  /* Set the polygon offset for the shadow */
        switch (Loop_Control->polygonOffsetVersion)
        {
          case EXTENSION:
            glEnable(GL_POLYGON_OFFSET_EXT);
            y = 0.0; // don't need a y offset when using polygon offset
            break;

          case ONE_DOT_ONE:
            glEnable(GL_POLYGON_OFFSET_FILL);
            y = 0.0; // don't need a y offset when using polygon offset
            break;

          case MISSING:
            /* Oh well. :) */
            y = 0.1;
            break;
        }
#endif
#ifdef GL_VERSION_1_1
        glEnable(GL_POLYGON_OFFSET_FILL);
	y = 0.0;
#else
  #ifdef GL_EXT_polygon_offset
        glEnable(GL_POLYGON_OFFSET_EXT);
	y = 0.0;
  #endif
#endif
        glPolygonOffset (-RUNWAY_FACTOR, -RUNWAY_OFFSET);
	if (y != 0.0)
	  glTranslated (0.0, y, 0.0);

 	platform_display(o->bounding_volume_set->bvset[0]->p[1][0],
                         o->bounding_volume_set->bvset[0]->p[2][0],
			 0.0,
			 0.0,
                         o->bounding_volume_set->bvset[0]->p[1][2],
                         o->bounding_volume_set->bvset[0]->p[4][2],
                         o) ;
   
#if 0
  /* had to replace this with the following code because, if one of these is
   * not defined (like, for instance GL_POLYGON_OFFSET_EXT) it won't compile! */
  /* Set the polygon offset for the shadow */
        // unset the polygon offset for the platform
        switch (Loop_Control->polygonOffsetVersion)
        {
          case EXTENSION:
            glDisable(GL_POLYGON_OFFSET_EXT);
            break;

          case ONE_DOT_ONE:
            glDisable(GL_POLYGON_OFFSET_FILL);
            break;

          case MISSING:
            /* Oh well. :) */
            break;
        }
#endif
#ifdef GL_VERSION_1_1
        glDisable(GL_POLYGON_OFFSET_FILL);
	y = 0.0;
#else
  #ifdef GL_EXT_polygon_offset
        glDisable(GL_POLYGON_OFFSET_EXT);
	y = 0.0;
  #endif
#endif

        tmpstr3 = tmpstr2 = strdup (o->name);
	tmpstr = strtok (tmpstr2, "-");

	glTranslatef(-lable_height+o->bounding_volume_set->bvset[0]->p[1][0]-0.2,0.1,-5.0) ;
	glRotatef(-90.0,0.0,0.0,1.0) ;
	glRotatef(-90.0,0.0,1.0,0.0) ;
	fl_draw_stroke_string( tmpstr, lable_height ) ;

    	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 0.0);    
        free (tmpstr3);
}

/* World Platform handler */
void platform_handle_world (OBJECT_TYPE *o)
{
  float PTemp[9];               /* temporary transform storage */

  /* TODO:  Come up with a real platform handler, not this hack */
  static int nbr_of_platforms = 0;
  static float current_available_x = 40.0;      /* -x edge of next platform */

  /* if there are more platforms than we can remember, 
     let it default & forget it */
  if (nbr_of_platforms >= MAX_NBR_OF_PLATFORMS)
    return;

  /* attach the platform */
  /* This just lines 'em up next to the road, starting @ the crossroads */
  /* TODO: do better */
  PTemp[0] = current_available_x - o->bounding_volume_set->bvset[0]->p[4][0];
  PTemp[1] = 0.0;                       /* y */
  PTemp[2] = -40.0 - o->bounding_volume_set->bvset[0]->p[4][2];    /* z */
  PTemp[3] = 0.0;                       /* pitch */
  PTemp[4] = 0.0;                       /* yaw */
  PTemp[5] = 0.0;                       /* roll */
  PTemp[6] = 0.0;                       /* scalex */
  PTemp[7] = 0.0;                       /* scaley */
  PTemp[8] = 0.0;                       /* scalez */

  o->orginal_first_node->MpoParams->P->assign (PTemp);

  o->orginal_first_node->MpoParams->Po[0] = TRANSLATE;

  /* set up platform drawing */
  o->drawfunc = draw_platform;
  o->drawing_on_flag = 1 ;

  /* set up the x value for the next object */
  /* (take the -(-x_point) + (+x_point) of bounding box + buffer distance) */
  current_available_x += -o->bounding_volume_set->bvset[0]->p[4][0] +
                          o->bounding_volume_set->bvset[0]->p[2][0] +
                          PLATFORM_BUFFER_DISTANCE;
}

/* World Sound callback */
void sound_world(OBJECT_TYPE *o)
{
}

void predraw_world(OBJECT_TYPE *o)
{
}

/* World Draw callback */
void draw_world(OBJECT_TYPE *o) 
{
  static bool first_pass = true;
  float mamb[4] = { 0.2, 0.4, 0.1, 1.0 };
  //float mdif[4] = { 0.1, 0.7, 0.2, 1.0 };
  //float mdif[4] = { 0.0, 0.0, 0.0, 1.0 };
  float mdif[4] = { 0.7, 0.6, 0.4, 1.0 };
  float mspec[4] = { 0.0, 0.0, 0.0, 1.0 };
  WORLD_GLOBALS *globals ;

  globals = (WORLD_GLOBALS*)o->ptr0 ;

  if( o->drawing_on_flag==0  || globals->display_flag==false)
    return ;

  /* Draw to the stencil and color buffers.  Replace every bit in the stencil 
   * buffer that corresponds to the earth & passes the depth test with a '1'. 
   * everything else with a 0. */
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0x00000001); /* draw if == 0 */
  glStencilOp(GL_REPLACE, GL_ZERO, GL_REPLACE);

  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;    
  
  /* make and draw earth surface mesh to display list */
  if (!glIsList(globals->PlanetList[current_main_window]))
  {
    globals->PlanetList[current_main_window] = glGenLists (1);
    glNewList( globals->PlanetList[current_main_window], GL_COMPILE_AND_EXECUTE ) ;
    if (globals->PlanetList[current_main_window] == 0)
      printf ("WARNING: Unable to create display list for the planet\n");

    fl_draw_mesh( globals->Earth_mesh ) ;
  
    glEndList() ;
  } else {
    if( globals->fill_flag == false )
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;
    if( globals->flat_flag==false)
      glCallList( globals->PlanetList[current_main_window] ) ;
    if( globals->fill_flag == false )
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
  }
 if (!glIsList(globals->PlanetListFlat[current_main_window]))
  {
    globals->PlanetListFlat[current_main_window] = glGenLists (1);
    glNewList( globals->PlanetListFlat[current_main_window], GL_COMPILE_AND_EXECUTE ) ;
    if (globals->PlanetListFlat[current_main_window] == 0)
      printf ("WARNING: Unable to create display list for the planet\n");

    fl_draw_mesh( globals->Earth_mesh_flat ) ;

    glEndList() ;
  } else {
    if( globals->fill_flag == false )
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;
    if( globals->flat_flag==true)
      glCallList( globals->PlanetListFlat[current_main_window] ) ;
    if( globals->fill_flag == false )
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
  }
  /* Turn off the stencil test (not strictly necessary) */
  glDisable(GL_STENCIL_TEST);

  /* draw the runway(s) */
  draw_runway (o);

  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;    
}

/* menu control callback */
void key_control_func (OBJECT_TYPE *o, char key)
{
  WORLD_GLOBALS *globals ;

  globals = (WORLD_GLOBALS *)o->ptr0 ;

  switch (key)
  {
    case 'r':   /* toggle runway */
    {
      if(globals->display_runway_flag ==  true )
        globals->display_runway_flag = false ;
      else
        globals->display_runway_flag = true ;
      break;
    }
    case 'f':   /* toggle fill */
    {
      if(globals->fill_flag ==  true )
        globals->fill_flag = false ;
      else
        globals->fill_flag = true ;
      break;
    }
    case 'p':   /* toggle flat */
    {
      if(globals->flat_flag ==  true )
        globals->flat_flag = false ;
      else
        globals->flat_flag = true ;
      break;
    }
    case 'd':   /* toggle display */
    {
      if(globals->display_flag ==  true )
        globals->display_flag = false ;
      else
        globals->display_flag = true ;
      break;
    }
    case 'l':   /* toggle display */
    {
      if(globals->display_logo_flag ==  true )
        globals->display_logo_flag = false ;
      else
        globals->display_logo_flag = true ;
      break;
    }
    break ;
  }
}

extern "C"
{
/* This is the well known initialization function. Everything comes from here*/
void Flatland_init (APPTYPE *App)
{
  int control_index = 0 ;
  bool display_sun = true;         /* should the sun be displayed? */
  bool fill_flag = true ;
  bool display_runway = true ;
  bool flat_flag = false ;
  bool display_flag = true ;
  char airport_fn [MAX_NAME_SIZE];      /* airport layout file name */
  float earthxmax=1000.0, earthxmin = -1000.0, earthzmax = 1000.0, earthzmin= -1000.0;

  int tmp;                      /* temporary */
  float PTemp[9];               /* temporary transform storage */
  OBJECT_TYPE *World;           /* the world object */
  GLfloat sun_loc [3];          /* location of the sun */
  GLfloat four_pts [4] [3];     /* four bounding corners of shadow rectangle */
  WORLD_GLOBALS *globals;       /* pointer to "globals" structure */
  OBJ_CONTROLS_ARRAY_TYPE *subMenu; /* submenu temp variable */

  /* parse the parameter list */
  for (int i=0; i < App->nparams; i++)
  {
    if (!strcmp (App->params[i][0], "airport_fn"))
      strncpy (airport_fn, App->params[i][2], MAX_NAME_SIZE);
    if (!strcmp (App->params[i][0], "fill"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      fill_flag = (bool)tmp ;
    }
    if (!strcmp (App->params[i][0], "runway"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      display_runway = (bool)tmp ;
    }
    if (!strcmp (App->params[i][0], "flat"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      flat_flag = (bool)tmp ;
    }
    if (!strcmp (App->params[i][0], "display"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      display_flag = (bool)tmp ;
    }

    if (!strcmp (App->params[i][0], "earthxmax"))
    {
      sscanf (App->params[i][2], "%f", &earthxmax);
    }
    if (!strcmp (App->params[i][0], "earthzmax"))
    {
      sscanf (App->params[i][2], "%f", &earthzmax);
    }
    if (!strcmp (App->params[i][0], "earthxmin"))
    {
      sscanf (App->params[i][2], "%f", &earthxmin);
    }
    if (!strcmp (App->params[i][0], "earthzmin"))
    {
      sscanf (App->params[i][2], "%f", &earthzmin);
    }
  }

  /* initialize the object for the world */
  /* make new object */
  World = fl_add_child_to_parent_by_name(App->universe_ptr,
                                      App->parent_object_name,
                                      App->appname) ;

#ifdef Flatland_version_C
  /* add the app name to the object, so it can be unloaded */
//  NOTE:  commented out April 2, 2001 by KLS.  Unloading earth is almost
//  always a bad idea as almost everything depends on it.
//  World->creator_app_ptr = App;
#endif

 /* generate the "globals" structure & attach it to the object */
  if ((globals = (WORLD_GLOBALS *) malloc (sizeof (WORLD_GLOBALS))) == NULL)
  {
    printf ("ERROR: failed to initialize memory for world globals\n");
    exit(1);
  }
  globals->fill_flag = fill_flag ;
  globals->display_runway_flag = display_runway ;
  globals->flat_flag = flat_flag ;
  globals->display_flag = display_flag ;
  globals->earthxmax = earthxmax ;
  globals->earthzmax = earthzmax ;
  globals->earthxmin = earthxmin ;
  globals->earthzmin = earthzmin ;
  if ( ( globals->PlanetList = (GLuint*) malloc( sizeof( GLuint ) * Loop_Control->NumberOfWindows ) ) == NULL)
  {
    fprintf(stderr, "%s: failed to initialize memory for Planetlist\n", App->appname );
    fflush(stderr);
    exit(1);
  }
  if ( ( globals->PlanetListFlat = (GLuint*) malloc( sizeof( GLuint ) * Loop_Control->NumberOfWindows ) ) == NULL)
  {
    fprintf(stderr, "%s: failed to initialize memory for PlanetlistFlat\n", App->appname );
    fflush(stderr);
    exit(1);
  }
  if ( ( globals->PlanetListTones = (GLuint*) malloc( sizeof( GLuint ) * Loop_Control->NumberOfWindows ) ) == NULL)
  {
    fprintf(stderr, "%s: failed to initialize memory for PlanetlistTones\n", App->appname );
    fflush(stderr);
    exit(1);
  }
  globals->Earth_mesh = fl_makerecmesh(40, 40, earthxmin,earthxmax,earthzmin,earthzmax )	;
  fl_mesh_rim_mountains( 8, 150.0, 0.05,  globals->Earth_mesh ) ;
  globals->Earth_mesh_flat = fl_makerecmesh(40, 40, earthxmin,earthxmax,earthzmin,earthzmax )	;
  fl_mesh_rim_mountains( 8, 0.0, 0.7,  globals->Earth_mesh_flat ) ;

  World->ptr0 = (void *) globals;      /* attach struct to object */


  /* set up the bounding volumes (position & culling) */
  World->bounding_volume_set = fl_makebvset(2) ;

  /* set up callbacks */
  World->soundfunc = sound_world ;
  World->predrawfunc = predraw_world ;
  World->drawfunc =  draw_world ;
  World->platformfunc = platform_handle_world ;
  World->trueShadowFunc = NULL ;        /* the world doesn't cast shadows */
  World->cheapShadowFunc = NULL ;       /* the world doesn't cast shadows */
  /* TODO:  conceivably the world _could_ cast shadows:  make an appropriate
   * shadow for the world */

  /* Because the world will have shadows cast apon it, set up shadow receiving.
   * We are using only shadow plane 0 here. */
  /* NOTE: The four points of the plane *MUST* be specified in CCW order */
  sun_loc [0] = sunx;
  sun_loc [1] = suny;
  sun_loc [2] = sunz;
  four_pts [0] [0] = earthxmax;
  four_pts [0] [1] = 0.0;
  four_pts [0] [2] = earthzmin;
  four_pts [1] [0] = earthxmin;
  four_pts [1] [1] = 0.0;
  four_pts [1] [2] = earthzmin;
  four_pts [2] [0] = earthxmin;
  four_pts [2] [1] = 0.0;
  four_pts [2] [2] = earthzmax;
  four_pts [3] [0] = earthxmax;
  four_pts [3] [1] = 0.0;
  four_pts [3] [2] = earthzmax;
  if (fl_shadow_init (World, 0, sun_loc, four_pts) != 0)
    /* shadows were initialized OK... turn 'em on */
    World->shadows_on_flag [0] = GL_TRUE;

  /* finally, turn on flags */
  World->sounding_on_flag = 0 ;
  World->drawing_on_flag = 1 ;

  /* attach the world in a default place */
  PTemp[0] = 0.0; //worldx;
  PTemp[1] = 0.0; //worldy;
  PTemp[2] = 0.0; //worldz;
  PTemp[3] = 0.0; //worldpitch;
  PTemp[4] = 0.0; //worldyaw;;
  PTemp[5] = 0.0; //worldroll;
  PTemp[6] = 1.0;
  PTemp[7] = 1.0;
  PTemp[8] = 1.0;
  World->orginal_first_node->MpoParams->Po[0] = TRANSLATE;
  World->orginal_first_node->MpoParams->P->assign( PTemp );

  /* allows control of the object from the keyboard, menues, and voice */
  World->object_controls = fl_makeControlsSet(5);

  /* Define controls for earth */
  control_index = 0 ;

  strcpy (World->object_controls->controls[control_index]->menuname, "Toggle Display");
  strcpy (World->object_controls->controls[control_index]->voicename, "Toggle Display");
  World->object_controls->controls[control_index]->keystroke = 'd';
  World->object_controls->controls[control_index++]->control_function = key_control_func ;

  strcpy (World->object_controls->controls[control_index]->menuname, "Toggle Runway");
  strcpy (World->object_controls->controls[control_index]->voicename, "Toggle Runway");
  World->object_controls->controls[control_index]->keystroke = 'r';
  World->object_controls->controls[control_index++]->control_function = key_control_func ;

  strcpy (World->object_controls->controls[control_index]->menuname, "Toggle Fill");
  strcpy (World->object_controls->controls[control_index]->voicename, "Toggle Fill");
  World->object_controls->controls[control_index]->keystroke = 'f';
  World->object_controls->controls[control_index++]->control_function = key_control_func ;

  strcpy (World->object_controls->controls[control_index]->menuname, "Toggle Planarity");
  strcpy (World->object_controls->controls[control_index]->voicename, "Toggle Planarity");
  World->object_controls->controls[control_index]->keystroke = 'p';
  World->object_controls->controls[control_index++]->control_function = key_control_func ;


  /* create & initialize the Sun object */
  init_sun(App);
  
  /* create & initialize the Runway object */
  init_runway(App, airport_fn);

  /* create & initialize the LogoArt object */
  init_logoart(App);
}
} // extern "C"
