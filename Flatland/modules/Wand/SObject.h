//
// SObject.h
//  
//  Copyright (C) 2003, University of New Mexico.
//  
//      This library is free software; you can redistribute it and/or
//      modify it under the terms of the GNU Lesser General Public
//      License as published by the Free Software Foundation; either
//      version 2.1 of the License, or (at your option) any later version.
//  
//      This library is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//      Lesser General Public License for more details.
//  
//      You should have received a copy of the GNU Lesser General Public
//      License along with this library; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//

#ifndef SOBJECT_H
#define SOBJECT_H

#include <stdio.h>

struct SVertex 
{
	float v[10];

	SVertex() {}
	~SVertex() {}
	void Draw();
	void Read(FILE *f);
	void Write(FILE *f);
};

struct SPart 
{
	int Size;
	SVertex *Vertex;

	SPart();
	~SPart();
	void SetSize(int n);
	void Read(FILE *f);
	void Write(FILE *f);
	void Draw();
};

struct SObject 
{
	int Size;
	SPart *Part;

	SObject();
	~SObject();
	void SetSize(int n);
	void Read(FILE *f);
	void Write(FILE *f);
	void Draw();
};

#endif //ndef SOBJECT_H
