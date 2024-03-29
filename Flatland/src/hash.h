/*
 * hash.h
 *
 * Header for hash.cpp
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
 */

#ifndef _INCLUDE_HASH_H
#define _INCLUDE_HASH_H

//#define _DEBUG_FUNCTIONS

typedef struct hash_elem
  {
  struct object *MyObject;
  hash_elem *next;
  } HASH_ELEM;

typedef struct hash_table
  {
  int mod_val;
  int num_elements;
  HASH_ELEM **ElemList;
  } HASH_TABLE;

HASH_TABLE *CreateTable();
void InsertElement(struct object *NewObject, HASH_TABLE *CurTable);
void DeleteElement(char *elem_name, HASH_TABLE *CurTable);
struct object *FindElement(char *elem_name, HASH_TABLE *CurTable);
int GetKeyVal(char *name, int max_name_size, HASH_TABLE *CurTable);
HASH_TABLE *ResizeTable(HASH_TABLE *OldTable, int min_size);
int GetNewPrime(int min_size, int max_size);
void PrintTable(HASH_TABLE *Table);

#endif
