/*
 * genexc.h
 *
 * Header file for genexc.c
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

#ifndef GENEXC__H
#define GENEXC__H

class general_exception{
protected:
    char Operation[2];
    char Message[81];
public:
    general_exception(void);
    general_exception(char *Msg);
    general_exception(const general_exception &N);
    general_exception &operator=(const general_exception &N);
    void operation(char *Op);
    char *operation(void);
    void message(char *Msg);
    char *message(void);
};
#endif


