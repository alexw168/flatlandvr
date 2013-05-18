/*
fl_ReadWindowDefinition.cpp.  Reads the window definition file.

Copyright (C) 2003  The University of New Mexico

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

#include "gheader.h"

void AddWindow( Loop_Control_Struct *ctrl )
{
	Window_Control_Struct *new_wcs = new Window_Control_Struct[ ctrl->NumberOfWindows + 1 ];
	int i;
	
	if (!new_wcs)
	{
		fprintf( stderr, "Could not allocate memory for Loop_Control_Struct");
		exit(-1);
	}
	
	for( i=0; i<ctrl->NumberOfWindows; i++ )
	{
		new_wcs[ i ] = ctrl->WindowControl[ i ];
	}

	new_wcs[ ctrl->NumberOfWindows ].NumberOfViewports = 0;
	new_wcs[ ctrl->NumberOfWindows ].LeftViewport = 0;
	new_wcs[ ctrl->NumberOfWindows ].RightViewport = 0;
	new_wcs[ ctrl->NumberOfWindows ].Wall = NULL;
	new_wcs[ ctrl->NumberOfWindows ].StartWall = NULL;
	new_wcs[ ctrl->NumberOfWindows ].Extent = NULL;
	new_wcs[ ctrl->NumberOfWindows ].StartExtent = NULL;
	new_wcs[ ctrl->NumberOfWindows ].Frustrum = NULL;
	new_wcs[ ctrl->NumberOfWindows ].Orientation = NULL;
	new_wcs[ ctrl->NumberOfWindows ].glutWindow = 0;
	new_wcs[ ctrl->NumberOfWindows ].Position.X = new_wcs[ ctrl->NumberOfWindows ].LastPosition.X = new_wcs[ ctrl->NumberOfWindows ].StartPosition.X = 0;
	new_wcs[ ctrl->NumberOfWindows ].Position.Y = new_wcs[ ctrl->NumberOfWindows ].LastPosition.Y = new_wcs[ ctrl->NumberOfWindows ].StartPosition.Y = 240 * ctrl->NumberOfWindows;
	new_wcs[ ctrl->NumberOfWindows ].Position.Width = new_wcs[ ctrl->NumberOfWindows ].LastPosition.Width = new_wcs[ ctrl->NumberOfWindows ].StartPosition.Width = 0;
	new_wcs[ ctrl->NumberOfWindows ].Position.Height = new_wcs[ ctrl->NumberOfWindows ].LastPosition.Height = new_wcs[ ctrl->NumberOfWindows ].StartPosition.Height = 0;

	delete[] ctrl->WindowControl;
	ctrl->Window = ctrl->NumberOfWindows;
	ctrl->NumberOfWindows++;
	ctrl->WindowControl = new_wcs;
}


void AddViewport( Loop_Control_Struct *ctrl )
{
	Window_Control_Struct *wcs = ctrl->WindowControl + ctrl->Window;
	Wall_Struct *new_wall = new Wall_Struct[ wcs->NumberOfViewports + 1 ];
	Wall_Struct *new_start_wall = new Wall_Struct[ wcs->NumberOfViewports + 1 ];
	Viewport_Struct *new_ext = new Viewport_Struct[ wcs->NumberOfViewports + 1 ];
	Viewport_Struct *new_start_ext = new Viewport_Struct[ wcs->NumberOfViewports + 1 ];
	Frustrum_Struct *new_frust = new Frustrum_Struct[ wcs->NumberOfViewports + 1 ];
	CORDORD *new_orient = new CORDORD[ wcs->NumberOfViewports + 1 ];
	int i;
	
	if (!new_wall || !new_ext || !new_frust || !new_orient)
	{
		fprintf( stderr, "Could not allocate memory for Window_Control_Struct");
		exit(-1);
	}
	
	for( i=0; i<wcs->NumberOfViewports; i++ )
	{
		new_wall[ i ] = wcs->Wall[ i ];
		new_start_wall[ i ] = wcs->StartWall[ i ];
		new_ext[ i ] = wcs->Extent[ i ];
		new_start_ext[ i ] = wcs->StartExtent[ i ];
		new_frust[ i ] = wcs->Frustrum[ i ];
		new_orient[ i ] = wcs->Orientation[ i ];
	}

	new_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 0 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 0 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 1 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 1 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 2 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 2 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 3 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 0 ][ 3 ] = 0;

	new_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 0 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 0 ] = 1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 1 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 1 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 2 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 2 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 3 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 1 ][ 3 ] = 0;

	new_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 0 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 0 ] = 1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 1 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 1 ] = 1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 2 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 2 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 3 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 2 ][ 3 ] = 0;

	new_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 0 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 0 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 1 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 1 ] = 1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 2 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 2 ] = -1;
	new_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 3 ] = new_start_wall[ wcs->NumberOfViewports ].Corner[ 3 ][ 3 ] = 0;

	new_ext[ wcs->NumberOfViewports ].X = 320 * wcs->NumberOfViewports;
	new_ext[ wcs->NumberOfViewports ].Y = 0;
	new_ext[ wcs->NumberOfViewports ].Width = 320;
	new_ext[ wcs->NumberOfViewports ].Height = 240;

	new_start_ext[ wcs->NumberOfViewports ].X = 320 * wcs->NumberOfViewports;
	new_start_ext[ wcs->NumberOfViewports ].Y = 0;
	new_start_ext[ wcs->NumberOfViewports ].Width = 320;
	new_start_ext[ wcs->NumberOfViewports ].Height = 240;

	new_frust[ wcs->NumberOfViewports ].Left = 0;
	new_frust[ wcs->NumberOfViewports ].Right = 0;
	new_frust[ wcs->NumberOfViewports ].Top = 0;
	new_frust[ wcs->NumberOfViewports ].Bottom = 0;
	new_frust[ wcs->NumberOfViewports ].Near = 0.2;
	new_frust[ wcs->NumberOfViewports ].Far = 7000.0;

	new_orient[ wcs->NumberOfViewports ].x = 90 * wcs->NumberOfViewports;
	new_orient[ wcs->NumberOfViewports ].y = 0;
	new_orient[ wcs->NumberOfViewports ].z = 0;
	new_orient[ wcs->NumberOfViewports ].roll = 0;
	new_orient[ wcs->NumberOfViewports ].pitch = 0;
	new_orient[ wcs->NumberOfViewports ].yaw = 0;

	delete[] wcs->Wall;
	delete[] wcs->StartWall;
	delete[] wcs->Extent;
	delete[] wcs->StartExtent;
	delete[] wcs->Frustrum;
	delete[] wcs->Orientation;
	ctrl->Viewport = wcs->NumberOfViewports;
	wcs->NumberOfViewports++;
	wcs->Wall = new_wall;
	wcs->StartWall = new_start_wall;
	wcs->Extent = new_ext;
	wcs->StartExtent = new_start_ext;
	wcs->Frustrum = new_frust;
	wcs->Orientation = new_orient;
}


enum ReadWindowState { NOT_IN, IN_WINDOW, IN_VIEWPORT, ERROR };

void fl_ReadWindowDefinition( char *filename, Loop_Control_Struct *ctrl )
{
	int i, j;
	ReadWindowState state;
	char *loc;
	char token[MAX_NAME_SIZE];
	char dummy[MAX_NAME_SIZE];
	float fparam;
	int iparam;
	FILE *infile;
		
	ctrl->NumberOfWindows = 0;
	ctrl->WindowControl = NULL;
	ctrl->Window = -1;
	ctrl->Viewport = -1;
	
	state = NOT_IN;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//

	// if the input file is "", set up some defaults and split 
	if (filename[0] == '\0')
	{
		AddWindow( ctrl );
		AddViewport( ctrl );
	}
	else
	{
		// open the input file 
		infile = fopen(filename, "r");
		if (infile == NULL)
		{
			fprintf( stderr, "Could not open window definition file.\n" );
			exit( -1 );
		}
		
		while ( fscanf( infile, "%s", token ) != EOF )
		{
			// take out comments 
			// TODO: figure out why the SGI compiler insists strchr returns bool 
			if ( ( loc = strchr ( token, '#' ) ) != NULL )
			{
				loc[0] = '\0';    // Wipe out the rest of the token containing the # 
				fgets(dummy, MAX_NAME_SIZE, infile);     // wipe the rest of the line 
			}
    
			if (state == NOT_IN)
			{
				if (!strncmp ("beginWindow", token, MAX_NAME_SIZE))
				{
					AddWindow( ctrl );
					state = IN_WINDOW;
				}
				if (!strncmp ("full_screen", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->showFullScreen = (bool) iparam;
				}
				if (!strncmp ("stereo", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->UseStereo = (bool) iparam;
				}
				if (!strncmp ("fixed_viewing_planes", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->FixedViewingPlanes = (bool) iparam;
				}
				if (!strncmp ("passive_stereo_via_windows", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->UsePassiveStereoViaWindow = (bool) iparam;
				}
				if (!strncmp ("passive_stereo_via_viewports", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->UsePassiveStereoViaViewport = (bool) iparam;
				}
				if (!strncmp ("left_window", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->LeftWindow = iparam;
				}
				if (!strncmp ("right_window", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->RightWindow = iparam;
				}
				if (!strncmp ("normalize", token, MAX_NAME_SIZE))
				{
					fscanf (infile, "%d", &iparam);
					Loop_Control->NormalizeVectors = (bool) iparam;
				}
			}
			else
			{
				if (state == IN_WINDOW)
				{
					// looking for valid tokens 
					if (!strncmp ("endWindow", token, MAX_NAME_SIZE))
					{
						state = NOT_IN;
					}
					if (!strncmp ("beginViewport", token, MAX_NAME_SIZE))
					{
						AddViewport( ctrl );
						state = IN_VIEWPORT;
					}
					if (!strncmp ("x", token, MAX_NAME_SIZE))
					{
						fscanf (infile, "%d", &iparam);
						ctrl->WindowControl[ ctrl->Window ].Position.X = ctrl->WindowControl[ ctrl->Window ].LastPosition.X = ctrl->WindowControl[ ctrl->Window ].StartPosition.X = iparam;
					}
					if (!strncmp ("y", token, MAX_NAME_SIZE))
					{
						fscanf (infile, "%d", &iparam);
						ctrl->WindowControl[ ctrl->Window ].Position.Y = ctrl->WindowControl[ ctrl->Window ].LastPosition.Y = ctrl->WindowControl[ ctrl->Window ].StartPosition.Y = iparam;
					}
					if (!strncmp ("left_viewport", token, MAX_NAME_SIZE))
					{
						fscanf (infile, "%d", &iparam);
						ctrl->WindowControl[ ctrl->Window ].LeftViewport = iparam;
					}
					if (!strncmp ("right_viewport", token, MAX_NAME_SIZE))
					{
						fscanf (infile, "%d", &iparam);
						ctrl->WindowControl[ ctrl->Window ].RightViewport = iparam;
					}
				}
				else
				{
					if (state == IN_VIEWPORT)
					{
						// looking for valid tokens 
						if (!strncmp ("endViewport", token, MAX_NAME_SIZE))
						{
							state = IN_WINDOW;
						}
						if (!strncmp ("x0", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[0][0] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[0][0] = fparam;
						}
						if (!strncmp ("y0", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[0][1] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[0][1] = fparam;
						}
						if (!strncmp ("z0", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[0][2] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[0][2] = fparam;
						}
						if (!strncmp ("x1", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[1][0] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[1][0] = fparam;
						}
						if (!strncmp ("y1", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[1][1] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[1][1] = fparam;
						}
						if (!strncmp ("z1", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[1][2] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[1][2] = fparam;
						}
						if (!strncmp ("x2", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[2][0] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[2][0] = fparam;
						}
						if (!strncmp ("y2", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[2][1] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[2][1] = fparam;
						}
						if (!strncmp ("z2", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[2][2] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[2][2] = fparam;
						}
						if (!strncmp ("x3", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[3][0] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[3][0] = fparam;
						}
						if (!strncmp ("y3", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[3][1] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[3][1] = fparam;
						}
						if (!strncmp ("z3", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Wall[ ctrl->Viewport ].Corner[3][2] = ctrl->WindowControl[ ctrl->Window ].StartWall[ ctrl->Viewport ].Corner[3][2] = fparam;
						}
						if (!strncmp ("near", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Frustrum[ ctrl->Viewport ].Near = fparam;
						}
						if (!strncmp ("far", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Frustrum[ ctrl->Viewport ].Far = fparam;
						}
						if (!strncmp ("roll", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Orientation[ ctrl->Viewport ].roll = fparam;
						}
						if (!strncmp ("pitch", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Orientation[ ctrl->Viewport ].pitch = fparam;
						}
						if (!strncmp ("yaw", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%f", &fparam);
							ctrl->WindowControl[ ctrl->Window ].Orientation[ ctrl->Viewport ].yaw = fparam;
						}
						if (!strncmp ("x", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%d", &iparam);
							ctrl->WindowControl[ ctrl->Window ].Extent[ ctrl->Viewport ].X = ctrl->WindowControl[ ctrl->Window ].StartExtent[ ctrl->Viewport ].X = iparam;
						}
						if (!strncmp ("y", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%d", &iparam);
							ctrl->WindowControl[ ctrl->Window ].Extent[ ctrl->Viewport ].Y = ctrl->WindowControl[ ctrl->Window ].StartExtent[ ctrl->Viewport ].Y = iparam;
						}
						if (!strncmp ("width", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%d", &iparam);
							ctrl->WindowControl[ ctrl->Window ].Extent[ ctrl->Viewport ].Width = ctrl->WindowControl[ ctrl->Window ].StartExtent[ ctrl->Viewport ].Width = iparam;
						}
						if (!strncmp ("height", token, MAX_NAME_SIZE))
						{
							fscanf (infile, "%d", &iparam);
							ctrl->WindowControl[ ctrl->Window ].Extent[ ctrl->Viewport ].Height = ctrl->WindowControl[ ctrl->Window ].StartExtent[ ctrl->Viewport ].Height = iparam;
						}
					}
				}
			}
		}
		fclose( infile );
	}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
	
	for (i = 0; i < ctrl->NumberOfWindows; i++)
	{
		for( j=0; j<ctrl->WindowControl[ i ].NumberOfViewports; j++ )
		{
			if ( ctrl->WindowControl[ i ].Position.Width < ctrl->WindowControl[ i ].Extent[j].X + ctrl->WindowControl[ i ].Extent[j].Width )
			{
				ctrl->WindowControl[ i ].Position.Width = ctrl->WindowControl[ i ].LastPosition.Width = ctrl->WindowControl[ i ].StartPosition.Width = ctrl->WindowControl[ i ].Extent[j].X + ctrl->WindowControl[ i ].Extent[j].Width;
			}
			if ( ctrl->WindowControl[ i ].Position.Height < ctrl->WindowControl[ i ].Extent[j].Y + ctrl->WindowControl[ i ].Extent[j].Height )
			{
				ctrl->WindowControl[ i ].Position.Height = ctrl->WindowControl[ i ].LastPosition.Height = ctrl->WindowControl[ i ].StartPosition.Height = ctrl->WindowControl[ i ].Extent[j].Y + ctrl->WindowControl[ i ].Extent[j].Height;
			}
		}
	}
	
	fprintf( stdout, "# Windows: %d\n", ctrl->NumberOfWindows );
	for (i = 0; i < ctrl->NumberOfWindows; i++)
	{
		fprintf( stdout, "# Window %d:\n", i );
		fprintf( stdout, "#     X: %d\n", ctrl->WindowControl[ i ].Position.X );
		fprintf( stdout, "#     Y: %d\n", ctrl->WindowControl[ i ].Position.Y );
		fprintf( stdout, "#     Width: %d\n", ctrl->WindowControl[ i ].Position.Width );
		fprintf( stdout, "#     Height: %d\n", ctrl->WindowControl[ i ].Position.Height );
		fprintf( stdout, "#     Viewports: %d\n", ctrl->WindowControl[ i ].NumberOfViewports );
		for( j=0; j<ctrl->WindowControl[ i ].NumberOfViewports; j++ )
		{
			fprintf( stdout, "#     Viewport %d:\n", j );
			fprintf( stdout, "#         Extent: (%d,%d) & (%dx%d)\n", ctrl->WindowControl[ i ].Extent[ j ].X, ctrl->WindowControl[ i ].Extent[ j ].Y, ctrl->WindowControl[ i ].Extent[ j ].Width, ctrl->WindowControl[ i ].Extent[ j ].Height );
			fprintf( stdout, "#         llc: (%f,%f,%f)\n", ctrl->WindowControl[ i ].Wall[ j ].Corner[0][0], ctrl->WindowControl[ i ].Wall[ j ].Corner[0][1], ctrl->WindowControl[ i ].Wall[ j ].Corner[0][2] );
			fprintf( stdout, "#         lrc: (%f,%f,%f)\n", ctrl->WindowControl[ i ].Wall[ j ].Corner[1][0], ctrl->WindowControl[ i ].Wall[ j ].Corner[1][1], ctrl->WindowControl[ i ].Wall[ j ].Corner[1][2] );
			fprintf( stdout, "#         urc: (%f,%f,%f)\n", ctrl->WindowControl[ i ].Wall[ j ].Corner[2][0], ctrl->WindowControl[ i ].Wall[ j ].Corner[2][1], ctrl->WindowControl[ i ].Wall[ j ].Corner[2][2] );
			fprintf( stdout, "#         ulc: (%f,%f,%f)\n", ctrl->WindowControl[ i ].Wall[ j ].Corner[3][0], ctrl->WindowControl[ i ].Wall[ j ].Corner[3][1], ctrl->WindowControl[ i ].Wall[ j ].Corner[3][2] );
		}
	}
}
