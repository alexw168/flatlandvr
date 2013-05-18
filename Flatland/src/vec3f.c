/* vec3f.cxx

   Float vector class

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

#ifndef USE_OLD_CPP_HEADERS
#	include <iostream>
#else
#	include <iostream.h>
#endif

#include <math.h>
#include "vec3f.h"

#ifndef USE_OLD_CPP_HEADERS
using namespace std;
#endif

enum {
  X = 0,
  Y,
  Z
};



//////////////////////////////////////////////////////////////
//
// Non member operators
//
//////////////////////////////////////////////////////////////

vec3f
operator+(vec3f& lhs, vec3f& rhs)
{
  return vec3f(lhs[X] + rhs[X], lhs[Y] + rhs[Y], lhs[Z] + rhs[Z]);
}

vec3f
operator-(vec3f& lhs, vec3f& rhs)
{
  return vec3f(lhs[X] - rhs[X], lhs[Y] - rhs[Y], lhs[Z] - rhs[Z]);
}

ostream&
operator<<(ostream& out, vec3f& rhs)
{
  return out << "(" << rhs[X] << ", "
		    << rhs[Y] << ", "
		    << rhs[Z] << ")";
}


//////////////////////////////////////////////////////////////
//
// Member functions
//
//////////////////////////////////////////////////////////////

vec3f::vec3f()
{
  vec[X] = 0.0f;
  vec[Y] = 0.0f;
  vec[Z] = 0.0f;
}

vec3f::vec3f(const float& v1, const float& v2, const float& v3)
{
  vec[X] = v1;
  vec[Y] = v2;
  vec[Z] = v3;
}

vec3f::vec3f(const vec3f& aVec)
{
  float v[3];

  aVec.get(v[0], v[1], v[2]);

  vec[X] = v[X];
  vec[Y] = v[Y];
  vec[Z] = v[Z];
}

vec3f::~vec3f()
{
}

vec3f&
vec3f::operator+=(const vec3f& rhs)
{
  vec[X] += rhs.vec[X];
  vec[Y] += rhs.vec[Y];
  vec[Z] += rhs.vec[Z];

  return *this;
}

vec3f&
vec3f::operator-=(const vec3f& rhs)
{
  vec[X] -= rhs.vec[X];
  vec[Y] -= rhs.vec[Y];
  vec[Z] -= rhs.vec[Z];

  return *this;
}

vec3f&
vec3f::operator=(const vec3f& rhs)
{
  if (&rhs == this) { 
    return *this;
  } else {
    vec[X] = rhs.vec[X];
    vec[Y] = rhs.vec[Y];
    vec[Z] = rhs.vec[Z];
  }

  return *this;
}

float&
vec3f::operator[](int ind)
{
  return vec[ind];
}

void
vec3f::set(float& v1, float& v2, float& v3)
{
  vec[X] = v1;
  vec[Y] = v2;
  vec[Z] = v3;
}

void
vec3f::get(float& v1, float& v2, float& v3) const
{
  v1 = vec[X];
  v2 = vec[Y];
  v3 = vec[Z];
}

void
vec3f::normalize()
{
  float vecLen = sqrtf(dot(*this));

  vec[X] /= vecLen;
  vec[Y] /= vecLen;
  vec[Z] /= vecLen;
}

float
vec3f::dot(vec3f& v) const
{
  return vec[X] * v[X] + vec[Y] * v[Y] + vec[Z] * v[Z];
}

// Perform vector cross product this x rhs.
vec3f
vec3f::cross(vec3f& rhs) const
{
  vec3f vOut;

  vOut[X] = vec[Y] * rhs[Z] - rhs[Y] * vec[Z];
  vOut[Y] = rhs[X] * vec[Z] - vec[X] * rhs[Z];
  vOut[Z] = vec[X] * rhs[Y] - rhs[X] * vec[Y];

  return vOut;
}
