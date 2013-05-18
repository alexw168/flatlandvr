/*
graph_funcs.c.  Things to do with the graph tree

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
 * 2-1-00: KLS, added fl_draw_all_shadows
 * 6-20-00: KLS, added fl_draw_all_bounding_spheres and fl_draw_all_bounding_cubes
 * 7-12-00: JAB, added fl_draw_all_bounding_planes
 * 10-17-00: TBE, added animatefunc to objects
 * 11-25-00: TBE, revised culling to use Loop_Control_Struct
 * 12-23-00: KLS, changed fl_draw_all_bounding_spheres and fl_draw_all_bounding_cubes
 *                to do hierarchical.
 * 12-27-00: KLS, Changed draw_all_bounding_* to mark selected bounding
 *                volumes in red.
 * 8-23-01: TBE, changed animatefunc to pregraphicsfunc
 *
 * 3-15-02: Moad Mowafi, added "wrappers" for collaboration
 * 1-??-03: LN, KLS, added shadow graphs.
 */

#include "gheader.h"

/* a general max and a general min functions */
float fmax (float arg1, float arg2)
{
  float retval;

  arg1 > arg2 ? retval = arg1 : retval = arg2;
  return (retval);
}

float fmin (float arg1, float arg2)
{
  float retval;

  arg1 < arg2 ? retval = arg1 : retval = arg2;
  return (retval);
}

double * fl_makevector(void) 
{
	double *v;
	
	v = (double *)malloc(4*sizeof(double));
	if( v==NULL ) {
		printf("fl_makevector- ERROR mallocing v.\n");
		exit(0);
	}
	return(v);
}

double *fl_makematrix(void) 
{
	double *m;
	
	m = (double *)malloc(16*sizeof(double));
	if( m==NULL ) {
		printf("fl_makematrix- ERROR mallocing m.\n");
		exit(0);
	}
	return(m);
}

OBJECT_TYPE *fl_makeobject(UNIVERSE_TYPE *w, char *name)
{
//	Loop_Control_Struct *lc = Loop_Control;

//	int oldwin = lc->Window;
//	Window_Control_Struct *wc; // = lc->WindowControl + win;
	OBJECT_TYPE *o;
	int i,database_insert_loc;

	if((int)strlen(name)>MAX_NAME_SIZE) {
		printf("fl_makeobject- ERROR name too long (>max chars).\n");
		exit(0);
	}	
	o = (OBJECT_TYPE *)calloc(1,sizeof(OBJECT_TYPE));
	if( o==NULL ) {
		printf("fl_makeobject- ERROR mallocing o.\n");
		exit(0);
	}

	o->object_alive_flag = (int *)malloc(sizeof(int));
	if( o->object_alive_flag==NULL ) {
		printf("fl_makeobject- ERROR mallocing o->object_alive_flag.\n");
		exit(0);
	}
	*(o->object_alive_flag) = 1; /* initialized to non negative, no other objects besides self using */

	strcpy(o->name,name);
        o->universe = w;

	/* poiner to the app that created this object */
	o->creator_app_ptr = NULL;
	
        /* point to the sim_time timer */
        o->sim_time_ptr = &sim_time;

        /* point to the Current_Time */
        o->current_time_ptr = &(Loop_Control->Current_Time);

	/* Increment Universe object count */
	w->nobjects++;
	
	/* Methods */
	o->transparentdrawfunc = NULL;
	o->preswapbufferfunc = NULL;
	o->pregraphicsfunc = NULL;
        o->predrawfunc = NULL;
        o->lightingfunc = NULL;
	o->drawfunc = NULL;
	o->soundfunc = NULL;
        o->trueShadowFunc = NULL;
        o->cheapShadowFunc = NULL;
        o->platformfunc = NULL;
	o->predeletefunc = NULL;
	o->glutMousefunc = NULL;
	o->glutMotionfunc = NULL;
	o->glutPassiveMotionfunc = NULL;
	o->glutEntryfunc = NULL;
	o->glutKeyboardfunc = NULL;
	o->glutKeyboardUpfunc = NULL;
	o->glutSpecialfunc = NULL;
	o->glutSpecialUpfunc = NULL;
        o->glutSpaceballMotionfunc = NULL;
        o->glutSpaceballRotatefunc = NULL;
        o->glutSpaceballButtonfunc = NULL;
        o->glutTabletMotionfunc = NULL;
        o->glutTabletButtonfunc = NULL;
	o->glutJoystickfunc = NULL;
	o->recievemessagefunc = NULL;
	
	/* Flags & states */
	o->drawing_on_flag = 0;
	o->drawing_on_old_flag = 0;
	o->sounding_on_flag = 0;
	o->viewable_flag = 1;
	o->beam_sphere_hit_flag = 0;
	o->beam_rect_hit_flag = 0;
	o->beam_on_flag = 0;
	o->beamable_flag = 0;
	o->object_state0 = 0;
	o->object_state1 = 0;
	o->object_state2 = 0;
	o->object_state3 = 0;
	o->object_state4 = 0;
	o->object_state5 = 0;
	o->object_state6 = 0;
	o->object_state7 = 0;

        /* shadow flags */
        for (i = 0; i < MAX_SHADOW_PLANES; i++)
          o->shadows_on_flag [i] = FALSE;

	/* Add this object to database */
	database_insert_loc = fl_database_find_empty_slot( w );
	strcpy(w->database[database_insert_loc].name,name);
	w->database[database_insert_loc].object = o;
	
	/* Initialize bounding volume set */
	o->bounding_volume_set = NULL;
	
        /* Initial control array */
        o->object_controls = NULL;

	/* Initialize graph parameters */
	o->nneighbors = 0;
	o->shadow_first_node = (GRAPH_NODE_TYPE**)malloc(sizeof(GRAPH_NODE_TYPE*)*w->num_shadows);
	for (int fn_i = 0; fn_i < w->num_shadows; fn_i++)
	  o->shadow_first_node[fn_i] = NULL;
	o->first_node = o->shadow_first_node [0];
	
	/* Allocate space for Mco */
	o->shadow_Mco = (float **)malloc(sizeof(float*)*(w->num_shadows));
	for (int shadow_i = 0; shadow_i < w->num_shadows; shadow_i++)
	  o->shadow_Mco[shadow_i] = (float*)malloc(sizeof(float)*16);
	o->Mco = o->shadow_Mco[0];

	//	o->Mco = (float *)malloc( 16 * sizeof(float) );
	//if( o->Mco == NULL ) {
	//	printf("fl_makeobject- ERROR mallocing o->Mco.\n");
	//		exit(0);
	//}
	
	/* set matrix to identity */
	for (int shadow2_i = 0; shadow2_i < w->num_shadows; shadow2_i++)
	  {
	    for(i=0; i<16; i++ ) {
	      o->shadow_Mco[shadow2_i][i] = 0.0;
	      if(i%5==0) {
		o->shadow_Mco[shadow2_i][i] = 1.0;
	      }	
	    }
	  }
		
	/* Generate control object menu entry for this object */
	//          glutSetMenu (selectCurrentObjMenu);
	//          glutAddMenuEntry(o->name, database_insert_loc);/

/***
	for( i=0; i<lc->NumberOfWindows; i++ )
	{
		wc = lc->WindowControl + i;
		lc->Window = i;
		glutSetWindow( wc->glutWindow );

		if ( wc->selectCurrentObjMenu )
		{
			glutDestroyMenu( wc->selectCurrentObjMenu );
			glutAddMenuEntry( o->name, database_insert_loc );
		}
	}
	wc = lc->WindowControl + oldwin;
	lc->Window = oldwin;
	glutSetWindow( wc->glutWindow );
***/

	/* set glut menu_entry_on_flag */
	o->menu_entry_on_flag = true;

	/* Initialize user mutexed buffer */
	o->object_mtbuf = NULL;

        /* set collaboration flags */	
	o->shared_flag = false;
	o->owner_flag = true;
	o->original_owner_flag = true;

	return( o );
}

void fl_delete_object_and_children_from_universe_by_name( UNIVERSE_TYPE *w, char *name ) 
{
	OBJECT_TYPE *o;

	if( name==NULL || w==NULL )
		return;

	o = fl_find_object_by_name( w, name);
	fl_delete_object_and_children_from_universe( w, o );
}

void fl_delete_object_and_children_from_universe( UNIVERSE_TYPE *w, OBJECT_TYPE *o ) 
{
	/* deletes an object from the graph, removes the name from the database and also 
	   deletes all of its children too. this assumes that the graph is in a standard tree configuration
	   in order for childern to be defined. tpc
	*/
	GRAPH_NODE_TYPE *current_node;
	OBJECT_TYPE *child;

	if( o==NULL || w==NULL )
		return;
	
	//printf("delete_object_from_universe- deleting object: %s\n",o->name);
	current_node = o->first_node->right_sibling_node;
	if( current_node!=NULL ) {
		while(o->nneighbors>1) {
			child = current_node->neighbor_node->self; /* pointer to a child objects */
			fl_delete_object_and_children_from_universe( w, child );
			current_node = current_node->right_sibling_node;
		}
	}
	fl_free_object_from_database( w, o );
	fl_free_object( o );
	//printf("delete_object_from_universe- deleted object: %s\n",o->name);
}	

int fl_delete_objects_by_app_ptr( UNIVERSE_TYPE *w, struct apptype *app ) 
{
        bool go_ahead = FALSE;
	int i;

        for( i=0; i<w->nobjects; i++ ) {
		if( w->database[i].object->creator_app_ptr != NULL &&
                    strcmp( app->appname, w->database[i].object->creator_app_ptr->appname)==0 ) {
                        go_ahead = TRUE;
                        break;
                }
        }
        
        if (!go_ahead)
                return (1);

        for( i=0; i<w->nobjects; i++ ) {
	        if( w->database[i].object->creator_app_ptr != NULL &&
                    strcmp( app->appname, w->database[i].object->creator_app_ptr->appname)==0 ) {
		        //printf("fl_delete_objects_by_app- Found object named: %s created by app: %s to be item #%d\n", 
			//        w->database[i].object->name,app->appname,i);
		        fl_delete_object_and_children_from_universe( w, w->database[i].object);
                        i--; /* must check this entry again, the db has changed */
	        }
        }

        return (0);
}

int fl_delete_objects_by_app_name( UNIVERSE_TYPE *w, char *appname )
{
	int i;

	printf("fl_delete_objects_by_app_name- entry.\n");
	for( i=0; i<loadableApps->napps; i++ ) {
		if( strcmp( loadableApps->applications[i]->appname, appname)==0 ) {
			printf("fl_delete_objects_by_app_name- found app named: %s\n",appname);
			return ( fl_delete_objects_by_app_ptr( w, loadableApps->applications[i] ) );
		}
	}
	printf("fl_delete_objects_by_app_name- App: %s not in loadableApps list.\n",appname);
        return (-1);
}

void fl_free_object( OBJECT_TYPE *o )
{
	/* frees the memory for an object and its Mpo type connecting it to its current parent,
	   and frees the first_node graph node only. tpc 
	*/
	MPO_TYPE *m;
	GRAPH_NODE_TYPE *parents_node, *left, *right;
	OBJECT_TYPE *parent;
        UNIVERSE_TYPE *w = o->universe;

	if( o==NULL )
		return;

        /* mark the object as dead */
        *(o->object_alive_flag) = -*(o->object_alive_flag) + 1;

        /* leave a little time for everybody to finish */
        /* TODO:  maybe a mutex instead? */
        fl_short_sleep (QUARTER_SECOND);

	/* clean up user defined memory and threads */
	if( o->predeletefunc!=NULL ) 
		(*o->predeletefunc)(o);
	
	/* remove the graph node in the parents node link list */
	parent = o->first_node->neighbor_node->self;
	parents_node = o->first_node->neighbor_node;
	left = parents_node->left_sibling_node;
	right = parents_node->right_sibling_node;
	left->right_sibling_node = right;
	right->left_sibling_node = left;
	free( parents_node );
	parent->nneighbors--;
	
	/* free the Mpo object */
	m = o->first_node->MpoParams;
	free(m->P );
	free(m->Pold);
	free(m->Po);
	//	free(m->Mpo);
	for (int shadow_Mpo_i = 0; shadow_Mpo_i < w->num_shadows; shadow_Mpo_i++)
	  free(m->shadow_Mpo[shadow_Mpo_i]);
	free(m->shadow_Mpo);

	free(m->shadow_xform_dir_flag);
	free(m->shadow_xform_dir_flag_old);
	
	free(m);

	//	free(o->Mco);
	for (int shadow_free_i = 0; shadow_free_i < w->num_shadows; shadow_free_i++)
	  free(o->shadow_Mco[shadow_free_i]);
	free(o->shadow_Mco);

	/* free the bounding volume set */
	fl_freebvset(o->bounding_volume_set);	
	
	/* Free the objects first node that points to its parent */
	//	free( o->first_node );
	for (int node_i = 0; node_i < w->num_shadows; node_i++)
	  free(o->shadow_first_node[node_i]);
	free(o->shadow_first_node);

	/* free controls , recursively, since they can have children */
	fl_freeControlSet( o->object_controls );

        /* check current object pointer */
        if (o == currentControlObject)
          currentControlObject = NULL;

	/* Free the object itself */
	free( o );	
	o = NULL;
}


bool fl_test_object_parenthood( OBJECT_TYPE *o, char *parentname )
{
  OBJECT_TYPE *p ;

  p = fl_get_objects_current_parent_ptr( o ) ;
  if( strcmp(p->name,parentname)==0 )
    return TRUE ;
  else
    return FALSE ;

}


OBJECT_TYPE *fl_get_shadow_objects_current_parent_ptr( OBJECT_TYPE *o, int shadow_number )
{
	if( o==NULL )
		return(NULL);
	return( o->shadow_first_node[shadow_number]->neighbor_node->self );
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
OBJECT_TYPE *fl_get_objects_current_parent_ptr( OBJECT_TYPE *o )
{
  return (fl_get_shadow_objects_current_parent_ptr( o, 0));
}

int *fl_get_object_alive_flag_ptr( UNIVERSE_TYPE *w, OBJECT_TYPE *o ) 
{
	if( o==NULL || w==NULL )
		return( NULL );

	if( o->object_alive_flag==NULL || *(o->object_alive_flag)<=0 )
		return NULL;   /* if the object has been deleted, do not allow any new objects to get this pointer */
	

	*(o->object_alive_flag) = *(o->object_alive_flag) + 1; /* if the object exists, this keeps track of the numer of apps that got this pointer */
	return( o->object_alive_flag ); 
}

int *fl_get_object_alive_flag_ptr_by_name( UNIVERSE_TYPE *w, char *name )
{
	OBJECT_TYPE *o;

/* Note: objects and apps should not get this pointer more that once without releasing it */

	if( name==NULL || w==NULL )
		return( NULL );

	o = fl_find_object_by_name( w, name);

	return( fl_get_object_alive_flag_ptr(w,o) );
}

int fl_does_object_still_exist( int *object_alive_flag_ptr )
{
	if( *(object_alive_flag_ptr)>0 )
		return TRUE;
	else
		return FALSE;
}

void fl_give_up_on_object_alive_flag_ptr( int *object_alive_flag_ptr ) 
{		
	if( *(object_alive_flag_ptr) > 1 ) {
		*(object_alive_flag_ptr) = *(object_alive_flag_ptr) - 1;  /* since it is positive, object still exists and this brings the count closer to zero */
	} else if( *(object_alive_flag_ptr) <= -1 ) {
		*(object_alive_flag_ptr) = *(object_alive_flag_ptr) + 1;  /* since it is negative, object is gone, this brings the count closer to zero */
	} else {
		//free( object_alive_flag_ptr ); /* a value of zero => no one is using it, and it may be freed */
	}
}

GRAPH_NODE_TYPE *fl_makegraphnode(OBJECT_TYPE *self) 
{ 
	GRAPH_NODE_TYPE *new_node, *temp;
	
	new_node = (GRAPH_NODE_TYPE *)malloc(sizeof(GRAPH_NODE_TYPE));
	if( new_node==NULL) {
		printf("fl_makegraphnode- ERROR mallocing new_node.\n");
		exit(0);
	}
	new_node->self= self; /* object pointer */
	new_node->neighbor_node = NULL; /* object pointer */
	new_node->right_sibling_node = new_node; /* link list pointers, set initialliy to self, circular list */
	new_node->left_sibling_node = new_node; /* link list pointers, set initialliy to self, circular list */
	new_node->MpoParams = NULL; /* MPO_TYPE pointer */
	
	if( self->first_node == NULL ) {                                 
	  /* first node, so leave links alone */
		self->shadow_first_node[0] = new_node;
	        self->first_node = self->shadow_first_node[0];
		self->orginal_first_node = new_node;
		self->nneighbors = 1;
	} else {   
		temp = self->first_node->right_sibling_node;
		self->first_node->right_sibling_node = new_node;
		new_node->right_sibling_node = temp;
		new_node->left_sibling_node = self->first_node;
		temp->left_sibling_node = new_node;
		self->nneighbors = self->nneighbors + 1;
	}
	return( new_node );
}

MPO_TYPE *fl_makempotype (UNIVERSE_TYPE *w,OBJECT_TYPE *original_child)
{
	MPO_TYPE *m;
	int i;
	
	m = (MPO_TYPE *)malloc(sizeof(MPO_TYPE) );
	if( m==NULL) {
		printf("fl_makempotype- ERROR mallocing m.\n");
		exit(0);
	}
	
	m->original_child = original_child;

	m->shadow_xform_dir_flag = (int*)calloc(w->num_shadows, sizeof(int));
	m->shadow_xform_dir_flag_old = (int*)malloc(sizeof(int)*w->num_shadows);
	for (int some_ungodly_variable = 0; some_ungodly_variable < w->num_shadows; some_ungodly_variable++)
	  {
	    m->shadow_xform_dir_flag_old[some_ungodly_variable] = 3;
	  }

	m->xform_dir_flag = 1;  /*  Positive is normal forward, neg is reverse */
	m->xform_dir_flag_old = 1;  /*  Saved for change detection */

	/* allocate space for xform parameters */
	m->P = new mt_buf( 9, sizeof(float) );
	//m->P = (float *)malloc( 9 * sizeof(float) );
	if( m->P==NULL ) {
		printf("fl_makempotype- ERROR mallocing P.\n");
		exit(0);
	}
	// for(i=0; i<9; i++ ) 
	//	m->P[i] = 0.0;

	/* allocate space for old xform parameters used for change detection */
	m->Pold = (float *)malloc( 9 * sizeof(float) );
	if( m->Pold==NULL ) {
		printf("fl_makempotype- ERROR mallocing Pold.\n");
		exit(0);
	}
	for(i=0; i<9; i++ ) 
		m->Pold[i] = 0.0;
		
	/* allocate space for xform codes */
	m->Po = (XFORM *)malloc(w->maxxformcodes*sizeof(int));
	if( m->Po==NULL ) {
		printf("fl_makempotype- ERROR mallocing m->Po.\n");
		exit(0);
	}
	for(i=0; i<w->maxxformcodes; i++ ) 
		m->Po[i] = NOOP; /* preload with Noops */
	
	/* allocate space for Mpo matrix */
	m->shadow_Mpo = (float**)malloc(sizeof(float*)*w->num_shadows);
	for (int mpo_i = 0; mpo_i < w->num_shadows; mpo_i++)
	  m->shadow_Mpo[mpo_i] = (float*)malloc(16*sizeof(float));
	m->Mpo = m->shadow_Mpo[0];

	//	m->Mpo = (float *)malloc( 16 * sizeof(float) );
	//if( m->Mpo == NULL ) {
	//	printf("fl_makempotype- ERROR mallocing m->Mpo.\n");
	//	exit(0);
	//}

	/* set matrices to identity */
	for(i=0; i<16; i++ ) {
		m->Mpo[i] = 0.0;
		if(i%5==0) {
			m->Mpo[i] = 1.0;
		}	
	}	
	return( m );
}

OBJECT_TYPE *fl_add_child_to_parent(UNIVERSE_TYPE *w, OBJECT_TYPE *parent, char *new_child_name ) 
{
//	int nchildren;
	OBJECT_TYPE *new_child/*, *current_child*/; 
	GRAPH_NODE_TYPE *new_child_node,*new_parent_node;
	MPO_TYPE *MpoParams;
	
	new_parent_node = fl_makegraphnode(parent); /* add node to parent object */
		
	if((int)strlen(new_child_name)>MAX_NAME_SIZE) {
		printf("add_child_to_object- ERROR name too long (>60 chars).\n");
		exit(0);
	}
		
	new_child = fl_makeobject(w,new_child_name);

        InsertElement(new_child, w->HashTable);
        if(w->HashTable->num_elements >= (w->HashTable->mod_val * .5))
          {
          w->HashTable = ResizeTable(w->HashTable, (w->HashTable->mod_val));
          }
	new_child_node = fl_makegraphnode(new_child); /* add first node to new child object */
	new_child_node->neighbor_node = new_parent_node;
	new_parent_node->neighbor_node = new_child_node; /* make nodes point to each's owners */
	
	MpoParams = fl_makempotype(w,new_child); /* make shared xform structure */
	new_child_node->MpoParams = MpoParams;
	new_parent_node->MpoParams = MpoParams; 
		
	return(new_child);
}

OBJECT_TYPE *fl_add_child_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name) 
{
	int i, database_parent_loc = -1;
	OBJECT_TYPE *parent, *child;
	
	//printf("fl_add_child_to_parent_by_name- Entry. parent: %s adding child: %s\n",parent_name,new_child_name);
	
	/* Using "maxobjects" results in finding multiple parents even there is only one. While the recent parent 
	 * is deleted from the graph, it's still in the database with NULL object's pointer -- Moad, 5/06/2002 */
	//for( i=0; i<w->maxobjects; i++ ) {
	for( i=0; i<w->nobjects; i++ ) {
		if( strcmp(w->database[i].name,parent_name) == 0 ) {
			if( database_parent_loc != -1) {
				printf("fl_add_child_to_parent_by_name- ERROR, multiple parents: %s found.\n",parent_name);
				printf("                           - Most recent used.\n");
			}
			database_parent_loc = i;
		}
	}
	if( database_parent_loc == -1) {
		printf("fl_add_child_to_parent_by_name- ERROR, parent: %s not found.\n",parent_name);
                return (NULL);
	}
	//printf("fl_add_child_to_parent_by_name- parent database index: %d\n",database_parent_loc);
	parent = w->database[database_parent_loc].object;
	child = fl_add_child_to_parent(w, parent, new_child_name);
	//printf("fl_add_child_to_parent_by_name- child: %s found parent: %s\n",child->name,parent->name);
	//printf("fl_add_child_to_parent_by_name- Exit.\n\n"); fflush(stdout);
	
	return( child );		
}

/* Create a platform, attach it to the parent, and return the platform */
OBJECT_TYPE *fl_add_platform_to_parent_by_name( UNIVERSE_TYPE *w, char *parent_name, char *platform_name, BV_TYPE *bvol)
{
  OBJECT_TYPE *Platform, *Parent;

  /* create a platform object */
  Platform = fl_add_child_to_parent_by_name(w, parent_name, platform_name);

  /* set up the bounding volumes (position & culling) */
  Platform->bounding_volume_set = fl_makebvset(2);

  /* Define bounding volume for object */
  Platform->bounding_volume_set->bvset[0]->npoints = 5;
  Platform->bounding_volume_set->bvset[0]->p[0][0]=bvol->p[0][0];
  Platform->bounding_volume_set->bvset[0]->p[0][1]=bvol->p[0][1];
  Platform->bounding_volume_set->bvset[0]->p[0][2]=bvol->p[0][2];
  Platform->bounding_volume_set->bvset[0]->p[1][0]=bvol->p[1][0];
  Platform->bounding_volume_set->bvset[0]->p[1][1]=bvol->p[1][1];
  Platform->bounding_volume_set->bvset[0]->p[1][2]=bvol->p[1][2];
  Platform->bounding_volume_set->bvset[0]->p[2][0]=bvol->p[2][0];
  Platform->bounding_volume_set->bvset[0]->p[2][1]=bvol->p[2][1];
  Platform->bounding_volume_set->bvset[0]->p[2][2]=bvol->p[2][2];
  Platform->bounding_volume_set->bvset[0]->p[3][0]=bvol->p[3][0];
  Platform->bounding_volume_set->bvset[0]->p[3][1]=bvol->p[3][1];
  Platform->bounding_volume_set->bvset[0]->p[3][2]=bvol->p[3][2];
  Platform->bounding_volume_set->bvset[0]->p[4][0]=bvol->p[4][0];
  Platform->bounding_volume_set->bvset[0]->p[4][1]=bvol->p[4][1];
  Platform->bounding_volume_set->bvset[0]->p[4][2]=bvol->p[4][2];
    
  /* Define culling sphere for object */
  /* NOTE: this culling sphere corresponds to the object, not the platform and
   * is therefore probably quite wrong.  The culling sphere for the platform 
   * should be set by the platform handler (or whoever does the platform 
   * draw) */
  Platform->bounding_volume_set->bvset[1]->npoints = 2;
  Platform->bounding_volume_set->bvset[1]->p[0][0]=bvol->p[0][0];
  Platform->bounding_volume_set->bvset[1]->p[0][1]=bvol->p[0][1];
  Platform->bounding_volume_set->bvset[1]->p[0][2]=bvol->p[0][2];
  Platform->bounding_volume_set->bvset[1]->p[1][0]=bvol->p[1][0];
  Platform->bounding_volume_set->bvset[1]->p[1][1]=bvol->p[1][1];
  Platform->bounding_volume_set->bvset[1]->p[1][2]=bvol->p[1][2];
    
  /* let the parent place the object (and coincidentally, let it know of the
   * object's existence) */
  Parent = fl_find_object_by_name( w, parent_name );
  if (Parent->platformfunc != NULL)
    (*Parent->platformfunc)(Platform);

  /* Return the requested object */
  return (Platform);
}

/* Add a draw function to an object.  Will *NOT* overwrite an existing ptr */
void add_drawfunc_to_object( UNIVERSE_TYPE *w, OBJECT_TYPE *o, void (*drawfunc)(object *))
{
  if (o->platformfunc != NULL)
  {
    o->drawfunc = drawfunc;
    o->drawing_on_flag = 1;
  }
}

/* add child to parent via a platform */
/* bvol [0] -> -x
        [1] -> -y
        [2] -> -z
        [3] -> +x
        [4] -> +y
        [5] -> +z
 */
OBJECT_TYPE *fl_add_child_to_parent_by_name_via_platform( UNIVERSE_TYPE *w, char *parent_name, char *new_child_name, BV_TYPE *bvol) 
{
  char platform_name [MAX_NAME_SIZE];
  OBJECT_TYPE *Platform, *Parent;

  /* create the platform object name */
  sprintf(platform_name, "%s-%s_platform", new_child_name, parent_name);

  /* create a platform object */
  Platform = fl_add_child_to_parent_by_name(w, parent_name, platform_name);

  /* set up the bounding volumes (position & culling) */
  Platform->bounding_volume_set = fl_makebvset(2);

  /* Define culling sphere for object */
  Platform->bounding_volume_set->bvset[1]->npoints = 2;
  Platform->bounding_volume_set->bvset[1]->p[0][0]=bvol->p[0][0];
  Platform->bounding_volume_set->bvset[1]->p[0][1]=bvol->p[0][1];
  Platform->bounding_volume_set->bvset[1]->p[0][2]=bvol->p[0][2];
  Platform->bounding_volume_set->bvset[1]->p[1][0]=bvol->p[1][0];
  Platform->bounding_volume_set->bvset[1]->p[1][1]=bvol->p[1][1];
  Platform->bounding_volume_set->bvset[1]->p[1][2]=bvol->p[1][2];
    
  /* Define bounding volume for object */
  Platform->bounding_volume_set->bvset[0]->npoints = 5;
  Platform->bounding_volume_set->bvset[0]->p[0][0]=bvol->p[0][0];
  Platform->bounding_volume_set->bvset[0]->p[0][1]=bvol->p[0][1];
  Platform->bounding_volume_set->bvset[0]->p[0][2]=bvol->p[0][2];
  Platform->bounding_volume_set->bvset[0]->p[1][0]=bvol->p[1][0];
  Platform->bounding_volume_set->bvset[0]->p[1][1]=bvol->p[1][1];
  Platform->bounding_volume_set->bvset[0]->p[1][2]=bvol->p[1][2];
  Platform->bounding_volume_set->bvset[0]->p[2][0]=bvol->p[2][0];
  Platform->bounding_volume_set->bvset[0]->p[2][1]=bvol->p[2][1];
  Platform->bounding_volume_set->bvset[0]->p[2][2]=bvol->p[2][2];
  Platform->bounding_volume_set->bvset[0]->p[3][0]=bvol->p[3][0];
  Platform->bounding_volume_set->bvset[0]->p[3][1]=bvol->p[3][1];
  Platform->bounding_volume_set->bvset[0]->p[3][2]=bvol->p[3][2];
  Platform->bounding_volume_set->bvset[0]->p[4][0]=bvol->p[4][0];
  Platform->bounding_volume_set->bvset[0]->p[4][1]=bvol->p[4][1];
  Platform->bounding_volume_set->bvset[0]->p[4][2]=bvol->p[4][2];
    
  /* let the parent place the object (and coincidentally, let it know of the
   * object's existence) */
  Parent = fl_find_object_by_name( w, parent_name );
  if (Parent->platformfunc != NULL)
    (*Parent->platformfunc)(Platform);

  /* Return the requested object */
  return (fl_add_child_to_parent(w, Platform, new_child_name));
}

BV_TYPE *fl_makebv(void) 
{
/* Makes a bounding volume structure, able to code four types of geometries:
	point, sphere, rectangular plane, and a rectalinear parallelapiped.
	The parameter npoints selescts the geometry to use. Need a minimum of
	a sphere for polygon culling. tpc */
	BV_TYPE *v;
	int i,j;
	int maxitems, maxpoints_bv ;

        // TODO: NOTE: this is a large number to cover up a mutex problem when
        // resizing the bv_intersection_database.
	// UPDATE - 06/19/03: going back to a small number and hoping there
	// isn't really a problem...
	maxitems = 2 ;
	maxpoints_bv = 5 ;
	
	v = (BV_TYPE *)malloc( sizeof(BV_TYPE) );
	if( v==NULL ) {
		printf("fl_makebv- ERROR mallocing v.\n");
		exit(0);
	}
	v->npoints = 0;
	v->beamable_flag= 0 ;
	v->children_beamable_flag = 0 ;
	v->children_bvset = NULL ;
  	v->bvbeamedfunc = NULL ;	
 	v->bvunbeamedfunc = NULL ;	
	v->bvcollisionfunc = NULL ;	
	v->bvuncollisionfunc = NULL ;	
	v->parent_object = NULL ;
        if (pthread_mutex_init (&v->database_mutex,NULL))
        {
          fprintf(stderr, "could not initalize mutex for bounding volume\n");
          exit(1);
        }
	v->database = fl_make_intersection_event_database( v, maxitems ) ;

	v->p = (float **)malloc( 5 * sizeof(float *) ); /* Max five defining points per bounding volume */
	if( v->p==NULL ) {
		printf("fl_makebv- ERROR mallocing v->p.\n");
		exit(0);
	}
	for( i=0 ; i<maxpoints_bv ; i++ ) {
		v->p[i] = (float *)malloc( 4 * sizeof(float) ) ;
		if( v->p[i]==NULL ) {
			printf("fl_makebv- ERROR mallocing v->p[%d].\n",i) ;
			exit(0) ;
		}
		for( j=0 ; j<3 ; j++ )
			v->p[i][j] = 0.0 ;
		v->p[i][3] = 1.0 ;
	}
	return( v );
}

BV_SET_TYPE *fl_makebvset( int nbvols ) 
{
/* makes a set of bounding volumes */
	BV_SET_TYPE *vs;
	int i;
	
        /* we need a *minimum* of two volumes */
        if( nbvols < 2 ) {
                printf("fl_makebvset- WARNING: attempted to allocate less than 2 volumes\n");
                return (NULL);
        }

	vs = (BV_SET_TYPE *)malloc( sizeof(BV_SET_TYPE) );
	if( vs==NULL ) {
		printf("fl_makebvset- ERROR mallocing vs.\n");
		exit(0);
	}
	vs->nbvols = nbvols;
	vs->bvset = (BV_TYPE **)malloc( nbvols * sizeof(BV_TYPE *) );
	if( vs->bvset==NULL ) {
		printf("fl_makebvset- ERROR mallocing vs->bvset.\n");
		exit(0);
	}
	for( i=0; i<vs->nbvols; i++ ) {
		vs->bvset[i] = fl_makebv();
	}
	return( vs );
}

void fl_freebvset( BV_SET_TYPE *bvs )
{
	int i,j;

	/* free the bounding volume set */
	for( i=0; i<bvs->nbvols; i++ ) {
		fl_free_intersection_db( bvs->bvset[i]->database ) ;
		for( j=0; j<bvs->bvset[i]->npoints; j++ ) 
			free(bvs->bvset[i]->p[j]);
		free(bvs->bvset[i]->p);
                pthread_mutex_destroy(&(bvs->bvset[i]->database_mutex));
		free(bvs->bvset[i]);
	}
	free(bvs->bvset);
	free(bvs);
}
	
OBJ_CONTROLS_TYPE *fl_makeControls(void) 
{
	OBJ_CONTROLS_TYPE *c;
	
	c = (OBJ_CONTROLS_TYPE *)malloc( sizeof(OBJ_CONTROLS_TYPE) );
	if( c==NULL ) {
		printf("fl_makeControls- ERROR mallocing c.\n");
		exit(0);
	}
        c->menuname [0] = '\0';
        c->voicename [0] = '\0';
        c->keystroke = '\0';
        c->mode = '\0';
        c->control_function = NULL;
        c->children = NULL;
	return( c );
}
	
OBJ_CONTROLS_ARRAY_TYPE *fl_makeControlsSet( int ncontrols ) 
{
/* makes a set of controls */
	OBJ_CONTROLS_ARRAY_TYPE *cs;
	int i;
	
	cs = (OBJ_CONTROLS_ARRAY_TYPE *)malloc( sizeof(OBJ_CONTROLS_ARRAY_TYPE) );
	if( cs==NULL ) {
		printf("makeControlSet- ERROR mallocing cs.\n");
		exit(0);
	}
	cs->ncontrols = ncontrols;
	cs->controls = (OBJ_CONTROLS_TYPE **)malloc( ncontrols *
                                                 sizeof(OBJ_CONTROLS_TYPE *) );
	if( cs->controls==NULL ) {
		printf("makeControlSet- ERROR mallocing cs->controls.\n");
		exit(0);
	}
	for( i=0; i<cs->ncontrols; i++ ) {
		cs->controls[i] = fl_makeControls();
	}
	return( cs );
}

void fl_freeControlSet( OBJ_CONTROLS_ARRAY_TYPE *cs )
{
	int i;
	OBJ_CONTROLS_TYPE *c;

	if( cs==NULL )
		return;

	for( i=0; i<cs->ncontrols; i++ ) {
		c = cs->controls[i];
		fl_freeControlSet( c->children );
		free( c );
	}
	free( cs->controls );
	free( cs );		
}

UNIVERSE_TYPE *fl_makeuniverse(char *name, int maxobjects, int maxxformcodes) 
{
	int i;
	UNIVERSE_TYPE *w;

	if((int)strlen(name)>MAX_NAME_SIZE) {
		printf("fl_makeuniverse- ERROR name too long (> max chars).\n");
		exit(0);
	}
	w = (UNIVERSE_TYPE *)malloc(sizeof(UNIVERSE_TYPE ));
	if( w==NULL ) {
		printf("fl_makeuniverse- ERROR mallocing w.\n");
		exit(0);
	}

        w->HashTable = CreateTable();
        w->num_shadows = 2;     // default number of shadow graphs
        w->shadow_root_object = (OBJECT_TYPE **) calloc (w->num_shadows, sizeof (OBJECT_TYPE *));
        w->shadow_list = (bool *) calloc (w->num_shadows, sizeof (bool));
        w->shadow_list[0] = true;
	strcpy(w->name,name);
	w->nobjects = 0;
	w->maxobjects = maxobjects;
	w->maxxformcodes = maxxformcodes;
		
	w->database = (DATABASE_TYPE *)malloc(maxobjects*sizeof(DATABASE_TYPE));
	if( w->database==NULL ) {
		printf("fl_makeuniverse- ERROR mallocing database.\n");
		exit(0);
	}
	
	for( i=0; i<maxobjects; i++ ) {
		strcpy(w->database[i].name,"");
		w->database[i].object = NULL;
	}
	
	return( w );
}

int fl_database_find_empty_slot( UNIVERSE_TYPE *w) 
{

/* Note that this function will resize the universe databas by factors of "resize_factor" when it reaches 
   its current size limit.  THis is the max number of objects in the unvierse.  tpc 9/19/2000
*/
	int i, newmaxobjects;
	int resize_factor = 2;
	DATABASE_TYPE *newbiggerdatabase;
	
	//printf("fl_database_find_empty_slot- Entry.\n");
	for( i=0; i<w->maxobjects; i++ ) {
		if( w->database[i].object == NULL ) {
			//printf("fl_database_find_empty_slot- Empty slot index: %d.\n",i);
			return( i );
		}
	}
	//printf("fl_database_find_empty_slot- database full for Universe: %s. Resizing...\n",w->name);

	/* this is where we need to do a resize of the database. tpc 8/24/00 */

	newmaxobjects = resize_factor * w->maxobjects;
	newbiggerdatabase = (DATABASE_TYPE *)malloc(newmaxobjects*sizeof(DATABASE_TYPE));
	if( newbiggerdatabase==NULL ) {
		printf("fl_makeuniverse- ERROR mallocing resized database for Universe: %s.\n",w->name);
		exit(0);
	}
	
	for( i=0; i<w->maxobjects; i++ ) {
		strcpy(newbiggerdatabase[i].name, w->database[i].name);
		newbiggerdatabase[i].object = w->database[i].object;
	}
	free( w->database );
	w->database = newbiggerdatabase;

	for( i=w->maxobjects; i<newmaxobjects; i++ ) {
		strcpy(w->database[i].name,"");
		w->database[i].object = NULL;
	} 

	//printf("fl_database_find_empty_slot- Done resizing database for Universe: %s.  New maxobjects: %d\n",w->name,newmaxobjects);

	i = w->maxobjects;
	w->maxobjects = newmaxobjects;
	return(i);
}


OBJECT_TYPE *fl_find_object_by_name( UNIVERSE_TYPE *w, char *name) 
{
//	int i, database_loc = -1;
//	
//	for( i=0; i<w->nobjects; i++ ) {
//		if( strcmp(w->database[i].name,name) == 0 ) {
//			if( database_loc != -1) {
//				printf("fl_find_object_by_name- ERROR, multiple objects: %s found.\n",name);
//				printf("                           - Most recent used.\n");
//			}
//			database_loc = i;
//		}
//	}
//	if( database_loc == -1) 
//		return NULL;
//
//	return(w->database[database_loc].object);
  return(FindElement(name, w->HashTable));
}
		
void fl_free_object_from_database( UNIVERSE_TYPE *w, OBJECT_TYPE *o )
{
	/* removes an object from the databse by shifting the list up one over the
	   object to be removed. tpc
	*/
	int i;
	int loc_flag = 0;

	if( o==NULL || w==NULL ) 
		return;
	
	//printf("fl_free_object_from_database- Entry.\n");
	
	for( i=0; i<w->nobjects; i++ ) {
		if( loc_flag>0 ) {
			w->database[i-1].object = w->database[i].object;
			strcpy(w->database[i-1].name, w->database[i].name);
		}
		if( strcmp(w->database[i].name,o->name) == 0 ) {
			loc_flag++;
			if( loc_flag>=2 ) {
				printf("fl_free_object_from_database- ERROR, multiply named objects in database. name: %s\n",o->name);
			}
		}
	}

  DeleteElement(o->name, w->HashTable);
	w->database[w->nobjects-1].object = NULL;
	w->nobjects--;
        w->database[w->nobjects].object = NULL;
}

void fl_print_database( UNIVERSE_TYPE *w )
{
	int i;

	printf("print_universe- nobjects: %d\n",w->nobjects);

	for( i=0; i<w->nobjects; i++ ) {
		printf(" i: %d name: %s object_alive_flag: %d\n",i,w->database[i].name, *(w->database[i].object->object_alive_flag));
	}
	printf("print_universe- Exit.\n");
}

	
void fl_save_current_GL_modelview_matrix( float *m )
{
	glGetFloatv(GL_MODELVIEW_MATRIX,m);
}

void fl_load_GL_modelview_matrix( float *m )
{
	glLoadMatrixf( m );
}

void fl_shadow_xform_update( UNIVERSE_TYPE *w, int shadow_number ) 
{	
	fl_shadow_xform_update_recursive( w, w->shadow_root_object [shadow_number], shadow_number );
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_xform_update( UNIVERSE_TYPE *w ) 
{
        fl_shadow_xform_update( w, 0 );
}

void fl_shadow_xform_update_recursive(  UNIVERSE_TYPE *w, OBJECT_TYPE *current_object, int shadow_number ) 
{
	OBJECT_TYPE *current_neighbor;
	GRAPH_NODE_TYPE *current_node/*, *neighbor_node*/;
	int nneighbors, count;;
	
	//printf("\nfl_xform_update_recursive- entry with object: %s\n",current_object->name);
	/* If NULL => dead end, go back up the tree one level */
	if(current_object==NULL ) {
		/* printf("fl_xform_update_recursive- empty object.\n"); */
		return;
	}

	// if first_node is NULL, bail
	if( !current_object->shadow_first_node [shadow_number] ) 
	{
		return;
	}
	
	nneighbors = current_object->nneighbors;
	count=1;
		
	/* If not NULL, update its xforms here */
	/* Note that current_object->first_node->neighbor is the current object's parent */
	if( strcmp(w->shadow_root_object[shadow_number]->name,current_object->name)!=0 && current_object->shadow_first_node[shadow_number]!=NULL) {
                fl_4x4_matrix_multiply (
                                        current_object->shadow_first_node[shadow_number]->neighbor_node->self->shadow_Mco[shadow_number],
                                        current_object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo[shadow_number],
                                        current_object->shadow_Mco[shadow_number]);
// KLS
//		fl_load_GL_modelview_matrix( current_object->shadow_first_node[shadow_number]->neighbor_node->self->shadow_Mco[shadow_number] );
// KLS
//		glMultMatrixf( current_object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo[shadow_number] );

// KLS
//		fl_save_current_GL_modelview_matrix( current_object->shadow_Mco[shadow_number] );
		//printf("  fl_xform_update_recursive- updating entry object, before working on children.\n");
		count = 2;
	}
	
	/* Next, operate on all of its children preorder traversal */
	/* starting with next node in the list */
	current_node = current_object->shadow_first_node[shadow_number]->right_sibling_node;
	//printf("fl_xform_update_recursive- before looking at children, current_object: %s, node pointing to object: %s\n",
		//current_object->name, current_node->neighbor_node->self->name);

	current_neighbor = current_node->neighbor_node->self;;
	while(count<=nneighbors) {
		/* Next recurse to this child's children */
		//printf("  fl_xform_update_recursive- from parent object: %s recursing down to child: %s\n",current_object->name,current_neighbor->name);		
		fl_shadow_xform_update_recursive( w, current_neighbor, shadow_number );
		//printf("  fl_xform_update_recursive- returning to parent object: %s from child: %s\n",current_object->name,current_neighbor->name);
		current_node = current_node->right_sibling_node;
		current_neighbor = current_node->neighbor_node->self;;
		count++;
	}
	//printf("fl_xform_update_recursive- exiting with object: %s\n",current_object->name); fflush(stdout);

	return;
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_xform_update_recursive(  UNIVERSE_TYPE *w, OBJECT_TYPE *current_object ) 
{
        fl_shadow_xform_update_recursive( w, current_object, 0 );
}

void fl_update_shadow_Mpo_all_objects(UNIVERSE_TYPE *w, int shadow_number)
{
	OBJECT_TYPE *object;
	MPO_TYPE *MPO;
	int i,j, k;
	int maxxformcodes;
	XFORM *Po;
	int code;
        bool changed_flag;
	float P[9];

	maxxformcodes = w->maxxformcodes;
	//printf("fl_update_shadow_Mpo_all_objects- Entry.\n");

	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL ) 
		{    
//			if ( !object->shadow_first_node[shadow_number] )
//			{
//				fprintf( stdout, "%s->shadow_first_node[shadow_number] = NULL\n", object->name );
//				fflush( stdout );
//				exit(-1);
//			}
			if ( object->shadow_first_node[shadow_number])
			{
				MPO = object->shadow_first_node[shadow_number]->MpoParams;
				MPO->P->read( P );
				Po = MPO->Po;

				/* Test if Mpo parameters have changed */
//				for( k=0; k<9; k++ ) 
//				{
//					if ( P[k] != MPO->Pold[k] ) 
//					{
//						changed_flag = true;
//						break;
//					}
//				}
				changed_flag = MPO->P->is_changed(shadow_number);

				/* Test if xform direction has changed */
				if ( MPO->shadow_xform_dir_flag[shadow_number] != MPO->shadow_xform_dir_flag_old[shadow_number])
				{
					changed_flag = true;
				}
				
				if ( changed_flag==true && strcmp( object->name, w->shadow_root_object[shadow_number]->name ) != 0 ) 
				{
					//printf("- Working on object: %s\n",object->name);
// KLS
					fl_loadIdentity(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number]);
//					glLoadIdentity();
					if( Po[0]!=NOOP)  /* Don't do anything if the first code is 5, assume Mpo = I */
					{
						if ( MPO->shadow_xform_dir_flag[shadow_number] == 1 ) 
						{
							//printf("  - Forward xform.\n");
							for( j=0; j<maxxformcodes; j++ ) 
							{
								code = Po[j];
								switch(code) 
								{
								case NOOP:			break;
// KLS
								case TRANSLATE :
                                                                        fl_translatef(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],P[0],P[1],P[2]);
                                                                        break;
								case ROTATE100 :
								        fl_rotatef100(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],P[3]);
								        break;
								case ROTATE010 :
								        fl_rotatef010(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],P[4]);
								        break;
								case ROTATE001 :
								        fl_rotatef001(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],P[5]);
								        break;
								case SCALE :
								        fl_scalef(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],P[6],P[7],P[8]);
								        break;
//								case TRANSLATE :	glTranslatef(P[0],P[1],P[2]);		break;
//								case ROTATE100 : 	glRotatef(P[3],1.0,0.0,0.0); 		break;
//								case ROTATE010 : 	glRotatef(P[4],0.0,1.0,0.0); 		break;
//								case ROTATE001 : 	glRotatef(P[5],0.0,0.0,1.0); 		break;
//								case SCALE : 		glScalef(P[6],P[7],P[8]); 		break;
								} 
							} 
						} 
						else
						{ 
							if ( MPO->shadow_xform_dir_flag[shadow_number] == -1 ) 
							{
								//printf("  - Inverse xform.\n");
								for( j=maxxformcodes-1; j>=0; j-- ) 
								{
									code = Po[j];
									switch(code) 
									{
									case NOOP:			break;
// KLS
								case TRANSLATE :
                                                                        fl_translatef(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],-P[0],-P[1],-P[2]);
                                                                        break;
								case ROTATE100 :
                                                                        fl_rotatef100(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],-P[3]);
                                                                        break;
								case ROTATE010 :
								        fl_rotatef010(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],-P[4]);
								        break;
								case ROTATE001 :
								        fl_rotatef001(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],-P[5]);
								        break;
								case SCALE :
								        fl_scalef(object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number],1.0/P[6],1.0/P[7],1.0/P[8]);
								        break;
//									case TRANSLATE :	glTranslatef(-P[0],-P[1],-P[2]);		break;
//									case ROTATE100 : 	glRotatef(-P[3],1.0,0.0,0.0); 			break;
//									case ROTATE010 : 	glRotatef(-P[4],0.0,1.0,0.0); 			break;
//									case ROTATE001 : 	glRotatef(-P[5],0.0,0.0,1.0); 			break;
//									case SCALE : 		glScalef(1.0/P[6],1.0/P[7],1.0/P[8]); 	break;
									} 
								}
							}
						}
					} 
// KLS
//					fl_save_current_GL_modelview_matrix( object->shadow_first_node[shadow_number]->MpoParams->shadow_Mpo [shadow_number] );
				}
				/* Save old values for testing change next cycle */
				for( k=0; k<9; k++ ) 
				{
                                // ***** Here we changed Po to P here, since it just looked wrong!
					MPO->Pold[k] = P[k];
				}
				MPO->shadow_xform_dir_flag_old[shadow_number] = MPO->shadow_xform_dir_flag[shadow_number];
			}
		}
	}
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_update_Mpo_all_objects( UNIVERSE_TYPE *w )
{
  fl_update_shadow_Mpo_all_objects(w, 0);
}

void fl_preswapbuffer_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
    	for( i=0; i<w->nobjects; i++ ) {
    		object = w->database[i].object;
    		if( object!=NULL && object->preswapbufferfunc!=NULL) {
    			(*object->preswapbufferfunc)(object);
    		}
    	}
}

void fl_pregraphics_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
    	for( i=0; i<w->nobjects; i++ ) {
    		object = w->database[i].object;
    		if( object!=NULL && object->pregraphicsfunc!=NULL) {
    			(*object->pregraphicsfunc)(object);
    		}
    	}
}

void fl_pre_draw_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
    	for( i=0; i<w->nobjects; i++ ) {
    		object = w->database[i].object;
    		if( object!=NULL && object->predrawfunc!=NULL) {
    			(*object->predrawfunc)(object);
    		}
    	}
}

void fl_light_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
    	for( i=0; i<w->nobjects; i++ ) {
    		object = w->database[i].object;
    		if( object!=NULL && object->lightingfunc!=NULL) {
			fl_load_GL_modelview_matrix( object->shadow_Mco [0] );
    			(*object->lightingfunc)(object);
    		}
    	}
}

void fl_draw_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if( object!=NULL && object->drawfunc!=NULL ) 
		{
			object->viewable_flag = fl_test_object_for_culling( object );
 			if( object->drawing_on_flag && object->viewable_flag ) 
//			if( object->drawing_on_flag ) 
 			{
				fl_load_GL_modelview_matrix( object->shadow_Mco [0] );
				(*object->drawfunc)(object);
			}
		}
	}
}

void transparent_fl_draw_all_objects( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if( object!=NULL && object->transparentdrawfunc!=NULL ) 
		{
			object->viewable_flag = fl_test_object_for_culling( object );
 			if( object->drawing_on_flag && object->viewable_flag ) 
 			{
				fl_load_GL_modelview_matrix( object->Mco );
				(*object->transparentdrawfunc)(object);
			}
		}
	}
}

void draw_bounding_sphere_and_recurse( BV_TYPE *bv )
{
        bool beamed;
        int i;
        float sc[4], ss[4], radius;

        /* draw */
//        if (bv->npoints > 1)
        if (bv->npoints == 2)
        {
          sc[0] = bv->p[0][0];
          sc[1] = bv->p[0][1];
          sc[2] = bv->p[0][2];
          sc[3] = bv->p[0][3];
          ss[0] = bv->p[1][0];
          ss[1] = bv->p[1][1];
          ss[2] = bv->p[1][2];
          ss[3] = bv->p[1][3];
          radius = sqrt( (sc[0] - ss[0])*(sc[0] -ss[0]) +
                         (sc[1] - ss[1])*(sc[1] -ss[1]) +
                         (sc[2] - ss[2])*(sc[2] -ss[2]) );
          glPushMatrix();
          glDisable(GL_LIGHTING);

          /* NOTE: the following code is the same as is_bv_beamed () in the
           * BasicIntersection service, but is reproduced here since that
           * service might or might not be loaded */
          beamed = false;
          pthread_mutex_lock (&(bv->database_mutex));
          for (i = 0; i < bv->database->numitems; i++)
            if (bv->database->interactions[i]->beam_state == BEAMED)
               beamed = true;
          pthread_mutex_unlock (&(bv->database_mutex));
          /* end is_bv_beamed () code */

          if (beamed)
            glColor3f(1.0,0.0,0.0);
          else
            glColor3f(1.0,1.0,1.0);
          glTranslatef (sc[0],sc[1],sc[2]);
          glutWireSphere( radius, 10, 10 );
          glEnable(GL_LIGHTING);
          glPopMatrix();
        }

        /* recurse */
        if (bv->children_bvset != NULL)
        {
          for (i = 0; i < bv->children_bvset->nbvols; i++)
          {
            draw_bounding_sphere_and_recurse( bv->children_bvset->bvset[i] );
          }
        }
}

void fl_draw_all_bounding_spheres( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
	int i;
        int j;
	
    	for( i=0; i<w->nobjects; i++ ) {
    		object = w->database[i].object;
    		if( object!=NULL && object->drawfunc!=NULL) {
			if( object->drawing_on_flag==1 &&
                            object->viewable_flag==1 &&
                            object->bounding_volume_set != NULL)
                        {
    				fl_load_GL_modelview_matrix( object->Mco );
                                for( j=0; j<object->bounding_volume_set->nbvols; j++ )
                                {
                                  draw_bounding_sphere_and_recurse( object->bounding_volume_set->bvset[j] );
                                }
    			}
    		}
    	}
}

void draw_bounding_cube_and_recurse (BV_TYPE *bv)
{
        bool beamed;
        int i;
        float top, bottom, right, left, front, back;

        //look for bvset(5): the bounding cube.  If found, draw it.
        if (bv->npoints == 5)
        {
          /* figure out each of the planes of the cube */
          top = fmax (bv->p[1][1],
                      bv->p[2][1]);
          top = fmax (top, bv->p[3][1]);
          top = fmax (top, bv->p[4][1]);

          bottom = fmin (bv->p[1][1], bv->p[2][1]);
          bottom = fmin (bottom, bv->p[3][1]);
          bottom = fmin (bottom, bv->p[4][1]);

          right = fmax (bv->p[1][0], bv->p[2][0]);
          right = fmax (right, bv->p[3][0]);
          right = fmax (right, bv->p[4][0]);

          left = fmin (bv->p[1][0], bv->p[2][0]);
          left = fmin (left, bv->p[3][0]);
          left = fmin (left, bv->p[4][0]);

          front = fmax (bv->p[1][2], bv->p[2][2]);
          front = fmax (front, bv->p[3][2]);
          front = fmax (front, bv->p[4][2]);

          back = fmin (bv->p[1][2], bv->p[2][2]);
          back = fmin (back, bv->p[3][2]);
          back = fmin (back, bv->p[4][2]);

          /* draw the cube */
          glPushMatrix();
          glDisable(GL_LIGHTING);

          /* NOTE: the following code is the same as is_bv_beamed () in the
           * BasicIntersection service, but is reproduced here since that
           * service might or might not be loaded */
          beamed = false;
          pthread_mutex_lock (&(bv->database_mutex));
          for (i = 0; i < bv->database->numitems; i++)
            if (bv->database->interactions[i]->beam_state == BEAMED)
               beamed = true;
          pthread_mutex_unlock (&(bv->database_mutex));
          /* end is_bv_beamed () code */

          if (beamed)
            glColor3f(1.0,0.0,0.0);
          else
            glColor3f(1.0,1.0,1.0);
          glLineWidth(2.0);
          glBegin(GL_LINES);
            glVertex3f(left, bottom, back);
            glVertex3f(right, bottom, back);
  
            glVertex3f(right, bottom, back);
            glVertex3f(right, top, back);
  
            glVertex3f(right, top, back);
            glVertex3f(right, top, front);
  
            glVertex3f(right, top, front);
            glVertex3f(left, top, front);
  
            glVertex3f(left, top, front);
            glVertex3f(left, bottom, front);
  
            glVertex3f(left, bottom, front);
            glVertex3f(left, bottom, back);
  
            glVertex3f(left, bottom, back);
            glVertex3f(left, top, back);
  
            glVertex3f(left, top, back);
            glVertex3f(left, top, front);
  
            glVertex3f(left, top, back);
            glVertex3f(right, top, back);
  
            glVertex3f(right, bottom, front);
            glVertex3f(left, bottom, front);
  
            glVertex3f(right, bottom, front);
            glVertex3f(right, top, front);
  
            glVertex3f(right, bottom, front);
            glVertex3f(right, bottom, back);
          glEnd();
          glEnable(GL_LIGHTING);
          glPopMatrix();
        }

        /* recurse */
        if (bv->children_bvset != NULL)
          for (i = 0; i < bv->children_bvset->nbvols; i++)
            draw_bounding_cube_and_recurse( bv->children_bvset->bvset[i] );
}

void fl_draw_all_bounding_cubes( UNIVERSE_TYPE *w )
{
  OBJECT_TYPE *object;
  int i, j;
	
  //for all objects
  for( i=0; i<w->nobjects; i++ ) 
  {
    object = w->database[i].object;
    //that aren't null and have drawfuncs
    if( object!=NULL && object->drawfunc!=NULL) 
    {
    	//and are in view and have bounding volumes
      if( object->drawing_on_flag==1 &&
          object->viewable_flag==1 &&
          object->bounding_volume_set != NULL)
      {
      	//for all bounding volumes (of each object)
        for ( j = 0; j < object->bounding_volume_set->nbvols; j++)
        {
          fl_load_GL_modelview_matrix( object->Mco );
          draw_bounding_cube_and_recurse(object->bounding_volume_set->bvset[j]);
        }
      }
    }
  }
}

void fl_draw_all_bounding_planes( UNIVERSE_TYPE *w )
{
	OBJECT_TYPE *object;
        BV_TYPE *bv;
        bool beamed;
	int i, j;
	float *bottom_left;
	float *bottom_right; 
	float *top_left; 
	float top_right[3];
		
	//for all objects
	for( i = 0; i < w->nobjects; i++ )
	{
		object = w->database[i].object;
		
		// that arent null and have drawfuncs
		if( object!=NULL && object->drawfunc!=NULL)
		{
			//and are in view and have bounding volumes
			if( object->drawing_on_flag==1 &&
				object->viewable_flag==1 &&
				object->bounding_volume_set != NULL)
			{
				//for all bounding volumes (of each object)
				for ( j=0; j<object->bounding_volume_set->nbvols; j++)
				{
					//look for bvset(4): the bounding plane
					if (object->bounding_volume_set->bvset[j]->npoints == 4)
					{
						bottom_left = object->bounding_volume_set->bvset[j]->p[1];
						bottom_right = object->bounding_volume_set->bvset[j]->p[3];
						top_left = object->bounding_volume_set->bvset[j]->p[2];
						
						//det x coord of 4th point
						top_right[0] = fmax (bottom_right[0],bottom_left[0]);
						top_right[0] = fmax (top_right[0],top_left[0]);
						//det y coord of 4th point
						top_right[1] = fmax (bottom_right[1],bottom_left[1]);
						top_right[1] = fmax (top_right[1],top_left[1]);
						//det z coord of fourth point
						top_right[2] = fmax (bottom_right[2],bottom_left[2]);
						top_right[2] = fmax (top_right[2],top_left[2]);
							
            			//draw the plane
						fl_load_GL_modelview_matrix( object->Mco );
						glPushMatrix();
						glDisable( GL_LIGHTING );
						glColor4f( 1.0, 1.0, 1.0, 0.2 );

                                                bv = object->bounding_volume_set->bvset[j];
                                               /* NOTE: the following code is the same as is_bv_beamed () in the
                                                * BasicIntersection service, but is reproduced here since that
                                                * service might or might not be loaded */
                                                beamed = false;
                                                pthread_mutex_lock (&(bv->database_mutex));
                                                for (i = 0; i < bv->database->numitems; i++)
                                                  if (bv->database->interactions[i]->beam_state == BEAMED)
                                                     beamed = true;
                                                pthread_mutex_unlock (&(bv->database_mutex));
                                                /* end is_bv_beamed () code */

                                                if (beamed)
                                                  glColor4f(1.0,0.0,0.0,0.2);
                                                else
                                                  glColor4f(1.0,1.0,1.0,0.2);
						glBegin( GL_QUADS );
							glVertex3fv(bottom_left);
							glVertex3fv(bottom_right);
							glVertex3fv(top_right);
							glVertex3fv(top_left);
						
						glEnd();
						glEnable(GL_LIGHTING);
						glPopMatrix();
					}//enf if
				}//end for
			}// end if
		}//end if
	}//end for
}//end func



void fl_draw_all_shadows( UNIVERSE_TYPE *w )
{
  GLfloat groundPlane [4],          /* temp ground plane */
          lightInEyeCoords [4],     /* light source converted to eye coords */
          planeInEyeCoords [4] [4]; /* shadow plane converted to eye coords */
  OBJECT_TYPE *robject, *cobject;

  glPushMatrix();

  /* Loop through the objects requiring shadows cast apon them */
  for (int i=0; i < w->nobjects; i++)
  {
    robject = w->database[i].object;
    if (robject != NULL)
    {
      fl_load_GL_modelview_matrix( robject->Mco );
      /* Loop through the number of shadow planes used by each */
      for (int plane_nbr=0; plane_nbr < MAX_SHADOW_PLANES; plane_nbr++)
      {
        /* if the shadows are on for this plane... */
        if (robject->shadows_on_flag[plane_nbr])
        {
          /* clear the stencil buffer */
          glClear(GL_STENCIL_BUFFER_BIT);

          /* calculate the shadow projection matrix in eye coords */
          /* convert the light & plane point coordinates to eye coords */
          fl_tpc_matrix_multiply (robject->Mco,
                               robject->shadowLightSrc [plane_nbr],
                               lightInEyeCoords);
          fl_tpc_matrix_multiply (robject->Mco,
                               robject->shadowPlanePts [plane_nbr] [0],
                               planeInEyeCoords [0]);
          fl_tpc_matrix_multiply (robject->Mco,
                               robject->shadowPlanePts [plane_nbr] [1],
                               planeInEyeCoords [1]);
          fl_tpc_matrix_multiply (robject->Mco,
                               robject->shadowPlanePts [plane_nbr] [2],
                               planeInEyeCoords [2]);

          /* calculate a new shadow projection matrix */
          fl_findPlane (groundPlane,
              planeInEyeCoords [0] [0], 
              planeInEyeCoords [0] [1],
              planeInEyeCoords [0] [2],
              planeInEyeCoords [1] [0], 
              planeInEyeCoords [1] [1],
              planeInEyeCoords [1] [2],
              planeInEyeCoords [2] [0], 
              planeInEyeCoords [2] [1],
              planeInEyeCoords [2] [2]);
          fl_shadowMatrix (robject->shadowMat [plane_nbr], groundPlane,
                        lightInEyeCoords);

          /* initialize shadowing */
          fl_shadow_setup ();

double plane[4] = { groundPlane[0], groundPlane[1], groundPlane[2], groundPlane[3] };
glClipPlane( GL_CLIP_PLANE0, plane );
glEnable( GL_CLIP_PLANE0 );

          for (int k=0; k < w->nobjects; k++)
          {
            cobject = w->database[k].object;
            if (cobject != NULL && cobject->drawfunc != NULL)
            {
              /* TODO: add shadow culling.  This will require another culling
               * sphere (or circle or something), and a check right here.  
               * For now shadows are always drawn. */

              /* Load shadow projection matrix & mult by object matrix */
              fl_load_GL_modelview_matrix 
                  ((GLfloat *)robject->shadowMat[plane_nbr]);
              glMultMatrixf ((GLfloat *) cobject->Mco);

              /* Draw appropriate shadow */
              if ( Loop_Control->showTrueShadows && cobject->trueShadowFunc != NULL )
			  {
                cobject->trueShadowFunc (cobject);
			  }
              else 
			  {
			  	if (cobject->cheapShadowFunc != NULL)
				{
                	cobject->cheapShadowFunc (cobject);
				}
			  }
            }
          }

glDisable( GL_CLIP_PLANE0 );

          /* cleanup shadowing */
          fl_shadow_cleanup ();

          /* draw the shadow that's been stored in the stencil buffer */
          fl_load_GL_modelview_matrix (robject->Mco);
          fl_draw_shadow_from_stencil (robject, plane_nbr);
        }
      }
    }
  }
  glPopMatrix();
}

int fl_test_object_for_culling( OBJECT_TYPE *o )
{
// Assume graph is structured for main camera (eye) tree and that
//	at least a sphere has been defined for the object 
//
// actually culling uses the second bounding volume, if it is a sphere.
	
	int win = Loop_Control->Window;
	Window_Control_Struct *wc = Loop_Control->WindowControl + win;
	int vp = Loop_Control->Viewport;
	Frustrum_Struct *fr = wc->Frustrum + vp;
	
	float sc[4], ss[4], r;
	float xfrustrumedge,yfrustrumedge,value;
	float frustrum_xRfactorpos;
	float frustrum_xRfactorneg;
	float frustrum_yRfactorpos;
	float frustrum_yRfactorneg;

	if(	o->bounding_volume_set->bvset[1]->npoints < 2 ) 
	{	
		return( 1 );
	}
	
	fl_tpc_matrix_multiply( o->Mco, o->bounding_volume_set->bvset[1]->p[0], sc );
	fl_tpc_matrix_multiply( o->Mco, o->bounding_volume_set->bvset[1]->p[1], ss );
	
	r = sqrt( (sc[0]-ss[0])*(sc[0]-ss[0]) + (sc[1]-ss[1])*(sc[1]-ss[1]) + (sc[2]-ss[2])*(sc[2]-ss[2]) );

	// cull is in positive z space 
	if( (sc[2]-r) >= 0.0 ) 
	{
		return( 0 );
	}	
	
	frustrum_xRfactorpos = sqrt( fr->Near * fr->Near + fr->Right * fr->Right ) / fr->Near;
	frustrum_xRfactorneg = sqrt( fr->Near * fr->Near + fr->Left * fr->Left ) / fr->Near;
	frustrum_yRfactorpos = sqrt( fr->Near * fr->Near + fr->Top * fr->Top ) / fr->Near;	
	frustrum_yRfactorneg = sqrt( fr->Near * fr->Near + fr->Bottom * fr->Bottom) / fr->Near;	

	// Test if in frustum in negative z space
	xfrustrumedge = -sc[2] * fr->Right/fr->Near;
	value = sc[0]-r*frustrum_xRfactorpos;
	if( value > xfrustrumedge ) // Positive half of x space test 
	{	
		return( 0 );
	}
	
	xfrustrumedge = -sc[2] * fr->Left/fr->Near;
	value = sc[0]+r*frustrum_xRfactorneg;
	if( value < xfrustrumedge ) // Negative half of x space test 
	{
		return( 0 );
	}

	yfrustrumedge = -sc[2] * fr->Top/fr->Near;
	value = sc[1]-r*frustrum_yRfactorpos;
	if( value > yfrustrumedge ) // Positive half of y space test 
	{
		return( 0 );
	}

	yfrustrumedge = -sc[2] * fr->Bottom/fr->Near;
	value = sc[1]+r*frustrum_yRfactorneg;
	if( value < yfrustrumedge ) // Negative half of y space test 
	{
		return( 0 );
	}

	return( 1 );	
}

void fl_tpc_matrix_multiply( float *in4x4, float *in1x4, float *out1x4 ) 
{
	int i,j;
	double sum;
	
	for( i=0; i<4; i++ ) 
	{
		sum = 0.0;
		for( j=0; j<4; j++ ) 
		{
			sum += ( in4x4[i+j*4] * in1x4[j] );
		}
		out1x4[i] = (float)sum;
	}
}

/* load a 4x4 matrix with identity */
void fl_loadIdentity (float *T)
{
  for(int i=0; i<16; i++ )
  {
    T[i] = 0.0;
    if(i%5==0)
    {
      T[i] = 1.0;
    }	
  }
}

/* copy one 4x4 matrix to another */
void fl_matrix_copy (float *in4x4, float *out4x4)
{
  /* TODO:  Maybe replace this with a memcpy (memmove)? */
  for (int i=0; i < 16; i++)
    out4x4[i] = in4x4[i];
}

/* our own 4x4 matrix multiply */
void fl_4x4_matrix_multiply (float *in1, float *in2, float *out)
{
  int i, l, m;
  double sum = 0;

  for (i=0; i < 4; i++)
    for (l=0; l < 4; l++)
    {
      sum = 0;
      for (m=0; m < 4; m++)
      {
        sum += in1 [i+4*m] * in2 [m+4*l];
      }
      out [i+4*l] = sum;
    }
}

/* our own translate */
float *fl_translatef (float *in4x4, float x, float y, float z)
{
  float T[16];          // translation matrix
  float out4x4[16];     // output matrix

  fl_loadIdentity (T);
  T[12] = x;
  T[13] = y;
  T[14] = z;

  fl_4x4_matrix_multiply (in4x4, T, out4x4);
//  fl_4x4_matrix_multiply (T, in4x4, out4x4);

  fl_matrix_copy (out4x4, in4x4);

  return (in4x4);
}

/* our own z rotate */
float *fl_rotatef001 (float *in4x4, float q)
{
  float s = sin (deg2rad*q);
  float c = cos (deg2rad*q);
  float T[16];          // translation matrix
  float out4x4[16];     // output matrix

  fl_loadIdentity (T);
  T[0] = c;
  T[1] = s;
  T[4] = -s;
  T[5] = c;

  fl_4x4_matrix_multiply (in4x4, T, out4x4);
//  fl_4x4_matrix_multiply (T, in4x4, out4x4);

  fl_matrix_copy (out4x4, in4x4);

  return (in4x4);
}

/* our own y rotate */
float *fl_rotatef010 (float *in4x4, float q)
{
  float s = sin (deg2rad*q);
  float c = cos (deg2rad*q);
  float T[16];          // translation matrix
  float out4x4[16];     // output matrix

  fl_loadIdentity (T);
  T[0] = c;
  T[2] = -s;
  T[8] = s;
  T[10] = c;

  fl_4x4_matrix_multiply (in4x4, T, out4x4);
//  fl_4x4_matrix_multiply (T, in4x4, out4x4);

  fl_matrix_copy (out4x4, in4x4);

  return (in4x4);
}

/* our own x rotate */
float *fl_rotatef100 (float *in4x4, float q)
{
  float s = sin (deg2rad*q);
  float c = cos (deg2rad*q);
  float T[16];          // translation matrix
  float out4x4[16];     // output matrix

  fl_loadIdentity (T);
  T[5] = c;
  T[6] = s;
  T[9] =-s;
  T[10]= c;

  fl_4x4_matrix_multiply (in4x4, T, out4x4);
//  fl_4x4_matrix_multiply (T, in4x4, out4x4);

  fl_matrix_copy (out4x4, in4x4);

  return (in4x4);
}

/* our own scale */
float *fl_scalef (float *in4x4, float x, float y, float z)
{
  float T[16];          // translation matrix
  float out4x4[16];     // output matrix

  fl_loadIdentity (T);
  T[0] = x;
  T[5] = y;
  T[10]= z;

  fl_4x4_matrix_multiply (in4x4, T, out4x4);
//  fl_4x4_matrix_multiply (T, in4x4, out4x4);

  fl_matrix_copy (out4x4, in4x4);

  return (in4x4);
}

void fl_restructure_shadow_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot, int shadow_number)
{
	OBJECT_TYPE *current_object;
	GRAPH_NODE_TYPE *neighbor_node, *old_first_node;
	char current_object_name[60];
	//int i=0;
	
//	strcpy(old_root_name, oldroot->name);
	if( strcmp(oldroot->name,newroot->name)==0 ) /* Do nothing if there has been no change */
		return;		

	/* preset Mco to Idenity, since it has not parent */
        fl_loadIdentity (newroot->shadow_Mco[shadow_number]);
// KLS
//	glLoadIdentity();    
// KLS
//	fl_save_current_GL_modelview_matrix( newroot->shadow_Mco[shadow_number]); 
	
	current_object = newroot;
        /* TODO: use pointers instead of strcpy here (and below) */
	strcpy(current_object_name,newroot->name);
	//printf("fl_restructure_tree- starting current_object_name: %s\n",current_object_name);
	
	old_first_node = current_object->shadow_first_node [shadow_number];
	
	while( strcmp(oldroot->name,current_object_name)!=0 ) {

		//printf("fl_restructure_tree- working on object: %s\n",current_object_name); fflush(stdout);
		neighbor_node = old_first_node->neighbor_node;
		old_first_node = neighbor_node->self->shadow_first_node [shadow_number];
		neighbor_node->self->shadow_first_node [shadow_number] = neighbor_node;
		neighbor_node->MpoParams->shadow_xform_dir_flag [shadow_number] *= -1;

		//printf("fl_restructure_tree- looking at object: %s\n",neighbor_node->self->name); fflush(stdout);
		current_object = neighbor_node->self;
		strcpy(current_object_name,current_object->name);
	}
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_restructure_subtree (OBJECT_TYPE *oldroot, OBJECT_TYPE *newroot)
{
  fl_restructure_shadow_subtree (oldroot, newroot, 0);
}

void fl_restructure_shadow_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot, int shadow_number )
{
        fl_restructure_shadow_subtree (w->shadow_root_object[shadow_number], newroot, shadow_number);
	w->shadow_root_object[shadow_number] = newroot; /* set Universe variable */
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_restructure_tree( UNIVERSE_TYPE *w, OBJECT_TYPE *newroot )
{
  fl_restructure_shadow_tree( w, newroot, 0 );
}

void fl_print_all_shadow_objects( UNIVERSE_TYPE *w, int shadow_number )
{
  OBJECT_TYPE *object;
  GRAPH_NODE_TYPE *current_node;
  char name[60],pname[60],cname[60];
  int i,j,xform_flag;
  
  printf( "\nfl_print_all_shadow_objects- Universe name: %s, first object: %s, nobjects: %d\n",
	  w->name, w->shadow_root_object[shadow_number]->name, w->nobjects );
  
  for( i=0; i<w->nobjects; i++ ) {
    object = w->database[i].object;
    if( object!=NULL) {
      strcpy(name,object->name);
      strcpy(pname,object->shadow_first_node [shadow_number]->neighbor_node->self->name);
      xform_flag = object->shadow_first_node [shadow_number]->MpoParams->shadow_xform_dir_flag [shadow_number];
      if( strcmp(name,w->shadow_root_object[shadow_number]->name)!=0 ) {
	printf("  DB#: %3d | Object name: %s, pname: %s, xflag: %1d\n",i,name,pname,xform_flag);
	current_node = object->shadow_first_node [shadow_number];
	for( j=0; j<object->nneighbors; j++ ) {
	  strcpy(cname,current_node->MpoParams->original_child->name);
	  printf("                Node: %3d cname: %s\n",j,cname);
	  current_node = current_node->right_sibling_node;
	}
      } else {
	printf("  DB#: %3d | Object name: %s, First Object, no parent.\n",i,name);
	current_node = object->shadow_first_node [shadow_number];
	for( j=0; j<object->nneighbors; j++ ) {
	  strcpy(cname,current_node->MpoParams->original_child->name);
	  printf("                Node: %3d cname: %s\n",j,cname);
	  current_node = current_node->right_sibling_node;
    			}
      }
    }
  }
  printf("fl_print_all_shadow_objects- Exiting.\n");
}

/* compatability function, added 14-jan-03, KLS.  Added for backward
 * compatability from the new shadow graph structure */
void fl_print_all_objects( UNIVERSE_TYPE *w )
{
  fl_print_all_shadow_objects( w, 0);
}

void fl_print_gl_matrix( float *x )
{
	int i,j;

	for( i=0; i<4; i++ ) {
		printf(" %1d|",i);
		for( j=0; j<4; j++ ) {
			printf("%10.5f |",x[i+j*4]);
		}
		printf("\n");
	}
}

void fl_eye_coords_in_my_space(UNIVERSE_TYPE *w, OBJECT_TYPE *o, float *point,
                      float *result)
{
/* Please do not use this function if at all possible. It is a large comp load.  tpc 9/19/2000 */
/* Use fl_root_coords_in_local_space() instead */
  OBJECT_TYPE *old_root;

  glPushMatrix();

  /* save old root object */
  old_root = w->root_object;

  /* Restucture tree from object perspective */
  fl_restructure_tree (w, o);
 
  /* Update all Mpo xforms */
  fl_update_Mpo_all_objects (w);

  /* traverse tree to update xform inheritance Mco */
  fl_xform_update (w);

  /* get the coordinates */
  fl_tpc_matrix_multiply (old_root->Mco, point, result);

  /* restore old root object */
  fl_restructure_tree (Universe, old_root);
  fl_update_Mpo_all_objects (Universe);
  fl_xform_update (Universe);
  glPopMatrix();
}

void fl_glutMouse_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutMousefunc!=NULL ) 
		{
			(*object->glutMousefunc)( object, button, state, x, y );
		}
	}
}

void fl_glutMotion_all_objects( UNIVERSE_TYPE *w, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutMotionfunc!=NULL ) 
		{
			(*object->glutMotionfunc)( object, x, y );
		}
	}
}

void fl_glutPassiveMotion_all_objects( UNIVERSE_TYPE *w, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutPassiveMotionfunc!=NULL ) 
		{
			(*object->glutPassiveMotionfunc)( object, x, y );
		}
	}
}

void fl_glutEntry_all_objects( UNIVERSE_TYPE *w, int state )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutEntryfunc!=NULL ) 
		{
			(*object->glutEntryfunc)( object, state );
		}
	}
}

void fl_glutKeyboard_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutKeyboardfunc!=NULL ) 
		{
			(*object->glutKeyboardfunc)( object, key, x, y );
		}
	}
}

void fl_glutKeyboardUp_all_objects( UNIVERSE_TYPE *w, unsigned char key, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutKeyboardUpfunc!=NULL ) 
		{
			(*object->glutKeyboardUpfunc)( object, key, x, y );
		}
	}
}

void fl_glutSpecial_all_objects( UNIVERSE_TYPE *w, int key, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutSpecialfunc!=NULL ) 
		{
			(*object->glutSpecialfunc)( object, key, x, y );
		}
	}
}

void fl_glutSpecialUp_all_objects( UNIVERSE_TYPE *w, int key, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutSpecialUpfunc!=NULL ) 
		{
			(*object->glutSpecialUpfunc)( object, key, x, y );
		}
	}
}

void fl_glutSpaceballMotion_all_objects( UNIVERSE_TYPE *w, int x, int y, int z )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutSpaceballMotionfunc!=NULL ) 
		{
			(*object->glutSpaceballMotionfunc)( object, x, y, z );
		}
	}
}

void fl_glutSpaceballRotate_all_objects( UNIVERSE_TYPE *w, int x, int y, int z )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutSpaceballRotatefunc!=NULL ) 
		{
			(*object->glutSpaceballRotatefunc)( object, x, y, z );
		}
	}
}

void fl_glutSpaceballButton_all_objects( UNIVERSE_TYPE *w, int button, int state )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutSpaceballButtonfunc!=NULL ) 
		{
			(*object->glutSpaceballButtonfunc)( object, button, state );
		}
	}
}

void fl_glutTabletMotion_all_objects( UNIVERSE_TYPE *w, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutTabletMotionfunc!=NULL ) 
		{
			(*object->glutTabletMotionfunc)( object, x, y );
		}
	}
}

void fl_glutTabletButton_all_objects( UNIVERSE_TYPE *w, int button, int state, int x, int y )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutTabletButtonfunc!=NULL ) 
		{
			(*object->glutTabletButtonfunc)( object, button, state, x, y );
		}
	}
}

void fl_glutJoystick_all_objects( UNIVERSE_TYPE *w, unsigned int buttonMask, int x, int y, int z )
{
	OBJECT_TYPE *object;
	int i;
	
	for( i=0; i<w->nobjects; i++ ) 
	{
		object = w->database[i].object;
		if ( object!=NULL && object->glutJoystickfunc!=NULL ) 
		{
			(*object->glutJoystickfunc)( object, buttonMask, x, y, z );
		}
	}
}


/* Multiplies a 3x3 matrix by a 1x3 vector */
void fl_kls_3x3_by_1x3_matrix_multiply (float *in3x3, float *in1x3, float *out1x3)
{
  int i,j;
  double sum;

  for (i = 0; i < 3; i++)
  {
    sum = 0.0;
    for (j = 0; j < 3; j++)
      sum += in3x3[i+j*3] * in1x3[j];
    out1x3[i] = (float) sum;
  }
}

/* Does most of the work of finding the inverse of a 3x3 column major matrix:
 * only leaves off dividing by the determinate */
void fl_kls_3x3_matrix_inverse_almost(float in3x3[3][3], float *out3x3, float *det)
{
  *det = in3x3[0][0] * (in3x3[1][1] * in3x3[2][2] - in3x3[2][1] * in3x3[1][2]) -
         in3x3[0][1] * (in3x3[1][0] * in3x3[2][2] - in3x3[2][0] * in3x3[1][2]) +
         in3x3[0][2] * (in3x3[1][0] * in3x3[2][1] - in3x3[2][0] * in3x3[1][1]);

  out3x3[0] = (in3x3[1][1] * in3x3[2][2] - in3x3[2][1] * in3x3[1][2]);
  out3x3[1] =-(in3x3[1][0] * in3x3[2][2] - in3x3[2][0] * in3x3[1][2]);
  out3x3[2] = (in3x3[1][0] * in3x3[2][1] - in3x3[2][0] * in3x3[1][1]);
  out3x3[3] =-(in3x3[0][1] * in3x3[2][2] - in3x3[2][1] * in3x3[0][2]);
  out3x3[4] = (in3x3[0][0] * in3x3[2][2] - in3x3[2][0] * in3x3[0][2]);
  out3x3[5] =-(in3x3[0][0] * in3x3[2][1] - in3x3[2][0] * in3x3[0][1]);
  out3x3[6] = (in3x3[0][1] * in3x3[1][2] - in3x3[1][1] * in3x3[0][2]);
  out3x3[7] =-(in3x3[0][0] * in3x3[1][2] - in3x3[1][0] * in3x3[0][2]);
  out3x3[8] = (in3x3[0][0] * in3x3[1][1] - in3x3[1][0] * in3x3[0][1]);
}

/* Finds the inverse of a 3x3 column major matrix */
void fl_kls_3x3_matrix_inverse (float in3x3[3][3], float *out3x3)
{
  float det;            /* determinant */

  fl_kls_3x3_matrix_inverse_almost (in3x3, out3x3, &det);

  det = 1.0/det;

  for (int i = 0; i < 9; i++)
    out3x3[i] = out3x3[i] * det;
}

/* Finds the coordinates of the root object in this object's space */
/* local_object is the object whose coordinates get used, coords is a 1x4
 * coordinate vector */
void fl_root_coords_in_local_space (OBJECT_TYPE *local_object, float *coords)
{
  int i,j;                      /* loop vars */
  float tmp;
  float det[1];                 /* determinate of the 3x3 matrix */
  float outputVector[3],        /* output vector (end result: x,y,z) */
        translateVector[3],     /* translation vector (from Mco matrix) */
        inverse3x3[3][3],       /* inverse of the 3x3 rotation matrix */
        rot3x3[3][3];           /* 3x3 rotation matrix (from Mco matrix) */
  
  /* get the translation vector: upper 3 elements of right column of Mco */
  for (i = 0; i < 3; i++)
  {
    /* don't ask me why, but if this assignment is done directly, the program
     * seg-faults. */
    tmp = local_object->Mco[i+12];
    translateVector[i] = tmp;
  }

  /* get the rotation/scaling matrix from Mco: upper left 3x3 portion of Mco */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
    {
      tmp = local_object->Mco[i+4*j];
      rot3x3[i][j] = tmp;
    }

  /* get the inverse of the rotation matrix */
  fl_kls_3x3_matrix_inverse_almost (rot3x3, (float *) inverse3x3, det);

  /* multiply the inverse by the translation vector, then pick off results */
  fl_kls_3x3_by_1x3_matrix_multiply ((float *) inverse3x3, translateVector, outputVector);
  *det = 1.0 / *det;
  for (i = 0; i < 3; i++)
    coords[i] = -outputVector[i] * *det;
  coords[3] = 1.0;
}


bool fl_SendMessage( OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message )
{
    Loop_Control_Struct *lc = Loop_Control;

    if( lc->LogMessages )
    {
	if( lc->OpenMessageLog )
	{
	    lc->OpenMessageLog = false;
	    lc->LogFile = fopen( lc->MessageLogName, "w" );
	}
	if( lc->LogFile )
	{
	    if( !lc->MessageLogFilter || lc->MessageLogFilter( from_object, to_object, message ) )
	    {
		// I'm not sure this is safe here... so far it works, but what about
		// other threads???
		// it does differ by a millisecond or two sometimes...
		int ct = glutGet( (GLenum) GLUT_ELAPSED_TIME );
//		fprintf( lc->LogFile, "[%d,%d]: ", lc->Current_Time, ct );
		fprintf( lc->LogFile, "[%d]: ", ct );
		int sh=0;
		if( from_object )
		{
		    if( from_object->shared_flag )
		    {
			sh = 1;
		    }
		    fprintf( lc->LogFile, "\"%s\"%1d, ", from_object->name, sh );
		}
		else
		{
		    fprintf( lc->LogFile, "NULL, " );
		}
		sh = 0;
		if( to_object )
		{
		    if( from_object->shared_flag )
		    {
			sh = 1;
		    }
		    fprintf( lc->LogFile, "\"%s\"%1d, ", to_object->name, sh );
		}
		else
		{
		    fprintf( lc->LogFile, "NULL, " );
		}
		if( message )
		{
		    fprintf( lc->LogFile, "\"%s\"\n", message );
		}
		else
		{
		    fprintf( lc->LogFile, "NULL\n" );
		}
		fflush( lc->LogFile );
	    }
	}
    }
    
    if ( to_object )
    {
      /* check to see if "to_object" is shared */
      if (to_object->shared_flag == true)
      {
	    fl_collab_send_objmsg_msg(from_object, to_object, message);
	    return true;
      }
      else  /* "to_object" is not shared */
      {
            if ( to_object->recievemessagefunc )	
            {
	      to_object->recievemessagefunc( to_object, from_object, message );
	      return true;
	    }
      }
    }
    return false;
}


bool fl_SendMessage( OBJECT_TYPE *from_object, char *to_object_name, char *message )
{
    OBJECT_TYPE *to_object = fl_find_object_by_name( Loop_Control->Universe, to_object_name );
    
    return fl_SendMessage( from_object, to_object, message );

    Loop_Control_Struct *lc = Loop_Control;

    if( !to_object )
    {
	if( lc->LogMessages )
	{
	    if( lc->LogFile )
	    {
		if( to_object_name )
		{
		    fprintf( lc->LogFile, "(\"%s\")\n", to_object_name );
		}
		else
		{
		    fprintf( lc->LogFile, "(NULL)\n" );
		}
		fflush( lc->LogFile );
	    }
	}
    }
}

// copy a shadow graph
void fl_copy_shadow_graph(UNIVERSE_TYPE * w, int source_index, int dest_index)
{
  int i;

  // The Mco and Mpo matricies are not copied, as they should change for the
  // new graph.  They are, however, allocated.
  for (i=0; i < w->nobjects; i++)
  {
    // the first_node pointer
    w->database[i].object->shadow_first_node[dest_index] =
      w->database[i].object->shadow_first_node[source_index];

    // the first_node's xform_dir_flags
    w->database[i].object->shadow_first_node[dest_index]->MpoParams->shadow_xform_dir_flag[dest_index] =
      w->database[i].object->shadow_first_node[source_index]->MpoParams->shadow_xform_dir_flag[source_index];
    w->database[i].object->shadow_first_node[dest_index]->MpoParams->shadow_xform_dir_flag_old[dest_index] =
      w->database[i].object->shadow_first_node[source_index]->MpoParams->shadow_xform_dir_flag_old[source_index];
  }
}

int fl_get_shadow_graph(UNIVERSE_TYPE * w)
{
//  void *tmpptr, *tmpptr2;
  int i;

  // look for an unused shadow graph already allocated
  for (i=0; i< w->num_shadows; i++)
    if (!w->shadow_list[i]) break;
  if (i == w->num_shadows) 
  {
    fprintf (stderr, "ERROR: Cannot allocate shadow graph.\n");
    fprintf (stderr, "       Change maxshadowgraphs in the Flatland.config\n");
    return (0);
//******* the following automatic resizing code is disabled because it would
//******* require mutexing every reference to the shadow graph... ugly! KLS
//    // double the size of the shadow array
//    int new_num = w->num_shadows << 1;
//
//    for (j=0; j < w->nobjects; j++)
//    {
//      OBJECT_TYPE *o = w->database[j].object;
//      // double the shadow_first_node in the object
//      tmpptr = o->shadow_first_node;
//      o->shadow_first_node = (GRAPH_NODE_TYPE**)malloc(sizeof(GRAPH_NODE_TYPE*)*w->num_shadows);
//      if (!o->shadow_first_node)
//      {
//        fprintf (stderr, "ERROR: could not resize shadow_first_node for object %s.  Crashing now.\n",
//                 o->name);
//        exit (1);
//      }
//      memcpy (o->shadow_first_node, tmpptr, w->num_shadows * sizeof(GRAPH_NODE_TYPE *));
//      free (tmpptr);
//      for (int fn_i = w->num_shadows; fn_i < new_num; fn_i++)
//        o->shadow_first_node[fn_i] = NULL;
//      o->first_node = o->shadow_first_node [0];
//	
//      // double the shadow_Mco space in the object
//      tmpptr = o->shadow_Mco;
//      o->shadow_Mco = (float **)malloc(sizeof(float*)*(w->num_shadows));
//      if (!o->shadow_Mco)
//      {
//        fprintf (stderr, "ERROR: could not resize shadow_Mco for object %s.  Crashing now.\n",
//                 o->name);
//        exit (1);
//      }
//      memcpy (o->shadow_Mco, tmpptr, w->num_shadows * sizeof(float *));
//      free (tmpptr);
//      for (int shadow_i = w->num_shadows; shadow_i < new_num; shadow_i++)
//        o->shadow_Mco[shadow_i] = (float*)malloc(sizeof(float)*16);
//      o->Mco = w->database[j].object->shadow_Mco[0];
//
//      // double the shadow_xform_dir_flag and shadow_xform_dir_flag_old in the
//      // node pointed to by shadow_first_node[0] (that should cover all nodes)
//      MPO_TYPE *m = o->shadow_first_node[0]->MpoParams;
//      tmpptr = m->shadow_xform_dir_flag;
//      tmpptr2 = m->shadow_xform_dir_flag_old;
//      m->shadow_xform_dir_flag = (int*)calloc(w->num_shadows, sizeof(int));
//      m->shadow_xform_dir_flag_old = (int*)malloc(sizeof(int)*w->num_shadows);
//      if (!m->shadow_xform_dir_flag || !m->shadow_xform_dir_flag_old)
//      {
//        fprintf (stderr, "ERROR: could not resize shadow_xform_dir_flag array for object %s.  Crashing now.\n", o->name);
//        exit (1);
//      }
//      memcpy (m->shadow_xform_dir_flag, tmpptr, w->num_shadows * sizeof(int));
//      memcpy (m->shadow_xform_dir_flag_old, tmpptr2, w->num_shadows * sizeof(int));
//      free (tmpptr);
//      free (tmpptr2);
//      for (int some_ungodly_variable = w->num_shadows; some_ungodly_variable < new_num; some_ungodly_variable++)
//      {
//        m->shadow_xform_dir_flag_old[some_ungodly_variable] = 3; // 3 -> not validly set -> forces recalculation
//      }
//    } // for nobjects
//    w->num_shadows = new_num;
  } // if all shadows used
  // mark this shadow graph as used
  w->shadow_list[i] = true;
  // copy the root shadow graph to the new one
  fl_copy_shadow_graph(w, 0, i);

  // Finally, copy the root pointer in universe object
  w->shadow_root_object[i] = w->shadow_root_object[0];

  return i;
}

void fl_release_shadow_graph(UNIVERSE_TYPE * w, int shadow_graph_num)
{
  w->shadow_list[shadow_graph_num] = false;
}
