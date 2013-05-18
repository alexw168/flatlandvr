/*
mtbuf.c.  a general-purpose MT buffer

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
 * Added "wrappers" for collaboration: 3-15-02, Moad Mowafi
 * 
 */

#include "gheader.h"

// Constructor - grabs mutex, inits data, release & return
mt_buf::mt_buf(const int num_elements, const int elem_size)
{
  try
  {
    int i;
    Mutex.lock();

    // Save blocksize (units of sizeof(float)s!)
    size = num_elements;
    num_bytes = num_elements * elem_size;
    
    // Allocate buffers based on # of birds
    buf_ptr = (float *) malloc(num_bytes);

    // Set initial data to zero
    for(i = 0; i < size; i++)
	    buf_ptr[i] = 0.0;

    // allocate the changed and set_changed_value arrays
    changed = (bool *) calloc (Loop_Control->Universe->num_shadows, sizeof (bool));
    set_changed_value = (bool *) calloc (Loop_Control->Universe->num_shadows, sizeof (bool));

    // Set value to be copied into changed when data changes
    for (i = 0; i < Loop_Control->Universe->num_shadows; i++)
      set_changed_value [i] = true;

    Mutex.unlock();
  }
  catch(general_exception &M)
  {
    Mutex.unlock();
    throw M;
  }
}

// Destructor just unlocks mutex, paranoia really.  
mt_buf::~mt_buf(void)
{
    free (changed);
    free (set_changed_value);

    Mutex.unlock();
}

// Function to save a data set of floats safely
//void mt_buf::assign(const float *data)
void mt_buf::assign(float *data, OBJECT_TYPE *object)
{
  bool changed_flag = false;

  /* Test if Mpo params have changed */
  changed_flag = false;
  for( int k=0; k<9; k++ ) {
    if ( buf_ptr[k] != data[k] ) {
      changed_flag = true;      // continue to change the data
      break;
    }
  }
    
  if (changed_flag)
  {
    /* check to see if the object is shared */
    if (object != NULL && object->shared_flag == true)
      {
        fl_collab_send_mpo_msg(object, data);
      }
    else  /* the object is not shared */
      do_assign ((const float *) data);
//      {      
//        /* TODO:  This should call do_assign() */
//        try
//	  {
//	    Mutex.lock();
//	  
//	    // Copy data as fast as possible
//	    memcpy(buf_ptr, data, (size_t) num_bytes);
//	  
//	    Mutex.unlock();
//	  }
//        catch(general_exception &M)
//	  {
//	    Mutex.unlock();
//	    throw M;
//	  }
//      }
  }
  return;
}

// Fn to read back a data set
void mt_buf::read(float *dest)
{
  try
    {
      Mutex.lock();

      // Copy data out as fast as we can for min lock time
      memcpy(dest, buf_ptr, (size_t) num_bytes);

      Mutex.unlock();
    }
  catch(general_exception &M)
    {
      Mutex.unlock();
      throw M;
    }
  
  return;
}

// Original assign() function -- used by Collaborator App
void mt_buf::do_assign(const float *data)
{
  try
    {
      Mutex.lock();
      
      // Copy data as fast as possible
      memcpy(buf_ptr, data, (size_t) num_bytes);

      set_changed ();   // mark the data as changed

      Mutex.unlock();
    }
  catch(general_exception &M)
    {
      Mutex.unlock();
      throw M;
    }

  return;
}

// returns the "shadow_num"th element of "changed" and clears it
bool mt_buf::is_changed (int shadow_num)
{
  bool tmp = false;

  try
  {
    Mutex.lock();
      
    if (shadow_num < Loop_Control->Universe->num_shadows)   // safty first!
    {
      tmp = changed [shadow_num];
      changed [shadow_num] = false;
    }

    Mutex.unlock();
  }
  catch(general_exception &M)
  {
    Mutex.unlock();
    throw M;
  }

  return tmp;
}

// sets all values of "changed" to "true"
void mt_buf::set_changed (void)
{
  try
  {
      
    // copy data as fast as possible
    memcpy (changed, set_changed_value, sizeof(bool) * Loop_Control->Universe->num_shadows);

  }
  catch(general_exception &M)
  {
    throw M;
  }

  return;
}
