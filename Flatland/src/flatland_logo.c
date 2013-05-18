/*
 * flatland_logo.c
 *
 * This file is lifted from part of the openGL-logo demo.
 * (c) Henk Kok (kok@wins.uva.nl)
 *
 * Copying, redistributing, etc is permitted as long as this copyright
 * notice and the Dutch variable names :) stay in tact.
 */

#include "gheader.h"

GLfloat lightpos[4] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat lightamb[4] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };
float speed=0, progress = 1;
unsigned char logo_done;//[MAX_MAIN_WINDOWS];
void SetCamera(void);

extern void fl_logo_randomize(void);
extern void def_logo(void);
extern void draw_logo(void);
extern GLfloat alpha;

void do_display (void)
{
	SetCamera();
	draw_logo();
	glFlush();
//        if (!single_window_flag || current_main_window == nbr_main_windows-1)
//	  glutSwapBuffers();
}

void fl_logo_display(void)
{
//        if (!single_window_flag || current_main_window == 0)
//          glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_DEPTH_BUFFER_BIT);
	do_display();
}

void fl_logoinit (void)
{
//        int i;

//        for (i = 0; i < MAX_MAIN_WINDOWS; i++)
 //         logo_done[i] = 0;
	logo_done = false;
	
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor3f(1.0, 1.0, 1.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_NORMALIZE);
	def_logo();
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
        printf ("\nPress ESC to skip logo\n\n");
}

/* ARGSUSED1 */
void parsekey(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0); break;
	case 13: break;
	case ' ': progress = 1; fl_logo_randomize(); break;
	}
}

/* ARGSUSED1 */
void parsekey_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		break;
	case GLUT_KEY_DOWN:		break;
	case GLUT_KEY_RIGHT:	break;
	case GLUT_KEY_LEFT:		break;
	}
}

int delayFlag = 1;

void fl_Animate(void)
{
        if (alpha <= 0) 
		{
//          logo_done[current_main_window] = 1; /*indicate the logo has finished*/
//          logo_done[0] = 1; /*indicate the logo has finished*/
			logo_done = true;
		}
        if (progress == 0.0)
        {
          if (delayFlag)
          {
            fl_short_sleep (ONE_SECOND);
            delayFlag = 0;
          }
        }
        else
		{
          delayFlag = 1;
		}
	speed = -0.95*speed + progress*0.05;
	if (progress > 0.0 && speed < 0.0003)
	{
		speed = 0.0003;
	}
	if (speed > 0.01)
	{
		speed = 0.01;
	}
	progress = progress - speed;
	if (progress < 0.0)
	{
		progress = 0.0;
		speed = 0;
	}
//	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	glMatrixMode (GL_MODELVIEW);
	glViewport (0, 0, w, h);
	glLoadIdentity();
	SetCamera();
}

void SetCamera(void)
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glFrustum (-0.1333, 0.1333, -0.1, 0.1, 0.2, 150.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,1.5,2, 0,1.5,0, 0,1,0);
	glTranslatef(0.0, -8.0, -45.0);
	glRotatef(-progress*720, 0.0, 1.0, 0.0);
}

//int main(int argc, char *argv[])
//{
//#if 0
//        int sh, sw;     /* screen height & width */
//        int spx, spy;   /* window starting position */
//        const width=640, height=480;    /* window width and height */
//#endif
//	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
//#if 0
//        sh = glutGet(GLUT_SCREEN_HEIGHT);
//        sw = glutGet(GLUT_SCREEN_WIDTH);
//        spx = sw/2 - width/2;
//        spy = sh/2 - height/2;
//	glutInitWindowPosition(spx, spy);
//	glutInitWindowSize(width, height);
//#endif
//	glutInitWindowPosition(200, 0);
//	glutInitWindowSize(640, 480);
//	glutCreateWindow("Rotating Flatland Logo");
//	glutDisplayFunc(display);
//	glutKeyboardFunc(parsekey);
//	glutSpecialFunc(parsekey_special);
//	glutReshapeFunc(myReshape);
//	glutIdleFunc(fl_Animate);
//	fl_logo_randomize();
//	fl_logoinit();
//	glutSwapBuffers();
//	glutMainLoop();
//	return 0;             /* ANSI C requires main to return int. */
//}
