// vec3f.h
// Float vector class
//
// Copyright (C) 2003, University of New Mexico.
// 
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//           
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Takeshi Hakamata <tkh@ahpcc.unm.edu>

#ifndef vec3f_h
#define vec3f_h

#ifndef USE_OLD_CPP_HEADERS
#	include <iostream>
#else
#	include <iostream.h>
#endif



class vec3f {
private:
  float vec[3];

public:
  vec3f();
  vec3f(const float& v1, const float& v2, const float& v3);
  vec3f(const vec3f& aVec);
  ~vec3f();

  vec3f& operator+= (const vec3f& rhs);
  vec3f& operator-= (const vec3f& rhs);
  vec3f& operator=  (const vec3f& rhs);
  float& operator[] (int ind);

  void  set(float& v1, float& v2, float& v3);
  void  get(float& v1, float& v2, float& v3) const;
  void  normalize();
  float dot(vec3f& aVec)  const;
  vec3f cross(vec3f& rhs) const;
};

vec3f    operator+ (vec3f& lhs, vec3f& rhs);
vec3f    operator- (vec3f& lhs, vec3f& rhs);
#ifndef USE_OLD_CPP_HEADERS
std::ostream& operator<<(std::ostream& out, vec3f& rhs);
#else
ostream& operator<<(ostream& out, vec3f& rhs);
#endif
#endif //vec3f_h
