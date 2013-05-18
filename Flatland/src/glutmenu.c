/*
glutmenu.c.  Main window menus

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
 * Created: 3-3-00
 * Added "wrappers" for collaboration: 3-15-02, Moad Mowafi
 *
 */

//#define DEBUG_MENU

/* globals */

#include "gheader.h"

/* "local" globals */
MENU_CALLBACKS callbacks [NBR_OF_CALLBACKS];    /* array of callback funcs &
                                                   their associated objects */

void null_select (int mode)
{
}

/* call the control function for the selected object's selected menu */
void mod_fl_menu_select (int mode)
{
  if (currentControlObject == NULL)
    return;

  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			 currentControlObject->object_controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    if (currentControlObject->object_controls->controls[mode]->control_function
	!= NULL)
      currentControlObject->object_controls->controls[mode]->control_function 
	(currentControlObject, 
	 currentControlObject->object_controls->controls[mode]->keystroke);
  }
}

/* build the select object menu */
//int fl_buildSelectObjMenu (struct universe_type *w)
int fl_buildSelectObjMenu ( void )
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;
	int i;
	OBJECT_TYPE *object;

	/* select a new current object menu */
	wc->selectCurrentObjMenu = glutCreateMenu( fl_object_select );
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - fl_buildSelectObjMenu\n", wc->selectCurrentObjMenu );
#endif

        /* add the top entry (center of the universe) object */
	object = lc->Universe->database[ 0 ].object;
	if ( object != NULL && object->menu_entry_on_flag == true )
	{
		glutAddMenuEntry( object->name, 0 );
	}

	/* add all remaining objects to the current object menu in reverse
         * order */
	for( i=lc->Universe->nobjects-1; i>0; i-- )
	{
		object = lc->Universe->database[ i ].object;
		if ( object != NULL && object->menu_entry_on_flag == true )
		{
			glutAddMenuEntry( object->name, i );
		}
	}

	return wc->selectCurrentObjMenu;
}

/* build a child menu (from the modCurrentObjMenu) (recersive) */
int buildChildMenu (OBJ_CONTROLS_ARRAY_TYPE *object_controls, int parentID)
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;
	int i;
	int childMenu;
	char menuName[ MAX_NAME_SIZE ];
	int subMenuID;


	/* find an available menu entry function */
	for (i = 0; i < NBR_OF_CALLBACKS; i++)
	{
		if ( !callbacks[i].in_use ) 
			break;
	}
	
	/* bail if we run out of callback functions (the submenu will not display) */
	if ( i == NBR_OF_CALLBACKS )
	{
		fprintf( stdout, "WARNING: Menu has too many submenus.  " );
		fprintf( stdout, "Not all 2D menu entries will be displayed.\n" );
		return 0;
	}

	/* Build the child menu and set up the callback function */
	callbacks[i].in_use = true;
	callbacks[i].controls = object_controls;
	childMenu = glutCreateMenu( callbacks[i].callback );
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - buildChildMenu\n", childMenu );
#endif
    	wc->SubMenuList[ wc->NumberOfSubMenus++ ] = childMenu;
	
	for ( i=0; i<object_controls->ncontrols; i++ )
	{
		/* create the menu name using menuname and key */
		if ( object_controls->controls[i]->keystroke != 0 )
		{
			snprintf( menuName, MAX_NAME_SIZE, "(%c) %s", object_controls->controls[i]->keystroke, object_controls->controls[i]->menuname );
		}
		else
		{
			snprintf( menuName, MAX_NAME_SIZE, "      %s", object_controls->controls[i]->menuname );
		}
		
		if ( object_controls->controls[i]->children == NULL )
		{
			/* add the menu entry */
			glutAddMenuEntry( menuName, i );
		}
		else
		{
			/* create a submenu and fill it in */
			subMenuID = buildChildMenu( object_controls->controls[i]->children, parentID );
			glutSetMenu( childMenu );
			glutAddSubMenu( menuName, subMenuID ); 
		}
	}
	return childMenu;
}

/* create the load app menu */
void fl_buildMenuLoadApp (void)
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;
	int cur_app, menu_nbr = 0;

	/* create the menu */
	wc->menuLoadAppMenu = glutCreateMenu( fl_load_app_menu_select );
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - fl_buildMenuLoadApp\n", wc->menuLoadAppMenu );
#endif

	/* fill it in */
	if ( loadableApps )
	{
		for( cur_app=0; cur_app<loadableApps->napps; cur_app++ )
		{
			if ( loadableApps->applications[cur_app]->loaded == FALSE && 
			     loadableApps->applications[cur_app]->menu_entry_on_flag == true )
			{
				glutAddMenuEntry( loadableApps->applications[cur_app]->appname, menu_nbr++ );
			}
		}
	}
}

/* create the unload app menu */
void fl_buildMenuUnloadApp (void)
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;
	int cur_app, cur_obj,  menu_nbr = 0;
/***
	OBJECT_TYPE *current_obj;
***/

	/* create the menu */
	wc->menuUnloadAppMenu = glutCreateMenu (fl_unload_app_menu_select);
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - fl_buildMenuUnloadApp\n", wc->menuUnloadAppMenu );
#endif

	/* fill it in */
	if ( loadableApps )
	{
	        /* go through the list of apps loaded from the config file */
		for( cur_app=0; cur_app<loadableApps->napps; cur_app++ )
		{
		        /* check to see if application is in fact loaded */
			if ( loadableApps->applications[cur_app]->loaded == TRUE &&
			     loadableApps->applications[cur_app]->menu_entry_on_flag == true )
			{
                          // KLS 15-Sept-03:  This causes crashes if an object nambed
                          // after the app doesn't exist:  replaced with a
                          // slower, more robust solution (below)
#if 0
			        /* get a ptr to this object */
				current_obj = fl_find_object_by_name ( loadableApps->applications[cur_app]->universe_ptr, loadableApps->applications[cur_app]->appname );
                        	if (current_obj->creator_app_ptr)
                        	{
					glutAddMenuEntry( loadableApps->applications[cur_app]->appname, menu_nbr++ );
                        	}
#endif
                                /* find an object that points to this app */
                                for (cur_obj=0; cur_obj < lc->Universe->nobjects; cur_obj++)
                                {
                                  if (lc->Universe->database[cur_obj].object->creator_app_ptr == loadableApps->applications[cur_app])
                                  {
                                    glutAddMenuEntry( loadableApps->applications[cur_app]->appname, menu_nbr++ );
                                    loadableApps->applications[cur_app]->unloadable = true;
                                    break;
                                  }
                                }
			}
		}
	}
}

/* create the modify object menu */
void fl_buildModCurrentObj (void)
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;
	int i;
	char menuName [MAX_NAME_SIZE];
	int subMenuID;

	/* create the menu */
//fprintf( stdout, "** win: %d, modCurrentObjMenu was %d, ", win, wc->modCurrentObjMenu );
//fflush( stdout );
	wc->modCurrentObjMenu = glutCreateMenu( mod_fl_menu_select );
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - fl_buildModCurrentObj\n", wc->modCurrentObjMenu );
#endif
//fprintf( stdout, "now %d...\n", wc->modCurrentObjMenu );
//fflush( stdout );

	if ( currentControlObject == NULL )
	{
		return;
	}

	/* fill it in */
	if ( currentControlObject->object_controls )
	{
		for( i=0; i<currentControlObject->object_controls->ncontrols; i++ )
		{
			/* create the menu name using menuname and key */
			if ( currentControlObject->object_controls->controls[i]->keystroke != 0 )
			{        
			snprintf( menuName, MAX_NAME_SIZE, "(%c) %s", currentControlObject->object_controls->controls[i]->keystroke, currentControlObject->object_controls->controls[i]->menuname );
			}
			else
			{
			snprintf( menuName, MAX_NAME_SIZE, "      %s", currentControlObject->object_controls->controls[i]->menuname );
			}
			if ( currentControlObject->object_controls->controls[i]->children == NULL )
			{
				/* add the menu entry */
				glutAddMenuEntry( menuName, i );
			}
			else
			{
				/* create a submenu and fill it in */
				subMenuID = buildChildMenu( currentControlObject->object_controls->controls[ i ]->children, i );
				glutSetMenu( wc->modCurrentObjMenu );
				glutAddSubMenu( menuName, subMenuID );
			}
		}
	}
}

/* routine to select which object is current */
void fl_object_select (int mode)
{
/***
	Loop_Control_Struct *lc = Loop_Control;
	int oldwin = lc->Window;
	Window_Control_Struct *wc; // = lc->WindowControl + win;
	int i;
***/
	
//fprintf( stdout, "fl_object_select(%d)...\n", mode );

//printf ("old CCO = %lx, new CCO = %lx\n", currentControlObject, Universe->database[mode].object);
	/* TODO: don't assume the global universe */
	currentControlObject = Universe->database[mode].object;

//fprintf( stdout, "selected \"%s\"...\n", currentControlObject->name );

	/* initialize the array of menu entry functions */
	/* NOTE: there must be the same number of initializations here as there are
	* menu callback functions defined at the end of this file *and* the same as
	* NBR_OF_CALLBACKS */
	callbacks[0].callback = fl_glut_menu_callback0;
	callbacks[0].in_use = false;
	callbacks[1].callback = fl_glut_menu_callback1;
	callbacks[1].in_use = false;
	callbacks[2].callback = fl_glut_menu_callback2;
	callbacks[2].in_use = false;
	callbacks[3].callback = fl_glut_menu_callback3;
	callbacks[3].in_use = false;
	callbacks[4].callback = fl_glut_menu_callback4;
	callbacks[4].in_use = false;
	callbacks[5].callback = fl_glut_menu_callback5;
	callbacks[5].in_use = false;
	callbacks[6].callback = fl_glut_menu_callback6;
	callbacks[6].in_use = false;
	callbacks[7].callback = fl_glut_menu_callback7;
	callbacks[7].in_use = false;
	callbacks[8].callback = fl_glut_menu_callback8;
	callbacks[8].in_use = false;
	callbacks[9].callback = fl_glut_menu_callback9;
	callbacks[9].in_use = false;

    	Loop_Control->Build_modCurrentObjMenu = true;
/***
	for( i=0; i<lc->NumberOfWindows; i++ )
	{
		wc = lc->WindowControl + i;
		lc->Window = i;
		glutSetWindow( wc->glutWindow );

		// delete and rebuild the select object menu 
		glutDestroyMenu( wc->modCurrentObjMenu );
		fl_buildModCurrentObj( );
	}
	wc = lc->WindowControl + oldwin;
	lc->Window = oldwin;
	glutSetWindow( wc->glutWindow );
***/
}

//void fl_glutMenu (struct universe_type *w)
void fl_glutMenu ( void )
{
	Loop_Control_Struct *lc = Loop_Control;
	int win = lc->Window;
	Window_Control_Struct *wc = lc->WindowControl + win;

//fprintf( stdout, "fl_glutMenu()\n" );

	// load app menu 
//	fl_buildMenuLoadApp ();

	// unload app menu 
//	fl_buildMenuUnloadApp ();

	// diddle with the current object menu 
//	fl_buildModCurrentObj ();

	// select object menu 
//	fl_buildSelectObjMenu ();

	// main menu 
	wc->MainMenu = glutCreateMenu( fl_menu_select );
#ifdef DEBUG_MENU
fprintf( stdout, "glutCreateMenu: %d - fl_glutMenu\n", wc->MainMenu );
#endif
	glutAddSubMenu( "Select Current Object", wc->selectCurrentObjMenu );
	glutAddSubMenu( "Twiddle Current Object", wc->modCurrentObjMenu );
	glutAddSubMenu( "Load Application", wc->menuLoadAppMenu );
	glutAddSubMenu( "Unload Application", wc->menuUnloadAppMenu );
	glutAddMenuEntry( "Quit", 1 );
	
//	glutSetMenu( wc->MainMenu )
//	glutAttachMenu( GLUT_RIGHT_BUTTON );
}

void fl_menu_select (int mode)
{
	switch (mode) 
	{
		case 1:       /* Quit */
			do_exit = TRUE;
			break;
	}
}

/* the load application function to load the selected app */
void fl_load_app_menu_select (int mode)
{
/***
	Loop_Control_Struct *lc = Loop_Control;
	int oldwin = lc->Window;
	Window_Control_Struct *wc; // = lc->WindowControl + win;
	int i;
***/
	int cur_app, menu_nbr = 0, rc;

	/* find the mode'th unloaded element */
	for( cur_app=0; cur_app<loadableApps->napps; cur_app++ )
	{
		if ( loadableApps->applications[ cur_app ]->loaded == FALSE )
		{
			if ( menu_nbr++ == mode )
			{
				break;
			}
		}
	}

	assert( cur_app != loadableApps->napps );  /* this should *never* be true */

	/* check to see if the selected app is shared */
	if (loadableApps->applications[cur_app]->shared_flag == true)
	{
	        fl_collab_send_load_msg(loadableApps->applications[cur_app]);
	}
        else    /* the selected app is not shared */
	{
	        /* load it */
	        if ( ( rc = fl_load_app( loadableApps->applications[ cur_app ] ) ) != 0 )
		{
		        fprintf( stderr, "ERROR: could not load application %s: load returned %d\n", loadableApps->applications[cur_app]->appname, rc );
		}

//	/* delete and rebuild the load menu */
//	glutDestroyMenu( wc->menuLoadAppMenu );
//	fl_buildMenuLoadApp( );
//
//	/* delete and rebuild the unload menu */
//	glutDestroyMenu( wc->menuUnloadAppMenu );
//	fl_buildMenuUnloadApp( );
//
//	/* delete and rebuild the select object menu */
//	glutDestroyMenu( wc->modCurrentObjMenu );
//	fl_buildModCurrentObj( );

    	    	Loop_Control->Build_selectCurrentObjMenu = true;
    	    	Loop_Control->Build_menuLoadAppMenu = true;
    	    	Loop_Control->Build_menuUnloadAppMenu = true;
//    	    	Loop_Control->Build_modCurrentObjMenu = true;
/*
		for( i=0; i<lc->NumberOfWindows; i++ )
		{
		        wc = lc->WindowControl + i;
			lc->Window = i;
			glutSetWindow( wc->glutWindow );
			
			// delete and rebuild the load menu 
			glutDestroyMenu( wc->menuLoadAppMenu );
			fl_buildMenuLoadApp( );

			// delete and rebuild the unload menu 
			glutDestroyMenu( wc->menuUnloadAppMenu );
			fl_buildMenuUnloadApp( );

			// delete and rebuild the select object menu 
			glutDestroyMenu( wc->modCurrentObjMenu );
			fl_buildModCurrentObj( );
		}
		wc = lc->WindowControl + oldwin;
		lc->Window = oldwin;
		glutSetWindow( wc->glutWindow );
*/
	}
}

void fl_unload_app_menu_select (int mode)
{
/***
	Loop_Control_Struct *lc = Loop_Control;
	int oldwin = lc->Window;
	Window_Control_Struct *wc; // = lc->WindowControl + win;
	int i;
***/
	int cur_app, menu_nbr = 0, rc;
/***
	OBJECT_TYPE *current_obj;
***/

	/* find the mode'th loaded element */
	for( cur_app=0; cur_app<loadableApps->napps; cur_app++ )
	{
		if ( loadableApps->applications[ cur_app ]->loaded && loadableApps->applications[ cur_app ]->unloadable && menu_nbr++ == mode )
		{
                        // KLS, 15-Sept-03:  test assumed object with app
#if 0
                        // name... replaced with more robust method
		        /* get a ptr to this object */
			// current_obj = fl_find_object_by_name ( loadableApps->applications[cur_app]->universe_ptr, loadableApps->applications[cur_app]->appname );
			/* if this is an unloadable app and the mode'th one.. */
                       	if (current_obj->creator_app_ptr && menu_nbr++ == mode )
                       	{
				break;
			}
#endif
                        break;
		}
	}

	assert( cur_app != loadableApps->napps );  /* this should *never* be true */

	/* check to see if the selected app is shared */
	if (loadableApps->applications[cur_app]->shared_flag == true)
	{
	        fl_collab_send_unload_msg(loadableApps->applications[cur_app]);
	}
        else    /* the selected app is not shared */
	{
	        /* unload it */
	        if ( ( rc = fl_unload_app( loadableApps->applications[ cur_app ] ) ) != 0 )
		{
		        fprintf( stderr, "ERROR: could not unload appliction %s: unload returned %d\n", loadableApps->applications[cur_app]->appname, rc );
		}

//	/* delete and rebuild the load menu */
//	glutDestroyMenu( wc->menuLoadAppMenu );
//	fl_buildMenuLoadApp( );
//
//	/* delete and rebuild the unload menu */
//	glutDestroyMenu( wc->menuUnloadAppMenu );
//	fl_buildMenuUnloadApp( );
//
//	/* delete and rebuild the select object menu */
//	glutDestroyMenu( wc->modCurrentObjMenu );
//	fl_buildModCurrentObj( );

    	    	Loop_Control->Build_selectCurrentObjMenu = true;
    	    	Loop_Control->Build_menuLoadAppMenu = true;
    	    	Loop_Control->Build_menuUnloadAppMenu = true;
    	    	Loop_Control->Build_modCurrentObjMenu = true;
/*
		for( i=0; i<lc->NumberOfWindows; i++ )
		{
		        wc = lc->WindowControl + i;
			lc->Window = i;
			glutSetWindow( wc->glutWindow );

			// delete and rebuild the load menu 
			glutDestroyMenu( wc->menuLoadAppMenu );
			fl_buildMenuLoadApp( );

			// delete and rebuild the unload menu 
			glutDestroyMenu( wc->menuUnloadAppMenu );
			fl_buildMenuUnloadApp( );

			// delete and rebuild the select object menu 
			glutDestroyMenu( wc->modCurrentObjMenu );
			fl_buildModCurrentObj( );
		}
		wc = lc->WindowControl + oldwin;
		lc->Window = oldwin;
		glutSetWindow( wc->glutWindow );
*/
	}
}

void fl_glut_menu_callback0 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[0].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[0].controls->controls[mode]->control_function (currentControlObject,
	 callbacks[0].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback1 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[1].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[1].controls->controls[mode]->control_function (currentControlObject,
         callbacks[1].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback2 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[2].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[2].controls->controls[mode]->control_function (currentControlObject,
         callbacks[2].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback3 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[3].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
   callbacks[3].controls->controls[mode]->control_function (currentControlObject,
         callbacks[3].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback4 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[4].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[4].controls->controls[mode]->control_function (currentControlObject,
         callbacks[4].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback5 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[5].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[5].controls->controls[mode]->control_function (currentControlObject,
         callbacks[5].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback6 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[6].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[6].controls->controls[mode]->control_function (currentControlObject,
         callbacks[6].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback7 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[7].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[7].controls->controls[mode]->control_function (currentControlObject,
         callbacks[7].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback8 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[8].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[8].controls->controls[mode]->control_function (currentControlObject,
         callbacks[8].controls->controls[mode]->keystroke);
  }
}

void fl_glut_menu_callback9 (int mode)
{
  /* check to see if the selected object is shared */
  if (currentControlObject->shared_flag == true)
  {
    fl_collab_send_ctrl_msg(currentControlObject, 
			    callbacks[9].controls->controls[mode]->keystroke);
  }
  else  /* the selected object is not shared */
  {
    callbacks[9].controls->controls[mode]->control_function (currentControlObject,
         callbacks[9].controls->controls[mode]->keystroke);
  }
}


void fl_Build_selectCurrentObjMenu( void )
{
    int i;
    
#ifdef DEBUG_MENU
fprintf( stdout, "fl_Build_selectCurrentObjMenu()\n" );
#endif

    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
    	Loop_Control->Window = i;
	glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

	if( !( Loop_Control->WindowControl[ i ].MenuInUse ) )
	{
	    // delete and rebuild the load menu 
	    if( Loop_Control->WindowControl[ i ].selectCurrentObjMenu )
	    {
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_selectCurrentObjMenu\n", Loop_Control->WindowControl[ i ].selectCurrentObjMenu );
#endif
		glutDestroyMenu( Loop_Control->WindowControl[ i ].selectCurrentObjMenu );
	    }
	    fl_buildSelectObjMenu( );
	}
    }
}
 
void fl_Build_modCurrentObjMenu( void )
{
    int i,j;
    
#ifdef DEBUG_MENU
fprintf( stdout, "fl_Build_modCurrentObjMenu()\n" );
#endif

    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
    	Loop_Control->Window = i;
	glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

	if( !( Loop_Control->WindowControl[ i ].MenuInUse ) )
	{
	    if( Loop_Control->WindowControl[ i ].NumberOfSubMenus > 0 )
	    {
		for( j=0; j<Loop_Control->WindowControl[ i ].NumberOfSubMenus; j++ )
		{
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_modCurrentObjMenu\n", Loop_Control->WindowControl[ i ].SubMenuList[ j ] );
#endif
	    	    glutDestroyMenu( Loop_Control->WindowControl[ i ].SubMenuList[ j ] );
		}
		Loop_Control->WindowControl[ i ].NumberOfSubMenus = 0;
	    }

	    // delete and rebuild the load menu 
	    if( Loop_Control->WindowControl[ i ].modCurrentObjMenu )
	    {
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_modCurrentObjMenu\n", Loop_Control->WindowControl[ i ].modCurrentObjMenu );
#endif
		glutDestroyMenu( Loop_Control->WindowControl[ i ].modCurrentObjMenu );
	    }
	    fl_buildModCurrentObj( );
	}
    }
}
  
void fl_Build_menuLoadAppMenu( void )
{
    int i;
    
#ifdef DEBUG_MENU
fprintf( stdout, "fl_Build_menuLoadAppMenu()\n" );
#endif

    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
    	Loop_Control->Window = i;
	glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

	if( !( Loop_Control->WindowControl[ i ].MenuInUse ) )
	{
	    // delete and rebuild the load menu 
	    if( Loop_Control->WindowControl[ i ].menuLoadAppMenu )
	    {
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_menuLoadAppMenu\n", Loop_Control->WindowControl[ i ].menuLoadAppMenu );
#endif
		glutDestroyMenu( Loop_Control->WindowControl[ i ].menuLoadAppMenu );
	    }
	    fl_buildMenuLoadApp( );
	}
    }
}
  
void fl_Build_menuUnloadAppMenu( void )
{
    int i;
    
#ifdef DEBUG_MENU
fprintf( stdout, "fl_Build_menuUnloadAppMenu()\n" );
#endif

    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
    	Loop_Control->Window = i;
	glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

	if( !( Loop_Control->WindowControl[ i ].MenuInUse ) )
	{
	    // delete and rebuild the load menu 
	    if( Loop_Control->WindowControl[ i ].menuUnloadAppMenu )
	    {
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_menuUnloadAppMenu\n", Loop_Control->WindowControl[ i ].menuUnloadAppMenu );
#endif
		glutDestroyMenu( Loop_Control->WindowControl[ i ].menuUnloadAppMenu );
	    }
	    fl_buildMenuUnloadApp( );
	}
    }
}
  
void fl_Build_MainMenu( void )
{
    int i;
    
#ifdef DEBUG_MENU
fprintf( stdout, "fl_Build_MainMenu()\n" );
#endif

    for( i=0; i<Loop_Control->NumberOfWindows; i++ )
    {
    	Loop_Control->Window = i;
	glutSetWindow( Loop_Control->WindowControl[ i ].glutWindow );

	if( !( Loop_Control->WindowControl[ i ].MenuInUse ) )
	{
	    // delete and rebuild the load menu 
	    if( Loop_Control->WindowControl[ i ].MainMenu )
	    {
		glutSetMenu( Loop_Control->WindowControl[ i ].MainMenu );
		glutDetachMenu( GLUT_RIGHT_BUTTON );
#ifdef DEBUG_MENU
fprintf( stdout, "glutDestroyMenu(%d) - fl_Build_MainMenu\n", Loop_Control->WindowControl[ i ].MainMenu );
#endif
		glutDestroyMenu( Loop_Control->WindowControl[ i ].MainMenu );
	    }
	    fl_glutMenu( );

	    glutSetMenu( Loop_Control->WindowControl[ i ].MainMenu );
	    glutAttachMenu( GLUT_RIGHT_BUTTON );
	}
    }
}
  
void fl_Build_Menus( void )
{
    // build changed menus

    Loop_Control->Build_MainMenu |= Loop_Control->Build_selectCurrentObjMenu;
    Loop_Control->Build_MainMenu |= Loop_Control->Build_modCurrentObjMenu;
    Loop_Control->Build_MainMenu |= Loop_Control->Build_menuLoadAppMenu;
    Loop_Control->Build_MainMenu |= Loop_Control->Build_menuUnloadAppMenu;

    if ( Loop_Control->Build_selectCurrentObjMenu )
    {
	fl_Build_selectCurrentObjMenu();
	Loop_Control->Build_selectCurrentObjMenu = false;
    }

    if ( Loop_Control->Build_modCurrentObjMenu )
    {
	fl_Build_modCurrentObjMenu();
	Loop_Control->Build_modCurrentObjMenu = false;
    }

    if ( Loop_Control->Build_menuLoadAppMenu )
    {
	fl_Build_menuLoadAppMenu();
	Loop_Control->Build_menuLoadAppMenu = false;
    }

    if ( Loop_Control->Build_menuUnloadAppMenu )
    {
	fl_Build_menuUnloadAppMenu();
	Loop_Control->Build_menuUnloadAppMenu  = false;
    }

    if ( Loop_Control->Build_MainMenu )
    {
	fl_Build_MainMenu();
	Loop_Control->Build_MainMenu  = false;
    }
}

void fl_glutMenuStatus( int status, int x, int y )
{
    Loop_Control->WindowControl[ Loop_Control->Window ].MenuInUse = status;
#ifdef DEBUG_MENU
fprintf( stdout, "fl_glutMenuStatus(%d)\n", status );
#endif
}
