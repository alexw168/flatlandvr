/*
usr_mtbuf.c. General-purpose user mutexed buffer

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

usr_mtbuf::usr_mtbuf()
{
  for (int idx=0; idx<MAXBUFS; idx++) {      
    try {
	buffs[idx].Mutex.lock();
	  
	buffs[idx].ptr = NULL;
	buffs[idx].filled = false;
    
	buffs[idx].Mutex.unlock();
    }
    catch(general_exception &M) {
      buffs[idx].Mutex.unlock();
      throw M;
    }
  }
}

usr_mtbuf::~usr_mtbuf(void)
{
  for (int idx=0; idx<MAXBUFS; idx++)
    buffs[idx].Mutex.unlock();
}

void usr_mtbuf::assign(int idx, void *data, OBJECT_TYPE *object)
{
  /* check to see if the object is shared */
  if (object != NULL && object->shared_flag == true) {

    fl_collab_send_mtbuf_msg(object, buffs[idx].hdr, buffs[idx].size, data);
  }
  else { /* the object is not shared */      

    do_assign(idx, data);
  }
}

void usr_mtbuf::read(int idx, void *data)
{
  try {
    buffs[idx].Mutex.lock();
    
    memcpy(data, buffs[idx].ptr, buffs[idx].size);
    
    buffs[idx].Mutex.unlock();
  }
  catch(general_exception &M) {
    buffs[idx].Mutex.unlock();
    throw M;
  }
  
  return;
}

void usr_mtbuf::do_assign(int idx, void *data)
{
  try {
    buffs[idx].Mutex.lock();
    
    memcpy(buffs[idx].ptr, data, buffs[idx].size);
    
    buffs[idx].Mutex.unlock();
  }
  catch(general_exception &M) {
    buffs[idx].Mutex.unlock();
    throw M;
  }
  
  return;
}


int fl_make_usrmtbuf(struct object *o, char * data_type, int num_elems) 
{
  int idx = -1;
    
  if (!o->object_mtbuf) {
    o->object_mtbuf = new usr_mtbuf;
    if (!o->object_mtbuf) {
      fprintf (stderr, "fl_make_usrmtbuf(): Failed to create mtbuf\n");
      return -1;
    }
  }
  
  for (int i=0; i<MAXBUFS; i++) {
    if (!o->object_mtbuf->buffs[i].filled) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    fprintf(stderr, "fl_make_usrmtbuf(): Mtbuf is full\n");
    return -1;
  }

  /* save buf header for collaboration */
  o->object_mtbuf->buffs[idx].hdr.index = idx;
  o->object_mtbuf->buffs[idx].hdr.num_elems = num_elems;
  strcpy(o->object_mtbuf->buffs[idx].hdr.type, data_type);
  
  try {
    o->object_mtbuf->buffs[idx].Mutex.lock();
    
    if (strcmp(data_type, "float") == 0) {
      if ((o->object_mtbuf->buffs[idx].ptr = malloc(num_elems * sizeof(float))) == NULL) {
	fprintf(stderr, "fl_make_usrmtbuf(): Failed to create buf[%d]\n", idx);
	return -1;
      }
      o->object_mtbuf->buffs[idx].size = num_elems * sizeof(float);
      o->object_mtbuf->buffs[idx].filled = true;
    }
        
    if (strcmp(data_type, "double") == 0) {
      if ((o->object_mtbuf->buffs[idx].ptr = malloc(num_elems * sizeof(double))) == NULL) {
	fprintf(stderr, "fl_make_usrmtbuf(): Failed to create buf[%d]\n", idx);
	return -1;
      }
      o->object_mtbuf->buffs[idx].size = num_elems * sizeof(double);
      o->object_mtbuf->buffs[idx].filled = true;
    }
        
    if (strcmp(data_type, "int") == 0) {
      if ((o->object_mtbuf->buffs[idx].ptr = malloc(num_elems * sizeof(int))) == NULL) {
	fprintf(stderr, "fl_make_usrmtbuf(): Failed to create buf[%d]\n", idx);
	return -1;
      }
      o->object_mtbuf->buffs[idx].size = num_elems * sizeof(int);
      o->object_mtbuf->buffs[idx].filled = true;
    }
        
    if (strcmp(data_type, "long") == 0) {
      if ((o->object_mtbuf->buffs[idx].ptr = malloc(num_elems * sizeof(long))) == NULL) {
	fprintf(stderr, "fl_make_usrmtbuf(): Failed to create buf[%d]\n", idx);
	return -1;
      }
      o->object_mtbuf->buffs[idx].size = num_elems * sizeof(long);
      o->object_mtbuf->buffs[idx].filled = true;
    }
        
    if (strcmp(data_type, "char") == 0) {
      if ((o->object_mtbuf->buffs[idx].ptr = malloc(num_elems * sizeof(char))) == NULL) {
	fprintf(stderr, "fl_make_usrmtbuf(): Failed to create buf[%d]\n", idx);
	return -1;
      }
      o->object_mtbuf->buffs[idx].size = num_elems * sizeof(char);
      o->object_mtbuf->buffs[idx].filled = true;
    }
        
    o->object_mtbuf->buffs[idx].Mutex.unlock();
  }
  catch(general_exception &M) {
    o->object_mtbuf->buffs[idx].Mutex.unlock();
    throw M;
  }

  return idx;
}


void fl_delete_usrmtbuf(struct object *o, int idx)
{
  if (idx < 0 || idx >= MAXBUFS) {
    fprintf(stderr, "fl_delete_usrmtbuf(): Mtbuf index out of range\n");
    return;
  }
  
  try {
    o->object_mtbuf->buffs[idx].Mutex.lock();
      
    if (o->object_mtbuf->buffs[idx].ptr) {
      free(o->object_mtbuf->buffs[idx].ptr);
      o->object_mtbuf->buffs[idx].ptr = NULL;
      o->object_mtbuf->buffs[idx].filled = false;
    }

    o->object_mtbuf->buffs[idx].Mutex.unlock();
  }
  catch(general_exception &M) {
    o->object_mtbuf->buffs[idx].Mutex.unlock();
    throw M;
  }
}
