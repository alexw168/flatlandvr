/*
 * collab_shared.h - Header file needed by: 
 *
 * src/collab_funcs.c
 * usr_modules/Collaborator/collab_net.c
 * usr_modules/Collaborator/collab_threads.c
 * usr_modules/Collaborator/collaborator.c
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
 *
 * Created: 3-15-02, Moad Mowafi
 *
 */

#ifndef COLLAB_SHARED_H
#define COLLAB_SHARED_H

typedef struct mpo_msg_type {
  float P[9];
  char object_name[MAX_NAME_SIZE];
  struct mpo_msg_type *next;
} MPO_MSG_TYPE;


typedef struct ctrl_msg_type {
  char key;
  char object_name[MAX_NAME_SIZE];
  struct ctrl_msg_type *next;
} CTRL_MSG_TYPE;


typedef struct load_msg_type {
  char app_name[MAX_NAME_SIZE];
  struct load_msg_type *next;
} LOAD_MSG_TYPE;


typedef struct unload_msg_type {
  char app_name[MAX_NAME_SIZE];
  struct unload_msg_type *next;
} UNLOAD_MSG_TYPE;


typedef struct mtbuf_msg_type {
  int buf_index;
  int num_elems;
  char data_type[10];  
  void *data;
  char object_name[MAX_NAME_SIZE];
  struct mtbuf_msg_type *next;
} MTBUF_MSG_TYPE;


typedef struct objmsg_msg_type {
  char *message;
  int msg_size;
  char from_object_name[MAX_NAME_SIZE];
  char to_object_name[MAX_NAME_SIZE];
  struct objmsg_msg_type *next;
} OBJMSG_MSG_TYPE;


extern MPO_MSG_TYPE *mpo_head;
extern MPO_MSG_TYPE *mpo_tail;
extern pthread_cond_t mpo_cond;
extern pthread_mutex_t mpo_mutex;

extern CTRL_MSG_TYPE *ctrl_head;
extern CTRL_MSG_TYPE *ctrl_tail;
extern pthread_cond_t ctrl_cond;
extern pthread_mutex_t ctrl_mutex;

extern LOAD_MSG_TYPE *load_head;
extern LOAD_MSG_TYPE *load_tail;
extern pthread_cond_t load_cond;
extern pthread_mutex_t load_mutex;

extern UNLOAD_MSG_TYPE *unload_head;
extern UNLOAD_MSG_TYPE *unload_tail;
extern pthread_cond_t unload_cond;
extern pthread_mutex_t unload_mutex;

extern MTBUF_MSG_TYPE *mtbuf_head;
extern MTBUF_MSG_TYPE *mtbuf_tail;
extern pthread_cond_t mtbuf_cond;
extern pthread_mutex_t mtbuf_mutex;

extern OBJMSG_MSG_TYPE *objmsg_head;
extern OBJMSG_MSG_TYPE *objmsg_tail;
extern pthread_cond_t objmsg_cond;
extern pthread_mutex_t objmsg_mutex;

#endif
