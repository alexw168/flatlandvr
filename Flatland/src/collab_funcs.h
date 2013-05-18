/*
 * collab_funcs.h - Header file for collab_funcs.c 
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
 * Created: 6-21-02, Moad Mowafi
 *
 */

#ifndef COLLAB_FUNCS_H
#define COLLAB_FUNCS_H

void fl_collab_send_ctrl_msg(struct object *object, char key);
void fl_collab_send_mpo_msg(struct object *object, float *P);
void fl_collab_send_mtbuf_msg(struct object *object, MTBUF_HDR_TYPE hdr, int size, void *data);
void fl_collab_send_objmsg_msg(struct object *from_object, struct object *to_object, char *message);
void fl_collab_send_load_msg(struct apptype *App);
void fl_collab_send_unload_msg(struct apptype *App);
void fl_collab_controlfunc(struct object *object, char key);

#endif
