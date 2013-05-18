/*
logoart.c.  Routines to initialize and display the town square art

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

#include "logoart.h"
#include "world.h"

/* LogoArt Platform handler */
void platform_handle_logoart (OBJECT_TYPE *o)
{
}

/* LogoArt Sound callback */
void sound_logoart(OBJECT_TYPE *o)
{
}

/* LogoArt Draw callback */
void draw_logoart(OBJECT_TYPE *o)
{
  float P[9];
  static int theta = 0;
  float mamb[4] = { 0.0,0.0,0.0,1.0 } ;
  float mdif[4] = { 0.0,0.0,0.0,1.0 } ;
  float mspec[4]= { 0.0,0.0,0.0,1.0 } ;
  WORLD_GLOBALS *globals ;

  globals = (WORLD_GLOBALS *)o->ptr0 ;

  if( globals->display_logo_flag==false )
    return ;

  o->first_node->MpoParams->P->read( P ) ;
  /* enable sun light0 */
  o->bounding_volume_set->bvset[1]->npoints = 2 ;
  o->bounding_volume_set->bvset[1]->p[0][0]=0.0 ;
  o->bounding_volume_set->bvset[1]->p[0][1]=0.0 ;
  o->bounding_volume_set->bvset[1]->p[0][2]=0.0 ;
  o->bounding_volume_set->bvset[1]->p[1][0]=6.0 ;
  o->bounding_volume_set->bvset[1]->p[1][1]=0.0 ;
  o->bounding_volume_set->bvset[1]->p[1][2]=0.0 ;

//  theta = (float)(sim_time%360);
  theta = ( theta + 1 ) % 360;
	
  glRotatef( theta * 1.0 , 0.0,1.0,0.0 ) ;

  draw_flatland_logoart() ;
  
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mspec) ;
   
  glTranslatef(0.0,-P[1],0.0) ;
  glBegin( GL_QUADS ) ;
    glVertex3f(1.3,0.35,1.3 ) ;
    glVertex3f(1.3,0.35,-1.3) ;
    glVertex3f(-1.3,0.35,-1.3) ;
    glVertex3f(-1.3,0.35,1.3) ;
  glEnd() ;

  mamb[0] = 0.3 ;
  mamb[1] = 0.3 ;
  mamb[2] = 0.3 ;
  mamb[3] = 1.0 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;

  glRotatef( -theta , 0.0,1.0,0.0 ) ;
   
  glBegin( GL_QUADS ) ;
    glVertex3f(4.0,0.3,4.0 ) ;
    glVertex3f(4.0,0.3,-4.0) ;
    glVertex3f(-4.0,0.3,-4.0) ;
    glVertex3f(-4.0,0.3,4.0) ;
  glEnd() ;
}

void draw_flatland_logoart(void) 
{
 	int i, Ntris ;
        float mamb[4],mdif[4],mspec[4] ;
        float x0,y0,z0 ;
        float x,y,z ;
        float dy, dy1 ,dz, h, f, eps ;
        float p0[3],p1[3],p2[3] ;
        float baseamb_red, baseamb_green, baseamb_blue, basedif_red, basedif_green, basedif_blue ;
	float alpha = 1.0 ;
       
        glPushMatrix() ;

        glDisable(GL_CULL_FACE);
        
        Ntris = 12 ;
        h = 2.0 ;
        x0 = 1.2 ;
        y0 = 0.6 ;
        z0 = 0.0 ;
        f = 0.9 ;
        dz = 0.2 ;
        dy1 = 0.2 ;
        dy = y0 ;
        eps = 0.001 ;
        
        baseamb_red = 0.0 ;
        baseamb_green = 0.2 ;
        baseamb_blue = 0.0 ;
    	mamb[0] = baseamb_red * alpha ;
    	mamb[1] = baseamb_green * alpha ;
    	mamb[2] = baseamb_blue * alpha ;
    	mamb[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    	basedif_red = 0.0 ;
    	basedif_green = 0.1 ;   
    	basedif_blue = 0.0 ;   
        mdif[0] = basedif_red * alpha ;
    	mdif[1] = basedif_green * alpha ;
    	mdif[2] = basedif_blue * alpha ;
    	mdif[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
    	mspec[0] = 0.0 * alpha ;
    	mspec[1] = 0.2 * alpha ;
    	mspec[2] = 0.0 * alpha ;
    	mspec[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec) ;    
    	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 100.0);    
        p0[0] = 0.0; p0[1] = 0.0; p0[2] = z0 ; 
        p1[0] = x0 ; p1[1] = y0 ; p1[2] = z0 ;
        p2[0] = -x0; p2[1] = y0 ; p2[2] = z0 ;  
        
        fl_draw_triangle( p0, p1, p2 ) ;
    	mamb[0] = 0.0 ;
    	mamb[1] = 0.0 ;
    	mamb[2] = 0.0 ;
    	mamb[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    	mdif[0] =  0.0 ;
    	mdif[1] =  0.0 ;
    	mdif[2] =  0.0 ;
    	mdif[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
        p0[0] = 0.0; p0[1] = 0.0; p0[2] = z0+eps ; 
        p1[0] = x0 ; p1[1] = y0 ; p1[2] = z0+eps ;
        p2[0] = -x0; p2[1] = y0 ; p2[2] = z0+eps ;      
        fl_draw_wire_triangle( p0, p1, p2 ) ;
        
        y = y0 ;
        z = z0 ;
        
        for( i=1 ; i<=Ntris ; i++ ) {
        
                z -= dz ;
                y += dy1 ;
                dy1 *=f ;
                x = (y-h)*x0/(y0-h) ;
                if( y>h ) break ;
                
        	mamb[0] = baseamb_red * alpha ;
        	mamb[1] = (baseamb_green +(1.0-baseamb_green)*(float)i/(float)Ntris) * alpha ;
        	mamb[2] = baseamb_blue * alpha ;
        	mamb[3] = 1.0 ;
        	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
        	mdif[0] =  basedif_red  * alpha ;
        	mdif[1] =  (basedif_green + (1.0-basedif_green)*(float)i/(float)Ntris) * alpha ;
        	mdif[2] =  basedif_blue  * alpha ;
        	mdif[3] = 1.0 ;
        	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
                p0[0] = 0.0 ; p0[1] = (y-dy) ; p0[2] = z ;
                p1[0] = x ; p1[1] = y ; p1[2] = z ;
                p2[0] = -x; p2[1] = y ; p2[2] = z ;     
                fl_draw_triangle( p0, p1, p2 ) ; 
                
        	mamb[0] = 0.0 ;
        	mamb[1] = 0.0 ;
        	mamb[2] = 0.0 ;
        	mamb[3] = 1.0 ;
        	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
        	mdif[0] =  0.0 ;
        	mdif[1] =  0.0 ;
        	mdif[2] =  0.0 ;
        	mdif[3] = 1.0 ;
        	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
                p0[0] = 0.0 ; p0[1] = (y-dy) ; p0[2] = z+eps ;
                p1[0] = x ; p1[1] = y ; p1[2] = z+eps ;
                p2[0] = -x; p2[1] = y ; p2[2] = z+eps ; 
                fl_draw_wire_triangle( p0, p1, p2 ) ;
                dy *=f ;
        }
        
    	mamb[0] = 0.5 * alpha ;
    	mamb[1] = 0.3 * alpha ;
    	mamb[2] = 0.1 * alpha ;
    	mamb[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb) ;
    	mdif[0] =  0.5 * alpha ;
    	mdif[1] =  0.2 * alpha ;
    	mdif[2] =  0.1 * alpha ;
    	mdif[3] = 1.0 ;
    	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif) ;
        p0[0] = 0.6*h ; p0[1] = 0.0 ; p0[2] = 0.0 ;
        p1[0] = 0.6*h ; p1[1] = h ; p1[2] = z ;
        p2[0] = 0.05*h; p2[1] = h ; p2[2] = z ; 
        fl_draw_triangle( p0, p1, p2 ) ; 
        p0[0] = -0.6*h ; p0[1] = 0.0 ; p0[2] = 0.0 ;
        p2[0] = -0.6*h ; p2[1] = h ; p2[2] = z ;
        p1[0] = -0.05*h; p1[1] = h ; p1[2] = z ;        
        fl_draw_triangle( p0, p1, p2 ) ; 

    
    	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 0.0);    
        
        glPopMatrix() ;
}

void logo_key_control_func (OBJECT_TYPE *o, char key)
{
  LOGO_GLOBALS *globals ;

  globals = (LOGO_GLOBALS *)o->ptr0 ;

  switch (key)
  {
    case 'd':   /* toggle logo */
    {
      if(globals->display_logo_flag ==  true )
        globals->display_logo_flag = false ;
      else
        globals->display_logo_flag = true  ;
      break;
    }
    break ;
  }
}


void init_logoart (APPTYPE *App)
{
  float PTemp[9];               /* temporary transform storage */
  OBJECT_TYPE *LogoArt;         /* the town square art object */
  int i,control_index,tmp;
  bool display_logo ;
  LOGO_GLOBALS *globals ;

  globals = (LOGO_GLOBALS*)malloc(sizeof(LOGO_GLOBALS) ) ;
  if( globals==NULL ) {
    printf("init_logo-  ERROR malloc error: globals.\n") ;
    exit(0) ;
  }

  for (i = 0; i < App->nparams; i++)
  {
    if (!strcmp (App->params[i][0], "logo"))
    {
      sscanf (App->params[i][2], "%d", &tmp);
      display_logo = (bool)tmp ;;
    }
  }

   /* initialize the object for the town square art */
  /* add ourselves to the earth, i.e., App->appname */
  LogoArt = fl_add_child_to_parent_by_name(App->universe_ptr,
                                        App->appname,
                                        "Earth_LogoArt") ;
  LogoArt->bounding_volume_set = fl_makebvset(2) ;
  LogoArt->soundfunc = sound_logoart ;
  LogoArt->sounding_on_flag = 0 ;
  LogoArt->drawfunc =  draw_logoart ;
  LogoArt->platformfunc = platform_handle_logoart ;
  LogoArt->drawing_on_flag = 1 ;

 /* set globals pointer */
 globals->display_logo_flag = display_logo ;
  LogoArt->ptr0 = (void *)globals ;

   /* allows control of the object from the keyboard, menues, and voice */
  LogoArt->object_controls = fl_makeControlsSet(1);

  control_index = 0 ;
  strcpy (LogoArt->object_controls->controls[control_index]->menuname, "Toggle Logo");
  strcpy (LogoArt->object_controls->controls[control_index]->voicename, "Toggle Logo");
  LogoArt->object_controls->controls[control_index]->keystroke = 'd';
  LogoArt->object_controls->controls[control_index++]->control_function = logo_key_control_func ;


  /* save LogoArt to Runway xform */
  PTemp[0] = 0.0;
  PTemp[1] = 1.0;
  PTemp[2] = 0.0;
  PTemp[3] = 0.0;
  PTemp[4] = 0.0;
  PTemp[5] = 0.0;
  PTemp[6] = 4.0;
  PTemp[7] = 4.0;
  PTemp[8] = 4.0;
  LogoArt->orginal_first_node->MpoParams->P->assign( PTemp );   		
  LogoArt->orginal_first_node->MpoParams->Po[0] = SCALE ;
  LogoArt->orginal_first_node->MpoParams->Po[1] = TRANSLATE ;
}
