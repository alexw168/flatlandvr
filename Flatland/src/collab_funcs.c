/*
collab_funcs.c. Collaboration-related functions

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
 * Created: 6-21-02, Moad Mowafi
 *
 */

#include "gheader.h"
#include "collab_shared.h"
#include "collab_init.h"

void fl_collab_send_mtbuf_msg(OBJECT_TYPE *object, MTBUF_HDR_TYPE hdr, int size, void *data)
{
  MTBUF_MSG_TYPE *msg;
  
  if ((msg = (MTBUF_MSG_TYPE *)malloc(sizeof(MTBUF_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_mtbuf_msg(): Failed to allocate memory for MTBUF message\n");
    exit(1);
  }
  
  if ((msg->data = malloc(size)) == NULL) {
    fprintf(stderr, "fl_collab_send_mtbuf_msg(): Failed to allocate memory for mtbuf data\n");
    exit(1);
  }        
  
  memcpy(msg->data, data, size);
  msg->buf_index = hdr.index;
  msg->num_elems = hdr.num_elems;
  strcpy(msg->data_type, hdr.type);
  strcpy(msg->object_name, object->name);
  msg->next = NULL;
      
  pthread_mutex_lock(&mtbuf_mutex);
    
  if (mtbuf_head == NULL)
    mtbuf_head = msg;
  else
    mtbuf_tail->next = msg;
  mtbuf_tail = msg;
      
  pthread_mutex_unlock(&mtbuf_mutex);
  pthread_cond_signal(&mtbuf_cond);
}


void fl_collab_send_mpo_msg(OBJECT_TYPE *object, float *data)
{
  MPO_MSG_TYPE *msg;
  
  if ((msg = (MPO_MSG_TYPE *)malloc(sizeof(MPO_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_mpo_msg(): Failed to allocate memory for MPO message\n");
    exit(1);
  }
  
  int num_bytes = 9 * sizeof(float);
  memcpy(msg->P, data, num_bytes);
  strcpy(msg->object_name, object->name);
  msg->next = NULL;
  
  pthread_mutex_lock(&mpo_mutex);
  
  if (mpo_head == NULL)
    mpo_head = msg;
  else
    mpo_tail->next = msg;
  mpo_tail = msg;
  
  pthread_mutex_unlock(&mpo_mutex);      
  pthread_cond_signal(&mpo_cond);
}


void fl_collab_send_ctrl_msg(OBJECT_TYPE *object, char key)
{
  CTRL_MSG_TYPE *msg;
  
  if ((msg = (CTRL_MSG_TYPE *)malloc(sizeof(CTRL_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_ctrl_msg(): Failed to allocate memory for CTRL message\n");
    exit(1);
  }
  
  msg->key = key;
  strcpy(msg->object_name, object->name);
  msg->next = NULL;
  
  pthread_mutex_lock(&ctrl_mutex);
  
  if (ctrl_head == NULL)
    ctrl_head = msg;
  else
    ctrl_tail->next = msg;
  ctrl_tail = msg;
  
  pthread_mutex_unlock(&ctrl_mutex);  
  pthread_cond_signal(&ctrl_cond); 
}


void fl_collab_send_load_msg(APPTYPE *App)
{
  LOAD_MSG_TYPE *msg;
     
  if ((msg = (LOAD_MSG_TYPE *)malloc(sizeof(LOAD_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_load_msg(): Failed to allocate memory for LOAD message\n");
    exit(1);
  }

  strcpy(msg->app_name, App->appname);
  msg->next = NULL;
		
  pthread_mutex_lock(&load_mutex);
      
  if (load_head == NULL)
    load_head = msg;
  else
    load_tail->next = msg;
  load_tail = msg;
     
  pthread_mutex_unlock(&load_mutex);
  pthread_cond_signal(&load_cond);
}


void fl_collab_send_unload_msg(APPTYPE *App)
{
  UNLOAD_MSG_TYPE *msg;
  
  if ((msg = (UNLOAD_MSG_TYPE *)malloc(sizeof(UNLOAD_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_ctrl_msg(): Failed to allocate memory for UNLOAD message\n");
    exit(1);
  }
  
  strcpy(msg->app_name, App->appname);
  msg->next = NULL;
  
  pthread_mutex_lock(&unload_mutex);
  
  if (unload_head == NULL)
    unload_head = msg;
  else
    unload_tail->next = msg;
  unload_tail = msg;
  
  pthread_mutex_unlock(&unload_mutex);
  pthread_cond_signal(&unload_cond);
}


void fl_collab_send_objmsg_msg(OBJECT_TYPE *from_object, OBJECT_TYPE *to_object, char *message)
{
  OBJMSG_MSG_TYPE *msg;
  
  if ((msg = (OBJMSG_MSG_TYPE *)malloc(sizeof(OBJMSG_MSG_TYPE))) == NULL) {
    fprintf(stderr, "fl_collab_send_objmsg_msg(): Failed to allocate memory for OBJMSG message\n");
    exit(1);
  }
  
  if ((msg->message = (char *) malloc ( (strlen(message)+1) * sizeof(char) )) == NULL) {
    fprintf(stderr, "fl_collab_send_objmsg_msg(): Failed to allocate memory for object message\n");
    exit(1);
  }    
  //strncpy(msg->message, message, strlen(message)+1);
  strcpy(msg->message, message);
  strcpy(msg->from_object_name, from_object->name);
  strcpy(msg->to_object_name, to_object->name);
  msg->msg_size = (int) strlen(message);
  msg->next = NULL;
  
  pthread_mutex_lock(&objmsg_mutex);
  
  if (objmsg_head == NULL)
    objmsg_head = msg;
  else
    objmsg_tail->next = msg;
  objmsg_tail = msg;
  
  pthread_mutex_unlock(&objmsg_mutex);  
  pthread_cond_signal(&objmsg_cond); 
}


void fl_collab_controlfunc(OBJECT_TYPE *object, char key)
{
  OBJECT_TYPE *oldControlObject;

  if (object != NULL && object->object_controls != NULL) {

    /* check to see if the object is shared */
    if (object->shared_flag == true) {
      
      fl_collab_send_ctrl_msg(object, key);
    }
    
    else {  /* the object is not shared */
      
      oldControlObject = currentControlObject;
      currentControlObject = object;
      
      fl_keyscan (object->object_controls, key);
      
      currentControlObject = oldControlObject;
    }
  }
}
