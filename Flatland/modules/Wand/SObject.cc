//
// SObject.cpp
//
/*
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

#include "SObject.h"
#include "flatland_app.h"
#include <stdio.h>

void SVertex::Draw()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   v);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   v);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  v);
	glNormal3fv(v+4);
	glVertex3fv(v+7);
}
	
void SVertex::Read(FILE *f)
{
	fscanf(f,"%f%f%f%f%f%f%f%f%f%f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8],&v[9]);
	int i=0;
	while (i!='\n') i=fgetc(f);
}

void SVertex::Write(FILE *f)
{
	fprintf(f,"%8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9]);
}

SPart::SPart() 
{
	Vertex=NULL;
	Size=0;
}

SPart::~SPart()
{
	if (Vertex) 
	{
		delete[] Vertex;
	}
}

void SPart::SetSize(int n)
{
	if (Vertex) 
	{
		delete[] Vertex;
	}
	Vertex=NULL;
	Size=0;
	if (n>0) 
	{
		Vertex=new SVertex[n];
	}
	if (Vertex) 
	{
		Size=n;
	}
}

void SPart::Read(FILE *f)
{
	if (Vertex) 
	{
		delete[] Vertex;
	}
	Vertex=NULL;
	int s,i=0;
	fscanf(f,"%d",&s);
	while (i!='\n') i=fgetc(f);
	SetSize(s);
	if (Vertex)
	{
		for(i=0;i<Size;i++)
		{
			Vertex[i].Read(f);
		}
	}
}

void SPart::Write(FILE *f)
{
	int i;
	
	fprintf(f,"%d\n",Size);
	for(i=0;i<Size;i++)
	{
		Vertex[i].Write(f);
	}
}

void SPart::Draw()
{
	int i;
	glBegin(GL_TRIANGLES);
	for(i=0;i<Size;i++)
	{
		Vertex[i].Draw();
	}
	glEnd();
}

SObject::SObject() 
{
	Part=NULL;
	Size=0;
}

SObject::~SObject()
{
	if (Part) 
	{
		delete[] Part;
	}
}

void SObject::SetSize(int n)
{
	if (Part) 
	{
		delete[] Part;
	}
	Part=NULL;
	Size=0;
	if (n>0) 
	{
		Part=new SPart[n];
	}
	if (Part) 
	{
		Size=n;
	}
}

void SObject::Read(FILE *f)
{
	if (Part) 
	{
		delete[] Part;
	}
	Part=NULL;
	int s,i=0;
	fscanf(f,"%d",&s);
	while (i!='\n') i=fgetc(f);
	SetSize(s);
	if (Part)
	{
		for(i=0;i<Size;i++)
		{
			Part[i].Read(f);
		}
	}
}

void SObject::Write(FILE *f)
{
	int i;
	
	fprintf(f,"%d\n",Size);
	for(i=0;i<Size;i++)
	{
		Part[i].Write(f);
	}
}

void SObject::Draw()
{
	int i;
	for(i=0;i<Size;i++)
	{
		Part[i].Draw();
	}
}



