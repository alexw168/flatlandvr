/*
2D_interactors.cpp.  All of the 2D display objectcs (buttons, sliders, etc).

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
 * Created: 1-17-00
 */

#include "gheader.h"

// Global Boolean to indicate thread termination
extern Boolean do_exit;

// Global boolean to indicate completion of the logo screen
//extern unsigned char logo_done;//[];

// Display resolution flag
bool hi_res = TRUE;

// Motion control mode flag
bool wand_loco = FALSE;

// old values of ww0 and wh0, so they can be restored after returning from
// fullscreen
//extern GLsizei last_ww0, last_wh0;

/******* Interactor funcs ********************************************************/

struct myslider_array_type *fl_make_my_sliders(int nsliders) 
{
	int i,j,k ;
	struct myslider_array_type *a ;
	
	a = (struct myslider_array_type *)malloc(sizeof(struct myslider_array_type)) ;
	if( a==NULL ) {
		printf("fl_make_my_sliders- ERROR mallocing a.\n") ;
		exit(0) ;
	}	
	a->nsliders = nsliders ;
	a->sliders = (struct myslider_type **)malloc( nsliders * sizeof(struct myslider_type *) ) ;
	if( a->sliders==NULL ) {
		printf("fl_make_my_sliders- ERROR mallocing a->sliders.\n") ;
		exit(0) ;
	}
	for( i=0 ; i<a->nsliders ; i++ ) {
		a->sliders[i] = (struct myslider_type *)malloc(sizeof(struct myslider_type)) ;
		if( a->sliders[i]==NULL ) {
			printf("fl_make_my_sliders- ERROR mallocing a->sliders[%d].\n",i) ;
			exit(0) ;
		}
		strcpy(a->sliders[i]->name,"none") ;
		for( j=0 ; j<4 ; j++ ) {
			a->sliders[i]->box[j].point = (double *)malloc(4*sizeof(double)) ;
			if(a->sliders[i]->box[j].point==NULL ) {
				printf("*fl_make_my_sliders- ERROR mallocing box: %d vertex: %d\n",i,j) ;
				exit(0) ;
			}
			a->sliders[i]->stop_box[j].point = (double *)malloc(4*sizeof(double)) ;
			if(a->sliders[i]->stop_box[j].point==NULL ) {
				printf("*fl_make_my_sliders- ERROR mallocing stop_box: %d vertex: %d\n",i,j) ;
				exit(0) ;
			}
			a->sliders[i]->slider[j].point = (double *)malloc(4*sizeof(double)) ;
			if(a->sliders[i]->slider[j].point==NULL ) {
				printf("*fl_make_my_sliders- ERROR mallocing slider: %d vertex: %d\n",i,j) ;
				exit(0) ;
			}
			for( k=0 ; k<3 ; k++ ) {
				a->sliders[i]->box[j].point[k] = 0.0 ;
				a->sliders[i]->stop_box[j].point[k] = 0.0 ;
				a->sliders[i]->slider[j].point[k] = 0.0 ;
			}
			a->sliders[i]->box_color[j] = 0.0 ;
			a->sliders[i]->stop_box_color[j] = 0.0 ;
			a->sliders[i]->slider_color[j] = 0.0 ;
		}
		a->sliders[i]->slider_max = 1.0 ;
		a->sliders[i]->slider_min = -1.0 ;
		a->sliders[i]->wyold = -1000 ;
		a->sliders[i]->moused_state = GLUT_UP ;
		a->sliders[i]->colorold[0] = 0.0 ;
		a->sliders[i]->colorold[1] = 0.0 ;
		a->sliders[i]->colorold[2] = 0.0 ;
	}
	return( a ) ;
}


void fl_set_sliders_params(struct myslider_array_type *a, int swidth, int sheight, int salley, double xstart, double ystart, double stoph) 
{
	int i ;
	double xoff, yoff ;
	
	xoff = xstart ;
	yoff = ystart ;
	for( i=0 ; i<a->nsliders ; i++ ) {
		a->sliders[i]->box[0].point[0] = xoff ;
		a->sliders[i]->box[0].point[1] = yoff ;
		a->sliders[i]->box[0].point[2] = -1.0 ;
		a->sliders[i]->box[1].point[0] = xoff+swidth ;
		a->sliders[i]->box[1].point[1] = yoff ;
		a->sliders[i]->box[1].point[2] = -1.0 ;
		a->sliders[i]->box[2].point[0] = xoff+swidth ;
		a->sliders[i]->box[2].point[1] = yoff+sheight ;
		a->sliders[i]->box[2].point[2] = -1.0 ;
		a->sliders[i]->box[3].point[0] = xoff ;
		a->sliders[i]->box[3].point[1] = yoff+sheight ;
		a->sliders[i]->box[3].point[2] = -1.0 ;

		a->sliders[i]->box_color[0] = 0.2 ;
		a->sliders[i]->box_color[1] = 1.0 ;
		a->sliders[i]->box_color[2] = 0.2 ;

		a->sliders[i]->stop_box[0].point[0] = xoff ;
		a->sliders[i]->stop_box[0].point[1] = yoff+0.5*sheight-stoph*0.5 ;
		a->sliders[i]->stop_box[0].point[2] = 0.0 ;
		a->sliders[i]->stop_box[1].point[0] = xoff+swidth ;
		a->sliders[i]->stop_box[1].point[1] = yoff+0.5*sheight-stoph*0.5 ;
		a->sliders[i]->stop_box[1].point[2] = 0.0 ;
		a->sliders[i]->stop_box[2].point[0] = xoff+swidth ;
		a->sliders[i]->stop_box[2].point[1] = yoff+0.5*sheight+stoph*0.5 ;
		a->sliders[i]->stop_box[2].point[2] = 0.0 ;
		a->sliders[i]->stop_box[3].point[0] = xoff ;
		a->sliders[i]->stop_box[3].point[1] = yoff+0.5*sheight+stoph*0.5 ;
		a->sliders[i]->stop_box[3].point[2] = 0.0 ;
		
		a->sliders[i]->stop_box_color[0] = 1.0 ;
		a->sliders[i]->stop_box_color[1] = 0.0 ;
		a->sliders[i]->stop_box_color[2] = 0.0 ;

		a->sliders[i]->slider_color[0] = 0.0 ;	
		a->sliders[i]->slider_color[1] = 0.1 ;	
		a->sliders[i]->slider_color[2] = 0.0 ;	
		
		xoff += (swidth + salley) ;
	}
}


void fl_set_slider_mouse_state( int wx, int wy, struct myslider_array_type *a, int state ) 
{
	int k ;
		
    for( k=0 ; k<a->nsliders ; k++ ) {
      a->sliders[k]->moused_state = GLUT_UP ;
      /* Test if in a particular slider */
      if( (GLdouble)wx >= a->sliders[k]->box[0].point[0] && (GLdouble)wx <= a->sliders[k]->box[2].point[0] ) 
	if( (GLdouble)wy >= a->sliders[k]->box[0].point[1] && (GLdouble)wy <= a->sliders[k]->box[2].point[1] ) {
	  a->sliders[k]->moused_state = state ;
        }
    }
}

void fl_slider_picker( int wx, int wy, struct myslider_array_type *a, int *index, double *v ) 
{
	int j,k ;
	double slider_middle ;
	float dim_factor,dim_medium ;
	
	dim_medium = 0.7 ;
	
	*index = -1 ;
	
    for( k=0 ; k<a->nsliders ; k++ ) {
      /* Test if in a particular slider */
      if( (GLdouble)wx >= a->sliders[k]->box[0].point[0] && (GLdouble)wx <= a->sliders[k]->box[2].point[0] ) 
	if( a->sliders[k]->moused_state==GLUT_DOWN && (GLdouble)wy >= a->sliders[k]->box[0].point[1] && (GLdouble)wy <= a->sliders[k]->box[2].point[1] ) {
	  *index = (int) k ;
	  //printf("fl_slider_picker- *index: %d\n",*index) ;
	  glLoadIdentity();
	  dim_factor = dim_medium ;
	  glColor3f( dim_factor*a->sliders[k]->box_color[0],dim_factor*a->sliders[k]->box_color[1],
		     dim_factor*a->sliders[k]->box_color[2]) ;
	  glBegin(GL_POLYGON) ;
	  for( j=0 ; j<4 ; j++ )
	    glVertex3f(a->sliders[k]->box[j].point[0],a->sliders[k]->box[j].point[1],a->sliders[k]->box[j].point[2]) ;
	  glEnd() ;
	  glColor3f( a->sliders[k]->stop_box_color[0],a->sliders[k]->stop_box_color[1],a->sliders[k]->stop_box_color[2]) ;
	  glBegin(GL_POLYGON) ;
	  for( j=0 ; j<4 ; j++ )
	    glVertex3f(a->sliders[k]->stop_box[j].point[0],a->sliders[k]->stop_box[j].point[1],a->sliders[k]->stop_box[j].point[2]) ;
	  glEnd() ;
	  //glFlush() ;
	  
	  glLineWidth(2) ;
				//printf("slider-picker- a->sliders[k]->wyold: %d\n", a->sliders[k]->wyold) ;
   				/* Test if in stop box of this slider */
	  if( (GLdouble)wy >= a->sliders[k]->stop_box[0].point[1] && (GLdouble)wy <= a->sliders[k]->stop_box[2].point[1] ) {
	    //printf("fl_slider_picker- Entered stop box.\n") ;
	    *v = (double)99999.0 ;
	    glColor3f( a->sliders[k]->colorold[0],a->sliders[k]->colorold[1],a->sliders[k]->colorold[2]) ;
	    if(wy!=a->sliders[k]->wyold) {
	      glBegin(GL_LINES) ;
	      glVertex3f(a->sliders[k]->box[0].point[0],a->sliders[k]->wyold,1.0) ;
	      glVertex3f(a->sliders[k]->box[2].point[0],a->sliders[k]->wyold,1.0) ;
	      glEnd() ;
	      //glFlush() ; /* ? */
	    }
	    a->sliders[k]->colorold[0] =  a->sliders[k]->stop_box_color[0] ;
	    a->sliders[k]->colorold[1] =  a->sliders[k]->stop_box_color[1] ;
	    a->sliders[k]->colorold[2] =  a->sliders[k]->stop_box_color[2] ;
	  } else {
	    if(wy!=a->sliders[k]->wyold) {
	      //printf("fl_slider_picker- Entered non-stop box with a change.\n") ;
	      glColor3f( a->sliders[k]->colorold[0],a->sliders[k]->colorold[1],
			 a->sliders[k]->colorold[2]) ;
	      glBegin(GL_LINES) ;
	      glVertex3f(a->sliders[k]->box[0].point[0],a->sliders[k]->wyold,1.0) ;
	      glVertex3f(a->sliders[k]->box[2].point[0],a->sliders[k]->wyold,1.0) ;
	      glEnd() ;
	      //glFlush() ;
	    }
	    //printf("fl_slider_picker- Before new slider line draw.\n") ;
	    glColor3f( a->sliders[k]->slider_color[0],a->sliders[k]->slider_color[1],a->sliders[k]->slider_color[2]) ;
	    glBegin(GL_LINES) ;
	    glVertex3f(a->sliders[k]->box[0].point[0], wy,1.0) ;
	    glVertex3f(a->sliders[k]->box[2].point[0], wy,1.0) ;
	    glEnd() ;
	    //glFlush() ;
	    a->sliders[k]->colorold[0] =  dim_factor*a->sliders[k]->box_color[0] ;
	    a->sliders[k]->colorold[1] =  dim_factor*a->sliders[k]->box_color[1] ;
	    a->sliders[k]->colorold[2] =  dim_factor*a->sliders[k]->box_color[2] ;
	    
	    //printf("fl_slider_picker-Before cal of v.\n") ;
	    if( wy>a->sliders[k]->stop_box[2].point[1] ) {
	      slider_middle = 0.5 * ( a->sliders[k]->slider_max + a->sliders[k]->slider_min) ;
	      *v = (a->sliders[k]->slider_max - slider_middle) * (GLdouble)(wy-a->sliders[k]->stop_box[2].point[1]) /
		(GLdouble)(a->sliders[k]->box[2].point[1]-a->sliders[k]->stop_box[2].point[1]) ;
	    } else {
	      slider_middle = 0.5 * ( a->sliders[k]->slider_max + a->sliders[k]->slider_min) ;
	      *v = ( a->sliders[k]->slider_min - slider_middle) * (GLdouble)(wy-a->sliders[k]->stop_box[1].point[1]) /
		(GLdouble)(a->sliders[k]->box[1].point[1]-a->sliders[k]->stop_box[1].point[1]) ;
	    }
	    //printf("fl_slider_picker- *v: %f\n",*v) ;
	  }
	  a->sliders[k]->wyold = wy ;
	}
    }
    return ;
}

void fl_reset_sliders( struct myslider_array_type *a ) 
{
/* after sliders have had their slider reset to neutral, erase the old slider */
	int i ;

	for( i=0 ; i<a->nsliders ; i++ ) {
	      glColor3f( a->sliders[i]->colorold[0],a->sliders[i]->colorold[1],a->sliders[i]->colorold[2]) ;
	      glLineWidth(2) ;
	      glBegin(GL_LINES) ;
	      	glVertex3f(a->sliders[i]->box[0].point[0],a->sliders[i]->wyold,1.0) ;
	      	glVertex3f(a->sliders[i]->box[2].point[0],a->sliders[i]->wyold,1.0) ;
	      glEnd() ;
	}
 
}
void fl_display_sliders( struct myslider_array_type *a ) 
{
	int i,j ;
	float dim_factor,dim_high,dim_low,dim_medium,swell_factor ;
	float xmiddle,ymiddle,x,y;
		
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING) ;
	glDisable(GL_BLEND) ;
	dim_low = 0.3 ;
	dim_high = 1.0 ;
	dim_medium = 0.7 ;
    for( i=0 ; i<a->nsliders ; i++ ) {
    	// Print lable 
    	glLoadIdentity();
    	glColor3f(0.0,0.0,0.0) ;
     	glRasterPos3f(a->sliders[i]->box[3].point[0] , a->sliders[i]->box[3].point[1]+10.0, a->sliders[i]->box[3].point[2]) ;
   		fl_draw_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_10, a->sliders[i]->name) ;
    	
    	// Draw edges of boxes
    	glLoadIdentity();
    	xmiddle = 0.5 * (a->sliders[i]->box[1].point[0] + a->sliders[i]->box[0].point[0] ) ;
    	ymiddle = 0.5 * (a->sliders[i]->box[2].point[1] + a->sliders[i]->box[1].point[1] ) ;
    	dim_factor=dim_low ;
    	swell_factor = 1.1 ;
    	glColor3f( dim_factor*a->sliders[i]->box_color[0],dim_factor*a->sliders[i]->box_color[1],
    		dim_factor*a->sliders[i]->box_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_low ;
    	glColor3f( dim_factor*a->sliders[i]->box_color[0],dim_factor*a->sliders[i]->box_color[1],
    		dim_factor*a->sliders[i]->box_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_high ;
    	glColor3f( dim_factor*a->sliders[i]->box_color[0],dim_factor*a->sliders[i]->box_color[1],
    		dim_factor*a->sliders[i]->box_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_high ;
    	glColor3f( dim_factor*a->sliders[i]->box_color[0],dim_factor*a->sliders[i]->box_color[1],
    		dim_factor*a->sliders[i]->box_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->sliders[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->sliders[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->sliders[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	
		// Draw boxes
   	 	dim_factor=dim_medium ;
    	glColor3f( dim_factor*a->sliders[i]->box_color[0],dim_factor*a->sliders[i]->box_color[1],
    		dim_factor*a->sliders[i]->box_color[2]) ;
     	glBegin(GL_POLYGON) ;
    		for( j=0 ; j<4 ; j++ )
    			glVertex3f(a->sliders[i]->box[j].point[0],a->sliders[i]->box[j].point[1],a->sliders[i]->box[j].point[2]) ;
   	 	glEnd() ;
   	 }
    for( i=0 ; i<a->nsliders ; i++ ) {
	glLineWidth(2) ;
	glColor3f( a->sliders[i]->slider_color[0], a->sliders[i]->slider_color[1], a->sliders[i]->slider_color[2]) ;
	glBegin(GL_LINES) ;
	     glVertex3f(a->sliders[i]->box[0].point[0],a->sliders[i]->wyold,1.0) ;
	     glVertex3f(a->sliders[i]->box[2].point[0],a->sliders[i]->wyold,1.0) ;
	glEnd() ;
    	glColor3f( a->sliders[i]->stop_box_color[0], a->sliders[i]->stop_box_color[1], a->sliders[i]->stop_box_color[2]) ;
    	glBegin(GL_POLYGON) ;
    	for( j=0 ; j<4 ; j++ )
	    glVertex3f(a->sliders[i]->stop_box[j].point[0], a->sliders[i]->stop_box[j].point[1], a->sliders[i]->stop_box[j].point[2]) ;
   	glEnd() ;

    }
   // glFlush() ;
}

struct myMCbutton_array_type *fl_make_my_MCbuttons(int nbuttons) 
{
	int i,j,k ;
	struct myMCbutton_array_type *a ;
	
	a = (struct myMCbutton_array_type *)malloc(sizeof(struct myMCbutton_array_type)) ;
	if( a==NULL ) {
		printf("fl_make_my_MCbuttons- ERROR mallocing a.\n") ;
		exit(0) ;
	}	
	a->nbuttons = nbuttons ;
	a->buttons = (struct myMCbutton_type **)malloc( nbuttons * sizeof(struct myMCbutton_type *) ) ;
	if( a->buttons==NULL ) {
		printf("fl_make_my_MCbuttons- ERROR mallocing a->sliders.\n") ;
		exit(0) ;
	}
	for( i=0 ; i<a->nbuttons ; i++ ) {
		a->buttons[i] = (struct myMCbutton_type *)malloc(sizeof(struct myMCbutton_type)) ;
		if( a->buttons[i]==NULL ) {
			printf("fl_make_my_MCbuttons- ERROR mallocing a->sliders[%d].\n",i) ;
			exit(0) ;
		}
		strcpy(a->buttons[i]->name,"none") ;
		a->buttons[i]->state = 0 ;
		for( j=0 ; j<4 ; j++ ) {
			a->buttons[i]->box[j].point = (double *)malloc(4*sizeof(double)) ;
			if(a->buttons[i]->box[j].point==NULL ) {
				printf("fl_make_my_MCbuttons- ERROR mallocing box: %d vertex: %d\n",i,j) ;
				exit(0) ;
			}
			for( k=0 ; k<3 ; k++ ) {
				a->buttons[i]->box[j].point[k] = 0.0 ;
			}
			a->buttons[i]->box_on_color[j] = 0.0 ;
			a->buttons[i]->box_off_color[j] = 0.0 ;
		}
	}
	
	return( a ) ;
}

void fl_set_MCbutton_params(struct myMCbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart) 
{
	int i ;
	double xoff, yoff ;
	
	xoff = xstart ;
	yoff = ystart ;
	for( i=0 ; i<a->nbuttons ; i++ ) {
		a->buttons[i]->box[0].point[0] = xoff ;
		a->buttons[i]->box[0].point[1] = yoff ;
		a->buttons[i]->box[0].point[2] = -1.0 ;
		a->buttons[i]->box[1].point[0] = xoff+swidth ;
		a->buttons[i]->box[1].point[1] = yoff ;
		a->buttons[i]->box[1].point[2] = -1.0 ;
		a->buttons[i]->box[2].point[0] = xoff+swidth ;
		a->buttons[i]->box[2].point[1] = yoff+sheight ;
		a->buttons[i]->box[2].point[2] = -1.0 ;
		a->buttons[i]->box[3].point[0] = xoff ;
		a->buttons[i]->box[3].point[1] = yoff+sheight ;
		a->buttons[i]->box[3].point[2] = -1.0 ;

		a->buttons[i]->box_on_color[0] = 0.2 ;
		a->buttons[i]->box_on_color[1] = 1.0 ;
		a->buttons[i]->box_on_color[2] = 0.2 ;
		
		a->buttons[i]->box_off_color[0] = 1.0 ;
		a->buttons[i]->box_off_color[1] = 0.2 ;
		a->buttons[i]->box_off_color[2] = 0.2 ;
		
		xoff += xalley ;
		yoff += yalley ;
	}
}

void fl_MCbutton_down_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index ) 
{
	int i,j,k ;
//	double button_middle ;
	float dim_factor,dim_high,dim_low,dim_medium,swell_factor ;
	float xmiddle,ymiddle,x,y;
	
	*index = -1 ;
	dim_high = 1.0 ;
	dim_low = 0.3 ;
	dim_medium = 0.7 ;
	swell_factor = 1.3 ;

    for( k=0 ; k<a->nbuttons ; k++ ) {
    	/* Test if in a particular button */
      if( (GLdouble)wx >= a->buttons[k]->box[0].point[0] && (GLdouble)wx <= a->buttons[k]->box[2].point[0] ) 
	if( (GLdouble)wy >= a->buttons[k]->box[0].point[1] && (GLdouble)wy <= a->buttons[k]->box[2].point[1] ) {
	  *index = (int) k ;
	  a->buttons[k]->state = 1 ;
	  //printf("fl_slider_picker- *index: %d\n",*index) ;
	  glLoadIdentity();
	  
	  /* Draw box edges */
	  xmiddle = 0.5 * (a->buttons[k]->box[1].point[0] + a->buttons[k]->box[0].point[0] ) ;
	  ymiddle = 0.5 * (a->buttons[k]->box[2].point[1] + a->buttons[k]->box[1].point[1] ) ;
	  dim_factor=dim_high ;
	  glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
		     dim_factor*a->buttons[k]->box_off_color[2]) ;
	  glBegin(GL_POLYGON) ;
	  x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
	  y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
	  glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
	  x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
	  y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_high ;
				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
					   dim_factor*a->buttons[k]->box_off_color[2]) ;
				glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_low ;
				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
					   dim_factor*a->buttons[k]->box_off_color[2]) ;
				glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_low ;
    			glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    				dim_factor*a->buttons[k]->box_off_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;

    			/* Draw box */
   	 			dim_factor=dim_medium ;
    			glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    				dim_factor*a->buttons[k]->box_off_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				for( j=0 ; j<4 ; j++ )
    					glVertex3f(a->buttons[k]->box[j].point[0],a->buttons[k]->box[j].point[1],a->buttons[k]->box[j].point[2]) ;
   	 			glEnd() ;
   	 			
      	 		// Draw distinguishing MC marking
   	 			glColor3f(0.0,0.0,0.0) ;
   	 			glPointSize(1.0) ;
   	 			glBegin(GL_POINTS) ;
   	 				for( i=-1 ; i<=1 ; i++ )
   	 				for( j=-1 ; j<=1 ; j++ )
   	 					glVertex3f(xmiddle+j,ymiddle+i,1.0) ;
   	 			glEnd() ;
	 			
				glFlush() ;
				
			}
    }
    return ;
}

void fl_MCbutton_up_picker( int wx, int wy, struct  myMCbutton_array_type *a, int *index ) 
{
	int i,j,k ;
//	double button_middle ;
	float dim_factor,dim_high,dim_low,dim_medium,swell_factor ;
	float xmiddle,ymiddle,x,y;
	
	*index = -1 ;
	dim_high = 1.0 ;
	dim_low = 0.3 ;
	dim_medium = 0.7 ;
    	swell_factor = 1.3 ;

    for( k=0 ; k<a->nbuttons ; k++ ) {
    	/* Test if in a particular button */
    	if( (GLdouble)wx >= a->buttons[k]->box[0].point[0] && (GLdouble)wx <= a->buttons[k]->box[2].point[0] ) 
    		if( (GLdouble)wy >= a->buttons[k]->box[0].point[1] && (GLdouble)wy <= a->buttons[k]->box[2].point[1] ) {
    		   	*index = (int) k ;
    		   	a->buttons[k]->state = 0 ;
    		   	//printf("fl_slider_picker- *index: %d\n",*index) ;
    			glLoadIdentity();
    			
    			/* Draw edges */
    			xmiddle = 0.5 * (a->buttons[k]->box[1].point[0] + a->buttons[k]->box[0].point[0] ) ;
    			ymiddle = 0.5 * (a->buttons[k]->box[2].point[1] + a->buttons[k]->box[1].point[1] ) ;
    			dim_factor=dim_low ;
    			glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    				dim_factor*a->buttons[k]->box_on_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_low ;
    			glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    				dim_factor*a->buttons[k]->box_on_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_high ;
    			glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    				dim_factor*a->buttons[k]->box_on_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
   	 			dim_factor=dim_high ;
    			glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    				dim_factor*a->buttons[k]->box_on_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;

    			/* Draw box */
   	 			dim_factor=dim_medium ;
    			glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    				dim_factor*a->buttons[k]->box_on_color[2]) ;
    			glBegin(GL_POLYGON) ;
    				for( j=0 ; j<4 ; j++ )
    					glVertex3f(a->buttons[k]->box[j].point[0],a->buttons[k]->box[j].point[1],a->buttons[k]->box[j].point[2]) ;
   	 			glEnd() ;
   	 			
   	 			// Draw distinguishing MC marking
   	 			glColor3f(0.0,0.0,0.0) ;
   	 			glPointSize(1.0) ;
   	 			glBegin(GL_POINTS) ;
   	 				for( i=-1 ; i<=1 ; i++ )
   	 				for( j=-1 ; j<=1 ; j++ )
   	 					glVertex3f(xmiddle+j,ymiddle+i,1.0) ;
   	 			glEnd() ;

				//glFlush() ;
				
			}
    }
    return ;
}

void fl_display_MCbuttons( struct myMCbutton_array_type *a ) 
{
	int i,j,k ;
	float dim_factor,dim_high,dim_low,dim_medium,swell_factor ;
	float xmiddle,ymiddle,x,y;
		
	dim_high = 1.0 ;
	dim_low = 0.3 ;
	dim_medium = 0.7 ;
    	swell_factor = 1.3 ;

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING) ;
	glDisable(GL_BLEND) ;
	
    for( i=0 ; i<a->nbuttons ; i++ ) {
    	// Print lable 
    	glLoadIdentity();
    	glColor3f(0.0,0.0,0.0) ;
     	glRasterPos3f(a->buttons[i]->box[3].point[0] , a->buttons[i]->box[3].point[1]+5.0, a->buttons[i]->box[3].point[2]) ;
   		fl_draw_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_10,a->buttons[i]->name) ;
    	// Draw edges of boxes
    	glLoadIdentity();
    	xmiddle = 0.5 * (a->buttons[i]->box[1].point[0] + a->buttons[i]->box[0].point[0] ) ;
    	ymiddle = 0.5 * (a->buttons[i]->box[2].point[1] + a->buttons[i]->box[1].point[1] ) ;
    	dim_factor=dim_low ;
    	glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],
    		dim_factor*a->buttons[i]->box_on_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_low ;
    	glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],
    		dim_factor*a->buttons[i]->box_on_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_high ;
    	glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],
    		dim_factor*a->buttons[i]->box_on_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	dim_factor=dim_high ;
    	glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],
    		dim_factor*a->buttons[i]->box_on_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	
		// Draw boxes
   	 	dim_factor=dim_medium ;
    	glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],
    		dim_factor*a->buttons[i]->box_on_color[2]) ;
    	glBegin(GL_POLYGON) ;
    		for( j=0 ; j<4 ; j++ )
    			glVertex3f(a->buttons[i]->box[j].point[0],a->buttons[i]->box[j].point[1],a->buttons[i]->box[j].point[2]) ;
   	 	glEnd() ;
   	 	
   	 	// Draw distinguishing MC marking
   	 	glColor3f(0.0,0.0,0.0) ;
   	 	glPointSize(1.0) ;
   	 	glBegin(GL_POINTS) ;
   	 		for( k=-1 ; k<=1 ; k++ )
   	 		for( j=-1 ; j<=1 ; j++ )
   	 			glVertex3f(xmiddle+j,ymiddle+k,1.0) ;
   	 	glEnd() ;
   	 	
   	 }
     glFlush() ;
}

struct myTbutton_array_type *fl_make_my_Tbuttons(int nbuttons) 
{
	int i,j,k ;
	struct myTbutton_array_type *a ;
	
	a = (struct myTbutton_array_type *)malloc(sizeof(struct myTbutton_array_type)) ;
	if( a==NULL ) {
		printf("fl_make_my_Tbuttons- ERROR mallocing a.\n") ;
		exit(0) ;
	}	
	a->nbuttons = nbuttons ;
	a->buttons = (struct myTbutton_type **)malloc( nbuttons * sizeof(struct myTbutton_type *) ) ;
	if( a->buttons==NULL ) {
		printf("fl_make_my_Tbuttons- ERROR mallocing a->buttons.\n") ;
		exit(0) ;
	}
	for( i=0 ; i<a->nbuttons ; i++ ) {
		a->buttons[i] = (struct myTbutton_type *)malloc(sizeof(struct myTbutton_type)) ;
		if( a->buttons[i]==NULL ) {
			printf("fl_make_my_Tbuttons- ERROR mallocing a->buttons[%d].\n",i) ;
			exit(0) ;
		}
		strcpy(a->buttons[i]->name,"none") ;
		a->buttons[i]->state = 0 ;
		for( j=0 ; j<4 ; j++ ) {
			a->buttons[i]->box[j].point = (double *)malloc(4*sizeof(double)) ;
			if(a->buttons[i]->box[j].point==NULL ) {
				printf("fl_make_my_Tbuttons- ERROR mallocing box: %d vertex: %d\n",i,j) ;
				exit(0) ;
			}
			for( k=0 ; k<3 ; k++ ) {
				a->buttons[i]->box[j].point[k] = 0.0 ;
			}
			a->buttons[i]->box_on_color[j] = 0.0 ;
			a->buttons[i]->box_off_color[j] = 0.0 ;
		}
	}
	return( a ) ;
}

void fl_set_Tbutton_params(struct myTbutton_array_type *a, int swidth, int sheight, int xalley, int yalley, double xstart, double ystart) 
{
	int i ;
	double xoff, yoff ;
	
	xoff = xstart ;
	yoff = ystart ;
	for( i=0 ; i<a->nbuttons ; i++ ) {
		a->buttons[i]->box[0].point[0] = xoff ;
		a->buttons[i]->box[0].point[1] = yoff ;
		a->buttons[i]->box[0].point[2] = -1.0 ;
		a->buttons[i]->box[1].point[0] = xoff+swidth ;
		a->buttons[i]->box[1].point[1] = yoff ;
		a->buttons[i]->box[1].point[2] = -1.0 ;
		a->buttons[i]->box[2].point[0] = xoff+swidth ;
		a->buttons[i]->box[2].point[1] = yoff+sheight ;
		a->buttons[i]->box[2].point[2] = -1.0 ;
		a->buttons[i]->box[3].point[0] = xoff ;
		a->buttons[i]->box[3].point[1] = yoff+sheight ;
		a->buttons[i]->box[3].point[2] = -1.0 ;

		a->buttons[i]->box_on_color[0] = 0.2 ;
		a->buttons[i]->box_on_color[1] = 1.0 ;
		a->buttons[i]->box_on_color[2] = 0.2 ;
		
		a->buttons[i]->box_off_color[0] = 1.0 ;
		a->buttons[i]->box_off_color[1] = 0.2 ;
		a->buttons[i]->box_off_color[2] = 0.2 ;
		
		xoff += xalley ;
		yoff += yalley ;
	}
}

void fl_Tbutton_down_picker( int wx, int wy, struct  myTbutton_array_type *a, int *index ) 
{
	int i,j,k ;
//	double button_middle ;
	float dim_factor,dim_high,dim_medium,dim_low,swell_factor ;
	float xmiddle,ymiddle,x,y;
	float xhalfwidth ;
	
	*index = -1 ;
	dim_high = 1.0 ;
	dim_medium = 0.7 ;
	dim_low = 0.3 ;
    	swell_factor = 1.3 ;
 	
    for( k=0 ; k<a->nbuttons ; k++ ) {
    	/* Test if in a particular button */
    	if( (GLdouble)wx >= a->buttons[k]->box[0].point[0] && (GLdouble)wx <= a->buttons[k]->box[2].point[0] ) {
    		if( (GLdouble)wy >= a->buttons[k]->box[0].point[1] && (GLdouble)wy <= a->buttons[k]->box[2].point[1] ) {
    		   	*index = (int) k ;
    		   	if( a->buttons[k]->state == 1 ) {
    		   		 a->buttons[k]->state = 0 ;
    		   		 glColor3f( a->buttons[k]->box_off_color[0],a->buttons[k]->box_off_color[1],a->buttons[k]->box_off_color[2]) ;
    		   	} else if( a->buttons[k]->state == 0 ){
    		   		a->buttons[k]->state = 1 ;
    		   		glColor3f( a->buttons[k]->box_on_color[0],a->buttons[k]->box_on_color[1],a->buttons[k]->box_on_color[2]) ;
    		   	}
    			glLoadIdentity();
				/* Draw edges */
				    	// Draw edges of boxes
    			glLoadIdentity();
    			xmiddle = 0.5 * (a->buttons[k]->box[1].point[0] + a->buttons[k]->box[0].point[0] ) ;
    			ymiddle = 0.5 * (a->buttons[k]->box[2].point[1] + a->buttons[k]->box[1].point[1] ) ;
   			if( a->buttons[k]->state == 0 ) {
    				dim_factor=dim_high ;
    				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    					dim_factor*a->buttons[k]->box_off_color[2]) ;
    			}
    			else if( a->buttons[k]->state == 1 ) {	
   					dim_factor=dim_low ;
     	    		glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    					dim_factor*a->buttons[k]->box_on_color[2]) ;
    			}
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
    			if( a->buttons[k]->state == 0 ) {
    				dim_factor=dim_high ;
    				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    					dim_factor*a->buttons[k]->box_off_color[2]) ;
    			}
    			else if( a->buttons[k]->state == 1 ) {	
   					dim_factor=dim_low ;
     	    		glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    					dim_factor*a->buttons[k]->box_on_color[2]) ;
    			}
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[1].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[1].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
    			if( a->buttons[k]->state == 0 ) {
    				dim_factor=dim_low ;
    				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    					dim_factor*a->buttons[k]->box_off_color[2]) ;
    			}
    			else if( a->buttons[k]->state == 1 ) {	
   					dim_factor=dim_high ;
     	    		glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    					dim_factor*a->buttons[k]->box_on_color[2]) ;
    			}
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[2].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[2].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;
    			if( a->buttons[k]->state == 0 ) {
    				dim_factor=dim_low ;
    				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    					dim_factor*a->buttons[k]->box_off_color[2]) ;
    			}
    			else if( a->buttons[k]->state == 1 ) {	
   					dim_factor=dim_high ;
     	    		glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    					dim_factor*a->buttons[k]->box_on_color[2]) ;
    			}
    			glBegin(GL_POLYGON) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[3].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[3].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle + swell_factor*(a->buttons[k]->box[0].point[0]-xmiddle) ;
    				y = ymiddle + swell_factor*(a->buttons[k]->box[0].point[1]-ymiddle) ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
    				x = xmiddle ;
    				y = ymiddle ;
    				glVertex3f(x,y,a->buttons[k]->box[0].point[2]) ;
   	 			glEnd() ;

				/* Draw Boxes */
    			if( a->buttons[k]->state == 0 ) {
    				dim_factor=dim_medium ;
    				glColor3f( dim_factor*a->buttons[k]->box_off_color[0],dim_factor*a->buttons[k]->box_off_color[1],
    					dim_factor*a->buttons[k]->box_off_color[2]) ;
    			}
    			else if( a->buttons[k]->state == 1 ) {	
   					dim_factor=dim_medium ;
     	    		glColor3f( dim_factor*a->buttons[k]->box_on_color[0],dim_factor*a->buttons[k]->box_on_color[1],
    					dim_factor*a->buttons[k]->box_on_color[2]) ;
    			}
    			glBegin(GL_POLYGON) ;
    				for( j=0 ; j<4 ; j++ )
    					glVertex3f(a->buttons[k]->box[j].point[0],a->buttons[k]->box[j].point[1],a->buttons[k]->box[j].point[2]) ;
   	 			glEnd() ;
   	 			
// Draw distinguishing T marking
			xhalfwidth = 0.5 * (a->buttons[k]->box[1].point[0] - a->buttons[k]->box[0].point[0]) ;

    			if( a->buttons[k]->state == 0 ) {
	   	 			glColor3f(0.8,0.8,0.8) ;
	   	 			glPointSize(1.0) ;
	   	 			glBegin(GL_POINTS) ;
	   	 				for( i=(int) -xhalfwidth ; i<(int) xhalfwidth ; i++ )
   	 						glVertex3f(xmiddle+i,ymiddle,1.0) ;
   	 				glEnd() ;
   	 				glColor3f(0.2,0.2,0.2) ;
   	 				glPointSize(1.0) ;
   	 				glBegin(GL_POINTS) ;
   	 					for( i=(int) -xhalfwidth ; i<(int) xhalfwidth ; i++ )
   	 						glVertex3f(xmiddle+i,ymiddle-1,1.0) ;
   	 				glEnd() ;
   	 			}
   	 			else if( a->buttons[k]->state == 1 ) {
	   	 			glColor3f(0.2,0.2,0.2) ;
	   	 			glPointSize(1.0) ;
	   	 			glBegin(GL_POINTS) ;
	   	 				for( i=(int) -xhalfwidth ; i<(int) xhalfwidth ; i++ )
   	 						glVertex3f(xmiddle+i,ymiddle,1.0) ;
   	 				glEnd() ;
   	 				glColor3f(0.8,0.8,0.8) ;
   	 				glPointSize(1.0) ;
   	 				glBegin(GL_POINTS) ;
   	 					for( i=(int) -xhalfwidth ; i<(int) xhalfwidth ; i++ )
   	 						glVertex3f(xmiddle+i,ymiddle-1,1.0) ;
   	 				glEnd() ;
   	 			}
   	 			
				glFlush() ;
			}
		}
    }
    return ;
}

void fl_display_Tbuttons( struct myTbutton_array_type *a ) 
{
	int i,j ;
	float dim_factor,dim_high,dim_low,dim_medium,swell_factor ;
	float xmiddle,ymiddle,x,y;
	float xhalfwidth ;
		
	dim_high = 1.0 ;
	dim_medium = 0.7 ;
	dim_low = 0.3 ;
    	swell_factor = 1.3 ;
	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING) ;
	glDisable(GL_BLEND) ;
	
    for( i=0 ; i<a->nbuttons ; i++ ) {
    	// Print lable 
    	glLoadIdentity();
    	glColor3f(0.0,0.0,0.0) ;
     	glRasterPos3f(a->buttons[i]->box[3].point[0] , a->buttons[i]->box[3].point[1]+5.0, a->buttons[i]->box[3].point[2]) ;
   		fl_draw_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_10,a->buttons[i]->name) ;

    	// Draw edges of boxes
    	glLoadIdentity();
    	xmiddle = 0.5 * (a->buttons[i]->box[1].point[0] + a->buttons[i]->box[0].point[0] ) ;
    	ymiddle = 0.5 * (a->buttons[i]->box[2].point[1] + a->buttons[i]->box[1].point[1] ) ;
   	if( a->buttons[i]->state == 0 ) {
    		dim_factor=dim_high ;
glColor3f( dim_factor*a->buttons[i]->box_off_color[0],dim_factor*a->buttons[i]->box_off_color[1],dim_factor*a->buttons[i]->box_off_color[2]) ;
    	} else if( a->buttons[i]->state == 1 ){
    		dim_factor=dim_low ;
    		glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],dim_factor*a->buttons[i]->box_on_color[2]) ;
	}
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
 
    	if( a->buttons[i]->state == 0 ) {
  	 	dim_factor=dim_high ;
    		glColor3f( dim_factor*a->buttons[i]->box_off_color[0],dim_factor*a->buttons[i]->box_off_color[1],dim_factor*a->buttons[i]->box_off_color[2]) ;
    	} else if( a->buttons[i]->state == 1 ){
  	 	dim_factor=dim_low ;
    		glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],dim_factor*a->buttons[i]->box_on_color[2]) ;
	}
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[1].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[1].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
 
    	if( a->buttons[i]->state == 0 ) {
   	 	dim_factor=dim_low ;
    		glColor3f( dim_factor*a->buttons[i]->box_off_color[0],dim_factor*a->buttons[i]->box_off_color[1],dim_factor*a->buttons[i]->box_off_color[2]) ;
    	} else if( a->buttons[i]->state == 1 ){
   	 	dim_factor=dim_high ;
    		glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],dim_factor*a->buttons[i]->box_on_color[2]) ;
	}
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[2].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[2].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;

     	if( a->buttons[i]->state == 0 ) {
   	 	dim_factor=dim_low ;
    		glColor3f( dim_factor*a->buttons[i]->box_off_color[0],dim_factor*a->buttons[i]->box_off_color[1],dim_factor*a->buttons[i]->box_off_color[2]) ;
    	} else if( a->buttons[i]->state == 1 ){
   	 	dim_factor=dim_high ;
    		glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],dim_factor*a->buttons[i]->box_on_color[2]) ;
	}
    	glBegin(GL_POLYGON) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[3].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[3].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle + swell_factor*(a->buttons[i]->box[0].point[0]-xmiddle) ;
    		y = ymiddle + swell_factor*(a->buttons[i]->box[0].point[1]-ymiddle) ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
    		x = xmiddle ;
    		y = ymiddle ;
    		glVertex3f(x,y,a->buttons[i]->box[0].point[2]) ;
   	 	glEnd() ;
   	 	
		// Draw boxes
    	if( a->buttons[i]->state == 0 ) {
   	 	dim_factor=dim_medium ;
    		glColor3f( dim_factor*a->buttons[i]->box_off_color[0],dim_factor*a->buttons[i]->box_off_color[1],dim_factor*a->buttons[i]->box_off_color[2]) ;
    	} else if( a->buttons[i]->state == 1 ){
   	 	dim_factor=dim_medium ;
    		glColor3f( dim_factor*a->buttons[i]->box_on_color[0],dim_factor*a->buttons[i]->box_on_color[1],dim_factor*a->buttons[i]->box_on_color[2]) ;
	}
    	glBegin(GL_POLYGON) ;
    		for( j=0 ; j<4 ; j++ )
    			glVertex3f(a->buttons[i]->box[j].point[0],a->buttons[i]->box[j].point[1],a->buttons[i]->box[j].point[2]) ;
   	 	glEnd() ;
   	 	
// Draw distinguishing T marking
			xhalfwidth = 0.5 * (a->buttons[i]->box[1].point[0] - a->buttons[i]->box[0].point[0]) ;

    			if( a->buttons[i]->state == 0 ) {
	   	 			glColor3f(0.8,0.8,0.8) ;
	   	 			glPointSize(1.0) ;
	   	 			glBegin(GL_POINTS) ;
	   	 				for( j=(int) -xhalfwidth ; j<(int) xhalfwidth ; j++ )
   	 						glVertex3f(xmiddle+j,ymiddle,1.0) ;
   	 				glEnd() ;
   	 				glColor3f(0.2,0.2,0.2) ;
   	 				glPointSize(1.0) ;
   	 				glBegin(GL_POINTS) ;
   	 					for( j=(int) -xhalfwidth ; j<(int) xhalfwidth ; j++ )
   	 						glVertex3f(xmiddle+j,ymiddle-1,1.0) ;
   	 				glEnd() ;
   	 			}
   	 			else if( a->buttons[i]->state == 1 ) {
	   	 			glColor3f(0.2,0.2,0.2) ;
	   	 			glPointSize(1.0) ;
	   	 			glBegin(GL_POINTS) ;
	   	 				for( j=(int) -xhalfwidth ; j<(int) xhalfwidth ; j++ )
   	 						glVertex3f(xmiddle+j,ymiddle,1.0) ;
   	 				glEnd() ;
   	 				glColor3f(0.8,0.8,0.8) ;
   	 				glPointSize(1.0) ;
   	 				glBegin(GL_POINTS) ;
   	 					for( j=(int) -xhalfwidth ; j<(int) xhalfwidth ; j++ )
   	 						glVertex3f(xmiddle+j,ymiddle-1,1.0) ;
   	 				glEnd() ;
   	 			}
  	 			
   	 	
   	 }
     //glFlush() ;
}

/* this function will return the best spot to place a child window.  Takes as
 * input the proposed window's size */
/* currently the algorithm is to line them up horizontally from left to right */
void fl_get_subwindow_location (GLsizei new_win_x,
                             GLsizei new_win_y,
                             GLsizei *suggested_x,
                             GLsizei *suggested_y)
{
GLsizei wx0 = 8, wy0 = 32 ;
GLsizei wh0 = 480;//, ww0 = 640 ;
GLsizei whdelta01 = 40 ;
GLsizei wwdelta01 = 16 ;
  static GLint current_x = wx0,         /* current suggested position */
               current_y = wy0 + wh0 + whdelta01; 

  *suggested_x = current_x;
  *suggested_y = current_y;

  current_x += new_win_x + wwdelta01;

  return;
}
