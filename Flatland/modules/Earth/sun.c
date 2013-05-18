/*
sun.c.  routines to initialize and perceptualize the sun

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

#include "sun.h"
#include "world.h"

/* Sun Platform handler */
void platform_handle_sun (OBJECT_TYPE *o)
{
}

/* Sun Sound callback */
void sound_sun(OBJECT_TYPE *o)
{
}

/* Sun predraw callback */
void predraw_sun(OBJECT_TYPE *o)
{
  /* set the light parameters here so they are ready when the other objects
   * begin drawing */

SUN_GLOBALS *globals ;

globals = (SUN_GLOBALS*)o->ptr0 ;

if( o->drawing_on_flag==0) {
glDisable(GL_LIGHT0);
return;
}
  float ldir0[4],lamb0[4],ldif0[4],lspec0[4] ;

  /* set light0 parameters */
  glEnable(GL_LIGHT0) ;
  ldir0[0] = sunx ;
  ldir0[1] = suny ;
  ldir0[2] = sunz ;
  ldir0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_POSITION, ldir0 ) ;
  lamb0[0] = 0.5 ;
  lamb0[1] = 0.5 ;
  lamb0[2] = 0.5 ;
  lamb0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_AMBIENT, lamb0 ) ;
  ldif0[0] = 1.0 ;
  ldif0[1] = 1.0 ;
  ldif0[2] = 1.0 ;
  ldif0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, ldif0 ) ;
  lspec0[0] = 0.5 ;
  lspec0[1] = 0.5 ;
  lspec0[2] = 0.5 ;
  lspec0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_SPECULAR, lspec0) ;
}

void lighting_sun(OBJECT_TYPE *o)
{
  /* set the light parameters here so they are ready when the other objects
   * begin drawing */

SUN_GLOBALS *globals ;

globals = (SUN_GLOBALS*)o->ptr0 ;

if( o->drawing_on_flag==0) {
glDisable(GL_LIGHT0);
return;
}

  float ldir0[4],lamb0[4],ldif0[4],lspec0[4] ;

  /* set light0 parameters */
  glEnable(GL_LIGHT0) ;
  ldir0[0] = sunx ;
  ldir0[1] = suny ;
  ldir0[2] = sunz ;
  ldir0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_POSITION, ldir0 ) ;
  lamb0[0] = 0.5 ;
  lamb0[1] = 0.5 ;
  lamb0[2] = 0.5 ;
  lamb0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_AMBIENT, lamb0 ) ;
  ldif0[0] = 1.0 ;
  ldif0[1] = 1.0 ;
  ldif0[2] = 1.0 ;
  ldif0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, ldif0 ) ;
  lspec0[0] = 0.5 ;
  lspec0[1] = 0.5 ;
  lspec0[2] = 0.5 ;
  lspec0[3] = 1.0 ;
  glLightfv(GL_LIGHT0, GL_SPECULAR, lspec0) ;
}
    
/* Sun Draw callback */
void draw_sun(OBJECT_TYPE *o)
{
  float sunR ;
  float mamb[4],mdif[4],mspec[4],memis[4] ;
  SUN_GLOBALS *globals ;

  globals = (SUN_GLOBALS*)o->ptr0 ;
  
  if( o->drawing_on_flag==0) {
  	glDisable(GL_LIGHT0) ;
        /* TODO: reenable when shadows are working */
  	//shadows_on_flag = 0 ;
  	return ;
  }
  
  /* enable sun light0 */
  glEnable(GL_LIGHT0) ;

  /* Set bounding sphere */
  /* NOTE: no bounding sphere for the sun.  If it were culled, the lighting
   * model would cease to work */
  sunR = suny / 20.0 ; // 0.5 degree 

  /* Draw sphere for sun */
  mamb[0] = 1.0 ;
  mamb[1] = 1.0 ;
  mamb[2] = 0.0 ;
  mamb[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  mdif[0] = 1.0 ;
  mdif[1] = 1.0 ;
  mdif[2] = 0.0 ;
  mdif[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  mspec[0] = 0.0 ;
  mspec[1] = 0.0 ;
  mspec[2] = 0.0 ;
  mspec[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec) ;
  memis[0] = 1.0 ;
  memis[1] = 1.0 ;
  memis[2] = 0.0 ;
  memis[3] = 1.0 ;   	
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, memis) ;
  if( o->viewable_flag==1 && globals->display_sun_flag==true) {
    glutSolidSphere(sunR,20,20) ;
  }
    
  memis[0] = 0.0 ;
  memis[1] = 0.0 ;
  memis[2] = 0.0 ;
  memis[3] = 1.0 ;   	
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, memis) ;
  glMaterialf(GL_FRONT,GL_SHININESS, 0.0) ; 
}

void sun_key_control_func (OBJECT_TYPE *o, char key)
{
  SUN_GLOBALS *globals ;

  globals = (SUN_GLOBALS *)o->ptr0 ;

  switch (key)
  {
    case 's':   /* toggle sun */
      if(globals->display_sun_flag ==  true ){
        globals->display_sun_flag = false ;
        o->drawing_on_flag = false;
      }
      else {
        globals->display_sun_flag = true  ;
        o->drawing_on_flag = true;
      }
      break;
  }
}


void init_sun (APPTYPE *App)
{
  float PTemp[9];               /* temporary transform storage */
  OBJECT_TYPE *Sun;             /* the sun object */
  SUN_GLOBALS *globals ;
  int i, tmp, control_index = 0 ;
  bool display_sun = true;

  globals = (SUN_GLOBALS*)malloc(sizeof(SUN_GLOBALS) ) ;
  if( globals==NULL ) {
    printf("init_sun-  ERROR malloc error: globals.\n") ;
    exit(0) ;
  }

  for (i = 0; i < App->nparams; i++)
  {
    if (!strcmp (App->params[i][0], "sun"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      display_sun = (bool)tmp ;;
    }
  }

  /* initialize the object for the Sun */
  /* attach the sun to the earth, i.e., the App->appname in this case */
  Sun = fl_add_child_to_parent_by_name(App->universe_ptr,
                                    App->appname,
                                    "Earth_Sun") ;
  Sun->bounding_volume_set = fl_makebvset(2) ;
  Sun->soundfunc = sound_sun ;
  Sun->sounding_on_flag = 0 ;
  Sun->predrawfunc = predraw_sun ;
  Sun->lightingfunc = lighting_sun ;
  Sun->drawfunc =  draw_sun ;
  Sun->platformfunc = platform_handle_sun ;
  Sun->drawing_on_flag = display_sun ;

  /* Set globals */
  globals->display_sun_flag = display_sun ;
  Sun->ptr0 = (void *)globals ;

   /* allows control of the object from the keyboard, menues, and voice */
  Sun->object_controls = fl_makeControlsSet(1);

  /* Define controls for earth */
  control_index = 0 ;
  strcpy (Sun->object_controls->controls[control_index]->menuname, "Toggle Sun");
  strcpy (Sun->object_controls->controls[control_index]->voicename, "Toggle Sun");
  Sun->object_controls->controls[control_index]->keystroke = 's';
  Sun->object_controls->controls[control_index++]->control_function = sun_key_control_func ;


  /* save Sun to Earth xform */
  PTemp[0] = sunx;
  PTemp[1] = suny;
  PTemp[2] = sunz;
  PTemp[3] = 0.0;
  PTemp[4] = 0.0;
  PTemp[5] = 0.0;
  PTemp[6] = 1.0;
  PTemp[7] = 1.0;
  PTemp[8] = 1.0;
  Sun->orginal_first_node->MpoParams->P->assign( PTemp );   		
  Sun->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;
}
