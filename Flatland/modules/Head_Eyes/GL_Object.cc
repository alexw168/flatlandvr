//
// GL_Object.cpp
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

//#include "stdafx.h"

#include "GL_Object.h"
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>

void SGL_Vertex::Draw(bool UseTexture)
{
	if (UseTexture)
	{
		glTexCoord2fv( v );
	}
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   v+2 );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   v+2 );
//	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  v+2 );
	glNormal3fv( v+6 );
	glVertex3fv( v+9 );
}
	
void SGL_Vertex::Read(FILE *f)
{
	fscanf( f, "%f%f%f%f%f%f%f%f%f%f%f%f", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9], &v[10], &v[11] );
	int i = 0;
	while (i!='\n') i = fgetc( f );

//	v[6] *= -1;
//	v[7] *= -1;
//	v[8] *= -1;
}

void SGL_Vertex::Write(FILE *f)
{
	fprintf( f, "%f %f %f %f %f %f %f %f %f %f %f %f\n", v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11] );
}

SGL_Part::SGL_Part() 
{
	Vertex = NULL;
	Size = 0;
	Filename = NULL;
	TextureName = 0;
}

SGL_Part::~SGL_Part()
{
	if (Vertex) 
	{
		delete[] Vertex;
	}
	Vertex = NULL;
	if (Filename) 
	{
		free( Filename );
	}
	Filename = NULL;
	if (TextureName) 
	{
		glDeleteTextures( 1, &TextureName );
	}
	TextureName = -1;
}

void SGL_Part::SetSize(int n)
{
	if (Vertex) 
	{
		delete[] Vertex;
	}
	Vertex=NULL;
	Size=0;
	if (n>0) 
	{
		Vertex=new SGL_Vertex[n];
	}
	if (Vertex) 
	{
		Size=n;
	}
}

void SGL_Part::Read(FILE *f)
{
	char str[1000]={""};
	if (Filename) 
	{
		free(Filename);
	}
	Filename=NULL;
	if (Vertex) 
	{
		delete[] Vertex;
	}
	Vertex=NULL;
	int s,t,i=0;
	fscanf(f,"%d",&t);
	i=fgetc(f);
	fscanf(f,"%s",str);
	if (t)
	{
		Filename = strdup(str);
	}
	else
	{
		Filename = NULL;
	}
	while (i!='\n') i=fgetc(f);
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

void SGL_Part::Write(FILE *f)
{
	int i;
	fprintf(f,"%d\n",Size);
	fprintf(f,"%d %s\n",Filename ? 1 : 0, Filename ? Filename : "none" );
	for(i=0;i<Size;i++)
	{
		Vertex[i].Write(f);
	}
}

void SGL_Part::Draw(bool UseTextures)
{
	int i;
	bool ut = UseTextures && ( TextureName >= -1 );
	if (ut)
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, TextureName );
	}
	else
	{
		// disable textures
		// should already be disabled by default...
		glDisable( GL_TEXTURE_2D );
	}
	glBegin(GL_TRIANGLES);
	for(i=0;i<Size;i++)
	{
		Vertex[i].Draw(ut);
	}
	glEnd();
	if (ut)
	{
		glDisable( GL_TEXTURE_2D );
	}
}

SGL_Object::SGL_Object() 
{
	Part=NULL;
	Size=0;
}

SGL_Object::~SGL_Object()
{
	if (Part) 
	{
		delete[] Part;
	}
}

void SGL_Object::SetSize(int n)
{
	if (Part) 
	{
		delete[] Part;
	}
	Part=NULL;
	Size=0;
	if (n>0) 
	{
		Part=new SGL_Part[n];
	}
	if (Part) 
	{
		Size=n;
	}
}

void SGL_Object::Read(FILE *f)
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

void SGL_Object::Write(FILE *f)
{
	int i;
	fprintf(f,"%d\n",Size);
	for(i=0;i<Size;i++)
	{
		Part[i].Write(f);
	}
}

void SGL_Object::Draw(bool UseTextures)
{
	int i;
	for(i=0;i<Size;i++)
	{
		Part[i].Draw(UseTextures);
	}
}

