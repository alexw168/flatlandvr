/* ray_triangle_intersect.c

   Ray-Triangle intersection algorithm.

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

   Author: Takeshi Hakamata <tkh@ahpcc.unm.edu>
*/

#include <stdio.h>
#include <math.h>
#include "gheader.h"
#include "vec3f.h"
#include "intersect.h"
#include "ray_triangle_intersect.h"

using namespace std;

// Internal function prototypes.
static char  segmentTriangleCross(vec3f T[3], vec3f& q, vec3f& r);
static char  inPlane(vec3f T[3], int m, vec3f& q, vec3f& r, vec3f& p);
static char  inTriangle2D(vec3f Tp[3], vec3f& pp);
static char  inTriangle3D(vec3f T[3], vec3f& p, int m);
static char  intersectSegmentPlane(vec3f T[3], vec3f& q, vec3f& r, vec3f& p, int& m);
static int   planeCoefficient(vec3f T[3], vec3f& N, float& T0dotN);
static vec3f computeNormalVector(vec3f& a, vec3f& b, vec3f& c);
static int   volumeSign(vec3f& a, vec3f& b, vec3f& c, vec3f& p);
static int   areaSign(vec3f& a, vec3f& b, vec3f& c);


static int
areaSign(vec3f& a, vec3f& b, vec3f& c)
{
    float area2 = (b[0] - a[0]) * (c[1] - a[1]) -
    		  (c[0] - a[0]) * (b[1] - a[1]);

    if (area2 > 0.5f)
    {
    	return 1;
    }
    else if (area2 < -0.5f)
    {
    	return -1;
    }

    return 0;
}

static int
volumeSign(vec3f& a, vec3f& b, vec3f& c, vec3f& p)
{
    float ax = a[X] - p[X];
    float ay = a[Y] - p[Y];
    float az = a[Z] - p[Z];
    float bx = b[X] - p[X];
    float by = b[Y] - p[Y];
    float bz = b[Z] - p[Z];
    float cx = c[X] - p[X];
    float cy = c[Y] - p[Y];
    float cz = c[Z] - p[Z];

    float vol = ax * (by * cz - bz * cy) +
		ay * (bz * cx - bx * cz) +
		az * (bx * cy - by * cx);

    if (vol > 0.5f)
    {
    	return 1;
    }
    else if (vol < -0.5f)
    {
    	return -1;
    }

    return 0;
}

static vec3f
computeNormalVector(vec3f& a, vec3f& b, vec3f& c)
{
    vec3f ca = c - a;
    vec3f ba = b - a;

    return ba.cross(ca);
}

static int
planeCoefficient(vec3f T[3], vec3f& N, float& T0dotN)
{
    N      = computeNormalVector(T[0], T[1], T[2]);
    T0dotN = T[0].dot(N);

    if (fabs(N[0]) < fabs(N[1]))
    {
	if (fabs(N[1]) < fabs(N[2]))
	{
	    return 2;
	}
	else
	{
	    return 1;
	}
    }

    return 0;
}


static char
intersectSegmentPlane(vec3f T[3], vec3f& q, vec3f& r, vec3f& p, int& m)
{
    vec3f N;
    float D;
    m = planeCoefficient(T, N, D);

    float num   = D - q.dot(N);
    vec3f rq    = r - q;
    float denom = rq.dot(N);
    float t;

    if (denom == 0.0f)
    {
   	if (num == 0.0f)
	{
	    return 'p';
	}
	else
	{
	    return NO_COLLISION;
	}
    }
    else
    {
    	t = num / denom;
    }

    for(int i = 0; i < 3; i++)
    {
    	p[i] = q[i] + t * (r[i] - q[i]);
    }

    if ((t > 0.0f) && (t < 1.0f))
    {
    	return '1';
    }
    else if (num == 0.0f)
    {
    	return 'q';
    }
    else if (num == denom)
    {
    	return 'r';
    }

    return '0';
}

static char
inTriangle3D(vec3f T[3], int m, vec3f& p) 
{
    vec3f pp;     // Projected p.
    vec3f Tp[3];  // Projected triangle.
    int j = 0;

    // Project out the coordinate m in both p and the triangular surface.
    for(int i = 0; i < 3; i++)
    {
    	if (i != m)
	{
	    pp[j] = p[i];

	    for(int k = 0; k < 3; k++)
	    {
	    	Tp[k][j] = T[k][i];
	    }

	    j++;
	}
    }

    return inTriangle2D(Tp, pp);
}

static char
inTriangle2D(vec3f Tp[3], vec3f& pp)
{
    int area[3];

    area[0] = areaSign(pp,  Tp[0], Tp[1]);
    area[1] = areaSign(pp,  Tp[1], Tp[2]);
    area[2] = areaSign(pp,  Tp[2], Tp[0]);

    if ((area[0] == 0) && (area[1] > 0) && (area[2] > 0) ||
	(area[1] == 0) && (area[0] > 0) && (area[2] > 0) ||
	(area[2] == 0) && (area[0] > 0) && (area[1] > 0))
    {
    	return 'E';
    }

    if ((area[0] == 0) && (area[1] < 0) && (area[2] > 0) ||
	(area[1] == 0) && (area[0] < 0) && (area[2] > 0) ||
	(area[2] == 0) && (area[0] < 0) && (area[1] > 0))
    {
    	return 'E';
    }

    if ((area[0] > 0) && (area[1] > 0) && (area[2] > 0) ||
    	(area[0] < 0) && (area[1] < 0) && (area[2] < 0))
    {
    	return 'F';
    }

    if ((area[0] == 0) && (area[1] == 0) && (area[2] == 0))
    {
    	//fprintf(stderr, "Error in inTriangle2D\n");
    }

    if ((area[0] == 0) && (area[1] == 0) ||
    	(area[0] == 0) && (area[2] == 0) ||
    	(area[1] == 0) && (area[2] == 0))
    {
    	return 'V';
    }

    return NO_COLLISION;
}

static char
inPlane(vec3f T[3], int m, vec3f& q, vec3f& r, vec3f& p)
{
    // Simply return p. This is what the book does.
    return 'p';
}

static char
segmentTriangleCross(vec3f T[3], vec3f& q, vec3f& r)
{
    int vol[3];

    vol[0] = volumeSign(q, T[0], T[1], r);
    vol[1] = volumeSign(q, T[1], T[2], r);
    vol[2] = volumeSign(q, T[2], T[0], r);

    if ((vol[0] > 0) && (vol[1] > 0) && (vol[2] > 0) ||
    	(vol[0] < 0) && (vol[1] < 0) && (vol[2] < 0))
    {
    	return 'f';
    }

    if ((vol[0] > 0) || (vol[1] > 0) || (vol[2] > 0) &&
    	(vol[0] < 0) || (vol[1] < 0) || (vol[2] < 0))
    {
    	return NO_COLLISION;
    }
    else if ((vol[0] == 0) && (vol[1] == 0) && (vol[2] == 0))
    {
    	//fprintf(stderr, "Error in Collide::segmentTriangleCross().\n");
	return COL_INVALID;
    }
    else if ((vol[0] == 0) && (vol[1] == 0) ||
    	     (vol[0] == 0) && (vol[2] == 0) ||
    	     (vol[1] == 0) && (vol[2] == 0))
    {
    	return 'v';
    }
    else if ((vol[0] == 0)|| (vol[1] == 0) || (vol[2] == 0))
    {
    	return 'e';
    }

    //fprintf(stderr, "Error in Collide::segmentTriangleCross().\n");
    return COL_INVALID;
}

// T: A triangle
// q: origin of the segment
// r: end point of the segment
// p: intersection point on the plane
char
intersectSegmentTriangle(vec3f T[3], vec3f& q, vec3f& r, vec3f& p)
{
    int m;
    char code = intersectSegmentPlane(T, q, r, p, m);

    if (code == 'q')
    {
	return inTriangle3D(T, m, q);
    }
    else if (code == 'r')
    {
	return inTriangle3D(T, m, r);
    }
    else if (code == 'p')
    {
	return inPlane(T, m, q, r, p);
    }
    else
    {
	return segmentTriangleCross(T, q, r);
    }
}

static bool
collideBeamBBox(float beam_p[][4], float p[][4])
{
    vec3f verts[8];
    char ret;

    verts[0].set(p[1][X], p[1][Y], p[1][Z]);
    verts[1].set(p[2][X], p[2][Y], p[2][Z]);
    vec3f tmp = verts[1] - verts[0];
    verts[2] = verts[2] + tmp;
    verts[3].set(p[3][X], p[3][Y], p[3][Z]);
    verts[4].set(p[4][X], p[4][Y], p[4][Z]);
    vec3f offset = verts[4] - verts[0];
    verts[5] = verts[1] + offset;
    verts[6] = verts[2] + offset;
    verts[7] = verts[3] + offset;

    vec3f q(beam_p[0][X], beam_p[0][Y], beam_p[0][Z]);
    vec3f r(beam_p[1][X], beam_p[1][Y], beam_p[1][Z]);
    vec3f triangle[3];
    vec3f intersectPoint;

    triangle[0] = verts[0];
    triangle[1] = verts[1];
    triangle[2] = verts[2];

    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 1) %c\n", ret);
	return true;
    }

    triangle[0] = verts[0];
    triangle[1] = verts[2];
    triangle[2] = verts[3];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 2) %c\n", ret);
	return true;
    }

    triangle[0] = verts[4];
    triangle[1] = verts[5];
    triangle[2] = verts[6];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 3) %c\n", ret);
	return true;
    }

    triangle[0] = verts[4];
    triangle[1] = verts[6];
    triangle[2] = verts[7];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 4) %c\n", ret);
	return true;
    }

    triangle[0] = verts[1];
    triangle[1] = verts[5];
    triangle[2] = verts[6];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 5) %c\n", ret);
	return true;
    }

    triangle[0] = verts[1];
    triangle[1] = verts[6];
    triangle[2] = verts[2];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 6) %c\n", ret);
	return true;
    }

    triangle[0] = verts[0];
    triangle[1] = verts[4];
    triangle[2] = verts[7];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 7) %c\n", ret);
	return true;
    }

    triangle[0] = verts[0];
    triangle[1] = verts[7];
    triangle[2] = verts[3];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 8) %c\n", ret);
	return true;
    }

    triangle[0] = verts[0];
    triangle[1] = verts[1];
    triangle[2] = verts[5];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 9) %c\n", ret);
	return true;
    }

    triangle[0] = verts[0];
    triangle[1] = verts[5];
    triangle[2] = verts[4];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 10) %c\n", ret);
	return true;
    }

    triangle[0] = verts[3];
    triangle[1] = verts[2];
    triangle[2] = verts[6];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 11) %c\n", ret);
	return true;
    }

    triangle[0] = verts[3];
    triangle[1] = verts[6];
    triangle[2] = verts[7];
    ret = intersectSegmentTriangle(triangle, q, r, intersectPoint);
    if ((ret != COL_INVALID) && (ret != NO_COLLISION))
    {
	fprintf(stderr, "(tri 12) %c\n", ret);
	return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////
//
// External functions
//
///////////////////////////////////////////////////////////////

void
fl_bbox_intersection_init(UNIVERSE_TYPE* w)
{
    OBJECT_TYPE* obj;

    for(int i = 0; i < w->nobjects; i++)
    {
    	obj = w->database[i].object;
	obj->beam_rect_hit_flag = 0;
    }
}

void
fl_bbox_intersection_test(OBJECT_TYPE* beamer, OBJECT_TYPE* beamed, BV_TYPE* bv)
{
    float beam_p[2][4];
    float points[5][4];

#if 0
    fprintf(stderr, "(fl_bbox_intersection_test) Entered.\n");
#endif

    fl_tpc_matrix_multiply(beamer->Mco, beamer->bounding_volume_set->bvset[2]->p[0], beam_p[0]);
    fl_tpc_matrix_multiply(beamer->Mco, beamer->bounding_volume_set->bvset[2]->p[1], beam_p[1]);

    if (beamed && beamed->drawfunc && beamed->drawing_on_flag && bv->npoints == 5)
    {
	// If there is a rectilinear bounding volume,
	// do beam-rectilinear intersection test for finer
	// collision detection.

	// Transform all the bounding box points into eye coordinate.
	fl_tpc_matrix_multiply(beamed->Mco, bv->p[0], points[0]);
	fl_tpc_matrix_multiply(beamed->Mco, bv->p[1], points[1]);
	fl_tpc_matrix_multiply(beamed->Mco, bv->p[2], points[2]);
	fl_tpc_matrix_multiply(beamed->Mco, bv->p[3], points[3]);
	fl_tpc_matrix_multiply(beamed->Mco, bv->p[4], points[4]);

	// If beam doesn't intersect with bounding box, collision flag set by
	// sphere_interaction_bv() needs to be set to NONCOLLISION.
	if (!collideBeamBBox(beam_p, points))
	{
	    int beamer_index = find_object_in_intersection_db_locked(bv->database, beamer->name ) ;
	    if (beamer_index == -1) {
        	beamer_index = find_empty_slot_intersection_db_locked(&bv->database);
        	bv->database->interactions[beamer_index]->beamer_object = beamer;
            } 

            BV_INTERACTIONS_TYPE *inter = bv->database->interactions[beamer_index];
	    inter->collision_state = NOTCOLLISION;

	    // XXXtkh
	    // Right now commenting out this, since it causes core dump.
	    //(*bv->bvuncollisionfunc)(bv, bv->database->interactions[beamer_index]);

	    fprintf(stderr, "No collision: object is %s.\n", beamed->name);

	} else {
	    fprintf(stderr, "Collision detected: object is %s.\n", beamed->name);
	}
    } else {
	fprintf(stderr, "(fl_bbox_intersection_test) Sanity check failed: npoints: %d\n", bv->npoints);
    }

#if 0
    fprintf(stderr, "(fl_bbox_intersection_test) Exiting.\n");
#endif
}
