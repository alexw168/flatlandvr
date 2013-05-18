/*
dll_funcs.cpp.  Handles all dynamic loading of apps

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
 * Created: 1-13-00
 */

/* Load all base apps */

#include "gheader.h"

int fl_load_all_base_apps (APPLISTTYPE *Apps)
{
  int i;
  int error_code = 0;
  char libstring [MAX_NAME_SIZE];
  void (*lib_init_func)(APPTYPE *);

  if (!Apps) return -1;

  for (i=0; i < Apps->napps; i++)
  {
    if (Apps->applications[i]->type == 0)       /* 0 = base app */
    {
      /* construct the library filename */
      snprintf (libstring, MAX_NAME_SIZE, "%s/%s",
                Apps->applications[i]->DLLloc, 
                Apps->applications[i]->DLLname);

      printf ("Loading %s...\n", libstring);

      /* Open the shared library */
      if ((Apps->applications[i]->libc=dlopen(libstring,RTLD_NOW)) == NULL)
      {
        fprintf (stderr, "fl_load_all_base_apps: Could not load library:\n  %s\n",
                 dlerror());
        error_code++;
        continue;
      }

      /* get a pointer to the well known init function */
      lib_init_func = (void (*)(APPTYPE *)) dlsym(Apps->applications[i]->libc, 
                                                  "Flatland_init");
      if (lib_init_func == NULL)
      {
        fprintf(stderr, "fl_load_all_base_apps: Could not load init func:\n  %s\n",
                dlerror());
        error_code++;
        continue;
      }

      /* Call the function */
      (*lib_init_func)(Apps->applications[i]);

      /* Mark the module loaded */
      Apps->applications[i]->loaded = TRUE;
    }
  }
  return (error_code);
}

int fl_load_app (APPTYPE *App)
{
/***
	Loop_Control_Struct *lc = Loop_Control;
	int oldwin = lc->Window;
	Window_Control_Struct *wc; // = lc->WindowControl + win;
	int i;
***/
	char libstring [MAX_NAME_SIZE];
	void (*lib_init_func)(APPTYPE *);

	/* construct the library filename */
	snprintf (libstring, MAX_NAME_SIZE, "%s/%s", App->DLLloc, App->DLLname);

	printf ("Loading %s...\n", libstring);

	if ((App->libc = dlopen (libstring, RTLD_NOW)) == NULL)
	{
		fprintf (stderr, "fl_load_app: Could not load library:\n  %s\n",
		dlerror());
		return (1);
	}

	/* get a pointer to the well known init function */
	lib_init_func = (void (*)(APPTYPE *)) dlsym(App->libc, "Flatland_init");
	if (lib_init_func == NULL)
	{
		fprintf(stderr, "fl_load_app: Could not load init func:\n  %s\n", dlerror());
		return (1);
	}

	/* Call the function */
	(*lib_init_func)(App);

	/* Mark the module loaded */
	App->loaded = TRUE;

    	Loop_Control->Build_selectCurrentObjMenu = true;
    	Loop_Control->Build_menuLoadAppMenu = true;
    	Loop_Control->Build_menuUnloadAppMenu = true;
/***
	for( i=0; i<lc->NumberOfWindows; i++ )
	{
		wc = lc->WindowControl + i;
		lc->Window = i;
		glutSetWindow( wc->glutWindow );

		glutDestroyMenu( wc->selectCurrentObjMenu );
		fl_buildSelectObjMenu( App->universe_ptr );
	}
	wc = lc->WindowControl + oldwin;
	lc->Window = oldwin;
	glutSetWindow( wc->glutWindow );
***/

	return 0;
}

int fl_unload_app (APPTYPE *App)
{
/***
	Loop_Control_Struct *lc = Loop_Control;
	int oldwin = lc->Window;
	Window_Control_Struct *wc; // = lc->WindowControl + win;
	int i;
***/
	int rc = 0;

	printf ("Unloading app %s\n", App->appname);

	/* TODO:  don't use the global universe */
	rc = fl_delete_objects_by_app_ptr (Universe, App);

	/* mark the app as unloaded */
	if (!rc)
	{
		App->loaded = FALSE;

		//    glutDestroyMenu (selectCurrentObjMenu);
		//    fl_buildSelectObjMenu (App->universe_ptr);

    	    	Loop_Control->Build_selectCurrentObjMenu = true;
    	    	Loop_Control->Build_menuLoadAppMenu = true;
    	    	Loop_Control->Build_menuUnloadAppMenu = true;
    	    	Loop_Control->Build_modCurrentObjMenu = true;
/****
		for( i=0; i<lc->NumberOfWindows; i++ )
		{
			wc = lc->WindowControl + i;
			lc->Window = i;
			glutSetWindow( wc->glutWindow );

			glutDestroyMenu( wc->selectCurrentObjMenu );
			fl_buildSelectObjMenu( App->universe_ptr );
		}
		wc = lc->WindowControl + oldwin;
		lc->Window = oldwin;
		glutSetWindow( wc->glutWindow );
****/
	}

	return (rc);
}
