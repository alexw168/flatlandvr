
//
//  invert_normals.cc
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

#include "GL/glut.h"
#include "SObject.h"

int main(int argc, char **argv)
{
	SObject object;
	SPart *part;
	SVertex *vertex;
	FILE *f;
	char *filename;
	int i,j;
	
	if (argc != 2)
	{
		fprintf(stdout, "Usage: %s <data_file>\n", argv[0] );
		return -1;
	}
	
	filename = argv[1];
	
	f = fopen(filename,"rb");
	if (!f)
	{
		fprintf(stdout, "Unable to open \"%s\"\n", filename );
		return -1;
	}

	object.Read(f);
	
	fclose(f);
	
	for(i=0;i<object.Size;i++)
	{
		part = &(object.Part[i]);
		
		for(j=0;j<part->Size;j++)
		{
			vertex = &(part->Vertex[j]);
			
			vertex->v[4] *= -1.0f;
			vertex->v[5] *= -1.0f;
			vertex->v[6] *= -1.0f;
		}
	}
	
	f = fopen(filename,"wb");

	object.Write(f);
	
	fclose(f);
	
	return 0;
}
