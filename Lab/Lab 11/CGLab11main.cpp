
/*
TCG6223 Computer Graphics
CGLab10and11main.cpp
Objective: Main Program for:
Lab10 on Lighting &
Lab11 on Textures
Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
INSTRUCTIONS
============
Please refer to CGLabmain.cpp for instructions

CHANGE LOG
==========
TO DO
=====
*/
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <GL/glut.h>
#include "CGLabmain.hpp"
#include "CGLab11.hpp"

CGLab11::MyVirtualWorld myvirtualworld;
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
MyWorld local;
MyAxis spotlightsaxis;
enum MyMovementType
{
movenone, moveworld, movelocal
};
MyMovementType movementType;
//END LAB10 Specific
using namespace std;
MyWindow window;
MyWorld world;
MyViewer viewer;
MySetting setting;
MyAxis worldaxis;
void myDisplayFunc(void)
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glPushMatrix();
glTranslatef(world.posX, world.posY, world.posZ);
glRotatef(world.rotateX, 1.0f, 0.0f, 0.0f);
glRotatef(world.rotateY, 0.0f, 1.0f, 0.0f);
glRotatef(world.rotateZ, 0.0f, 0.0f, 1.0f);
glScalef(world.scaleX, world.scaleY, world.scaleZ);
worldaxis.draw();
myvirtualworld.draw();
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
glPushMatrix();
glTranslatef(0.0f, 20.0f, 0.0f);
glTranslatef(local.posX, local.posY, local.posZ);
glRotatef(local.rotateX, 1.0f, 0.0f, 0.0f);
glRotatef(local.rotateY, 0.0f, 1.0f, 0.0f);
glRotatef(local.rotateZ, 0.0f, 0.0f, 1.0f);
glScalef(local.scaleX, local.scaleY, local.scaleZ);
spotlightsaxis.draw();
myvirtualworld.drawSpotLights();
glPopMatrix();
//END LAB10 Specific
glPopMatrix();
glFlush(); // send any buffered output to be rendered
glutSwapBuffers();
//Same as in Lab09
myvirtualworld.tickTime(); //tick the clock
glutPostRedisplay(); //force openGL to call m yDisplayFunc() again
}
void myReshapeFunc(int width, int height)
{
window.width = width;
window.height = height;
glViewport(0, 0, width, height);
}
void myKeyboardFunc(unsigned char key, int x, int y)
{
GLfloat xinc,yinc,zinc;
xinc = yinc = zinc = 0.0;
switch (key)
{
case 'a': case 'A': xinc = -setting.posInc; break;
case 'd': case 'D': xinc = setting.posInc; break;
case 'q': case 'Q': yinc = -setting.posInc; break;
case 'e': case 'E': yinc = setting.posInc; break;
case 'w': case 'W': zinc = -setting.posInc; break;
case 's': case 'S': zinc = setting.posInc; break;
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
case '1': movementType = moveworld;
break;
case '2': movementType = movelocal;
break;
//END LAB10 Specific
case 27 : exit(1); break;
}
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
//world.move(xinc, yinc, zinc);
switch (movementType)
{
case moveworld: world.move(xinc, yinc, zinc);
break;
case movelocal: local.move(xinc, yinc, zinc);
break;
}
//END LAB10 Specific
glutPostRedisplay();
}
void mySpecialFunc(int key, int x, int y)
{
switch (key)
{
case GLUT_KEY_DOWN : world.rotateX -= setting.angleInc; break;
case GLUT_KEY_UP : world.rotateX += setting.angleInc; break;
case GLUT_KEY_LEFT : world.rotateY -= setting.angleInc; break;
case GLUT_KEY_RIGHT : world.rotateY += setting.angleInc; break;
case GLUT_KEY_HOME : myDataInit(); break;
case GLUT_KEY_F1 : setting.shadingMode = !setting.shadingMode;
if (setting.shadingMode)
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
else
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
break;
case GLUT_KEY_F2 : worldaxis.toggle();
//BEGIN LAB10 Specific � toggle the axis of spotlight
spotlightsaxis.toggle();
//END LAB10 Specific
break;
//BEGIN LAB10 Specific
///TASK 3 start =================================================================
case GLUT_KEY_F5 : myvirtualworld.toggleLight(0);
break;
case GLUT_KEY_F6 : myvirtualworld.toggleLight(1);
break;
case GLUT_KEY_F7 : myvirtualworld.toggleLight(2);
break;
case GLUT_KEY_F8 : myvirtualworld.toggleLight(3);
break;
case GLUT_KEY_F9 : myvirtualworld.toggleLight(4);
break;
case GLUT_KEY_F10 : myvirtualworld.toggleLight(5);
break;
case GLUT_KEY_F11 : myvirtualworld.toggleLight(6);
break;
///TASK 3 end =================================================================
//END LAB10 Specific
case GLUT_KEY_F3 : GLboolean lightingIsOn;
glGetBooleanv(GL_LIGHTING, &lightingIsOn);
if (lightingIsOn==GL_TRUE)
glDisable(GL_LIGHTING);
else glEnable(GL_LIGHTING);
break;
/// LAB 12 TASK 2 start =================================================================
case GLUT_KEY_F4 : myvirtualworld.toggleTextures();
break;
/// LAB 12 TASK 2 end =================================================================
}
glutPostRedisplay();
}
void myMouseFunc(int button, int state, int x, int y)
{
y = window.height - y;
switch (button)
{
case GLUT_RIGHT_BUTTON:
if (state==GLUT_DOWN && !setting.mouseRightMode)
{
setting.mouseX = x;
setting.mouseY = y;
setting.mouseRightMode = true;
}
if (state==GLUT_UP && setting.mouseRightMode)
    {
setting.mouseRightMode = false;
}
break;
case GLUT_LEFT_BUTTON:
if (state==GLUT_DOWN && !setting.mouseLeftMode)
{
setting.mouseX = x;
setting.mouseY = y;
setting.mouseLeftMode = true;
}
if (state==GLUT_UP && setting.mouseLeftMode)
{
setting.mouseLeftMode = false;
}
break;
}
}
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
void updateWorldRotation(int xinc, int yinc)
{
if (setting.mouseRightMode)
{
world.rotate(0.0f, 0.0f, -xinc*0.5);
}
if (setting.mouseLeftMode)
{
world.rotate(-yinc*0.5, xinc*0.5, 0.0f);
}
}
void updateLocalRotation(int xinc, int yinc)
{
if (setting.mouseRightMode)
{
local.rotate(0.0f, 0.0f, -xinc*0.5);
}
if (setting.mouseLeftMode)
{
local.rotate(-yinc*0.5, xinc*0.5, 0.0f);
}
}
//END LAB10 Specific
void myMotionFunc(int x, int y)
{
y = window.height - y;
GLint xinc = x - setting.mouseX;
GLint yinc = y - setting.mouseY;
//BEGIN LAB10 Specific � define local axis for the spotlight and
// allow us to move the spot lights around
/*if(setting.mouseRightMode)
{
world.rotate(0.0f, 0.0f, -xinc*0.5);
}
if(setting.mouseLeftMode)
{
world.rotate(-yinc*0.5, xinc*0.5, 0.0f);
}*/
switch (movementType)
{
case moveworld: updateWorldRotation(xinc,yinc);
break;
case movelocal: updateLocalRotation(xinc,yinc);
break;
}
//END LAB10 Specific
setting.mouseX = x;
setting.mouseY = y;
glutPostRedisplay();
}
void myDataInit()
{
window.title = "TCS2111 Computer Graphics";
window.posX = 100;
window.posY = 100;
window.width = 800;
window.height = 500;
world.rotateX = 0.0;
world.rotateY = 0.0;
world.rotateZ = 0.0;
world.posX = 0.0;
world.posY = 0.0;
world.posZ = 0.0;
world.scaleX = 1.0;
world.scaleY = 1.0;
world.scaleZ = 1.0;
viewer.upX = 0.0;
viewer.upY = 1.0;
viewer.upZ = 0.0;
viewer.zNear = 0.1;
viewer.zFar = 500.0;
viewer.fieldOfView = 60.0;
viewer.aspectRatio = static_cast<GLdouble> (window.width) / window.height;
setting.mouseX = 0;
setting.mouseY = 0;
setting.mouseRightMode = false;
setting.mouseLeftMode = false;
setting.shadingMode = true;
//BEGIN LAB10 Specific � change the viewing/camera position/parameters
// and add other parameters
//viewer.eyeX = 0.0;
//viewer.eyeY = 0.0;
//viewer.eyeZ = 40.0;
//viewer.centerX = 0.0;
//viewer.centerY = 0.0;
//viewer.centerZ = 0.0;
//setting.posInc = 1.0;
//setting.angleInc = 2.0;
viewer.eyeX = 0.0;
viewer.eyeY = 20.0;
viewer.eyeZ = 50.0;
viewer.centerX = 0.0;
viewer.centerY = 20.0;
viewer.centerZ = 0.0;
setting.posInc = 10.0;
setting.angleInc = 3.0;
local.rotateX = 0.0;
local.rotateY = 0.0;
local.rotateZ = 0.0;
local.posX = 0.0;
local.posY = 0.0;
local.posZ = 0.0;
local.scaleX = 1.0;
local.scaleY = 1.0;
local.scaleZ = 1.0;
movementType = moveworld;
spotlightsaxis.setLength(10.0f, 10.0f, 10.0f);
spotlightsaxis.setLineWidth(3);
spotlightsaxis.setLineStipple(1, 0xff60);
//END LAB10 Specific
}
void myViewingInit()
{
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(viewer.fieldOfView,
viewer.aspectRatio,
viewer.zNear,
viewer.zFar);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
gluLookAt(viewer.eyeX, viewer.eyeY, viewer.eyeZ,
viewer.centerX,viewer.centerY,viewer.centerZ,
viewer.upX, viewer.upY, viewer.upZ );
}
//BEGIN LAB10 Specific - delete this for Lab12, lighting will be initialized
// in the virtualworld
/*void myLightingInit()
{
static GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat position[] = {10.0f, 10.0f, 10.0f, 1.0f };
short shininess = 128;
glEnable(GL_LIGHTING);
glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
glLightfv(GL_LIGHT0, GL_POSITION, position);
glEnable(GL_LIGHT0);
glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
glEnable(GL_COLOR_MATERIAL);
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
glMateriali(GL_FRONT, GL_SHININESS, shininess);
glEnable(GL_NORMALIZE);
}*/
//END LAB10 Specific
void myInit()
{
myDataInit();
glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
glutInitWindowPosition(window.posX, window.posY); // Set top-left position
glutInitWindowSize(window.width, window.height); //Set width and height
glutCreateWindow(window.title.c_str());// Create display window
glutDisplayFunc(myDisplayFunc); // Specify the display callback function
glutReshapeFunc(myReshapeFunc);
glutKeyboardFunc(myKeyboardFunc);
glutSpecialFunc(mySpecialFunc);
glutMotionFunc(myMotionFunc);
glutMouseFunc(myMouseFunc);
glPointSize(4.0);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glFrontFace(GL_CCW);
glShadeModel (GL_SMOOTH);
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glEnable(GL_CULL_FACE);
myViewingInit();
//BEGIN LAB10 Specific � lighting is now initialized within virtualworld
//myLightingInit();
glutFullScreen(); //set it to full screen, this is optional but nice
//END LAB10 Specific
myvirtualworld.init();
}
void myWelcome()
{
cout << "*****************************************************************\n";
cout << "* TCG6223 Computer Graphics *\n";
cout << "* FIST, Multimedia University *\n";
cout << "* Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my> *\n";
cout << "*****************************************************************\n";
cout << "| Press: |\n";
cout << "| <a>,<d>,<w>,<s>,<q>,<e> => move world |\n";
cout << "| <arrows> => rotate world |\n";
cout << "| HOME => restore defaults |\n";
cout << "| ESC => exit |\n";
cout << "| |\n";
cout << "| F1 => toggle shading / wire-frame mode |\n";
cout << "| F2 => toggle rendering of axes |\n";
cout << "| F3 => toggle lighting on / off |\n";
cout << "| |\n";
cout << "| Mouse (Left Drag or Right Drag) => rotate world |\n";
cout << "| |\n";
cout << "*****************************************************************\n";
cout << "| H A V E F U N !!! |\n";
cout << "*****************************************************************\n";

//BEGIN LAB10 Specific � these keys will be defined later
cout << "+---------------------------------------------------------------+\n";
cout << "| ADDITIONAL KEYS FOR LAB11 : |\n";
cout << "| 1 => 'moving world' mode |\n";
cout << "| 2 => 'moving spotlights' mode |\n";

cout << "| F5 => toggle direction light |\n";
cout << "| F6 => toggle spotlight #1 |\n";
cout << "| F7 => toggle spotlight #2 |\n";
cout << "| F8 => toggle spotlight #3 |\n";
cout << "| F9 => toggle swinging point light #1 |\n";
cout << "| F10 => toggle swinging point light #2 |\n";
cout << "| F11 => toggle flying point light |\n";
cout << "+---------------------------------------------------------------+\n";
//END LAB10 Specific

/// LAB 12 TASK 2 start =================================================================
cout << "| ADDITIONAL KEYS FOR LAB11: |\n";
cout << "| F4 => toggle textures on / off |\n";
cout << "+---------------------------------------------------------------+\n";
/// LAB 12 TASK 2 end =================================================================

system("pause"); //let you see the welcome screen before going fullscreen
}
//--------------------------------------------------------------------
int main(int argc, char **argv)
{
glutInit(&argc, argv);
myWelcome();
myInit();
glutMainLoop(); // Display everything and wait
}
//--------------------------------------------------------------------











