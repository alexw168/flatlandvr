/*
 * Def_logo.c
 *
 * This file is lifted from part of the openGL-logo demo.
 * (c) Henk Kok (kok@wins.uva.nl)
 *
 * Copying, redistributing, etc is permitted as long as this copyright
 * notice and the Dutch variable names :) stay in tact.
 */

#include "gheader.h"

//#define TYPEO

void draw_flatland_logo(void);

/* some math.h's don't define M_PI */
#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define ACC 8
#define ACC2 16
#define ACC3 48
#define ACC4 24
#define THLD 0.6
#define THLD2 0.8

extern int angle, rotating;
extern float progress;


GLfloat TRANS[9][3];
GLfloat ROTAXIS[9][3];
GLfloat ROT[9];

GLfloat char_A[ACC2][ACC][3];
GLfloat normal_A[ACC2][ACC][3];

GLfloat char_D[ACC2][ACC][3];
GLfloat normal_D[ACC2][ACC][3];

GLfloat char_P[ACC2][ACC][3];
GLfloat normal_P[ACC2][ACC][3];

GLfloat accSIN[ACC], accCOS[ACC];

GLfloat difmat4[4] = { 0.425, 0.570, 0.964, 1.0 };
GLfloat difamb4[4] = { 0.425, 0.570, 0.964, 1.0 };
GLfloat matspec4[4] = { 1.974, 1.974, 1.974, 1.0 };
GLfloat difmat4_2[4];
GLfloat difamb4_2[4];
GLfloat matspec4_2[4];
GLfloat alpha = 1.0;

int swap_position;

int rnd(int i)
{
	return (int)( i * ( random() / (RAND_MAX+1.0) ) );
}


void srnd()
{
    timeval time;
    gettimeofday( &time, 0 );
    srandom( time.tv_sec );
    random();
    random();

/*    
    int i;
    int j;
    for( j=0; j<10; j++ )
    {
    	i = rnd( 10 );
	fprintf( stdout, "rnd = %d\n", i );
    }
*/    
}


void groen_texture(void)
{
    int i;

    if (progress == 0.0 && alpha > 0.0)
      alpha -= 0.01;
    else if (progress > 0.0)
      alpha = 1.0;
    for (i = 0; i < 4; i++)
    {
      difmat4_2[i] = difmat4[i] * alpha;
      difamb4_2[i] = difamb4[i] * alpha;
      matspec4_2[i] = matspec4[i] * alpha;
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difmat4_2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, difamb4_2);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matspec4_2);
    glMaterialf(GL_FRONT, GL_SHININESS, 125.0);
}

void draw_flatland_logo(void) 
{
	const double scale_factor = 16.0;
	int i, Ntris;
	float mamb[4],mdif[4],mspec[4];
	float x0,y0,z0;
	float x,y,z;
	float dy, dy1 ,dz, h, f, eps;
	float p0[3],p1[3],p2[3];
	float baseamb_red, baseamb_green, baseamb_blue, basedif_red, 
	basedif_green, basedif_blue;

	glPushMatrix();

	//glDisable(GL_CULL_FACE);

	glScaled ( scale_factor, scale_factor, scale_factor );
	Ntris = 12;
	h = 2.0;
	x0 = 1.2;
	y0 = 0.6;
	z0 = 0.0;
	f = 0.9;
	dz = 0.2;
	dy1 = 0.2;
	dy = y0;
	eps = 0.001;

	baseamb_red = 0.0;
	baseamb_green = 0.2;
	baseamb_blue = 0.0;
	mamb[0] = baseamb_red * alpha;
	mamb[1] = baseamb_green * alpha;
	mamb[2] = baseamb_blue * alpha;
	mamb[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);

	basedif_red = 0.0;
	basedif_green = 0.1;   
	basedif_blue = 0.0;   
	mdif[0] = basedif_red * alpha;
	mdif[1] = basedif_green * alpha;
	mdif[2] = basedif_blue * alpha;
	mdif[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif);

	mspec[0] = 0.0 * alpha;
	mspec[1] = 0.2 * alpha;
	mspec[2] = 0.0 * alpha;
	mspec[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec);    

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 100.0);    

	p0[0] = 0.0; p0[1] = 0.0; p0[2] = z0; 
	p1[0] = x0; p1[1] = y0; p1[2] = z0;
	p2[0] = -x0; p2[1] = y0; p2[2] = z0;  
	fl_draw_triangle( p0, p1, p2 );

	mamb[0] = 0.0;
	mamb[1] = 0.0;
	mamb[2] = 0.0;
	mamb[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);

	mdif[0] =  0.0;
	mdif[1] =  0.0;
	mdif[2] =  0.0;
	mdif[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif);

	p0[0] = 0.0; p0[1] = 0.0; p0[2] = z0+eps; 
	p1[0] = x0; p1[1] = y0; p1[2] = z0+eps;
	p2[0] = -x0; p2[1] = y0; p2[2] = z0+eps;      
	fl_draw_wire_triangle( p0, p1, p2 );

	y = y0;
	z = z0;

	for( i=1; i<=Ntris; i++ ) 
	{
		z -= dz;
		y += dy1;
		dy1 *=f;
		x = (y-h)*x0/(y0-h);

		if( y>h ) break;

		mamb[0] = baseamb_red * alpha;
		mamb[1] = (baseamb_green +(1.0-baseamb_green)*(float)i/(float)Ntris) * alpha;
		mamb[2] = baseamb_blue * alpha;
		mamb[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);

		mdif[0] =  basedif_red  * alpha;
		mdif[1] =  (basedif_green + (1.0-basedif_green)*(float)i/(float)Ntris) * alpha;
		mdif[2] =  basedif_blue  * alpha;
		mdif[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif);

		p0[0] = 0.0; p0[1] = (y-dy); p0[2] = z;
		p1[0] = x; p1[1] = y; p1[2] = z;
		p2[0] = -x; p2[1] = y; p2[2] = z;     
		fl_draw_triangle( p0, p1, p2 ); 

		mamb[0] = 0.0;
		mamb[1] = 0.0;
		mamb[2] = 0.0;
		mamb[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);

		mdif[0] =  0.0;
		mdif[1] =  0.0;
		mdif[2] =  0.0;
		mdif[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif);

		p0[0] = 0.0; p0[1] = (y-dy); p0[2] = z+eps;
		p1[0] = x; p1[1] = y; p1[2] = z+eps;
		p2[0] = -x; p2[1] = y; p2[2] = z+eps; 
		fl_draw_wire_triangle( p0, p1, p2 );

		dy *= f;
	}

	mamb[0] = 0.5 * alpha;
	mamb[1] = 0.3 * alpha;
	mamb[2] = 0.1 * alpha;
	mamb[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);
	mdif[0] =  0.5 * alpha;
	mdif[1] =  0.2 * alpha;
	mdif[2] =  0.1 * alpha;
	mdif[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdif);
	p0[0] = 0.6*h; p0[1] = 0.0; p0[2] = 0.0;
	p1[0] = 0.6*h; p1[1] = h; p1[2] = z;
	p2[0] = 0.05*h; p2[1] = h; p2[2] = z; 
	fl_draw_triangle( p0, p1, p2 ); 
	p0[0] = -0.6*h; p0[1] = 0.0; p0[2] = 0.0;
	p2[0] = -0.6*h; p2[1] = h; p2[2] = z;
	p1[0] = -0.05*h; p1[1] = h; p1[2] = z;        
	fl_draw_triangle( p0, p1, p2 ); 

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 0.0);    

	glPopMatrix();
}


void def_P(void)
{
	float a, s, c, ln;
	int i,j,k,l,m,n;
	float dx, dy;
	float dx1, dx2, dy1, dy2, dz1, dz2;
	GLfloat center_P[ACC2][3];
	GLfloat width_P[ACC2];
	for (i=0;i<ACC2;i++)
	{
		a = 2.0*((float) i)*M_PI/ACC2;
		s = 1+sin(a);
		c = cos(a);
		center_P[i][0] = c*2.15;
		center_P[i][1] = s*2.1 + (s<-1.01?-0.7:(s>1.01?0.7:0)) + 0.7;
		center_P[i][2] = 0.0;
		width_P[i] = 0.5;
	}

	for (i=0;i<ACC2;i++)
	{
		j = (i+1)%ACC2;
		k = (i+ACC2-1)%ACC2;
		for (a=0;a<ACC;a++)
		{
			accCOS[(int) a] = c = cos(a*M_PI*2.0/ACC);
			accSIN[(int) a] = s = sin(a*M_PI*2.0/ACC);
			dx = center_P[j][0] - center_P[k][0];
			dy = center_P[j][1] - center_P[k][1];
			ln = sqrt(dx*dx+dy*dy);
			dx = dx/ln;
			dy = dy/ln;
			char_P[i][(int) a][0] = center_P[i][0] + width_P[i] * dy * c;
			char_P[i][(int) a][1] = center_P[i][1] - width_P[i] * dx * c;
			char_P[i][(int) a][2] = (s<-THLD?-THLD:(s>THLD?THLD:s));
		}
	}
	for (i=0;i<ACC;i++)
	{
		j = (i+1)%ACC;
		k = (i-1+ACC)%ACC;
		for (l=0;l<ACC2;l++)
		{
			m = (l+1)%ACC2;
			n = (l+ACC2-1)%ACC2;
			dx1 = char_P[m][i][0] - char_P[n][i][0];
			dy1 = char_P[m][i][1] - char_P[n][i][1];
			dz1 = char_P[m][i][2] - char_P[n][i][2];
			dx2 = char_P[l][k][0] - char_P[l][j][0];
			dy2 = char_P[l][k][1] - char_P[l][j][1];
			dz2 = char_P[l][k][2] - char_P[l][j][2];
			normal_P[l][i][0] = dy2*dz1 - dy1*dz2;
			normal_P[l][i][1] = dz2*dx1 - dz1*dx2;
			normal_P[l][i][2] = dx2*dy1 - dx1*dy2;
		}
	}
}

void def_A(void)
{
	float a, s, c, ln;
	int i,j,k,l,m,n;
	float dx, dy;
	float dx1, dx2, dy1, dy2, dz1, dz2;
	GLfloat center_A[ACC2][3];
	GLfloat width_A[ACC2];
	for (i=0;i<ACC2;i++)
	{
		a = 2.0*((float) i)*M_PI/ACC2;
		s = 1+sin(a);
		c = cos(a);
		center_A[i][0] = c*2.15;
		center_A[i][1] = s*2.1 + (s<-1.01?-0.7:(s>1.01?0.7:0)) + 0.7;
		center_A[i][2] = 0.0;
		width_A[i] = 0.5;
	}

	for (i=0;i<ACC2;i++)
	{
		j = (i+1)%ACC2;
		k = (i+ACC2-1)%ACC2;
		for (a=0;a<ACC;a++)
		{
			accCOS[(int) a] = c = cos(a*M_PI*2.0/ACC);
			accSIN[(int) a] = s = sin(a*M_PI*2.0/ACC);
			dx = center_A[j][0] - center_A[k][0];
			dy = center_A[j][1] - center_A[k][1];
			ln = sqrt(dx*dx+dy*dy);
			dx = dx/ln;
			dy = dy/ln;
			char_A[i][(int) a][0] = center_A[i][0] + width_A[i] * dy * c;
			char_A[i][(int) a][1] = center_A[i][1] - width_A[i] * dx * c;
			char_A[i][(int) a][2] = (s<-THLD?-THLD:(s>THLD?THLD:s));
		}
	}

	for (i=0;i<ACC;i++)
	{
		j = (i+1)%ACC;
		k = (i-1+ACC)%ACC;
		for (l=0;l<ACC2;l++)
		{
			m = (l+1)%ACC2;
			n = (l+ACC2-1)%ACC2;
			dx1 = char_A[m][i][0] - char_A[n][i][0];
			dy1 = char_A[m][i][1] - char_A[n][i][1];
			dz1 = char_A[m][i][2] - char_A[n][i][2];
			dx2 = char_A[l][k][0] - char_A[l][j][0];
			dy2 = char_A[l][k][1] - char_A[l][j][1];
			dz2 = char_A[l][k][2] - char_A[l][j][2];
			normal_A[l][i][0] = dy2*dz1 - dy1*dz2;
			normal_A[l][i][1] = dz2*dx1 - dz1*dx2;
			normal_A[l][i][2] = dx2*dy1 - dx1*dy2;
		}
	}
}

void def_D(void)
{
	float a, s, c, ln;
	int i,j,k,l,m,n;
	float dx, dy;
	float dx1, dx2, dy1, dy2, dz1, dz2;
	GLfloat center_D[ACC2][3];
	GLfloat width_D[ACC2];
	for (i=0;i<ACC2;i++)
	{
		a = 2.0*((float) i)*M_PI/ACC2;
		s = 1+sin(a);
		c = cos(a);
		center_D[i][0] = c*2.15;
		center_D[i][1] = s*2.1 + (s<-1.01?-0.7:(s>1.01?0.7:0)) + 0.7;
		center_D[i][2] = 0.0;
		width_D[i] = 0.5;
	}

	for (i=0;i<ACC2;i++)
	{
		j = (i+1)%ACC2;
		k = (i+ACC2-1)%ACC2;
		for (a=0;a<ACC;a++)
		{
			accCOS[(int) a] = c = cos(a*M_PI*2.0/ACC);
			accSIN[(int) a] = s = sin(a*M_PI*2.0/ACC);
			dx = center_D[j][0] - center_D[k][0];
			dy = center_D[j][1] - center_D[k][1];
			ln = sqrt(dx*dx+dy*dy);
			dx = dx/ln;
			dy = dy/ln;
			char_D[i][(int) a][0] = center_D[i][0] + width_D[i] * dy * c;
			char_D[i][(int) a][1] = center_D[i][1] - width_D[i] * dx * c;
			char_D[i][(int) a][2] = (s<-THLD?-THLD:(s>THLD?THLD:s));
		}
	}

	for (i=0;i<ACC;i++)
	{
		j = (i+1)%ACC;
		k = (i-1+ACC)%ACC;
		for (l=0;l<ACC2;l++)
		{
			m = (l+1)%ACC2;
			n = (l+ACC2-1)%ACC2;
			dx1 = char_D[m][i][0] - char_D[n][i][0];
			dy1 = char_D[m][i][1] - char_D[n][i][1];
			dz1 = char_D[m][i][2] - char_D[n][i][2];
			dx2 = char_D[l][k][0] - char_D[l][j][0];
			dy2 = char_D[l][k][1] - char_D[l][j][1];
			dz2 = char_D[l][k][2] - char_D[l][j][2];
			normal_D[l][i][0] = dy2*dz1 - dy1*dz2;
			normal_D[l][i][1] = dz2*dx1 - dz1*dx2;
			normal_D[l][i][2] = dx2*dy1 - dx1*dy2;
		}
	}
}



void fl_logo_randomize(void)
{
	int i;
	for (i=0;i<9;i++)
	{
		TRANS[i][0] = rnd(100)-rnd(100);
		TRANS[i][1] = rnd(100)-rnd(100);
		TRANS[i][2] = rnd(100)-rnd(100);
		ROTAXIS[i][0] = rnd(100)-rnd(100);
		ROTAXIS[i][1] = rnd(100)-rnd(100);
		ROTAXIS[i][2] = rnd(100)-rnd(100);
		ROT[i]=rnd(3600)-rnd(3600);
	}
}

void def_logo(void)
{
    	srnd();
	swap_position = rnd( 15 );
	def_P();
        def_A();
        def_D();
}


void draw_A(void)
{
	int i,j,k;
    for (i=0;i<ACC;i++)
	{
		k = i+1;
		if (k>=ACC)
			k = 0;
		glBegin(GL_QUAD_STRIP);
		for (j=0;j<ACC2;j++)
		{
			glNormal3f(normal_A[j][k][0], normal_A[j][k][1], normal_A[j][k][2]);
			glVertex3f(char_A[j][k][0], char_A[j][k][1], char_A[j][k][2]);
			glNormal3f(normal_A[j][i][0], normal_A[j][i][1], normal_A[j][i][2]);
			glVertex3f(char_A[j][i][0], char_A[j][i][1], char_A[j][i][2]);
		}
		glNormal3f(normal_A[0][k][0], normal_A[0][k][1], normal_A[0][k][2]);
		glVertex3f(char_A[0][k][0], char_A[0][k][1], char_A[0][k][2]);
		glNormal3f(normal_A[0][i][0], normal_A[0][i][1], normal_A[0][i][2]);
		glVertex3f(char_A[0][i][0], char_A[0][i][1], char_A[0][i][2]);
		glEnd();
	}
	j = 0;
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(normal_A[0][i][0], normal_A[0][i][1], normal_A[0][i][2]);
		glVertex3f(char_A[0][i][0]+0.0, 0.0, 1.0*char_A[0][i][2]);
		glVertex3f(char_A[0][i][0]+0.0, 3.0, 1.0*char_A[0][i][2]);
	}
	glNormal3f(normal_A[0][0][0], normal_A[0][0][1], normal_A[0][0][2]);
	glVertex3f(char_A[0][0][0]+0.0, 0.0, 1.0*char_A[0][0][2]);
	glVertex3f(char_A[0][0][0]+0.0, 3.0, 1.0*char_A[0][0][2]);
	glEnd();
}


void draw_D(void)
{
	int i,j,k;
    for (i=0;i<ACC;i++)
	{
		k = i+1;
		if (k>=ACC)
			k = 0;
		glBegin(GL_QUAD_STRIP);
		for (j=0;j<ACC2;j++)
		{
			glNormal3f(normal_D[j][k][0], normal_D[j][k][1], normal_D[j][k][2]);
			glVertex3f(char_D[j][k][0], char_D[j][k][1], char_D[j][k][2]);
			glNormal3f(normal_D[j][i][0], normal_D[j][i][1], normal_D[j][i][2]);
			glVertex3f(char_D[j][i][0], char_D[j][i][1], char_D[j][i][2]);
		}
		glNormal3f(normal_D[0][k][0], normal_D[0][k][1], normal_D[0][k][2]);
		glVertex3f(char_D[0][k][0], char_D[0][k][1], char_D[0][k][2]);
		glNormal3f(normal_D[0][i][0], normal_D[0][i][1], normal_D[0][i][2]);
		glVertex3f(char_D[0][i][0], char_D[0][i][1], char_D[0][i][2]);
		glEnd();
	}
	j = 0;
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(normal_D[0][i][0], normal_D[0][i][1], normal_D[0][i][2]);
		glVertex3f(char_D[0][i][0]+0.0, 3.0, 1.0*char_D[0][i][2]);
		glVertex3f(char_D[0][i][0]+0.0, 9.6, 1.0*char_D[0][i][2]);
	}
	glNormal3f(normal_D[0][0][0], normal_D[0][0][1], normal_D[0][0][2]);
	glVertex3f(char_D[0][0][0]+0.0, 3.0, 1.0*char_D[0][0][2]);
	glVertex3f(char_D[0][0][0]+0.0, 9.6, 1.0*char_D[0][0][2]);
	glEnd();
}


void draw_N(void)
{
	int i,j,k;
    for (i=0;i<ACC;i++)
	{
		k = i+1;
		if (k>=ACC)
			k = 0;
		glBegin(GL_QUAD_STRIP);
		for (j=0;j<=ACC2/2;j++)
		{
			glNormal3f(normal_P[j][k][0], normal_P[j][k][1], normal_P[j][k][2]);
			glVertex3f(char_P[j][k][0], char_P[j][k][1], char_P[j][k][2]);
			glNormal3f(normal_P[j][i][0], normal_P[j][i][1], normal_P[j][i][2]);
			glVertex3f(char_P[j][i][0], char_P[j][i][1], char_P[j][i][2]);
		}
		glEnd();
	}

	j = 0;
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(normal_P[0][i][0], normal_P[0][i][1], normal_P[0][i][2]);
		glVertex3f(char_P[0][i][0]-4.3, 0.2, 1.0*char_P[0][i][2]);
		glVertex3f(char_P[0][i][0]-4.3, 6.0, 1.0*char_P[0][i][2]);
	}
	glNormal3f(normal_P[0][0][0], normal_P[0][0][1], normal_P[0][0][2]);
	glVertex3f(char_P[0][0][0]-4.3, 0.2, 1.0*char_P[0][0][2]);
	glVertex3f(char_P[0][0][0]-4.3, 6.0, 1.0*char_P[0][0][2]);
	glEnd();
	j = 0;

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(normal_P[0][i][0], normal_P[0][i][1], normal_P[0][i][2]);
		glVertex3f(char_P[0][i][0], 0.2, 1.0*char_P[0][i][2]);
		glVertex3f(char_P[0][i][0], 3.4, 1.0*char_P[0][i][2]);
	}
	glNormal3f(normal_P[0][0][0], normal_P[0][0][1], normal_P[0][0][2]);
	glVertex3f(char_P[0][0][0], 0.2, 1.0*char_P[0][0][2]);
	glVertex3f(char_P[0][0][0], 3.4, 1.0*char_P[0][0][2]);
	glEnd();
}


void draw_T(void)
{
	int i;

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(accSIN[i], 0.0, accCOS[i] );
		glVertex3f(0.5*accSIN[i], 9.6, 0.5*accCOS[i]);
		glVertex3f(0.5*accSIN[i], 0.0, 0.5*accCOS[i] );
	}
	glNormal3f(accSIN[0], 0.0, accCOS[0] );
	glVertex3f(0.5*accSIN[0], 9.6, 0.5*accCOS[0]);
	glVertex3f(0.5*accSIN[0], 0.0, 0.5*accCOS[0] );
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(0.0, accSIN[i], accCOS[i] );
		glVertex3f(-2.0, 6.9+0.5*accSIN[i], 0.5*accCOS[i] );
		glVertex3f(2.0, 6.9+0.5*accSIN[i], 0.5*accCOS[i]);
	}
	glNormal3f(0.0, accSIN[0], accCOS[0] );
	glVertex3f(-2.0, 6.9+0.5*accSIN[0], 0.5*accCOS[0] );
	glVertex3f(2.0, 6.9+0.5*accSIN[0], 0.5*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(2.0, 6.9, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(2.0, 6.9+0.5*accSIN[i], 0.5*accCOS[i]);
	glVertex3f(2.0, 6.9+0.5*accSIN[ACC-1], 0.5*accCOS[ACC-1]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(-2.0, 6.9, 0.0);
	for (i=0;i<ACC-1;i++)
		glVertex3f(-2.0, 6.9+0.5*accSIN[i], 0.5*accCOS[i]);
	glVertex3f(-2.0, 6.9+0.5*accSIN[0], 0.5*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 9.6, 0.0);
	for (i=0;i<ACC-1;i++)
		glVertex3f(0.5*accSIN[i], 9.6, 0.5*accCOS[i]);
	glVertex3f(0.5*accSIN[0], 9.6, 0.5*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(0.5*accSIN[i], 0.0, 0.5*accCOS[i]);
	glVertex3f(0.5*accSIN[ACC-1], 0.0, 0.5*accCOS[ACC-1]);
	glEnd();
}

void draw_L(void)
{
	int i;

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(accSIN[i], 0.0, accCOS[i] );
		glVertex3f(0.5*accSIN[i], 9.6, 0.5*accCOS[i]);
		glVertex3f(0.5*accSIN[i], 0.0, 0.5*accCOS[i] );
	}
	glNormal3f(accSIN[0], 0.0, accCOS[0] );
	glVertex3f(0.5*accSIN[0], 9.6, 0.5*accCOS[0]);
	glVertex3f(0.5*accSIN[0], 0.0, 0.5*accCOS[0] );
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 9.6, 0.0);
	for (i=0;i<ACC;i++)
		glVertex3f(0.5*accSIN[i], 9.6, 0.5*accCOS[i]);
	glVertex3f(0.5*accSIN[0], 9.6, 0.5*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(0.5*accSIN[i], 0.0, 0.5*accCOS[i]);
	glVertex3f(0.5*accSIN[ACC-1], 0.0, 0.5*accCOS[ACC-1]);
	glEnd();
}

void draw_F(void)
{
	int i;

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(accSIN[i], 0.0, accCOS[i] );
		glVertex3f(0.9*accSIN[i], 9.6-0.9*accSIN[i], 0.9*accCOS[i]);
		glVertex3f(0.9*accSIN[i], 0.0, 0.9*accCOS[i] );
	}
	glNormal3f(accSIN[0], 0.0, accCOS[0] );
	glVertex3f(0.9*accSIN[0], 9.6-0.9*accSIN[0], 0.9*accCOS[0]);
	glVertex3f(0.9*accSIN[0], 0.0, 0.9*accCOS[0] );
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(0.0, accSIN[i], accCOS[i] );
		glVertex3f(-0.9*accSIN[i], 9.6+0.9*accSIN[i], 0.9*accCOS[i] );
		glVertex3f(5.6, 9.6+0.9*accSIN[i], 0.9*accCOS[i]);
	}
	glNormal3f(0.0, accSIN[0], accCOS[0] );
	glVertex3f(-0.9*accSIN[0], 9.6+0.9*accSIN[0], 0.9*accCOS[0] );
	glVertex3f(5.6, 9.6+0.9*accSIN[0], 0.9*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(5.6, 9.6, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(5.6, 9.6+0.9*accSIN[i], 0.9*accCOS[i]);
	glVertex3f(5.6, 9.6+0.9*accSIN[ACC-1], 0.9*accCOS[ACC-1]);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<ACC;i++)
	{
		glNormal3f(0.0, accSIN[i], accCOS[i] );
		glVertex3f(0.45, 5.0+0.9*accSIN[i], 0.9*accCOS[i] );
		glVertex3f(3.0, 5.0+0.9*accSIN[i], 0.9*accCOS[i]);
	}
	glNormal3f(0.0, accSIN[0], accCOS[0] );
	glVertex3f(0.45, 5.0+0.9*accSIN[0], 0.9*accCOS[0] );
	glVertex3f(3.0, 5.0+0.9*accSIN[0], 0.9*accCOS[0]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(3.0, 5.0, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(3.0, 5.0+0.9*accSIN[i], 0.9*accCOS[i]);
	glVertex3f(3.0, 5.0+0.9*accSIN[ACC-1], 0.9*accCOS[ACC-1]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	for (i=ACC-1;i>=0;i--)
		glVertex3f(0.9*accSIN[i], 0.0, 0.9*accCOS[i]);
	glVertex3f(0.9*accSIN[ACC-1], 0.0, 0.9*accCOS[ACC-1]);
	glEnd();
}
void draw_part(int i)
{
	glPushMatrix();
	glTranslatef(TRANS[i][0]*progress, TRANS[i][1]*progress, TRANS[i][2]*progress);
	glRotatef(ROT[i]*progress, ROTAXIS[i][0], ROTAXIS[i][1], ROTAXIS[i][2]);
	switch(i)
	{
	case 0: draw_flatland_logo(); break;
	case 1: draw_F(); break;
	case 2: draw_L(); break;
	case 3: draw_A(); break;
	case 4: draw_T(); break;
	case 5: draw_L(); break;
	case 6: draw_A(); break;
	case 7: draw_N(); break;
	case 8: draw_D(); break;
	}
	glPopMatrix();
}

void draw_logo(void)
{
    float left_start = -20.0;
    float f_width = 7.2;
    float l_width = 4.3;
    float a_width = 6.6;
    float t_width = 3.7;
    float n_width = 6.6;
    float d_width = 6.6;
    float position;
    char flatland[] = "flatland";
    int i; 
    char tmp;

    
#ifdef TYPEO
	if( swap_position>=0 && swap_position<8 )
	{ 
	    tmp = flatland[swap_position];
	    flatland[swap_position] = flatland[swap_position+1];
	    flatland[swap_position+1] = tmp;
    	}
#endif
	        
	glEnable(GL_CULL_FACE);

    	glPushMatrix( );
	
    	glTranslatef(1.0, -5.0, 0.0);

    	glPushMatrix( );
        glRotatef(15.0, 1.0, 0.0, 0.0);
	draw_part(0);
    	glPopMatrix( );
    	
    	groen_texture();

    	position = left_start;

    	for( i=0; i<8; i++ )
	{
    	    switch( flatland[i] )
	    {
	    case 'f':
    		// width of F = 7.2
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef(-20.0, 4.3, 5.0);
		draw_part(1);
    		glPopMatrix( );
    		position += f_width;
    	    	break;

    	    case 'l':
		//width of l = 4.3
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef(-12.8, 4.3, 5.0);
	//	glTranslatef(7.2, 0.0, 0.0);
		draw_part(2);
    		glPopMatrix( );
    		position += l_width;
    	    	break;
		
	    case 'a':
    		// width of a = 4.7
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef( left_start, 4.3, 5.0);
	//	glTranslatef(-8.5, 4.3, 5.0);
	//	glTranslatef(4.3, 0.0, 0.0);
		draw_part(3);
    		glPopMatrix( );
    		position += a_width;
    	    	break;
		
	    case 't':
    		// width of t = 3.7
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef(-3.8, 4.3, 5.0);
	//	glTranslatef(4.7, 0.0, 0.0);
		draw_part(4);
    		glPopMatrix( );
    		position += t_width;
    	    	break;
		
//    		// width of l = 4.3
//    		glPushMatrix( );
//		glTranslatef( position, 4.3, 5.0);
//	//	glTranslatef(-0.1, 4.3, 5.0);
//	//	glTranslatef(3.7, 0.0, 0.0);
//		draw_part(5);
//    		glPopMatrix( );
//    		position += l_width;
//    	    	
//    		// width of a = 6.6
//    		glPushMatrix( );
//		glTranslatef( position, 4.3, 5.0);
//	//	glTranslatef(4.2, 4.3, 5.0);
//	//	glTranslatef(4.3, 0.0, 0.0);
//		draw_part(6);
//    		glPopMatrix( );
//    		position += a_width;
    	    	
	    case 'n':
    		// width of n = 6.6
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef(10.8, 4.3, 5.0);
	//	glTranslatef(6.6, 0.0, 0.0);
		draw_part(7);
    		glPopMatrix( );
    		position += n_width;
    	    	break;
		
	    case 'd':
    		// width of d = 6.6
    		glPushMatrix( );
		glTranslatef( position, 4.3, 5.0);
	//	glTranslatef(17.4, 4.3, 5.0);
	//        glTranslatef(6.8, 0.0, 0.0);
        	draw_part(8);
    		glPopMatrix( );
    		position += d_width;
		break;
		
	    default:
	    	break;
	    }
    	}
    	glPopMatrix( );
}
