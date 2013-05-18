/*
hash.c.  Provides a hash lookup table for the object database.

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

#include "gheader.h"

#ifndef USE_OLD_CPP_HEADERS
using namespace std;
#endif

HASH_TABLE *CreateTable()
  {
  HASH_TABLE *NewTable;
  int i;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering CreateTable" << endl;
  #endif

  NewTable = new HASH_TABLE;
  if(!NewTable)
    {
    cout << "Could not allocate memory for NewTable.  Exiting." << endl;
    exit(1);
    }

  NewTable->mod_val = 131;
  NewTable->num_elements = 0;

  NewTable->ElemList = new HASH_ELEM *[NewTable->mod_val];
  if(!NewTable->ElemList)
    {
    cout << "Could not allocate memory for NewTable->ElemList.  Exiting."
         << endl;
    exit(1);
    }

  for(i = 0; i < 131; i++)
    {
    NewTable->ElemList[i] = NULL;
    }
  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving CreateTable" << endl;
  #endif
  return(NewTable);
  }

void InsertElement(OBJECT_TYPE *NewObject, HASH_TABLE *CurTable)
  {
  HASH_ELEM *NewElem;
  HASH_ELEM *CurElem;
  bool done;
  int key_val;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering InsertElement" << endl;
  #endif

  NewElem = new HASH_ELEM;
  if(!NewElem)
    {
    cout << "Could not allocate memory for NewElem.  Exiting." << endl;
    exit(1);
    }

  NewElem->next = NULL;

  NewElem->MyObject = NewObject;

  key_val = GetKeyVal(NewObject->name, 20, CurTable);
  //cout << "Key val: " << key_val << endl;
  //cout << "Maximum key val: " << CurTable->mod_val << endl;

  if(CurTable->ElemList[key_val])
    {
    CurElem = CurTable->ElemList[key_val];
    done = FALSE;
    while(!done)
      {
      if(strcmp(CurElem->MyObject->name, NewObject->name) == 0)
        {
        cout << "Duplicate object name.  Ignoring insert." << endl;
        done = TRUE;
        }
      else if(CurElem->next == NULL)
        {
        // The next chain element is free.  Insert our new element there.
        CurElem->next = NewElem;
        NewElem = NULL;
        CurTable->num_elements++;
        done = TRUE;
        }
      else
        {
        // Check the next chain element.
        CurElem = CurElem->next;
        }
      }
    }
  else
    {
    CurTable->ElemList[key_val] = NewElem;
    CurTable->num_elements++;
    }

  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving InsertElement" << endl;
  #endif
  }

void DeleteElement(char *elem_name, HASH_TABLE *CurTable)
  {
  HASH_ELEM *CurElem;
  HASH_ELEM *LastElem;
  int key_val;
  bool done;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering DeleteElement" << endl;
  #endif

  key_val = GetKeyVal(elem_name, 20, CurTable);

  LastElem = NULL;
  if(CurTable->ElemList[key_val])
    {
    CurElem = CurTable->ElemList[key_val];
    done = FALSE;
    while(!done)
      {
      if(strcmp(CurElem->MyObject->name, elem_name) == 0)
        {
        if(LastElem == NULL)
          {
          CurTable->ElemList[key_val] = CurElem->next;
          // Delete CurElem here...
          CurElem->MyObject = NULL;
          free(CurElem);
          CurElem = NULL;
          }
        else
          {
          LastElem->next = CurElem->next;
          // Delete CurElem here...
          CurElem->MyObject = NULL;
          free(CurElem);
          CurElem = NULL;
          }
        CurTable->num_elements--;
        done = TRUE;
        }
      else if(CurElem == NULL)
        {
        #ifdef _DEBUG_FUNCTIONS
        cout << "No object " << elem_name << " exists to delete." << endl;
        #endif
        done = TRUE;
        }
      else
        {
        // Check the next chain element.
        LastElem = CurElem;
        CurElem = CurElem->next;
        }
      }
    }
  else
    {
    #ifdef _DEBUG_FUNCTIONS
    cout << "No object " << elem_name << " exists to delete." << endl;
    #endif
    }
  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving DeleteElement" << endl;
  #endif
  }

OBJECT_TYPE *FindElement(char *elem_name, HASH_TABLE *CurTable)
  {
  HASH_ELEM *CurElem;
  OBJECT_TYPE *CurObject;
  bool done;
  int key_val;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering FindElement" << endl;
  #endif

  key_val = GetKeyVal(elem_name, 20, CurTable);

  CurObject = NULL;
  if(CurTable->ElemList[key_val])
    {
    CurElem = CurTable->ElemList[key_val];
    done = FALSE;
    while(!done)
      {
      if(CurElem == NULL)
        {
        #ifdef _DEBUG_FUNCTIONS
        cout << "No match on name " << elem_name << endl;
        #endif
        done = TRUE;
        }
      else if(strcmp(CurElem->MyObject->name, elem_name) == 0)
        {
        CurObject = CurElem->MyObject;
        done = TRUE;
        }
      else
        {
        // Check the next chain element.
        CurElem = CurElem->next;
        }
      }
    }
  else
    {
    #ifdef _DEBUG_FUNCTIONS
    cout << "No match on name " << elem_name << endl;
    #endif
    }
  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving FindElement" << endl;
  #endif
  return(CurObject);
  }

int GetKeyVal(char *name, int max_name_size, HASH_TABLE *CurTable)
  {
  int key_val;
  int multiplier;
  int i;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering GetKeyVal" << endl;
  #endif

  key_val = 0;
  multiplier = 1;
  for(i = 0; i < max_name_size; i++)
    {
    if(name[i] != '\0')
      {
      key_val += (name[i] * multiplier);
      if(multiplier == 1)
        {
        multiplier = 0;
        }
      multiplier += 128;
      }
    else
      {
      i = max_name_size;
      }
    }
  key_val %= CurTable->mod_val;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving GetKeyVal" << endl;
  #endif
  return(key_val);
  }

HASH_TABLE *ResizeTable(HASH_TABLE *OldTable, int min_size)
  {
  HASH_TABLE *NewTable;
  HASH_ELEM **NewElems;
  HASH_ELEM *CurElem;
  HASH_ELEM *NextElem;
  int new_mod;
  int max_size;
  int i;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering ResizeTable" << endl;
  #endif

  NewTable = new HASH_TABLE;
  if(!NewTable)
    {
    cout << "Could not allocate memory for NewTable.  Exiting." << endl;
    exit(1);
    }

  max_size = min_size * 2;
  new_mod = GetNewPrime(min_size, max_size);
  while(new_mod <= min_size)
    {
    max_size += min_size;
    new_mod = GetNewPrime(min_size, max_size);
    }

  NewTable->mod_val = new_mod;
  NewTable->num_elements = 0;

  NewElems = new HASH_ELEM *[new_mod];
  if(!NewElems)
    {
    cout << "Could not allocate memory for NewElems." << endl;
    return(OldTable);
    }

  for(i = 0; i < new_mod; i++)
    {
    NewElems[i] = NULL;
    }

  NewTable->ElemList = NewElems;

  for(i = 0; i < OldTable->mod_val; i++)
    {
    CurElem = OldTable->ElemList[i];
    while(CurElem != NULL)
      {
      NextElem = CurElem->next;
      CurElem->next = NULL;
      InsertElement(CurElem->MyObject, NewTable);
      CurElem->MyObject = NULL;
      free(CurElem);
      CurElem = NextElem;
      }
    }
  free(OldTable);
  OldTable = NULL;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving ResizeTable" << endl;
  #endif
  return(NewTable);
  }

int GetNewPrime(int min_size, int max_size)
  {
  int i;
  int j;
  bool got_prime;

  #ifdef _DEBUG_FUNCTIONS
  cout << "Entering GetNewPrime" << endl;
  #endif

  got_prime = FALSE;
  for(i = max_size;  i > min_size; i--)
    {
    for(j = (int) sqrt((float)i); j > 1; j--)
      {
      if((i % j) == 0)
        {
        break;
        }
      }
    if(j == 1)
      {
      got_prime = TRUE;
      break;
      }
    }
  #ifdef _DEBUG_FUNCTIONS
  cout << "Leaving GetNewPrime" << endl;
  #endif
  if(got_prime)
    {
    return(i);
    }
  return(0);
  }

void PrintTable(HASH_TABLE *Table)
  {
  HASH_ELEM *CurElem;
  int i;

  cout << "Hash Table by name: " << endl;
  cout << "Number of elements: " << Table->num_elements << endl;
  cout << "Number of keys: " << Table->mod_val << endl;
  cout << endl;
  for(i = 0; i < Table->mod_val; i++)
    {
    CurElem = Table->ElemList[i];
    while(CurElem != NULL)
      {
      cout << " " << CurElem->MyObject->name << " ";
      CurElem = CurElem->next;
      if(CurElem != NULL)
        {
        cout << "->";
        }
      }
    cout << endl;
    }
  cout << endl;
  }
