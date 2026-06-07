

/*
TCG6223 Computer Graphics
CGLab10and11.cpp
Objective: Lab10 on Lighting &
Lab11 on Textures
Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
INSTRUCTIONS
============
* Please refer to CGLabmain.cpp for instructions
CHANGE LOG
==========
*/
#include <GL/glut.h>
#include "CGLab10and11.hpp"


using namespace CGLab10and11;

MyVirtualWorld::MyVirtualWorld()
{
xmin = zmin =-50.0f;
xmax = zmax = 50.0f;
height = 50.0f;
xdim = xmax-xmin;
zdim = zmax-zmin;
ydim=height;

/// LAB 11 TASK 2 start =================================================================
numberOfTexture = 0;
textureNumbers = NULL;
textureison = false;
/// LAB 11 TASK 2 end =================================================================
}
MyVirtualWorld::~MyVirtualWorld()
{
/// LAB 11 TASK 2 start =================================================================
if (textureNumbers != NULL)
delete[] textureNumbers;
/// LAB 11 TASK 2 end =================================================================
}
void MyVirtualWorld::init()
{
setupLights();
/// LAB 11 TASK 2 start =================================================================
setupTextures();
/// LAB 11 TASK 2 end =================================================================
timeold = glutGet(GLUT_ELAPSED_TIME);
}

/// LAB 11 TASK 2 start =================================================================
void MyVirtualWorld::setupTextures()
{
    numberOfTexture = 3;
textureNumbers = new GLuint[numberOfTexture];
//Set pixel storage mode
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//set texture environment parameters
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//Generate texture map identifiers
glGenTextures(numberOfTexture, textureNumbers);
//Read image data into memory
MyImage myimage[numberOfTexture];
myimage[0].loadJPG("images/marble1.jpg");
myimage[1].loadJPG("images/wood1.jpg");
myimage[2].loadJPG("images/brick1.jpg");
//Try this: myimage[2].loadBMP("images/rocks.bmp");
//for each texture map identifiers
for (int i=0; i<numberOfTexture; i++)
{
//Bind a named texture to a texturing target
glBindTexture(GL_TEXTURE_2D, textureNumbers[i]);
//Set texture parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//Set texture map
glTexImage2D(GL_TEXTURE_2D, // defining a 2D texture
0, // level (not multi-resolution)
GL_RGBA, // format of OpenGL's texture
myimage[i].width, myimage[i].height,// dimensions of the image
0, // border
GL_RGB, GL_UNSIGNED_BYTE, // format of image
myimage[i].buffer // pixel data of image
);
}
textureison = true;
}
void MyVirtualWorld::toggleTextures()
{
textureison = !textureison;
}
/// LAB 11 TASK 2 end =================================================================

void MyVirtualWorld::setupLights()
{
//to be filled later.....
/// TASK 1B start =================================================================
glEnable(GL_LIGHTING);
//GL_COLOR_MATERIAL
// * relevant only if lighting is enabled
// * disabled by default
// * if enabled, glColor*(...) is in effect to change the color
// tracked by glColorMaterial
// (meaning that in our case here, glColor*(...) affect
// the diffuse color of the frant face)
// * if disbled, glMaterial*(...) is in effect to change the color
// glColor*(...) will not work!
//glColor*(...) always in effect if lighting is not enabled
glColorMaterial(GL_FRONT, GL_DIFFUSE);
glEnable(GL_COLOR_MATERIAL);
//ensure unit vectors remain unit vectors after
// modelview scaling
glEnable(GL_NORMALIZE);
///TASK 1B end =================================================================

///TASK 2A start =================================================================
myspotlights.setupLights();
///TASK 2A end =================================================================

///TASK 2B end =================================================================
myswinglights.setupLights();
///TASK 2B end =================================================================

///TASK 1C start =================================================================
//define the color of light, i.e. LIGHT0
GLfloat mycolor[] = { 0.15, 0.15, 0.15};
glLightfv(GL_LIGHT0, GL_DIFFUSE, mycolor);
//enable the light, i.e. LIGHT0
glEnable(GL_LIGHT0);
///TASK 1C end =================================================================

///TASK 3 end =================================================================
for (int i=0; i<6; ++i)
lighton[i] = true;
///TASK 3 end =================================================================
}

///TASK 3 start =================================================================
//light 0 is a directional light
//lights 1,2,3 are lights 0,1,2 of spotlights
//lights 4,5 are lights 0,1 of swinglights
///TASK 3 end =================================================================
void MyVirtualWorld::toggleLight(int lightno)
{
//to be filled later.....
///TASK 3 start =================================================================
if (lightno==0)
{
lighton[0] = !lighton[0];
if (lighton[0])
glEnable( GL_LIGHT0 );
else
glDisable( GL_LIGHT0 );
}
else if (lightno>=1 && lightno<=3)
{
myspotlights.toggleLight(lightno-1);
}
else if (lightno>=4 && lightno<=5)
{
myswinglights.toggleLight(lightno-4);
}
///TASK 3 end =================================================================
}
void MyVirtualWorld::draw()
{
///TASK 1C start =================================================================
//define the GL_POSITION of the light, since the 4th number
// is 0.0 rather than 1.0, this light is a directional light
//Take note that the GL_POSITION should be specified in the
// modeling code (here) so that it will be relative to the
// current coordinate system.
GLfloat position[] = {1.0f, 1.0f, 0.0f, 0.0f};
glLightfv(GL_LIGHT0, GL_POSITION, position);
///TASK 1C end =================================================================
glPushMatrix();
mymovingsmiley.draw();
myfan.draw();
drawFourTeapot();
glPushMatrix();
glTranslatef(0.0f, -0.01f, 0.0f);
drawFloor();
drawRoof();
drawWalls();
glPopMatrix();
glPushMatrix();
glTranslatef(0.0f, height, 0.0f);
myswinglights.draw();
glPopMatrix();
glPopMatrix();
}
void MyVirtualWorld::drawSpotLights()
{
myspotlights.draw();
}
void MyVirtualWorld::drawFloor()
{
/// LAB 11 TASK 3A start =================================================================
//Coordinates for the floor is:
// xmin<=x<=xmax, y=0, zmin<=z<=zmax
// (declared in constructor of MyVirtualWorld)
//Texture coordinates for vertex (xmin, 0.0, zmin) is:
// (s=0, t=0)
//Texture coordinates for vertex (xmax, 0.0, zmax) is:
// (s=texSMax, t=texTMax)
GLfloat texSMax = 5.0f;
GLfloat texTMax = 5.0f;

//The size of each quad used is:
// quadSizeX x quadSizeZ
GLfloat quadSizeX = 0.25f;
GLfloat quadSizeZ = 0.25f;
//For each quad, the 2 end corners have coord below:
// (x1, 0.0, z1) and (x2, 0.0, z2)
// and the texture coord for those corners are:
// (s=s1, t=t1) and (s=s2, t=t2)
GLfloat x1, z1, x2, z2;
GLfloat s1, t1, s2, t2;
//Given a vertex (x,y,z) on the roof, the texture coord (s,t) is given by:
// s = (texSMax/xdim) * (x-xmin)
// t = (texTMax/zdim) * (z-zmin)
//Thus, the pre-calulated scaling factors as below:
GLfloat texScaleX = texSMax/xdim;
GLfloat texScaleZ = texTMax/zdim;
if (textureison)
{
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, textureNumbers[0]);
}
glPushMatrix();
for (z1=zmin; z1<zmax; z1+=quadSizeZ)
{
z2 = z1 + quadSizeZ;
t1 = texScaleZ*(z1-zmin);
t2 = texScaleZ*(z2-zmin);
for (x1=xmin; x1<xmax; x1+=quadSizeX)
{
x2 = x1 + quadSizeX;
s1 = texScaleX*(x1-xmin);
s2 = texScaleX*(x2-xmin);
glColor3fv( mywhite );
glBegin(GL_QUADS);
//pointing upwards
glNormal3f(0.0f, 1.0f, 0.0f);
//Note: Anti-clockwise as looking from inside
glTexCoord2f(s1, t1);
glVertex3f(x1, 0.0f, z1);
glTexCoord2f(s1, t2);
glVertex3f(x1, 0.0f, z2);
glTexCoord2f(s2, t2);
glVertex3f(x2, 0.0f, z2);
glTexCoord2f(s2, t1);
glVertex3f(x2, 0.0f, z1);
glEnd();
}
}
glPopMatrix();
glDisable(GL_TEXTURE_2D);
/// LAB 11 TASK 3A end =================================================================

//glPushMatrix();
//GLfloat tilesize=0.25f;
//for (GLfloat z=zmin; z<zmax; z+=tilesize)
//{
//for (GLfloat x=xmin; x<xmax; x+=tilesize)
//{
//glColor3fv( mywhite );
//glBegin(GL_QUADS);
//glNormal3f(0.0f, 1.0f, 0.0f);
//glVertex3f(x , 0.0f, z);
//glVertex3f(x , 0.0f, z+tilesize);
//glVertex3f(x+tilesize, 0.0f, z+tilesize);
//glVertex3f(x+tilesize, 0.0f, z);
//glEnd();
//}
//}
//glPopMatrix();
}
void MyVirtualWorld::drawRoof()
{
/// LAB 11 TASK 3B start =================================================================

//Coordinates for the roof is:
// xmin<=x<=xmax, y=height, zmin<=z<=zmax
// (declared in constructor of MyVirtualWorld)
//Texture coordinates for vertex (xmin, height, zmin) is:
// (s=0, t=0)
//Texture coordinates for vertex (xmax, height, zmax) is:
// (s=texSMax, t=texTMax)
GLfloat texSMax = 10.0f;
GLfloat texTMax = 10.0f;
//The roof consists of checker boxes, dimension of each checker box:
// checkerboxSizeX x checkerboxSizeZ
GLfloat checkerboxSizeX = 5.0f;
GLfloat checkerboxSizeZ = 5.0f;
//Number of quads per checker box:
// quadsPerCheckerboxX x quadsPerCheckerboxZ
// (for each directions)
GLint quadsPerCheckerboxX = 2;
GLint quadsPerCheckerboxZ = 2;
//Thus the size of each quad is:
// quadSizeX x quadSizeZ
GLfloat quadSizeX = checkerboxSizeX / quadsPerCheckerboxX;
GLfloat quadSizeZ = checkerboxSizeZ / quadsPerCheckerboxZ;
//Thus, for each quad, the 2 end corners have coord below:
// (x1, height, z1) and (x2, height, z2)
// and the texture coord for those corners are:
// (s=s1, t=t1) and (s=s2, t=t2)
GLfloat x1, z1, x2, z2;
GLfloat s1, t1, s2, t2;
//Given a vertex (x,y,z) on the roof, the texture coord (s,t) is given by:
// s = (texSMax/xdim) * (x-xmin)
// t = (texTMax/zdim) * (z-zmin)
//Thus, the pre-calulated scaling factors as below:
GLfloat texScaleX = texSMax/xdim;
GLfloat texScaleZ = texTMax/zdim;
static GLfloat* color[] = { myblack, mywhite };
//take note, the mywhite and myblack etc. are defined in CGLabmain.hpp
if (textureison)
{
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, textureNumbers[1]);
}
glPushMatrix();
int startcolorno = 0;
for (GLfloat z=zmin; z<zmax; z+=checkerboxSizeZ)
{
int colorno = startcolorno;
for (GLfloat x=xmin; x<xmax; x+=checkerboxSizeX)
{
GLint i,j;
//draw all the quads in the checker box
glColor3fv( color[colorno] );
for (j=0, z1 = z, z2 = z+quadSizeZ;
j<quadsPerCheckerboxZ;
++j, z1+=quadSizeZ, z2+=quadSizeZ)
{
t1 = texScaleZ*(z1-zmin);
t2 = texScaleZ*(z2-zmin);
for (i=0, x1 = x, x2 = x+quadSizeX;
i<quadsPerCheckerboxX;
++i, x1+=quadSizeX, x2+=quadSizeX)
{
s1 = texScaleX*(x1-xmin);
s2 = texScaleX*(x2-xmin);
glBegin(GL_QUADS);
//pointing downwards
glNormal3f(0.0f, -1.0f, 0.0f);
//Note: Anti-clockwise as looking from inside
glTexCoord2f(s1, t1);
glVertex3f(x1, height, z1);
glTexCoord2f(s2, t1);
glVertex3f(x2, height, z1);
glTexCoord2f(s2, t2);
glVertex3f(x2, height, z2);
glTexCoord2f(s1, t2);
glVertex3f(x1, height, z2);
glEnd();
} //for i
} //for j
if (colorno==0) colorno=1;
else colorno = 0;
} //for x
if (startcolorno==0) startcolorno=1;
else startcolorno = 0;
} //for z
glPopMatrix();
glDisable(GL_TEXTURE_2D);
/// LAB 11 TASK 3B end =================================================================

//static GLfloat* color[] = { myblack, mywhite };
//glPushMatrix();
//GLfloat xstep=5.0f;
//GLfloat zstep=5.0f;
//GLfloat tilesize=2.5f;
//int startcolorno = 0;
//for (GLfloat z=zmin; z<zmax; z+=zstep)
//{
//int colorno = startcolorno;
//for (GLfloat x=xmin; x<xmax; x+=xstep)
//{
//glColor3fv( color[colorno] );
//GLfloat xmax = x+xstep;
//GLfloat zmax = z+zstep;
//for (GLfloat xx=x; xx<xmax; xx+=tilesize)
//for (GLfloat zz=z; zz<zmax; zz+=tilesize)
//{
//glBegin(GL_QUADS);
//glNormal3f(0.0f, -1.0f, 0.0f);
//glVertex3f(xx , height, zz);
//glVertex3f(xx+tilesize, height, zz);
//glVertex3f(xx+tilesize, height, zz+tilesize);
//glVertex3f(xx , height, zz+tilesize);
//glEnd();
//}
//++colorno;
//if (colorno>1) colorno=0;
//}
//++startcolorno;
//if (startcolorno>1) startcolorno=0;
//}
//glPopMatrix();
}
void MyVirtualWorld::drawWalls()
{

/// LAB 11 TASK 4 start =================================================================
//Note: the method of building the walls is not exactly the best way
// to do it, the better way is to build one wall and transform the same
// wall to another location to build another
static GLfloat* color[] =
{ myblack, myred, myyellow, mygreen,
mycyan, myblue, mymagenta, mywhite };
//take note, the myred, myblue etc. are defined in CGLabmain.hpp
if (textureison)
{
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, textureNumbers[2]);
}
GLfloat texSMax = 5.0f;
GLfloat texTMax = 5.0f;
GLfloat brickSizeY = 5.0f;
GLfloat brickSizeZ = 5.0f;
GLint quadsPerBrickY = 2;
GLint quadsPerBrickZ = 2;
GLfloat quadSizeY = brickSizeY / quadsPerBrickY;
GLfloat quadSizeZ = brickSizeZ / quadsPerBrickZ;
GLfloat y1, z1, y2, z2;
GLfloat s1, t1, s2, t2;
GLfloat texScaleY = texSMax/ydim;
GLfloat texScaleZ = texTMax/zdim;
glPushMatrix();
int startcolorno = 0;
for (GLfloat z=zmin; z<zmax; z+=brickSizeZ)
{
int colorno = startcolorno;
for (GLfloat y=0; y<height; y+=brickSizeY)
{
GLint i,j;
//draw all the quads in the checker box
if (textureison) //use single color if texture is used
colorno = 7;
glColor3fv( color[colorno] );
for (j=0, z1 = z, z2 = z+quadSizeZ;
j<quadsPerBrickZ;
++j, z1+=quadSizeZ, z2+=quadSizeZ)
{
t1 = texScaleZ*(z1-zmin);
t2 = texScaleZ*(z2-zmin);
for (i=0, y1 = y, y2 = y+quadSizeY;
i<quadsPerBrickY;
++i, y1+=quadSizeY, y2+=quadSizeY)
{
s1 = texScaleY*y1; //ymin is 0.0
s2 = texScaleY*y2; //ymin is 0.0
glBegin(GL_QUADS);
//Wall on positive-x, normal pointing to negative-x
glNormal3f(-1.0f, 0.0f, 0.0f);
//Note: Anti-clockwise as looking from inside
glTexCoord2f(s1, t1);
glVertex3f(xmax, y1, z1);
glTexCoord2f(s1, t2);
glVertex3f(xmax, y1, z2);
glTexCoord2f(s2, t2);
glVertex3f(xmax, y2, z2);
glTexCoord2f(s2, t1);
glVertex3f(xmax, y2, z1);
//Wall on negative-x, normal pointing to positive-x
glNormal3f(1.0f, 0.0f, 0.0f);
//Note: Anti-clockwise as looking from inside
glVertex3f(xmin, y1, z1);
glVertex3f(xmin, y2, z1);
glVertex3f(xmin, y2, z2);
glVertex3f(xmin, y1, z2);
glEnd();
} //for i
} //for j
++colorno;
if (colorno>7) colorno=0;
} //for y
++startcolorno;
if (startcolorno>7) startcolorno=0;
} //for z
GLfloat brickSizeX = 5.0f;
GLint quadsPerBrickX = 2;
GLfloat quadSizeX = brickSizeX / quadsPerBrickX;
GLfloat x1, x2;
glPushMatrix();
startcolorno = 0;
for (GLfloat x=xmin; x<xmax; x+=brickSizeX)
{
int colorno = startcolorno;
for (GLfloat y=0; y<height; y+=brickSizeY)
{
GLint i,j;
//draw all the quads in the checker box
if (textureison) //use single color if texture is used
colorno = 7; glColor3fv( color[colorno] );
for (j=0, x1 = x, x2 = x+quadSizeX;
j<quadsPerBrickX;
++j, x1+=quadSizeX, x2+=quadSizeX)
{
for (i=0, y1 = y, y2 = y+quadSizeY;
i<quadsPerBrickY;
++i, y1+=quadSizeY, y2+=quadSizeY)
{
glBegin(GL_QUADS);
//Wall on positive-z, normal pointing to negative-z
glNormal3f(0.0f, 0.0f, -1.0f);
//Note: Anti-clockwise as looking from inside
glVertex3f(x1, y1, zmax);
glVertex3f(x1, y2, zmax);
glVertex3f(x2, y2, zmax);
glVertex3f(x2, y1, zmax);
//Wall on negative-z, normal pointing to positive-z
glNormal3f(0.0f, 0.0f, 1.0f);
//Note: Anti-clockwise as looking from inside
glVertex3f(x1, y1, zmin);
glVertex3f(x2, y1, zmin);
glVertex3f(x2, y2, zmin);
glVertex3f(x1, y2, zmin);
glEnd();
} //for i
} //for j
++colorno;
if (colorno>7) colorno=0;
} //for y
++startcolorno;
if (startcolorno>7) startcolorno=0;
} //for z
glPopMatrix();
glDisable(GL_TEXTURE_2D);
/// LAB 11 TASK 4 end =================================================================

//static GLfloat* color[] =
//{ myblack, myred, myyellow, mygreen,
//mycyan, myblue, mymagenta, mywhite };
////take note, the myred, myblue etc. are defined in CGLabmain.hpp
//glPushMatrix();
//GLfloat ystep=5.0f;
//GLfloat zstep=5.0f;
//GLfloat tilesize=2.5f;
//GLfloat xdim=xmax-xmin;
//GLfloat ydim=height;
//GLfloat zdim=zmax-zmin;
//GLfloat ymin=0.0f;
//GLfloat ymax=height;
//int startcolorno = 0;
//for (GLfloat z=zmin; z<zmax; z+=zstep)
//{
//int colorno = startcolorno;
//for (GLfloat y=0.0f; y<height; y+=ystep)
//{
//glColor3fv( color[colorno] );
//GLfloat ymax = y+ystep;
//GLfloat zmax = z+zstep;
//for (GLfloat yy=y; yy<ymax; yy+=tilesize)
//for (GLfloat zz=z; zz<zmax; zz+=tilesize)
//{
//glBegin(GL_QUADS);
//glNormal3f(-1.0f, 0.0f, 0.0f);
//glVertex3f(xmax, yy , zz);
//glVertex3f(xmax, yy , zz+tilesize);
//glVertex3f(xmax, yy+tilesize, zz+tilesize);
//glVertex3f(xmax, yy+tilesize, zz);
//glNormal3f(1.0f, 0.0f, 0.0f);
//glVertex3f(xmin, yy , zz);
//glVertex3f(xmin, yy+tilesize, zz);
//glVertex3f(xmin, yy+tilesize, zz+tilesize);
//glVertex3f(xmin, yy , zz+tilesize);
//glEnd();
//}
//++colorno;
//if (colorno>7) colorno=0;
//}
//++startcolorno;
//if (startcolorno>7) startcolorno=0;
//}
//GLfloat xstep=5.0f;
//startcolorno = 0;
//for (GLfloat x=xmin; x<xmax; x+=xstep)
//{
//int colorno = startcolorno;
//for (GLfloat y=0.0f; y<height; y+=ystep)
//{
//glColor3fv( color[colorno] );
//GLfloat ymax = y+ystep;
//GLfloat xmax = x+xstep;
//for (GLfloat yy=y; yy<ymax; yy+=tilesize)
//for (GLfloat xx=x; xx<xmax; xx+=tilesize)
//{
//glBegin(GL_QUADS);
//glNormal3f(0.0f, 0.0f, 1.0f);
//glVertex3f(xx , yy , zmin);
//glVertex3f(xx+tilesize, yy , zmin);
//glVertex3f(xx+tilesize, yy+tilesize, zmin);
//glVertex3f(xx , yy+tilesize, zmin);
//glNormal3f(0.0f, 0.0f, -1.0f);
//glVertex3f(xx , yy , zmax);
//glVertex3f(xx , yy+tilesize, zmax);
//glVertex3f(xx+tilesize, yy+tilesize, zmax);
//glVertex3f(xx+tilesize, yy , zmax);
//glEnd();
//}
//++colorno;
//if (colorno>7) colorno=0;
//}
//++startcolorno;
//if (startcolorno>7) startcolorno=0;
//}
//glPopMatrix();
}
void MyVirtualWorld::drawFourTeapot()
{
//for some reason, glutSolidTeapot are using
// clock-wise winding, thus must set this for them
// to be properly rendered
glFrontFace(GL_CW);
GLfloat dx = xmax-xmin-20.0f;
GLfloat dz = zmax-zmin-20.0f;
glPushMatrix();
glTranslatef(0.0f, 5.0f, 0.0f);
glTranslatef(xmax-10.0f, 0.0f, zmax-10.0f);
glColor3f(1.0f, 0.0f, 0.0f );
glutSolidTeapot(5);
glTranslatef(-dx, 0.0f, 0.0f);
glColor3f(0.0f, 1.0f, 0.0f );
glutSolidTeapot(5);
glTranslatef(0.0f, 0.0f, -dz);
glColor3f(0.0f, 0.0f, 1.0f );
glutSolidTeapot(5);
glTranslatef(dx, 0.0f, 0.0f);
glColor3f(0.0f, 1.0f, 1.0f );
glutSolidTeapot(5);
glPopMatrix();
//set back to Counter-Clock Winding
glFrontFace(GL_CCW);
}
MySpotLights::MySpotLights()
{
pObj = gluNewQuadric();
rotateangle= 0.0f;
rotatespeed = 360.0; //degree per sec
gluQuadricNormals(pObj, GLU_SMOOTH);
}
MySpotLights::~MySpotLights()
{
}
void MySpotLights::setupLights()
{
//to be filled later.....
///TASK 2A start =================================================================
glLightfv(GL_LIGHT1, GL_AMBIENT, myred);
glLightfv(GL_LIGHT1, GL_DIFFUSE, myred);
glLightfv(GL_LIGHT1, GL_SPECULAR, myred);
glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 35);
glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.04);
glLightfv(GL_LIGHT2, GL_AMBIENT, mygreen);
glLightfv(GL_LIGHT2, GL_DIFFUSE, mygreen);
glLightfv(GL_LIGHT2, GL_SPECULAR, mygreen);
glLightf (GL_LIGHT2, GL_SPOT_CUTOFF, 35);
glLightf (GL_LIGHT2, GL_SPOT_EXPONENT, 2.0);
glLightf (GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);
glLightf (GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.04);
glLightfv(GL_LIGHT3, GL_AMBIENT, myblue);
glLightfv(GL_LIGHT3, GL_DIFFUSE, myblue);
glLightfv(GL_LIGHT3, GL_SPECULAR, myblue);
glLightf (GL_LIGHT3, GL_SPOT_CUTOFF, 35);
glLightf (GL_LIGHT3, GL_SPOT_EXPONENT, 2.0);
glLightf (GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0);
glLightf (GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.04);
glEnable(GL_LIGHT1);
glEnable(GL_LIGHT2);
glEnable(GL_LIGHT3);
///TASK 2A end =================================================================

///TASK 3 start =================================================================
for (int i=0; i<3; ++i)
lighton[i] = true;
///TASK 3 end =================================================================
}
void MySpotLights::toggleLight(int lightno)
{
//to be filled later.....
///TASK 3 start =================================================================
static GLenum tag[] = {GL_LIGHT1, GL_LIGHT2, GL_LIGHT3};
lighton[lightno] = !lighton[lightno];
if (lighton[lightno])
glEnable( tag[lightno] );
else
glDisable( tag[lightno] );
///TASK 3 end =================================================================
}

void MySpotLights::draw()
{

GLboolean cullingIsOn;
glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
glDisable(GL_CULL_FACE);
///TASK 2A start =================================================================
static GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat direction[] = {0.0f, 0.0f, 1.0f, 1.0f };
///TASK 2A end =================================================================

GLfloat radius=9.0f;
glPushMatrix();
glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
glRotatef(rotateangle, 0.0f, 0.0f, 1.0f);
glColor3f(1.0f, 0.5f, 0.3f);
gluDisk(pObj,radius-1.0f, radius+1.0f, 20, 5);
glPushMatrix();
glTranslatef(0.0f, 0.0f, -0.1f);
glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
gluDisk(pObj,radius-1.0f, radius+1.0f, 20, 5);
glPopMatrix();
glTranslatef(radius, 0.0f, 0.0f);
///EXTRA EXERCISE start =================================================================
glDisable(GL_COLOR_MATERIAL);
if (lighton[0])
glMaterialfv(GL_FRONT,GL_EMISSION,myred);
else
glMaterialfv(GL_FRONT,GL_EMISSION,myblack);
///EXTRA EXERCISE end =================================================================

glColor3f(1.0f, 0.0f, 0.0f);

///EXTRA EXERCISE start =================================================================
glMaterialfv(GL_FRONT,GL_DIFFUSE,myred);
///EXTRA EXERCISE end =================================================================

gluCylinder(pObj,1.0f, 2.0f, 3.0f, 30, 5);
///TASK 2A start =================================================================
glLightfv(GL_LIGHT1, GL_POSITION, position);
glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
///TASK 2A end =================================================================
GLfloat dx=radius*0.5; // = radius*cos(M_PI/3);
GLfloat dy=radius*0.8660;// = radius*sin(M_PI/3);
glTranslatef(-radius-dx, dy, 0.0f);

///EXTRA EXERCISE start =================================================================
if (lighton[1])
glMaterialfv(GL_FRONT,GL_EMISSION,mygreen);
else
glMaterialfv(GL_FRONT,GL_EMISSION,myblack);
///EXTRA EXERCISE end =================================================================

glColor3f(0.0f, 1.0f, 0.0f);
///EXTRA EXERCISE start =================================================================
glMaterialfv(GL_FRONT,GL_DIFFUSE,mygreen);
///EXTRA EXERCISE end =================================================================

gluCylinder(pObj,1.0f, 2.0f, 3.0f, 30, 5);
///TASK 2A start =================================================================
glLightfv(GL_LIGHT2, GL_POSITION, position);
glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction);
///TASK 2A end =================================================================

glTranslatef(0.0f, -2*dy, 0.0f);

///EXTRA EXERCISE start =================================================================
if (lighton[2])
glMaterialfv(GL_FRONT,GL_EMISSION,myblue);
else
glMaterialfv(GL_FRONT,GL_EMISSION,myblack);
///EXTRA EXERCISE end =================================================================

glColor3f(0.0f, 0.0f, 1.0f);

///EXTRA EXERCISE start =================================================================
glMaterialfv(GL_FRONT,GL_DIFFUSE,myblue);
///EXTRA EXERCISE end =================================================================

gluCylinder(pObj,1.0f, 2.0f, 3.0f, 30, 5);
///TASK 2A start =================================================================
glLightfv(GL_LIGHT3, GL_POSITION, position);
glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction);
///TASK 2A end =================================================================


///EXTRA EXERCISE start =================================================================
glEnable(GL_COLOR_MATERIAL);
glColorMaterial(GL_FRONT,GL_EMISSION);
glColor3f(0.0f, 0.0f, 0.0f);
glColorMaterial(GL_FRONT,GL_DIFFUSE);
///EXTRA EXERCISE end =================================================================

glPopMatrix();
if (cullingIsOn==GL_TRUE) glEnable(GL_CULL_FACE);
}
void MySpotLights::tickTime(long int elapseTime) //elapsetime in milisec
{
rotateangle += elapseTime * rotatespeed / 1000.0;
if (rotateangle>=360)
{
rotateangle = 360 - rotateangle;
}
};
MySwingLights::MySwingLights()
{
pObj = gluNewQuadric();
gluQuadricNormals(pObj, GLU_SMOOTH);
length = 25; //length of the pendulum
timestart = glutGet(GLUT_ELAPSED_TIME);
///TASK 3 start =================================================================
for (int i=0; i<2; ++i)
lighton[i] = true;
///TASK 3 end =================================================================
}
MySwingLights::~MySwingLights()
{
}
void MySwingLights::setupLights()
{
//to be filled up later...
///TASK 2B end =================================================================
glLightfv(GL_LIGHT4, GL_AMBIENT, mywhite);
glLightfv(GL_LIGHT4, GL_DIFFUSE, mywhite);
glLightfv(GL_LIGHT4, GL_SPECULAR, mywhite);
glLightf (GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.0);
glLightf (GL_LIGHT4, GL_LINEAR_ATTENUATION, 0.2);
glLightfv(GL_LIGHT5, GL_AMBIENT, mywhite);
glLightfv(GL_LIGHT5, GL_DIFFUSE, mywhite);
glLightfv(GL_LIGHT5, GL_SPECULAR, mywhite);
glLightf (GL_LIGHT5, GL_CONSTANT_ATTENUATION, 1.0);
glLightf (GL_LIGHT5, GL_LINEAR_ATTENUATION, 0.2);
glEnable(GL_LIGHT4);
glEnable(GL_LIGHT5);
///TASK 2B end =================================================================

///TASK 3 start =================================================================
for (int i=0; i<2; ++i)
lighton[i] = true;
///TASK 3 end =================================================================
}
void MySwingLights::toggleLight(int lightno)
{
//to be filled up later...
///TASK 3 start =================================================================
static GLenum tag[] = {GL_LIGHT4, GL_LIGHT5};
lighton[lightno] = !lighton[lightno];
if (lighton[lightno])
glEnable( tag[lightno] );
else
glDisable( tag[lightno] );
///TASK 3 end =================================================================
}
void MySwingLights::draw()
{
///TASK 2B end =================================================================
    static GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f };
///TASK 2B end =================================================================
float degreePerRad = 180/M_PI;
glPushMatrix();
glTranslatef(0.0f, 0.0f, -20.0f);
glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
glRotatef(asin(horizDisp1/length)*degreePerRad, 0.0f, 1.0f, 0.0f);
glColor3f(1.0f, 1.0f, 1.0f);
gluCylinder(pObj, 0.1f, 0.1f, length, 10, 10);
glTranslatef(0.0f, 0.0f, length);
gluSphere(pObj, 0.5, 10, 10);
///TASK 2B end =================================================================
glLightfv(GL_LIGHT4, GL_POSITION, position);
///TASK 2B end =================================================================
glPopMatrix();
glPushMatrix();
glTranslatef(0.0f, 0.0f, 20.0f);
glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
glRotatef(asin(horizDisp2/length)*degreePerRad, 0.0f, 1.0f, 0.0f);
glColor3f(1.0f, 1.0f, 1.0f);
gluCylinder(pObj, 0.1f, 0.1f, length, 10, 10);
glTranslatef(0.0f, 0.0f, length);
gluSphere(pObj, 0.5, 10, 10);
///TASK 2B end =================================================================
glLightfv(GL_LIGHT5, GL_POSITION, position);
///TASK 2B end =================================================================
glPopMatrix();
}
void MySwingLights::tickTime(long int elapseTime) //elapsetime in milisec
{
//we ignore the elapseTime passed to this function,
// because we are calculating the position based on
// the time since the pendulum starts.
// This is another way to do timing for animation
float currentTime = timestart - glutGet(GLUT_ELAPSED_TIME);
float currentTimeInSec = currentTime / 1000.0;
float D = 25; //D is max horizontal displacement
float g= 50.0; //gravity, bigger value to swing faster
float freq = sqrt( g/length ); //frequency of pendulum
float k = freq * currentTimeInSec;
//This equation is meant for pendulum of small angle
//we use this as approximation though our angle is big
horizDisp1 = D*sin( k - M_PI );
horizDisp2 = D*sin( k );
}













