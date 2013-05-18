/*
console.c.  Console package.  Includes console, wands, and locomotion code

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
 * Created: 1-24-00
 */

#include "flatland_app.h"
#include "console.h"
//#include "awand.h"
//#include "wands.h"
//#include "locomotion.h"

/* Globals */
GLuint *ConsoleList=NULL;       /* Console display list */

/* "local" globals */
//static UNIVERSE_TYPE *console_universe_ptr; /* the universe of the console */

/* Constants */

/* Platform handler */
void platform_handler_console (OBJECT_TYPE *o)
{
}

/* Sound callback */
void sound_console (OBJECT_TYPE *o)
{
}

/* Draw callback */
void draw_console (OBJECT_TYPE *o) 
{
//	int current_main_window = Loop_Control->Window;
	
  if (glIsList(ConsoleList[current_main_window]))
  {
    glCallList( ConsoleList[current_main_window] );
  }
  else
  {
    ConsoleList[current_main_window] = glGenLists( 1 );
    glNewList( ConsoleList[current_main_window], GL_COMPILE_AND_EXECUTE );
      make_console( );
    glEndList( );
  }
}

void predraw_console (OBJECT_TYPE *o)
{
  /* Beam intersect both wands with objects (this is done here since it
   * applies to both wands */
  //beam_intersect_wands (console_universe_ptr, Wand1, Wand2);

  /* Test if wands are in locomotion region of the console */
//  wand_loco_control (console_universe_ptr, o, Wand1, Wand2);
}

/* actual console drawing routine */
void make_console (void)
{
	float mamb[4],mdif[4],mspec[4] ;

	mamb[0] = 0.2 ;
	mamb[1] = 0.2 ;
	mamb[2] = 0.2 ;
	mamb[3] = 1.0 ;
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
	mdif[0] = 0.4 ;
	mdif[1] = 0.4 ;
	mdif[2] = 0.3 ;
	mdif[3] = 1.0 ;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
	mspec[0] = 0.0 ;
	mspec[1] = 0.0 ;
	mspec[2] = 0.0 ;
	mspec[3] = 1.0 ;
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;

	// Dongshan modify on 1-10-2002

	glPushMatrix() ;
	glTranslatef(0.02,-0.025,0.025) ;
	fl_draw_cube(0.965,0.45,0.005) ;
	glPopMatrix() ;

	// the following is original one
	/* Draw main desk plate */

	/*
	glPushMatrix() ;
	glTranslatef(0.0,0.165,0.0) ;
	fl_draw_cube(0.61,0.33,0.005) ;
	glPopMatrix() ;

	glRotatef(-35.0,1.0,0.0,0.0) ;

        Draw side arms 
	
	glPushMatrix() ;
	glTranslatef(0.30625,0.016,0.0) ;
	fl_draw_cube(0.1025,0.5325,0.005) ;
	glPopMatrix() ;
	glPushMatrix() ;
	glTranslatef(-0.30625,0.016,0.0) ;
	fl_draw_cube(0.1025,0.5325,0.005) ;
	glPopMatrix() ;

        Draw loco rests 
	glPushMatrix() ;
	glTranslatef(0.375,0.0975,0.005) ;
	fl_draw_cube(0.14,0.195,0.005) ;
	glPopMatrix() ;
	glPushMatrix() ;
	glTranslatef(-0.375,0.0975,0.005) ;
	fl_draw_cube(0.14,0.195,0.005) ;
	glPopMatrix() ;
	*/

	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0) ;
}

void draw_console_cheap_shadow (OBJECT_TYPE *o)
{
}

extern "C"
{
/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
	float PTemp[9];                 /* temporary transform storage */
	OBJECT_TYPE *Console;
	float CONSOLE_X = 0.0;
	float CONSOLE_Y = 0.0;
	float CONSOLE_Z = 0.0;
	bool known;
	char *Name = App->appname;
	int i;
		
	if ( ( ConsoleList = new GLuint[ Loop_Control->NumberOfWindows ] ) == NULL )
	{
		fprintf(stderr, "%s: failed to initialize memory for ConsoleDataMembers\n", App->appname);
		fflush(stderr);
		exit(1);
	}

	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &CONSOLE_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &CONSOLE_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &CONSOLE_Z);
			known = true;
		}
/***
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale"))
		{
			sscanf(App->params[i][2], "%f", &VESSEL_SCALE);
			known = true;
		}
***/

		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}
	
	
	/* initialize the object for the console */
	/* make new object */
	Console =  fl_add_child_to_parent_by_name(App->universe_ptr,
                        	 App->parent_object_name,
                        	 App->appname) ;

	/* set up the bounding volumes (position & culling) */
	Console->bounding_volume_set = fl_makebvset(2) ;

	/* set up callbacks */
	Console->soundfunc = sound_console ;
	Console->predrawfunc =  predraw_console ;
	Console->drawfunc =  draw_console ;
	Console->platformfunc = platform_handler_console ;
	Console->trueShadowFunc = draw_console ;  /* same as drawfunc */
	Console->cheapShadowFunc = draw_console_cheap_shadow ;

	/* Finally, turn on flags */
	Console->sounding_on_flag = 0 ;
	Console->drawing_on_flag = 1 ;

	/* Define bounding sphere for culling */
	Console->bounding_volume_set->bvset[1]->npoints = 2 ;
	Console->bounding_volume_set->bvset[1]->p[0][0]=0.0 ;
	Console->bounding_volume_set->bvset[1]->p[0][1]=0.0 ;
	Console->bounding_volume_set->bvset[1]->p[0][2]=0.0 ;
	Console->bounding_volume_set->bvset[1]->p[1][0]=0.7 ;
	Console->bounding_volume_set->bvset[1]->p[1][1]=0.0 ;
	Console->bounding_volume_set->bvset[1]->p[1][2]=0.0 ;

	/* attach the console in a default place */
	PTemp[0] = CONSOLE_X; 
	PTemp[1] = CONSOLE_Y;
	PTemp[2] = CONSOLE_Z; 
	//	PTemp[3] = -55.0;
	PTemp[3] = 90.0;
	PTemp[4] = 0.0;
	PTemp[5] = 0.0;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	Console->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	Console->orginal_first_node->MpoParams->Po[1] = ROTATE100 ;
	Console->orginal_first_node->MpoParams->P->assign( PTemp );

	/* save the universe pointer */
	//  console_universe_ptr = App->universe_ptr;

	/* initialize the wands */
	//  wands_init (App);
}
} // extern "C"
