/*
 * glutmenu.h
 *
 * Header file for glutmenu.c
 *
 * Copyright (C) 2003, University of New Mexico.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *           
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GLUTMENU_H
#define GLUTMENU_H

/* Globals */

/* Constants */
const int NBR_OF_CALLBACKS = 10;        /* NOTE: Must be the number of
                                           callback functions defined at the
                                           end of glutmenu.c */

/* structure defs */
typedef struct {
  void (*callback)(int);        /* the callback function */
  struct obj_controls_array_type *controls;/*the control struct for this menu */
  int func_index;               /* The function index to the control callback */
  bool in_use;                  /* Is this callback currently assigned? */
} MENU_CALLBACKS;

/* Function prototypes */
//void fl_glutMenu (struct universe_type *w);
//void fl_glutMenu ( void);

void fl_menu_select (int mode);
void fl_load_app_menu_select (int mode);
void fl_unload_app_menu_select (int mode);
void fl_object_select (int mode);

void fl_glut_menu_callback0 (int mode);
void fl_glut_menu_callback1 (int mode);
void fl_glut_menu_callback2 (int mode);
void fl_glut_menu_callback3 (int mode);
void fl_glut_menu_callback4 (int mode);
void fl_glut_menu_callback5 (int mode);
void fl_glut_menu_callback6 (int mode);
void fl_glut_menu_callback7 (int mode);
void fl_glut_menu_callback8 (int mode);
void fl_glut_menu_callback9 (int mode);

// use flags in Loop_Control to call these functions in idle function
void fl_Build_Menus( void );
//void fl_Build_MainMenu( void ); 
//void fl_Build_selectCurrentObjMenu( void ); 
//void fl_Build_modCurrentObjMenu( void ); 
//void fl_Build_menuLoadAppMenu( void ); 
//void fl_Build_menuUnloadAppMenu( void ); 

//int fl_buildSelectObjMenu (struct universe_type *w);

/* needed by Collaborator App */
//void fl_buildMenuLoadApp (void);
//void fl_buildMenuUnloadApp (void);
//void fl_buildModCurrentObj (void);

void fl_glutMenuStatus( int status, int x, int y );

#endif
