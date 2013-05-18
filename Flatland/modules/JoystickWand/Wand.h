// JoystickWand.h
// Header file for JoystickWand.cxx
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


#ifndef JoystickWand_H
#define JoystickWand_H


// Joystick button mask
#define BUTTON1  0x00000001
#define BUTTON2  0x00000002
#define BUTTON3  0x00000004
#define BUTTON4  0x00000008
#define BUTTON5  0x00000010
#define BUTTON6  0x00000020
#define BUTTON7  0x00000040
#define BUTTON8  0x00000080
#define BUTTON9  0x00000100
#define BUTTON10 0x00000200
#define BUTTON11 0x00000400
#define BUTTON12 0x00000800


typedef struct wand_members
{
  bool (*GetButton)(OBJECT_TYPE *obj, const int which);	// is button pressed? 
  bool (*GetBeamingObject)(OBJECT_TYPE *WandObject);	// is an object being beamed?
  OBJECT_TYPE *(*GetBeamedObject)(struct object *WandObject);	// return closest beamed object
  int (*SetHand)(OBJECT_TYPE *WandObject, int Hand);	// set which hand to use 
  int (*SetHead)(OBJECT_TYPE *WandObject, int Head);	// set which head to use 
	
  unsigned int      JoystickButtonMask;


  /////// STAY OUT OF THESE ///////////////////////////////////////////////
  bool TopButton;			// is button pressed? 
  bool MiddleButton;			// is button pressed? 
  bool BottomButton;			// is button pressed? 
  int TopNumber;			// button number 
  int MiddleNumber;			// button number 
  int BottomNumber;			// button number
  bool DataReady;			// Has the data been read in? 
  int Hand;				// <0=left, 0=none, >0=right
  int Head;				// 0: default
  char LeftHandDataFile[MAX_NAME_SIZE];	// data filename
  char RightHandDataFile[MAX_NAME_SIZE];	// data filename
  char WandHandleDataFile[MAX_NAME_SIZE];	// data filename
  char DefaultHeadDataFile[MAX_NAME_SIZE];// data filename
  GLfloat Scale;			// scale wand		
  int BeamColor;			// color of beam (mod 5)
  void *LeftHand;			// wand object
  void *RightHand;			// wand object
  void *WandHandle;			// wand object
  void *DefaultHead;			// wand object
  CORDORD Position;			// where is the wand located
  GLuint *WandList;			// Wand display lists
} WAND_MEMBERS;

#endif //JoystickWand_H
