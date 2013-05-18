/*
DisplayFlatlandTree.cc  An app representing the flatland universe tree

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

// 
// Maintenance history
//
// Created: 9-7-00, TBE
//

// include the master Flatland library 
#include "flatland_app.h"
#include "DisplayFlatlandTree.h"

#define DEFAULT_ROOT_NAME "CenterOfTheUniverse"

// Platform handler 
void platformfunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	// no platform handling for this object 
}

// Sound callback 
void soundfunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	// do nothing
}

void DrawBox(char *message, float h, bool highlight)
{
	float w = strlen(message)*0.6*h;
	float black[4] = {0,0,0,1};
	float blue[4] = {0,0,0.5,1};
	float red[4] = {0.5,0,0,1};

	glEnable(GL_LIGHTING);
	if ( highlight )
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, red );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, red );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black );
	}
	else
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, blue );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, blue );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black );
	}
	glPushMatrix();
		glTranslatef(h/2.0,0,0);
		glScalef(w+1,h*1.5,h*1.5);
		glTranslatef(h/2.0,0,0);
		glutSolidCube(1);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(h,0,0);
		glPushMatrix();
			glTranslatef(0,-h/3.0,h*1.5/1.95);
			fl_draw_stroke_string(message,h);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-h/3.0,-h*1.5/1.95);
			fl_draw_stroke_string(message,h);
		glPopMatrix();
		glPushMatrix();
			glRotatef(90,1,0,0);
			glTranslatef(0,-h/3.0,h*1.5/1.95);
			fl_draw_stroke_string(message,h);
		glPopMatrix();
		glPushMatrix();
			glRotatef(90,1,0,0);
			glTranslatef(w,0,0);
			glRotatef(180,0,1,0);
			glTranslatef(0,-h/3.0,h*1.5/1.95);
			fl_draw_stroke_string(message,h);
		glPopMatrix();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

// find the width of a tree
int TreeWidth(OBJECT_TYPE *root, OBJECT_TYPE *parent)
{
    GRAPH_NODE_TYPE *n, *p = root->orginal_first_node;
	int i; //, num = root->nneighbors;
	int k = 0;
	
	for(i = 0, p = root->orginal_first_node; i == 0 || p != root->orginal_first_node; p = p->right_sibling_node, i++)
	{
		n = p->neighbor_node;
    	if (n->self != parent && n->self != root)
    	{
	    	k += TreeWidth(n->self, root);
	    }
	}
	if (k==0) k++; // include itself if there are no children
	return k;
}

void DrawTree(OBJECT_TYPE *root, OBJECT_TYPE *parent, int depth, int highlight_depth)
{
	glPushMatrix();
    
	// draw the current root
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glutSolidCube(0.2);
	glEnable(GL_LIGHTING);

	glPushMatrix();
	if ( depth == highlight_depth )
	{
		glRotatef(180,1,0,0);
	}
	glRotatef(-90,0,1,0);
	DrawBox(root->name,1,(depth == highlight_depth));
	glPopMatrix();

    GRAPH_NODE_TYPE *n, *p;
	int i, j;
	float x_step = 1;
	float x;
	int w[1000];
	int tw = 0;
	int xo;
	
	for(i=0;i<1000;i++)
	{
		w[i]=0;
	}
		  
	for(i = 0, j = 0, p = root->orginal_first_node; i == 0 || p != root->orginal_first_node; p = p->right_sibling_node, i++)
	{
		n = p->neighbor_node;
    	if (n->self != parent && n->self != root)
    	{
	    	w[j] = TreeWidth(n->self, root);
	    	tw += w[j];
	    	j++;
	    }
	}
	if (tw==0)
	{
		tw++;
	}
	xo = -tw;
	
	for(i = 0, j = 0, p = root->orginal_first_node; i == 0 || p != root->orginal_first_node; p = p->right_sibling_node, i++)
	{
		n = p->neighbor_node;
    	if (n->self != parent && n->self != root)
    	{
			xo = xo + w[j];
	    	x = xo * x_step;
			xo = xo + w[j];
			
	    	glPushMatrix();
			glDisable(GL_LIGHTING);
	    	glColor3f(1,1,1);
	    	glBegin(GL_LINES);
	    	glVertex3f(0,0,0);
	    	glVertex3f(x,3,0);
	    	glEnd();    	
			glEnable(GL_LIGHTING);
	    	glTranslatef(x,3,0);
	    	DrawTree(n->self, root, depth+1, highlight_depth);
	    	glPopMatrix();
	    	j++;
	    }
	}
   	glPopMatrix();
   	glColor3f(0,0,0);
}

// Draw callback 
void drawfunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
	float point[4] = {0,0,0,1};
	float root_point[4] = {0,0,0,1};
	float result[4];
	float color[5][4] = {
		{0,0,0,1},
		{1,0,0,1},
		{0,1,0,1},
		{0,0,1,1},
		{1,1,1,1}};
	static int ind=0;
	
	ind = DisplayFlatlandTreeDataMembers->Color;			

	if ( !DisplayFlatlandTreeObject->drawing_on_flag || !DisplayFlatlandTreeObject->viewable_flag ) 
	{
		return;
	}
	
	OBJECT_TYPE *root, *parent;

	root = fl_find_object_by_name(DisplayFlatlandTreeObject->universe, DisplayFlatlandTreeDataMembers->RootName);		
    parent = root;

	if (root)
	{
	
		int w = TreeWidth(root, parent);
	
	    glPushMatrix();

		glTranslatef(-9*DisplayFlatlandTreeDataMembers->Scale,1+w,0);
		glRotatef(-90,0,0,1);
	
		DrawTree(root,parent,1,DisplayFlatlandTreeDataMembers->HighlightDepth);

		glPopMatrix();
		
		root_point[0] = -9*DisplayFlatlandTreeDataMembers->Scale;
		root_point[1] = 1+w;
		root_point[2] = 0;

		// draw line to the actual object...
		glDisable(GL_LIGHTING);
		glColor3fv(color[ind]);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_LINES);
		fl_tpc_matrix_multiply(root->Mco, point, result);
		glVertex3fv(result);
		fl_tpc_matrix_multiply(DisplayFlatlandTreeObject->Mco, root_point, result);
		glVertex3fv(result);
		glEnd();	
		glPopMatrix();
		glEnable(GL_LIGHTING);
	    
	}
}

// true shadow callback 
void trueShadowFunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
//	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers;			// "global" structure pointer 
//	DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
	drawfunc(DisplayFlatlandTreeObject);
}

// cheap shadow callback 
void cheapShadowFunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
}

// menu control callback 
void EmptyMenu(OBJECT_TYPE *DisplayFlatlandTreeObject, char key)
{
}

// menu control callback 
void NextMenuFunc(OBJECT_TYPE *DisplayFlatlandTreeObject, char key)
{
    DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
    
    switch( key )
    {
	case 'N':
	{
	    char *root_name = DisplayFlatlandTreeDataMembers->RootName;
	    OBJECT_TYPE *root = fl_find_object_by_name(DisplayFlatlandTreeObject->universe, DisplayFlatlandTreeDataMembers->RootName);
	    if( root )
	    {
		int n = DisplayFlatlandTreeObject->universe->nobjects;
		DATABASE_TYPE *db = DisplayFlatlandTreeObject->universe->database;
		OBJECT_TYPE *p;
		int i, old_i=-1;
		for( i=0; i<n; i++)
		{
		    p = db[i].object;
		    if( p == root )
		    {
			old_i = i;
			break;
		    }
		}
		if( old_i >= 0 )
		{
		    int new_i = ( old_i + 1 ) % n;
		    char *new_name = db[new_i].object->name;
    		    strncpy(DisplayFlatlandTreeDataMembers->RootName,new_name,MAX_NAME_SIZE);
	printf( "\"%s\" selected as root of tree...\n", DisplayFlatlandTreeDataMembers->RootName );
		}
		else
		{
		    // bad, no object found...
		}
	    }
	    else
	    {
		strncpy(DisplayFlatlandTreeDataMembers->RootName,"CenterOfTheUniverse",MAX_NAME_SIZE);
	    }		
	}
    	break;
	
	case 'n':
	{
	    char *root_name = DisplayFlatlandTreeDataMembers->RootName;
	    OBJECT_TYPE *root = fl_find_object_by_name(DisplayFlatlandTreeObject->universe, DisplayFlatlandTreeDataMembers->RootName);
	    if( root )
	    {
		int n = DisplayFlatlandTreeObject->universe->nobjects;
		DATABASE_TYPE *db = DisplayFlatlandTreeObject->universe->database;
		OBJECT_TYPE *p;
		int i, old_i=-1;
		for( i=0; i<n; i++)
		{
		    p = db[i].object;
		    if( p == root )
		    {
			old_i = i;
			break;
		    }
		}
		if( old_i >= 0 )
		{
		    int new_i = ( n + old_i - 1 ) % n;
		    char *new_name = db[new_i].object->name;
    		    strncpy(DisplayFlatlandTreeDataMembers->RootName,new_name,MAX_NAME_SIZE);
	printf( "\"%s\" selected as root of tree...\n", DisplayFlatlandTreeDataMembers->RootName );
		}
		else
		{
		    // bad, no object found...
		}
	    }
	    else
	    {
		strncpy(DisplayFlatlandTreeDataMembers->RootName,"CenterOfTheUniverse",MAX_NAME_SIZE);
	    }		
	}
	break;
	
    }	    
}

// menu control callback 
void SelectHighlight(OBJECT_TYPE *DisplayFlatlandTreeObject, char key)
{
	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
	int i = ( key <= '9' && key >='0' ) ? key - '0' : key - 'a' + 10;
	
	DisplayFlatlandTreeDataMembers->HighlightDepth = i;
}

// menu control callback 
void SelectRoot(OBJECT_TYPE *DisplayFlatlandTreeObject, char key)
{
	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
	int i = key-' ';
	
	strncpy(DisplayFlatlandTreeDataMembers->RootName,DisplayFlatlandTreeObject->object_controls->controls[2]->children->controls[i]->menuname,MAX_NAME_SIZE);
}

void FreeControlSet(OBJ_CONTROLS_ARRAY_TYPE *cs);
void FreeControl(OBJ_CONTROLS_TYPE *c);

void FreeControl(OBJ_CONTROLS_TYPE *c)
{
	if (c)
	{
		FreeControlSet(c->children);
		free(c);
	}
}

void FreeControlSet(OBJ_CONTROLS_ARRAY_TYPE *cs)
{
	int i;

	if (cs)
	{
		for(i=0; i<cs->ncontrols; i++)
		{
			FreeControl(cs->controls[i]);
		}
		free(cs);
	}
}

void BuildMenu(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	int i;
	int n = DisplayFlatlandTreeObject->universe->nobjects;
	if (n>0)
	{
	
		if (!DisplayFlatlandTreeObject->object_controls->controls[2]->children)	
		{
			DisplayFlatlandTreeObject->object_controls->controls[2]->children = fl_makeControlsSet(n);
		}
	
		if (n!=DisplayFlatlandTreeObject->object_controls->controls[2]->children->ncontrols)
		{
			FreeControlSet(DisplayFlatlandTreeObject->object_controls->controls[2]->children);
			DisplayFlatlandTreeObject->object_controls->controls[2]->children = fl_makeControlsSet(n);
		}
	
		for(i=0; i<n; i++)
		{
			strcpy(DisplayFlatlandTreeObject->object_controls->controls[2]->children->controls[i]->menuname, DisplayFlatlandTreeObject->universe->database[i].name);
			strcpy(DisplayFlatlandTreeObject->object_controls->controls[2]->children->controls[i]->voicename, DisplayFlatlandTreeObject->universe->database[i].name);
			DisplayFlatlandTreeObject->object_controls->controls[2]->children->controls[i]->control_function = SelectRoot;
			DisplayFlatlandTreeObject->object_controls->controls[2]->children->controls[i]->keystroke = (char)(i+' ');
		}	
	}
}

// predraw callback 
void predrawfunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	BuildMenu(DisplayFlatlandTreeObject);
}

// animate callback 
void animatefunc(OBJECT_TYPE *DisplayFlatlandTreeObject)
{
	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) DisplayFlatlandTreeObject->ptr1;
	
	DisplayFlatlandTreeDataMembers->Color = ( DisplayFlatlandTreeDataMembers->Color + 1 ) % 5;
}

extern "C"
{

/* This is the well known initialization function. Everything comes from here */
void Flatland_init (APPTYPE *App)
{
	int i;
	float PTemp[9];						/* temporary transform storage */
	GLfloat DISPLAY_FLATLAND_TREE_SCALE = 1.0;			/* default head size (overriden from config) */
	GLfloat DISPLAY_FLATLAND_TREE_X = 0.0;				/* default head x location */
	GLfloat DISPLAY_FLATLAND_TREE_Y = 0.0;				/* default head y location (also set below) */
	GLfloat DISPLAY_FLATLAND_TREE_Z = 0.0;				/* default head z location */
	GLfloat DISPLAY_FLATLAND_TREE_ROLL = 0.0;			/* default head roll */
	GLfloat DISPLAY_FLATLAND_TREE_PITCH = 0.0;			/* default head pitch */
	GLfloat DISPLAY_FLATLAND_TREE_YAW = 0.0;				/* default head yaw */
	OBJECT_TYPE *DisplayFlatlandTree;					/* This object */
	DISPLAY_FLATLAND_TREE_DATA_MEMBERS *DisplayFlatlandTreeDataMembers;			/* pointer to "DisplayFlatlandTreeDataMembers" structure */
	char DISPLAY_FLATLAND_TREE_ROOT_NAME[MAX_NAME_SIZE] = {DEFAULT_ROOT_NAME};  
	BV_TYPE *bvol;                // bounding volume (application extents) 
	char *Name = App->appname;
	bool known;
	char ch;
	char buf[50];
	
	assert(App);
			 
	/* generate the "DisplayFlatlandTreeDataMembers" structure & attach it to the object */
	if ((DisplayFlatlandTreeDataMembers = (DISPLAY_FLATLAND_TREE_DATA_MEMBERS *) malloc (sizeof (DISPLAY_FLATLAND_TREE_DATA_MEMBERS))) == NULL)
	{
		fprintf(stderr, "%s: failed to initialize memory for DisplayFlatlandTreeDataMembers\n", Name);
		fflush(stderr);
		exit(1);
	}

	/* parse the parameter list */
	for (i=0; i < App->nparams; i++)
	{
		known = false;
		
		if (!strcmp (App->params[i][0], "x"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_X);
			known = true;
		}
		if (!strcmp (App->params[i][0], "y"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_Y);
			known = true;
		}
		if (!strcmp (App->params[i][0], "z"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_Z);
			known = true;
		}
		if (!strcmp (App->params[i][0], "pitch"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_PITCH);
			known = true;
		}
		if (!strcmp (App->params[i][0], "yaw"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_YAW);
			known = true;
		}
		if (!strcmp (App->params[i][0], "roll"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_ROLL);
			known = true;
		}
		if (!strcmp (App->params[i][0], "scale"))
		{
			sscanf(App->params[i][2], "%f", &DISPLAY_FLATLAND_TREE_SCALE);
			known = true;
		}
		if (!strcmp (App->params[i][0], "root"))
		{
			sscanf(App->params[i][2], "%s", DISPLAY_FLATLAND_TREE_ROOT_NAME);
			known = true;
		}

		if (!known)
		{
			fprintf(stderr, "%s: unknown parameter \"%s\"\n", Name, App->params[i][0]);
		}
	}

	// tiny hack...
	DISPLAY_FLATLAND_TREE_SCALE *= 2.0;

	DisplayFlatlandTreeDataMembers->HighlightDepth = 0;
	
	strncpy(DisplayFlatlandTreeDataMembers->RootName,DISPLAY_FLATLAND_TREE_ROOT_NAME,MAX_NAME_SIZE);

	DisplayFlatlandTreeDataMembers->Color = 0;
	DisplayFlatlandTreeDataMembers->Scale = DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTreeDataMembers->Position.x = DISPLAY_FLATLAND_TREE_X;
	DisplayFlatlandTreeDataMembers->Position.y = DISPLAY_FLATLAND_TREE_Y;
	DisplayFlatlandTreeDataMembers->Position.z = DISPLAY_FLATLAND_TREE_Z;
	DisplayFlatlandTreeDataMembers->Position.roll = DISPLAY_FLATLAND_TREE_ROLL;
	DisplayFlatlandTreeDataMembers->Position.pitch = DISPLAY_FLATLAND_TREE_PITCH;
	DisplayFlatlandTreeDataMembers->Position.yaw = DISPLAY_FLATLAND_TREE_YAW;

	// generate a bounding volume array   BV_TYPE *bvol;                // bounding volume (application extents) 
	bvol = fl_makebv ();

	// build a bounding volume (four corners and a center) 
	bvol->npoints = 5;
	bvol->p[0][0]=0.0;           // center point 
	bvol->p[0][1]=0.0;
	bvol->p[0][2]=0.0;
	bvol->p[1][0]=-10 * DISPLAY_FLATLAND_TREE_SCALE;    // -x, -y, -z corner 
	bvol->p[1][1]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[1][2]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[2][0]=10 * DISPLAY_FLATLAND_TREE_SCALE;    // +x, -y, -z corner 
	bvol->p[2][1]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[2][2]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[3][0]=-10 * DISPLAY_FLATLAND_TREE_SCALE;    // -x, +y, -z corner 
	bvol->p[3][1]=10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[3][2]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[4][0]=-10 * DISPLAY_FLATLAND_TREE_SCALE;    // -x, -y, +z corner 
	bvol->p[4][1]=-10 * DISPLAY_FLATLAND_TREE_SCALE;
	bvol->p[4][2]=10 * DISPLAY_FLATLAND_TREE_SCALE;
 

	DisplayFlatlandTree = fl_add_child_to_parent_by_name_via_platform(
								App->universe_ptr,
								App->parent_object_name,
								App->appname,
								bvol);

	DisplayFlatlandTree->ptr1 = (void *) DisplayFlatlandTreeDataMembers;	/* attach struct to object */

	/* set up callbacks */
	DisplayFlatlandTree->soundfunc = soundfunc;
	DisplayFlatlandTree->sounding_on_flag = 0;
	DisplayFlatlandTree->platformfunc = platformfunc;
	DisplayFlatlandTree->predrawfunc = predrawfunc;
	DisplayFlatlandTree->drawfunc = drawfunc;
	DisplayFlatlandTree->trueShadowFunc = trueShadowFunc;
	DisplayFlatlandTree->cheapShadowFunc = cheapShadowFunc;
	DisplayFlatlandTree->drawing_on_flag = 1 ;
	DisplayFlatlandTree->pregraphicsfunc = animatefunc;

	//// Bounding Volume Setup ////

	DisplayFlatlandTree->bounding_volume_set = fl_makebvset(3);

	/* define a bounding volume (four corners and a center) */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->npoints = 5;
	/* center point */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[0][0] = 0.0;           
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[0][1] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[0][2] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[0][3] = 1.0;
	/* -x, -y, -z corner */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[1][0] = -10 * DISPLAY_FLATLAND_TREE_SCALE;    
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[1][1] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[1][2] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[1][3] = 1.0;
	/* +x, -y, -z corner */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[2][0] = 10 * DISPLAY_FLATLAND_TREE_SCALE;    
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[2][1] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[2][2] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[2][3] = 1.0;
	/* -x, +y, -z corner */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[3][0] = -10 * DISPLAY_FLATLAND_TREE_SCALE;    
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[3][1] = 10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[3][2] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[3][3] = 1.0;
	/* -x, -y, +z corner */
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[4][0] = -10 * DISPLAY_FLATLAND_TREE_SCALE;    
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[4][1] = -10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[4][2] = 10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[0]->p[4][3] = 1.0;

#if 0
	/* Define bounding sphere for culling */
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->npoints = 2 ;
	/* Center point */
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[0][0] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[0][1] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[0][2] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[0][3] = 1.0;
	/* Surface point */
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[1][0] = 10 * DISPLAY_FLATLAND_TREE_SCALE;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[1][1] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[1][2] = 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[1]->p[1][3] = 1.0;
#endif
    
	/* Define a bounding point */
	DisplayFlatlandTree->bounding_volume_set->bvset[2]->npoints = 1 ;
	/* Center point */
	DisplayFlatlandTree->bounding_volume_set->bvset[2]->p[0][0]= 0.0 ;
	DisplayFlatlandTree->bounding_volume_set->bvset[2]->p[0][1]= 0.0 ;
	DisplayFlatlandTree->bounding_volume_set->bvset[2]->p[0][2]= 0.0;
	DisplayFlatlandTree->bounding_volume_set->bvset[2]->p[0][3]= 1.0 ;
  
	/* attach to the console in a default place */
	PTemp[0] = DISPLAY_FLATLAND_TREE_X;
	PTemp[1] = DISPLAY_FLATLAND_TREE_Y;
	PTemp[2] = DISPLAY_FLATLAND_TREE_Z;
	PTemp[3] = DISPLAY_FLATLAND_TREE_PITCH;
	PTemp[4] = DISPLAY_FLATLAND_TREE_YAW;
	PTemp[5] = DISPLAY_FLATLAND_TREE_ROLL;
	PTemp[6] = 1.0;
	PTemp[7] = 1.0;
	PTemp[8] = 1.0;
	DisplayFlatlandTree->orginal_first_node->MpoParams->Po[0] = TRANSLATE ;	
	DisplayFlatlandTree->orginal_first_node->MpoParams->Po[1] = ROTATE010 ;
	DisplayFlatlandTree->orginal_first_node->MpoParams->Po[2] = ROTATE100 ;
	DisplayFlatlandTree->orginal_first_node->MpoParams->Po[3] = ROTATE001 ;
	DisplayFlatlandTree->orginal_first_node->MpoParams->P->assign( PTemp );

	//// Menu Setup ////

	/* set up the control structure (three subcontrols under one) */
	/* allows control of the object from the keyboard, menues, and voice */
	DisplayFlatlandTree->object_controls = fl_makeControlsSet(4);

	/* Define spin/nospin controls for cube */
	strcpy (DisplayFlatlandTree->object_controls->controls[0]->menuname, "Select Previous Object in Database");
	strcpy (DisplayFlatlandTree->object_controls->controls[0]->voicename, "Select Previous Object in Database");
	DisplayFlatlandTree->object_controls->controls[0]->control_function = NextMenuFunc;
	DisplayFlatlandTree->object_controls->controls[0]->keystroke = 'n';

	/* Define spin/nospin controls for cube */
	strcpy (DisplayFlatlandTree->object_controls->controls[1]->menuname, "Select Next Object in Database");
	strcpy (DisplayFlatlandTree->object_controls->controls[1]->voicename, "Select Next Object in Database");
	DisplayFlatlandTree->object_controls->controls[1]->control_function = NextMenuFunc;
	DisplayFlatlandTree->object_controls->controls[1]->keystroke = 'N';

	/* Define spin/nospin controls for cube */
	strcpy (DisplayFlatlandTree->object_controls->controls[2]->menuname, "Root");
	strcpy (DisplayFlatlandTree->object_controls->controls[2]->voicename, "Root");
	DisplayFlatlandTree->object_controls->controls[2]->control_function = NULL;

	/* Define spin/nospin controls for cube */
	strcpy (DisplayFlatlandTree->object_controls->controls[3]->menuname, "Highlight");
	strcpy (DisplayFlatlandTree->object_controls->controls[3]->voicename, "Highlight");
	DisplayFlatlandTree->object_controls->controls[3]->control_function = NULL;

	DisplayFlatlandTree->object_controls->controls[3]->children = fl_makeControlsSet(20);

	for(i=0; i<20; i++)
	{
//		char ch = '0' + i;
//		char buf[] = { 'L', 'e', 'v', 'e', 'l', ' ', ch, '\0' };
		ch = ( i < 10 ) ? '0' + i : 'a' + i - 10;
		sprintf( buf, "Level %d", i );
		strcpy(DisplayFlatlandTree->object_controls->controls[3]->children->controls[i]->menuname, buf);
		strcpy(DisplayFlatlandTree->object_controls->controls[3]->children->controls[i]->voicename, buf);
		DisplayFlatlandTree->object_controls->controls[3]->children->controls[i]->control_function = SelectHighlight;
		DisplayFlatlandTree->object_controls->controls[3]->children->controls[i]->keystroke = ch;
	}
}

} // extern "C"
