/*
locowin.c.  Routines to display the locomotion control window.

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
 * Created: 4-12-00, KLS
 */

#include "locowin.h"
#include "world.h"

/* "local" globals */
static GLint loco_window;               /* window handle */
static GLint loco_ww1_cur = 140,        /* current size of the window */
             loco_wh1_cur = 140;
static myslider_array_type *loco_sliders1; /* locomotion slider stucture */
static myMCbutton_array_type *MCloco1;  /* All stop button structure */
static GLfloat worldvx = 0.0,           /* calculated values for velocities */
               worldvy = 0.0,
               worldvz = 0.0,
               worldvyaw = 0.0;

float mapping1( float in, float outmax ) 
{
	// quadratic in [-1,1], assume "in" is bounded by +/-1 

	if( in>=0 )
	{
		return( outmax*in*in ) ;
	}
	else
	{
		return( -outmax*in*in ) ;
	}
}

float mapping2( float in, float outmid ) 
{
// forth power in [0,2], assume "in" is bounded by +/-1 ,
// returns outmid when in=0, returns 0 when in = -1 ,
// and returns 16*outmid when in = +1.  
	
	return( outmid * (in+1.0)*(in+1.0)*(in+1.0)*(in+1.0) ) ;
}

void world_loco_display (void)
{
  glutSetWindow(loco_window);
  glLoadIdentity();

  /* Labels */
  glColor3f(0.0,0.0,0.0);
  glRasterPos3f(16.0,122.,0.0);
  fl_draw_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_10, "LOCOMOTION");
  glRasterPos3f(26.0,112.,0.0);
  fl_draw_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_10, "(velocity)");

  /* dividing lines */
  glBegin(GL_LINES);
    glVertex3f(5.0,2.0,0.0);
    glVertex3f(5.0,128.0,0.0);
  glEnd();
  glBegin(GL_LINES);
    glVertex3f(135.0,2.0,0.0);
    glVertex3f(135.0,128.0,0.0);
  glEnd();

  glFlush();
}

void world_loco_mouse (int button, int state, int x, int y)
{
  int wx,wy;
  int slider_index, button_index;
  double value, quadmax;
  static GLfloat global_velocity_scale = 5.0;
    
  glutSetWindow(loco_window);
  wy = loco_wh1_cur - y;
  wx = x;
  
  if (button == GLUT_LEFT_BUTTON) 
  {
    fl_set_slider_mouse_state( wx, wy, loco_sliders1, state ) ;
    if (state == GLUT_DOWN) 
    {
      /* velocity sliders */
      fl_slider_picker( wx, wy, loco_sliders1, &slider_index, &value );
      if(slider_index>=0) 
      {
        if( value==99999.0 )
          value = 0.0;
        if( slider_index==0 ) 
        {
          quadmax = 10.0*global_velocity_scale;
          worldvx = mapping1( value, quadmax );
        }
        if( slider_index==1 ) 
        {
          quadmax = 10.0*global_velocity_scale;
          worldvy = mapping1( value, quadmax );
        }
        if( slider_index==2 ) 
        {
          quadmax = 10.0*global_velocity_scale;
          worldvz = mapping1( value, quadmax );
        }
        if( slider_index==3 ) 
        {
          quadmax = 10.0*global_velocity_scale;
          worldvyaw = mapping1( value, quadmax );
        }
        if( slider_index==4 ) 
        {
          global_velocity_scale = mapping2( value, 1.0 );
        }
      }
      
      /* all stop button */
      fl_MCbutton_down_picker( wx, wy, MCloco1, &button_index );
      if(button_index>=0 ) 
      {
        if( button_index==0 ) 
        {
          worldvx =0.0;
          worldvy =0.0;
          worldvz =0.0;
          worldvyaw =0.0;
 	  fl_reset_sliders( loco_sliders1 ) ;
        }
      }
    }
  }

  if (state == GLUT_UP)
    fl_MCbutton_up_picker( wx, wy, MCloco1, &button_index );
}

void world_loco_motion (int x, int y)
{
  int wx,wy;
  int slider_index;
  double value, quadmax; 
  static GLfloat global_velocity_scale = 5.0;
    
  glutSetWindow(loco_window);
  wy = loco_wh1_cur - y;
  wx = x;
    
  fl_slider_picker( wx, wy, loco_sliders1, &slider_index, &value );
  if(slider_index>=0) 
  {
    if( value==99999.0 )
      value = 0.0;
    if( slider_index==0 ) 
    {
      quadmax =10.0*global_velocity_scale;
      worldvx = mapping1( value, quadmax );
     }
    if( slider_index==1 ) 
    {
      quadmax =10.0*global_velocity_scale;
      worldvy = mapping1( value, quadmax );
    }
    if( slider_index==2 ) 
    {
      quadmax =10.0*global_velocity_scale;
      worldvz = mapping1( value, quadmax );
    }
    if( slider_index==3 ) 
    {
      quadmax =10.0*global_velocity_scale;
      worldvyaw = mapping1( value, quadmax );
    }
    if( slider_index==4 ) 
    {
      global_velocity_scale = mapping2( value, 1.0 );
    }
  }
}

void world_loco_key (unsigned char c, int x, int y)
{
  glutPostRedisplay();
}

void world_loco_reshape (int w, int h)
{
  /* select this window */
  glutSetWindow(loco_window);
  loco_ww1_cur = w;
  loco_wh1_cur = h;
  
  /* resize window viewport */
  glViewport(0,0,w,h);
  glMatrixMode( GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0,(GLdouble)w,0.0,(GLdouble)h,-10.0,10.0);
  
  /* clear the window and redisplay contents */
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.7, 0.2, 0.8, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  fl_display_sliders( loco_sliders1 );
  fl_display_MCbuttons( MCloco1 );
}

void world_loco_visibility (int state)
{
  if( state==GLUT_VISIBLE) 
  {
    glutSetWindow(loco_window);
    glClearColor(0.7, 0.2, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    fl_display_sliders( loco_sliders1 );
    fl_display_MCbuttons( MCloco1 );
  }
}

void init_loco_window(void)
{
  GLsizei loco_wx1, loco_wy1;             /* window position */
	GLint old_win = glutGetWindow( );
	
  /* define the window */
  fl_get_subwindow_location (loco_ww1_cur, loco_wh1_cur, &loco_wx1, &loco_wy1);
  glutInitDisplayMode (GLUT_RGB | GLUT_SINGLE);
  glutInitWindowPosition (loco_wx1, loco_wy1);
  glutInitWindowSize (loco_ww1_cur, loco_wh1_cur);
  loco_window = glutCreateWindow ("Flatland, Locomotion Controls");

  /* initialize the view */
  glViewport(0,0,loco_ww1_cur,loco_wh1_cur);
  glMatrixMode( GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0,(GLdouble)loco_ww1_cur,0.0,(GLdouble)loco_wh1_cur,-10.0,10.0);

  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.7, 0.2, 0.8, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Make locomotion sliders and set values */
  loco_sliders1 = fl_make_my_sliders(5);
  fl_set_sliders_params(loco_sliders1,16,80,10,10.0,10.0,6.0);
  strcpy(loco_sliders1->sliders[0]->name,"R/L");
  strcpy(loco_sliders1->sliders[1]->name,"U/D");
  strcpy(loco_sliders1->sliders[2]->name,"F/R");
  strcpy(loco_sliders1->sliders[3]->name,"Yaw");
  strcpy(loco_sliders1->sliders[4]->name,"GVS");
  fl_display_sliders( loco_sliders1 );

  /* make all stop locomotion MC button */
  MCloco1 = fl_make_my_MCbuttons( 1 );
  fl_set_MCbutton_params(MCloco1, 8, 8, 0, 0, 94, 109);
  strcpy(MCloco1->buttons[0]->name,"All Stop");		
  fl_display_MCbuttons( MCloco1 );

  /* Register GLUT callbacks for loco_window */
  glutDisplayFunc (world_loco_display);
  glutMouseFunc (world_loco_mouse);
  glutMotionFunc (world_loco_motion);
  glutKeyboardFunc (world_loco_key);
  glutReshapeFunc (world_loco_reshape);
  glutVisibilityFunc (world_loco_visibility);

  /* make the main window active again for the next guy */
  glutSetWindow( old_win );
}
