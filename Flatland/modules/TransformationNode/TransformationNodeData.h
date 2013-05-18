//
// TransformationNodeData.h
//
// Include file for TransformationNode.c
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

#ifndef TRANSFORMATIONNODEDATA_H
#define TRANSFORMATIONNODEDATA_H

typedef struct transformation_node_data_members
{
/////// STAY OUT OF THESE ///////////////////////////////////////////////
	char TrackerClientName[MAX_NAME_SIZE];	// tracker client
	bool TrackerClientSet;					// was tracker client specified
	OBJECT_TYPE *TrackerClient;				// where is the tracker
	int TransducerNumber;					// tracker id
	bool TransducerSet;						// was tracker specified
	bool UseTracking;						// use tracker info? default true
	bool UseTranslation;						// use translation tracker info? default true
	bool UseRotation;						// use rotation tracker info? default true
	bool DisplayAxes;						// show axes?
	bool MoveChild;							// default true, who moves relative to who?
	CORDORD Position;						// where is the node located
	CORDORD DefaultPosition;				// where is the node located without tracker
        CORDORD TrackerScaling;                                 // scale factors for tracker input
	bool PreDrawn;	
} TRANSFORMATION_NODE_DATA_MEMBERS;

#endif //ndef TRANSFORMATIONNODE_H
