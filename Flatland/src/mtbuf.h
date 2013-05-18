/*
 * mtbuf.h.  Mutex buffer definition
 * 
 * Copyright (C) 2003, University of New Mexico.
 * 
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

/* 
 * Maintenance history
 *
// pfh 12/1998
// starting to create a skeleton for a flock of birds daemon thread
// using the MT/C++ book. Interesting stuff!
// Update 2/99 Now takes number of elements as constructor argument, changed name
// to mt_buf to reflect new general use as thread-safe mailbox.
// pfh
 *
 * Added "wrappers" for collaboration: 3-15-02, Moad Mowafi
 *
 */

#ifndef MTBUF_H
#define MTBUF_H

// Local typedefs
#if !defined( FALSE )
 typedef enum {FALSE, TRUE} Boolean;
#else
 typedef bool Boolean;
#endif 

class mt_buf
{
private:
  mutex Mutex;					// Miss Manners
  float *buf_ptr;		 // Ptr to data
  int	  num_bytes;	 // Internal; actual number of bytes to keep around
  bool  *changed;                               // has data changed
  bool  *set_changed_value;                     // quick set value for changed
  
  mt_buf();					// Disallow default constructor; must have argument!
  void set_changed (void);                      // set all changed values to true
  
public:
  int size;					// Number of elements (eg floats) stored

  mt_buf(const int num_elems, const int elem_size);
  ~mt_buf();
  //void assign(const float *data);
  void assign(float *data, struct object *object = NULL);  // Modified to be used for collaboration; Backward compatible with Flatland_C
  void do_assign(const float *data);                       // Original assign() function -- used by Collaborator App
  void read(float *dest);
  bool is_changed (int shadow_num);             // has the data changed for this shadow num

};


#endif
