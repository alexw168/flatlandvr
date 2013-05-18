//
// TransformationNode.h
//
// Include file for TransformationNode.cc "API"
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
//

#ifndef TRANSFORMATIONNODE_H
#define TRANSFORMATIONNODE_H

typedef struct TransformationNodeAccessMembers
{
	int (*SetTracking)(OBJECT_TYPE *TransformationNodeObject, bool tracking);
	int (*GetTracking)(OBJECT_TYPE *TransformationNodeObject, bool *tracking);
	int (*SetTrackerClient)(OBJECT_TYPE *TransformationNodeObject, char *TrackerClientName);
	int (*GetTrackerClient)(OBJECT_TYPE *TransformationNodeObject, char *TrackerClientName);
	int (*SetTransducer)(OBJECT_TYPE *TransformationNodeObject, int transducer);
	int (*GetTransducer)(OBJECT_TYPE *TransformationNodeObject, int *transducer);
	int (*DisconnectTracking)(OBJECT_TYPE *TransformationNodeObject);
	int (*SetDefaultPosition)(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position);
	int (*GetDefaultPosition)(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position);
	int (*GetPosition)(OBJECT_TYPE *TransformationNodeObject, CORDORD *Position);
} TRANSFORMATION_NODE_ACCESS_MEMBERS;

#endif //ndef TRANSFORMATIONNODE_H
