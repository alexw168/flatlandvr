//
// GL_Object.h
//
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

#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <stdio.h>
#include <GL/gl.h>

struct SGL_Vertex 
{
	float v[12]; // T2F_C4F_N3F_V3F

	void Draw(bool UseTexture);
	void GetTexture(float &s, float &t) { s=v[0]; t=v[1]; }
	void SetTexture(float s, float t) { v[0]=s; v[1]=t; }
	void GetColor(float &r, float &g, float &b, float &a) { r=v[2]; g=v[3]; b=v[4]; a=v[5]; }
	void SetColor(float r, float g, float b, float a) { v[2]=r; v[3]=g; v[4]=b; v[5]=a; }
	void GetNormal(float &x, float &y, float &z) { x=v[6]; y=v[7]; z=v[8]; }
	void SetNormal(float x, float y, float z) { v[6]=x; v[7]=y; v[8]=z; }
	void GetPosition(float &x, float &y, float &z) { x=v[9]; y=v[10]; z=v[11]; }
	void SetPosition(float x, float y, float z) { v[9]=x; v[10]=y; v[11]=z; }
	void Read(FILE *f);
	void Write(FILE *f);
};

struct SGL_Part 
{
	int Size;
	SGL_Vertex *Vertex;
	char *Filename;
	GLuint TextureName; // OpenGL texture name (if loaded, 0 if not)

	SGL_Part();
	~SGL_Part();
	void SetSize(int n);
	void Read(FILE *f);
	void Write(FILE *f);
	void Draw(bool UseTexture);
};

struct SGL_Object 
{
	int Size;
	SGL_Part *Part;

	SGL_Object();
	~SGL_Object();
	void SetSize(int n);
	void Read(FILE *f);
	void Write(FILE *f);
	void Draw(bool UseTexture = true);
};

#endif //ndef GL_OBJECT_H
