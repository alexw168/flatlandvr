/*
simple.c.  A simple app with a hovering cube.  Used for testing

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
 * Created: 2-1-00, KLS
 */

/* include the master Flatland library */
#include "flatland_app.h"
#include "simple.h"

/* Function to read in a data file */
void *read_data (void *X)
{
  int face, property;           /* loop variables */
  FILE *inFile;                 /* input file handle */
  OBJECT_TYPE *Simple;          /* pointer to the object passed in (this obj) */
  SIMPLE_GLOBALS *globals;      /* "global" structure pointer */

  Simple = (OBJECT_TYPE *) X;
  globals = (SIMPLE_GLOBALS *) Simple->ptr0;

  printf ("Simple reading data file '%s'\n", globals->data_file);

  if (strlen (globals->data_file) > 0)  /* check for non-zero filename */
  {
    /* open the file */
    inFile = fopen (globals->data_file, "r");
    if (inFile == NULL)
    {
      fprintf (stderr, "Simple could not open input data file '%s'\n", 
               globals->data_file);
      fprintf (stderr, "    Simple cube will not display\n");
      return (NULL);
    }

    /* Read in the diffuse, ambient, and specular properties for each face of 
     * the cube (i.e., 6 x 3 RGBA values) */
    for (face = 0; face < 6; face++)
      for (property = 0; property < 3; property++)
      {
        fscanf (inFile, "%f %f %f %f",&globals->face_colors[face][property][0],
                                      &globals->face_colors[face][property][1],
                                      &globals->face_colors[face][property][2],
                                      &globals->face_colors[face][property][3]);
      }

    /* All read in, tell everybody else */
    globals->data_ready = true;
  }

  return (NULL);
}

/* beamed function */
void simpleBeamedFunc (struct bv *bv, struct bv_interactions_type *inter)
{
  printf ("!!!!!!simpleBeamedFunc from %s\n", inter->beamer_object->name);
}

/* unbeamed function */
void simpleUnbeamedFunc (struct bv *bv, struct bv_interactions_type *inter)
{
  printf ("!!!!!!simpleUnbeamedFunc from %s\n", inter->beamer_object->name);
}

/* collision function */
void simpleCollisionFunc (struct bv *bv, struct bv_interactions_type *inter)
{
  printf ("!!!!!!simpleCollisionFunc from %s\n", inter->beamer_object->name);
}

/* uncollision function */
void simpleUncollisionFunc (struct bv *bv, struct bv_interactions_type *inter)
{
  printf ("!!!!!!simpleUncollisionBeamedFunc from %s\n", inter->beamer_object->name);
}

/* Platform handler */
void platform_handler_simple (OBJECT_TYPE *o)
{
  /* no platform handling for this object */
}

/* Sound callback */
/* make the "THX" noise in a loop */
void sound_simple (OBJECT_TYPE *o)
{
  SIMPLE_GLOBALS *globals;
  GLfloat position[4];

  /* get "global" structure in a more convenient form */
  globals = (SIMPLE_GLOBALS *) o->ptr0;

  /* check to see if we are ready to make a sound (data file loaded?) */
  if (!globals->data_ready) return;

  /* On the first time through, open the sound files */
  if(globals->sounds_opened == false)
  {
    globals->sounds_opened = true;

    /* open_sound */
    /* request sound list id */
    globals->snd_list = slGenLists (1);

    /* Open/upload sound */
    slNewList (globals->snd_list, "thx.wav");

    /* Initial position is at the origin */
    position [0] = 0.0;
    position [1] = 0.0;
    position [2] = 0.0;
    position [3] = 1.0;         /* This must *always* be 1.0! */
    slPositionList (globals->snd_list, position);

    /* Initial volume is 1.0 */
    slVolumeList (globals->snd_list, 1.0);

    /* Attenuation (how far away before the sound fades) */
    slAttenuationList(globals->snd_list, 1.0);

    /* Loop mode */
    slLoopList (globals->snd_list, LOOP_MODE);

    /* AC3 priority */
    slPriorityList (globals->snd_list, MED_PRIORITY);

    /* Rewind the list (I'm not sure why... historically done this way) */
    slRewindList (globals->snd_list);

    /* Start playing sound */
    slCallList (globals->snd_list);
  }
}

/* Draw callback */
/* Draw a rotated cube */
void draw_simple (OBJECT_TYPE *o)
{
  GLfloat CUBE_SIZE;
  SIMPLE_GLOBALS *globals;

  /* get "global" structure */
  globals = (SIMPLE_GLOBALS *) o->ptr0;

  /* check to see if we are ready to display or not (data file loaded?) */
  if (!globals->data_ready) return;
  
  /* fetch the cube size from the bounding box */
  /* (doing this to avoid a global, which makes the app non-multiple use) */
  CUBE_SIZE = o->bounding_volume_set->bvset[1]->p[1][0];

  glPushMatrix ();      /* Always be a good neighbor */

  /* rotate to proper angle */
  glRotated (o->object_param0, 0.0, 1.0, 0.0);

  /* set the material properties of the first face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[0][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[0][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[0][2]);

  /* Draw the first face */
  glBegin(GL_POLYGON);
    glNormal3f (  0.0,   0.0,  -1.0);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
  glEnd();

  /* set the material properties of the second face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[1][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[1][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[1][2]);

  /* draw the second face */
  glBegin(GL_POLYGON);
    glNormal3f (  0.0,   0.0,   1.0);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
  glEnd();

  /* set the material properties of the third face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[2][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[2][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[2][2]);

  /* draw the third face */
  glBegin(GL_POLYGON);
    glNormal3f ( -1.0,   0.0,   0.0);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
  glEnd();

  /* set the material properties of the fourth face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[3][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[3][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[3][2]);

  /* draw the fourth face */
  glBegin(GL_POLYGON);
    glNormal3f (  1.0,   0.0,   0.0);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
  glEnd();

  /* set the material properties of the fifth face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[4][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[4][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[4][2]);

  /* draw the fifth face */
  glBegin(GL_POLYGON);
    glNormal3f (  0.0,  -1.0,   0.0);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (  CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (- CUBE_SIZE, - CUBE_SIZE,   CUBE_SIZE);
  glEnd();

  /* set the material properties of the sixth face */
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, globals->face_colors[5][0]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, globals->face_colors[5][1]);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, globals->face_colors[5][2]);

  /* draw the sixth face */
  glBegin(GL_POLYGON);
    glNormal3f (  0.0,   1.0,   0.0);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
    glVertex3f (- CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE,   CUBE_SIZE);
    glVertex3f (  CUBE_SIZE,   CUBE_SIZE, - CUBE_SIZE);
  glEnd();

  glPopMatrix ();
}

/* cheap shadow callback */
void draw_simple_cheap_shadow (OBJECT_TYPE *o)
{
  GLfloat CUBE_SIZE;
  SIMPLE_GLOBALS *globals;

  /* get "global" structure */
  globals = (SIMPLE_GLOBALS *) o->ptr0;

  /* check to see if we are ready to display or not (data file loaded?) */
  if (!globals->data_ready) return;
  
  /* fetch the cube size from the bounding box */
  /* (doing this to avoid a global, which makes the app non-multiple use) */
  CUBE_SIZE = o->bounding_volume_set->bvset[1]->p[1][0];

  /* a simple square, the same shape as the cube, 
   * drawn in the y=0 plane */
  glRotated (o->object_param0, 0.0, 1.0, 0.0);

  glBegin(GL_POLYGON);
    glNormal3f (  0.0,   1.0,   0.0);
    glVertex3f (-CUBE_SIZE,   0.0, -CUBE_SIZE);
    glVertex3f (-CUBE_SIZE,   0.0,  CUBE_SIZE);
    glVertex3f ( CUBE_SIZE,   0.0,  CUBE_SIZE);
    glVertex3f ( CUBE_SIZE,   0.0, -CUBE_SIZE);
  glEnd();
}

/* predraw callback */
void predraw_simple (OBJECT_TYPE *o)
{
  /* adjust the rotation direction based on object_state0 */
  if (o->object_state0 > 0)
    o->object_param0 += 1.0;
  else if(o->object_state0 < 0)
    o->object_param0 -= 1.0;
}

/* menu control callback */
void toggle_rotate (OBJECT_TYPE *o, char key)
{
  switch (key)
  {
    case 'a':   /* Rotate counterclockwise */
      o->object_state0 = -1;
      break;

    case 'd':   /* Rotate clockwise */
      o->object_state0 = 1;
      break;

    case 's':   /* Stop rotation */
      o->object_state0 = 0;
      break;
  }
}

extern "C"
{
/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
  int CUBE_SOUND = 0;           /* Make sound? (default no) */
  char data_file [MAX_NAME_SIZE] = {"modules/Simple/colors.dat"}; /* Name of the input 
                                                                     data file */  
  float PTemp[9];               /* temporary transform storage */
  float extents [6];            /* size of a fixed box containing the rotating 
                                   cube */
  GLfloat CUBE_SIZE = 5.0,      /* default cube size (overriden from config) */
          CUBE_EXT,             /* distance from center to vertex of cube */
          CUBE_X = 0.0,         /* default cube x location */
          CUBE_Y = 5.0,         /* default cube y location (also set below) */
          CUBE_Z = 0.0,         /* default cube z location */
          CUBE_ROLL = 0.0,      /* default cube roll */
          CUBE_PITCH = 0.0,     /* default cube pitch */
          CUBE_YAW = 0.0;       /* default cube yaw */
  OBJECT_TYPE *Simple;          /* This object */
  BV_TYPE *bvol;                /* bounding volume (application extents) */
  OBJ_CONTROLS_ARRAY_TYPE *subMenu; /* submenu temp variable */
  pthread_t data_read_thread;   /* thread handle for data input */
  SIMPLE_GLOBALS *globals;      /* pointer to "globals" structure */

  /* parse the parameter list */
  for (int i=0; i < App->nparams; i++)
  {
    if (!strcmp (App->params[i][0], "cube_sound"))
      sscanf(App->params[i][2], "%d", &CUBE_SOUND);
    if (!strcmp (App->params[i][0], "cube_pitch"))
      sscanf(App->params[i][2], "%f", &CUBE_PITCH);
    if (!strcmp (App->params[i][0], "cube_yaw"))
      sscanf(App->params[i][2], "%f", &CUBE_YAW);
    if (!strcmp (App->params[i][0], "cube_roll"))
      sscanf(App->params[i][2], "%f", &CUBE_ROLL);
    if (!strcmp (App->params[i][0], "cube_size"))
      sscanf(App->params[i][2], "%f", &CUBE_SIZE);
    if (!strcmp (App->params[i][0], "data_file"))
      sscanf(App->params[i][2], "%s", data_file);
  }

  /* generate a bounding volume array */
  bvol = fl_makebv ();

  /* mark the extents of the object */
  CUBE_EXT = sqrt (CUBE_SIZE*CUBE_SIZE + 
                   CUBE_SIZE*CUBE_SIZE +
                   CUBE_SIZE*CUBE_SIZE);
  extents [0] = -CUBE_EXT;
  extents [1] = -CUBE_EXT;
  extents [2] = -CUBE_EXT;
  extents [3] =  CUBE_EXT;
  extents [4] =  CUBE_EXT;
  extents [5] =  CUBE_EXT;

  /* build a bounding volume (four corners and a center) */
  bvol->npoints = 5;
  bvol->p[0][0]=0.0;           /* center point */
  bvol->p[0][1]=0.0;
  bvol->p[0][2]=0.0;
  bvol->p[1][0]=extents[0];    /* -x, -y, -z corner */
  bvol->p[1][1]=extents[1];
  bvol->p[1][2]=extents[2];
  bvol->p[2][0]=extents[3];    /* +x, -y, -z corner */
  bvol->p[2][1]=extents[1];
  bvol->p[2][2]=extents[2];
  bvol->p[3][0]=extents[0];    /* -x, +y, -z corner */
  bvol->p[3][1]=extents[4];
  bvol->p[3][2]=extents[2];
  bvol->p[4][0]=extents[0];    /* -x, -y, +z corner */
  bvol->p[4][1]=extents[1];
  bvol->p[4][2]=extents[5];
 
  /* initialize the object for the Simple Object */
  /* make new object */
  Simple = fl_add_child_to_parent_by_name_via_platform(App->universe_ptr,
                                      App->parent_object_name,
                                      App->appname,
                                      bvol);

#ifdef Flatland_version_C
  /* add the app name to the object, so it can be unloaded */
  Simple->creator_app_ptr = App;

  /* add the app name to the object's platform, so *it* can be unloaded */
  Simple->orginal_first_node->neighbor_node->self->creator_app_ptr = App;
#endif

  /* set up the bounding volumes (position & culling) */
  Simple->bounding_volume_set = fl_makebvset(2);

  /* Define bounding box */
  /* NOTE: same as the bounding box we gave to the platform */
  Simple->bounding_volume_set->bvset[0]->npoints = 5;
  Simple->bounding_volume_set->bvset[0]->p[0][0]=bvol->p[0][0];
  Simple->bounding_volume_set->bvset[0]->p[0][1]=bvol->p[0][1];
  Simple->bounding_volume_set->bvset[0]->p[0][2]=bvol->p[0][2];
  Simple->bounding_volume_set->bvset[0]->p[1][0]=bvol->p[1][0];
  Simple->bounding_volume_set->bvset[0]->p[1][1]=bvol->p[1][1];
  Simple->bounding_volume_set->bvset[0]->p[1][2]=bvol->p[1][2];
  Simple->bounding_volume_set->bvset[0]->p[2][0]=bvol->p[2][0];
  Simple->bounding_volume_set->bvset[0]->p[2][1]=bvol->p[2][1];
  Simple->bounding_volume_set->bvset[0]->p[2][2]=bvol->p[2][2];
  Simple->bounding_volume_set->bvset[0]->p[3][0]=bvol->p[3][0];
  Simple->bounding_volume_set->bvset[0]->p[3][1]=bvol->p[3][1];
  Simple->bounding_volume_set->bvset[0]->p[3][2]=bvol->p[3][2];
  Simple->bounding_volume_set->bvset[0]->p[4][0]=bvol->p[4][0];
  Simple->bounding_volume_set->bvset[0]->p[4][1]=bvol->p[4][1];
  Simple->bounding_volume_set->bvset[0]->p[4][2]=bvol->p[4][2];
    
  /* Define bounding sphere for culling */
  Simple->bounding_volume_set->bvset[1]->npoints = 2;
  Simple->bounding_volume_set->bvset[1]->p[0][0]=0.0;
  Simple->bounding_volume_set->bvset[1]->p[0][1]=0.0;
  Simple->bounding_volume_set->bvset[1]->p[0][2]=0.0;
  Simple->bounding_volume_set->bvset[1]->p[1][0]=CUBE_SIZE;
  Simple->bounding_volume_set->bvset[1]->p[1][1]=CUBE_SIZE;
  Simple->bounding_volume_set->bvset[1]->p[1][2]=CUBE_SIZE;
  Simple->bounding_volume_set->bvset[1]->bvbeamedfunc = simpleBeamedFunc;
  Simple->bounding_volume_set->bvset[1]->bvunbeamedfunc = simpleUnbeamedFunc;
  Simple->bounding_volume_set->bvset[1]->bvcollisionfunc = simpleCollisionFunc;
  Simple->bounding_volume_set->bvset[1]->bvuncollisionfunc = simpleUncollisionFunc;
  Simple->bounding_volume_set->bvset[1]->beamable_flag = 1;
    
  /* generate the "globals" structure & attach it to the object */
  if ((globals = (SIMPLE_GLOBALS *) malloc (sizeof (SIMPLE_GLOBALS))) == NULL)
  {
    printf ("ERROR: failed to initialize memory for simple globals\n");
    exit(1);
  }
  globals->data_ready = false;          /* no data has been read yet */
  strncpy (globals->data_file, data_file, MAX_NAME_SIZE); /*copy data filename*/
  globals->sounds_opened = false;       /* no sounds have been opened yet */
  globals->snd_list = 0;                /* no sound list has been allocated */
  Simple->ptr0 = (void *) globals;      /* attach struct to object */

  /* spawn a thread to read the data file */
  pthread_create (&data_read_thread, NULL, &read_data, (void *) Simple);

  /* set up callbacks */
  Simple->soundfunc = sound_simple;
  Simple->predrawfunc =  predraw_simple;
  Simple->drawfunc =  draw_simple;
  Simple->platformfunc = platform_handler_simple;
  Simple->trueShadowFunc = draw_simple;  /* same as drawfunc */
  Simple->cheapShadowFunc = draw_simple_cheap_shadow;


  /* set up the control structure (three subcontrols under one) */
  /* allows control of the object from the keyboard, menues, and voice */
  Simple->object_controls = fl_makeControlsSet(2);

  /* Define spin/nospin controls for cube */
  strcpy (Simple->object_controls->controls[0]->menuname, "Rotate");
  strcpy (Simple->object_controls->controls[0]->voicename, "Rotate");
  Simple->object_controls->controls[0]->children = fl_makeControlsSet(3);
  Simple->object_controls->controls[0]->control_function = toggle_rotate;
  subMenu = Simple->object_controls->controls[0]->children;

  strcpy (Simple->object_controls->controls[1]->menuname, "Stop");
  strcpy (Simple->object_controls->controls[1]->voicename, "Stop");
  Simple->object_controls->controls[1]->keystroke = 's';
  Simple->object_controls->controls[1]->control_function = toggle_rotate;

  strcpy (subMenu->controls[0]->menuname, "Clockwise");
  strcpy (subMenu->controls[0]->voicename, "Clockwise");
  subMenu->controls[0]->keystroke = 'd';
  subMenu->controls[0]->control_function = toggle_rotate;

  strcpy (subMenu->controls[1]->menuname, "Counterclockwise");
  strcpy (subMenu->controls[1]->voicename, "Counterclockwise");
  subMenu->controls[1]->keystroke = 'a';
  subMenu->controls[1]->control_function = toggle_rotate;

  strcpy (subMenu->controls[2]->menuname, "Stop");
  strcpy (subMenu->controls[2]->voicename, "Stop");
  subMenu->controls[2]->keystroke = 's';
  subMenu->controls[2]->control_function = toggle_rotate;


  /* set the height of the cube above the ground */
  CUBE_Y = 5.0 + CUBE_EXT;

  /* attach the cube */
  PTemp[0] = CUBE_X;    /* x */
  PTemp[1] = CUBE_Y;    /* y */
  PTemp[2] = CUBE_Z;    /* z */ 
  PTemp[3] = CUBE_PITCH;/* pitch */
  PTemp[4] = CUBE_YAW;  /* yaw */
  PTemp[5] = CUBE_ROLL; /* roll */
  PTemp[6] = 1.0;       /* scale x */
  PTemp[7] = 1.0;       /* scale y */
  PTemp[8] = 1.0;       /* scale z */

  Simple->orginal_first_node->MpoParams->P->assign( PTemp );
	
  Simple->orginal_first_node->MpoParams->Po[0] = TRANSLATE;
  Simple->orginal_first_node->MpoParams->Po[1] = ROTATE001;
  Simple->orginal_first_node->MpoParams->Po[2] = ROTATE100;
  Simple->orginal_first_node->MpoParams->Po[3] = ROTATE010;

  /* initialize the rotation flag to start rotating */
  Simple->object_state0 = 1;
  Simple->object_param0 = 0;

  /* Finally, turn on flags */
  Simple->sounding_on_flag = CUBE_SOUND;
  Simple->drawing_on_flag = 1;
  Simple->beamable_flag = true;
}
} // extern "C"
