/*
read_config.cpp.  Reads the config file.

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

Modified 01/16/03, KLS to remove printing of the config file at runtime.  Also removed obvious debug statements.

Modified 09/09/03,
    Added general substitution of environment variables in config file, Dave Hite.

Modified 10/08/03,
    Changed cout to std::cout for sgi compiler, Dave Hite.

Modified 11/25/03, TBE
    Changed sustitution code so it does not use c++ code i.e. <string>
*/

//#define DEBUG

#include "gheader.h"
#include <sys/utsname.h>

#include <stdlib.h>

/* Globals */
struct utsname fl_sysInfo;
char *fl_sysname;

/* Temporary globals:  these are here until thier proper place can be found */
/* TODO: place these properly */
char sysname [MAX_NAME_SIZE];
char universe [MAX_NAME_SIZE];

/* do name substitutions in string parameters */
void name_subst (char *sparam)
{
	char *p = 0, *q = 0, *r = 0;
	int s = 0;
	char variable_name[ 500 ] = "";
	char *variable_replacement = 0;
	char final_string[ 5000 ] = ""; // arbitrary large size, overruns bad...
	bool change = false;

	for( r = sparam; r && *r; ) // start at the beginning, and process while there is something left
	{
		p = strchr( r, '$' ); // search for a variable name
		if( p )
		{  // start of variable name found
			if( p - r ) // is there something before it?
			{ // there is, copy it
				s = strlen( final_string );
				strncat( final_string, r, p - r );
				final_string[ s + p - r ] = 0;
				r = p; // move to start of variable name
			}
			p++; // skip '$'
			if( *p == '(' ) // is this a substitution from environment?
			{ // yes
				p++; // skip '('
				q = strchr( p, ')' ); // find end of variable name
				if( q ) // is it there?
				{ // yes
					s = q - p; // variable name size
					strncpy( variable_name, p, s ); // copy it
					variable_name[ s ] = 0;

	  				variable_replacement = getenv( variable_name ); // look up environment variable

					if( variable_replacement ) // was it found?
					{ // yes
						strcat( final_string, variable_replacement ); // add new value
						change = true; // a change has occured
#ifdef DEBUG
						fprintf( stdout, "replacing \"$(%s)\" with \"%s\" in \"%s\"\n", variable_name, variable_replacement, sparam );
#endif
					}
					else
					{ // environment variable not found
#ifdef DEBUG
						fprintf( stdout, "unrecognized variable \"%s\"...\n", variable_name );
#endif
						// add it back onto string unaltered... (mostly)
						strcat( final_string, "$(" );
						strcat( final_string, variable_name );
						strcat( final_string, ")" );
					}
					r = q + 1; // move on
				}
				else
				{ // no ending to variable name found...
					fprintf( stdout, "ERROR in string substitution, missing ')' in \"%s\"\n", sparam );
					strcat( final_string, r ); // copy rest of string unaltered...
					r = 0; // done
				}
			}
			else
			{ // not environment variable
				if( !strncmp( p, "SYSNAME", 7 ) )
				{
					strcat( final_string, fl_sysname ); // substitute system name
					r = p + 7; // move past SYSNAME
					change = true; // changed string
#ifdef DEBUG
					fprintf( stdout, "replacing \"$SYSNAME\" with \"%s\" in \"%s\"\n", fl_sysname, sparam );
#endif
				}
				else
				{  // not a recognized variable
#ifdef DEBUG
					fprintf( stdout, "unrecognized variable...\n" );
#endif
					strcat( final_string, "$" ); // ignore $
					r++; // keep going
				}
			}
		}
		else
		{ // nothing found, copy rest of string (could be a waste...)
			if( change )
			{ // a change has occued, copy rest of string
				strcat( final_string, r );
			}
			r = 0; // done
		}
	}
		
#ifdef DEBUG
	if( change )
	{
		fprintf( stdout, "\"%s\" now \"%s\"\n", sparam, final_string );
	}
#endif

	if( change )
	{ // substitution made, copy result back
    	// Put result in input param
    	strncpy( sparam, final_string, MAX_NAME_SIZE );
	}

/*****************************************/

/*** old version, using <string> class

    // Convert to string for ease of manipulation
    string strParam = sparam; 

    // Get size
    int size = strParam.size();

    // The index of the start of the variable string...
    int start = 0;

    // Variable for the total substitued string.
    string total_string;

    // Loop through string param checking for variables to substitute
    for( int i = 0; i < size; i++ )
    {
      // Variable to be substituted
      string var_name;

      // Substitution result from getenv()
      char* var_res = NULL;

      //std::cout<<"...checking "<<i<<" "<<strParam[i]<<"...\n";


      // Check for $   - beginning of variable
      if( strParam[i] == '$' )
      {
	// Search for '(' and ')' enclosing variable name
	if( strParam[i + 1] == '(' )
	{
	  int j = i + 2;

	  // find closing paren, while inside string
	  while( strParam[j] != ')' && j < size )
	  {
	    //std::cout<<"  ...checking "<<j<<" "<<strParam[j]<<"...\n";

	    // copy variable name into tmp var
	    var_name += strParam[j];

	    j++;
	  }

	  // get variable substitution from system call
	  var_res = getenv( var_name.c_str() );

	  // Make sure variable value was found
	  if( var_res != NULL )
	  {
	    // **************************
	    // Get first part of variable string if not at beginning
	    // i = '$', j = closing ')'
	    // **************************
	    if( i - 1 > 0 )
	    {
	      // get substring at index: start, of size: (i-1) - start
	      total_string += strParam.substr( start, i - start );
	    }

	    // Append the substitued variable
	    total_string += var_res;

	    // reset the next start to the end of the var
	    start = j + 1;
	    
	    // reset i
	    i = j;

	  }
	  else
	  {
		  cout<<"\nVariable "<<var_name<<" is not found. "<<endl;
	  }

	}// end if check for '('

	// Check for SYSNAME
	else if( strParam.substr( i+1, 7 ) == "SYSNAME" )
	{
	  // Get initial part of variable string
	  if( i -1 > 0 )
	  {
	    total_string += strParam.substr( start, i - start );
	  }

	  // Append variable substitution
	  total_string += fl_sysname;

	  // reset i
	  i += 7;

	  // reset the next start to the end of the var
	  start = i + 1;

	} // end check for SYSNAME

	// No opening paren found...
	else
	{
		std::cerr<<"\n\nError! No opening paren found in variable name."<<endl;
		std::cerr<<strParam<<endl<<endl;

	  	exit(-1);
	}

      } // end check for '$'

    } // end for

    // Append rest of string, check if variable is not at end of string.
    if( strParam[ start ] != ')' && start != size )
    {
      total_string += strParam.substr( start ); 
    }

    //cout<<"\n\n"<<total_string<<endl<<endl;

    // Put result in input param
    strncpy( sparam, total_string.c_str(), MAX_NAME_SIZE );
      
    //exit
    return;

************************************/

}


APPLISTTYPE *fl_read_config_file(char *file, bool debug_config) {
        FILE *fpin ;
        int i, j, done ;
        int iparam ;
        int currentparam ;
        int ca ;
	int maxapps = 10 ;
        char var[MAX_NAME_SIZE],type[MAX_NAME_SIZE],section[MAX_NAME_SIZE];
        char sparam[MAX_NAME_SIZE] ;
        char comment[MAX_NAME_SIZE] ;
        float fparam ;
        APPLISTTYPE *A = NULL ;
        
        /* get some system info */
        if (uname (&fl_sysInfo))
        {
          fprintf (stderr, "ERROR: could not get system information.\n");
          exit (1);
        }

        /* get the system name (the type of OS) */
        fl_sysname = fl_sysInfo.sysname;
        
        //printf("fl_read_config_file- Opening and reading Flatland.config....\n") ;
        ca = -1 ;

        fpin = fopen( file,"r" ) ;
        if( fpin==NULL ) {
                printf("init0- ERROR opening Flatland.config, using defaults.\n") ;
                return( NULL ) ;
        }
        while( 1 ) {
NEXT:           fscanf(fpin,"%s",section) ;
                if (debug_config) printf("%s\n",section) ;
                if( strcmp(section,"EOF")==0 ) 
                        goto END ;
                if( strncmp( section,"#", 1 )==0 ) {
                        fgets( comment, MAX_NAME_SIZE, fpin ) ;
                        if (debug_config) printf("%s",comment) ;
                        goto NEXT ;
                }
                if( strcmp(section,"BeginSystem")==0 ) {
                        done = 0 ;
                        while( done==0 ) {
                  READ1:        fscanf(fpin,"%s",var ) ;
                                if (debug_config) printf("%s",var) ;
                                if( strcmp(var,"End")==0 ) {
                                        done = 1 ;
                                        if (debug_config) printf("\n\n") ;
                                }
                                if( strncmp(var,"#", 1)==0 ) {
                                	fgets( comment, MAX_NAME_SIZE, fpin ) ;
                                        if (debug_config) printf("%s",comment) ;
                                        goto READ1 ;
                                }
                                if( done == 0 ) {
                                        fscanf(fpin,"%s",type) ;
                                        if (debug_config) printf(" %s ",type) ; 
                                        if( strcmp(type,"int")==0 ) {
                                                fscanf(fpin,"%d", &iparam) ;
                                                if (debug_config) printf(" %d\n",iparam) ;
//                                                if( strcmp(var,"ww0")==0 ) 
//                                                        ww0 = iparam ;
//                                                if( strcmp(var,"wh0")==0 ) 
//                                                        wh0 = iparam ;
//                                                if( strcmp(var,"full_screen")==0 )
//                                                        full_screen_flag = iparam ;
//                                                if( strcmp(var,"single_window")==0 )
//                                                        single_window_flag = iparam ;
                                                if( strcmp(var,"maxapps")==0 ) { 
                                                        maxapps = iparam ;
                                                        A = fl_makeapplist( maxapps ) ;
                                                }
                                                if( strcmp(var,"maxshadowgraphs")==0 ) {
                                                        if (iparam != Loop_Control->Universe->num_shadows)
                                                        {
                                                                // we get away with this here because no threads have been started yet
                                                                // don't try it elsewhere!
                                                                free (Loop_Control->Universe->shadow_list);
                                                                free (Loop_Control->Universe->shadow_root_object);
                                                                Loop_Control->Universe->shadow_root_object = (OBJECT_TYPE **) calloc (Loop_Control->Universe->num_shadows, sizeof (OBJECT_TYPE *));
                                                                Loop_Control->Universe->shadow_list = (bool *) calloc (Loop_Control->Universe->num_shadows, sizeof (OBJECT_TYPE *));
                                                                Loop_Control->Universe->num_shadows = iparam ;
                                                        }
                                                }
                                                if( strcmp(var,"log_messages")==0 )
						{
                                                    Loop_Control->LogMessages = iparam != 0;
						}
//                                                if( strcmp(var,"nbr_main_windows")==0 )
//                                                        nbr_main_windows = iparam ;
#ifdef ENABLE_SDL
                                                if( strcmp(var,"max_open-close_sounds") == 0)
                                                        max_oc_sounds = iparam ;
                                                if( strcmp(var,"maxsounds")==0 )
                                                        MAX_SOUND_LISTS = iparam ;
#endif
//                                                if( strcmp(var,"stereo")==0 )
////                                                        enableStereo = (Boolean) iparam ;
//                                                        Loop_Control->UseStereo = (bool) iparam;
//                                                if( strcmp(var,"fixed_viewing_planes")==0 )
////                                                        FixedViewingPlanes = (Boolean) iparam ;
//                                                        Loop_Control->FixedViewingPlanes = (bool) iparam;
//                                                if ( !strcmp(var,"normalize") )
//                                                        Loop_Control->NormalizeVectors = (bool) iparam;
                                        } else if( strcmp(type,"float")==0 ) {
                                                fscanf(fpin,"%f", &fparam) ;
                                                if (debug_config) printf(" %f\n",fparam) ;
//                                                if(strcmp(var,"head_eyex")==0)
//                                                        head_eyex = fparam;
//                                                if(strcmp(var,"head_eyey")==0)
//                                                        head_eyey = fparam;
//                                                if(strcmp(var,"head_eyez")==0)
//                                                        head_eyez = fparam;
//                                                if(strcmp(var,"head_eyepitch")==0)
//                                                        head_eyepitch = fparam;
//                                                if(strcmp(var,"head_eyeyaw")==0)
//                                                        head_eyeyaw = fparam;
//                                                if(strcmp(var,"head_eyeroll")==0)
//                                                        head_eyeroll = fparam;
//                                                if(strcmp(var,"stereo_iod")==0)
//                                                        IOD = fparam;
//                                                if(strcmp(var,"stereo_eye_rot_ang")==0)
//                                                        STEREO_EYE_ROTATION = fparam;
//                                                if(strcmp(var,"stereo_flat_plane_dist")==0)
//                                                        FLAT_PLANE_DIST = fparam;
                                        } else if( strcmp(type,"string")==0 ) {
                                                fscanf(fpin,"%s", sparam) ;

                                                //printf(" \ntype string: Before substitution: \n");
                                                //printf(" %s\n",sparam) ;

                                                /* do name substitution */
                                                name_subst (sparam);

                                                //printf(" \ntype string: After substitution: \n");
                                                //printf(" %s\n",sparam) ;

                                                if (debug_config) printf(" %s\n",sparam) ;
                                                if(strcmp(var,"name")==0)
                                                	strcpy(sysname,sparam) ;
                                                if(strcmp(var,"universe")==0)
                                                	strcpy(universe,sparam) ;
//                                                if(strcmp(var,"frustum_def_filename")==0)
//                                                	strcpy(frustum_file,sparam) ;
                                                if(strcmp(var,"window_def_filename")==0)
                                                	strcpy(window_definition_file,sparam) ;
//                                                if(strcmp(var,"tracker_port")==0)
//                                                        strcpy(TRACKER_SERIAL_PORT,sparam) ;
                                                if ( !strcmp(var,"left_eye_name") )
                                                	strcpy( Loop_Control->LeftEyeName, sparam );
                                                if ( !strcmp(var,"right_eye_name") )
                                                	strcpy( Loop_Control->RightEyeName, sparam );
                                                if ( !strcmp(var,"center_eye_name") )
                                                	strcpy( Loop_Control->CenterEyeName, sparam );
                                                if ( !strcmp(var,"head_tracker_name") )
                                                	strcpy( Loop_Control->HeadTrackerName, sparam );
                                                if ( !strcmp(var,"tracker_client_name") )
                                                	strcpy( Loop_Control->TrackerClientName, sparam );
                                                if ( !strcmp(var,"head_name") )
                                                	strcpy( Loop_Control->HeadName, sparam );
                                                if ( !strcmp(var,"message_log_name") )
						{
                                                    strcpy( Loop_Control->MessageLogName, sparam );
						}
#ifdef ENABLE_SDL
                                                if(strcmp(var,"sound_server")==0) 
                                                        strcpy(sound_server,sparam) ;
#endif
#ifdef Linux
						if (!strcmp(var, "glut_library"))
						        strcpy(glut_library, sparam);
						if (!strcmp(var, "joystick_device"))
						        strcpy(joystick_device, sparam);
#endif //Linux
                                        } else {
                                                printf(" ERROR, Unrecognized variable type. \n") ;
                                                exit(0) ;
                                        }
                                }
                        }
                }
                if( strcmp(section,"BeginBaseApp")==0 || strcmp(section,"BeginUserApp")==0 ) {
                        ca++ ;
                        if( ca==maxapps) {
                                printf("fl_read_config_file- ERROR, too many apps. \n") ;
                                exit(0) ;
                        }
                        if( strcmp(section,"BeginBaseApp")==0)
                        	A->applications[ca] = fl_makeapp( 0 ) ;
                        else if( strcmp(section,"BeginUserApp")==0)
                        	A->applications[ca] = fl_makeapp( 1 ) ;
                        done = 0 ;
                        currentparam = 0 ;
                        while( done==0 ) {
                    READ2:      fscanf(fpin,"%s",var) ;
                                if (debug_config) printf("%s",var) ;
                                if( strcmp(var,"End")==0 ) {
                                        if (debug_config) printf("\n\n") ;
                                        goto NEXT ;
                                }
                                if( strncmp(var,"#", 1)==0 ) {
                                	fgets( comment, MAX_NAME_SIZE, fpin ) ;
                                        if (debug_config) printf("%s",comment) ;
                                        goto READ2 ;
                                }
                               fscanf(fpin,"%s",type) ;
                                if (debug_config) printf(" %s ",type) ; 
                                if( strcmp(type,"int")==0 ) {
                                       fscanf(fpin,"%d", &iparam) ;
                                       if (debug_config) printf(" %d\n",iparam) ;
                                       if( strcmp(var,"nparams")==0 ) { 
                                                A->applications[ca]->nparams = iparam ;
                                                 if( iparam>0 ) {
                                                        	A->applications[ca]->params = (char ***)malloc(A->applications[ca]->nparams*sizeof(char **)) ;
                                                        	if( A->applications[ca]->params==NULL ) {
                                                                	printf("fl_read_config_file- ERROR mallocing param space for app: %s\n",A->applications[ca]->DLLname)  ;
                                                                	exit(0) ;
                                                        	}
                                                        	for( i=0 ; i<A->applications[ca]->nparams ; i++ ) {
                                                                	A->applications[ca]->params[i] = (char **)malloc(3*sizeof(char *)) ;
                                                                	if( A->applications[ca]->params[i]==NULL ) {
                                                                       		printf("fl_read_config_file- ERROR mallocing param[%d] space for app: %s\n",i,A->applications[ca]->DLLname) ;
                                                                        	exit(0) ;
                                                                	}
                                                                	for( j=0 ; j<3 ; j++ ) {
                                                                        	A->applications[ca]->params[i][j] = (char *)malloc(100*sizeof(char)) ;
                                                                        	if( A->applications[ca]->params[i][j]==NULL ) {
                                                                                	printf("fl_read_config_file- ERROR mallocing param[%d][%d] space for app: %s\n",i,j ,A->applications[ca]->DLLname) ;
                                                                                	exit(0) ;
                                                                        	}
                                                                        	strcpy(A->applications[ca]->params[i][j],"NULL") ;
                                                                	}
                                                          	}
								// Copy app parameters from config file into app variables
                                                          	while(1) {
								READ3:          fscanf(fpin,"%s",var) ;
                                						if (debug_config) printf("%s",var) ;
                                						if( strcmp(var,"End")==0 ) {
										  if (debug_config) printf("\n\n") ;
										  goto NEXT ;
										}
										if( strncmp(var,"#", 1)==0 ) {
										  fgets( comment, 200, fpin ) ;
										  if (debug_config) printf("%s",comment) ;
										  goto READ3 ;
										}
 
										fscanf(fpin,"%s %s",type, sparam) ;
										if (debug_config) printf(" %s  %s\n",type,sparam) ;
										if( currentparam==A->applications[ca]->nparams) {
										  printf("fl_read_config_file- ERROR, too many app params.\n") ;
										  exit(0) ;
										}

										strcpy(A->applications[ca]->params[currentparam][0],var) ;
										strcpy(A->applications[ca]->params[currentparam][1],type) ;

										// Perform variable substitution on string parameters only
										if( strncmp( type, "string", 6 ) == 0 )
										{
										  name_subst (sparam);
										}

										strcpy(A->applications[ca]->params[currentparam][2],sparam) ;
										currentparam++ ;

								} // end copy app params

                                                        }
                                                }
                                                if( strcmp(var,"share")==0 ) { 
						         A->applications[ca]->shared_flag = (iparam != 0);
						}
                                        } else if( strcmp(type,"float")==0 ) {
                                                fscanf(fpin,"%f", &fparam) ;
                                                if (debug_config) printf(" %f\n",fparam) ;
                                        } else if( strcmp(type,"string")==0 ) {
                                                char strtmp[MAX_NAME_SIZE];
                                                fgets( strtmp, MAX_NAME_SIZE, fpin ) ;
                                                //fscanf(fpin,"%s", sparam) ;
                                                sscanf(strtmp,"%s", sparam) ;

                                                //printf(" \nBefore substitution: \n");
                                                //printf(" %s\n",sparam) ;

                                                /* do name substitution */
                                                name_subst (sparam);

                                                //printf(" \nAfter substitution: \n");
                                                //printf(" %s\n",sparam) ;

                                                if( strcmp(var,"dllloc")==0)
                                                        strcpy(A->applications[ca]->DLLloc, sparam ) ;
                                                if( strcmp(var,"dllname")==0) 
                                                        strcpy(A->applications[ca]->DLLname, sparam) ;
                                                if( strcmp(var,"connect")==0)
                                                        strcpy(A->applications[ca]->parent_object_name, sparam) ;
                                                if( strcmp(var,"universe")==0)
                                                        strcpy( A->applications[ca]->universe_name, sparam) ;
                                                if( strcmp(var,"name")==0) 
                                                        strcpy(A->applications[ca]->appname, sparam) ;
                                        } else {
                                                printf(" ERROR, Unrecognized variable type. \n") ;
                                                exit(0) ;
                                        }                                        
                                
                        }
                }
        } 
END:    //printf("fl_read_config_file- Closing Flatland.config\n") ;
        fclose( fpin ) ;
        
        A->napps = ca + 1 ;
        
        return( A ) ;
}

APPTYPE *fl_makeapp(int type)
{
        APPTYPE *a ;

        a = (APPTYPE *)malloc( sizeof(APPTYPE) ) ;
        if( a==NULL ) {
                printf("fl_makeapp- ERROR mallocing a.\n") ;
                exit(0) ;
        }
        a->type = type ;
        a->loaded = false ;
        a->unloadable = false;
        a->nparams = 0 ;
        a->params = NULL ;
        strcpy( a->DLLloc, "NULL" ) ;
        strcpy( a->DLLname, "NULL" ) ;        
	a->menu_entry_on_flag = true;
	a->shared_flag = false;

        return( a ) ;
}

APPLISTTYPE *fl_makeapplist( int maxapps )
{
	APPLISTTYPE *a ;
	
	a = (APPLISTTYPE *) malloc( sizeof(APPLISTTYPE) ) ;
	if( a==NULL ) {
		printf("fl_makeapplist- ERROR mallocing a.\n") ;
		exit(0) ;
	}
	a->maxapps = maxapps ;
	a->napps = 0 ;
	a->applications = (APPTYPE **)malloc( maxapps * sizeof(APPTYPE *) ) ;
    if( a->applications == NULL ) {
    	printf("fl_read_config_file- ERROR mallocing A->applications.\n") ;
    	exit(0) ;
    }
    
    return( a ) ;
}
