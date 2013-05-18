/*
genexc.c.  general purpose exception handler

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
 * Created: 1-13-00
 */

#include "gheader.h"

general_exception::general_exception(void)
{
   strcpy(Operation,"");
   strcpy(Message,"Exception Encountered");
}

general_exception::general_exception(char *Msg)
{
   strcpy(Message,Msg);
   strcpy(Operation,"");
}

general_exception::general_exception(const general_exception &N)
{
    strcpy(Operation,N.Operation);
    strcpy(Message,N.Message);
}

general_exception &general_exception::operator=(const general_exception &N)
{

    strcpy(Operation,N.Operation);
    strcpy(Message,N.Message);
    return(*this);
}
void general_exception::operation(char *Op)
{
    strcpy(Operation,Op);
}

char *general_exception::operation(void)
{

    return(Operation);
}


void general_exception::message(char *Msg)
{
    strcpy(Message,Msg);
}

char *general_exception::message(void)
{
    return(Message);
}

