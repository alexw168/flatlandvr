/*
 * collab_init.h - Header file needed by collab_funcs.c
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
 * Created: 3-15-02, Moad Mowafi
 *
 */

#ifndef COLLAB_INIT_H
#define COLLAB_INIT_H

MPO_MSG_TYPE *mpo_head = NULL;
MPO_MSG_TYPE *mpo_tail = NULL;
pthread_cond_t mpo_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mpo_mutex = PTHREAD_MUTEX_INITIALIZER;

CTRL_MSG_TYPE *ctrl_head = NULL;
CTRL_MSG_TYPE *ctrl_tail = NULL;
pthread_cond_t ctrl_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t ctrl_mutex = PTHREAD_MUTEX_INITIALIZER;

LOAD_MSG_TYPE *load_head = NULL;
LOAD_MSG_TYPE *load_tail = NULL;
pthread_cond_t load_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t load_mutex = PTHREAD_MUTEX_INITIALIZER;

UNLOAD_MSG_TYPE *unload_head = NULL;
UNLOAD_MSG_TYPE *unload_tail = NULL;
pthread_cond_t unload_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t unload_mutex = PTHREAD_MUTEX_INITIALIZER;

MTBUF_MSG_TYPE *mtbuf_head = NULL;
MTBUF_MSG_TYPE *mtbuf_tail = NULL;
pthread_cond_t mtbuf_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

OBJMSG_MSG_TYPE *objmsg_head = NULL;
OBJMSG_MSG_TYPE *objmsg_tail = NULL;
pthread_cond_t objmsg_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t objmsg_mutex = PTHREAD_MUTEX_INITIALIZER;

#endif
