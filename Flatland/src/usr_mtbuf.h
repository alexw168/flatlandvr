/*
 * usr_mtbuf.h - Header file for usr_mtbuf.c
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
 * Created: 6-21-02, Moad Mowafi
 *
 */

#ifndef USR_MTBUF_H
#define USR_MTBUF_H

#define MAXBUFS 100

class usr_mtbuf
{
 public:
  usr_mtbuf();
  ~usr_mtbuf();

  void assign(int idx, void *data, struct object *object=NULL);
  void read(int idx, void *data);
  void do_assign(int idx, void *data);
  
  struct BufHdr {
    int index;
    int num_elems;
    char type[10];
  };

  struct MtBuf {
    mutex Mutex;
    void *ptr;
    int size;
    bool filled;
    BufHdr hdr;
  };

  MtBuf buffs[MAXBUFS];
};

typedef usr_mtbuf::BufHdr MTBUF_HDR_TYPE;

int fl_make_usrmtbuf(struct object *o, char * data_type, int num_elems);
void fl_delete_usrmtbuf(struct object *o, int idx);

#endif
