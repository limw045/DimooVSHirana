
/*
TCG6223 Computer Graphics
CGLab09.cpp
Objective: Lab09 on Animation
Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
INSTRUCTIONS
============
Please refer to CGLabmain.cpp for instructions
*/
#include <GL/glut.h>
#include "CGLab09.hpp"
using namespace CGLab09;
MyMovingSmiley::MyMovingSmiley()
{
//Setup Quadric Object
pObj = gluNewQuadric();
gluQuadricNormals(pObj, GLU_SMOOTH);
posx = -5.0f; posy = 0.0f; posz = 2.0f;
roty = 30.0f;
//initial velocity (in unit per second)
velx=40.0f;
vely= 0.0f;
velz=30.0f;
}
MyMovingSmiley::~MyMovingSmiley()
{
gluDeleteQuadric(pObj);
}
void MyMovingSmiley::draw()
{
glPushMatrix();
glTranslatef(posx, posy, posz);
glRotatef(roty, 0.0f, 1.0f, 0.0f);
glPushMatrix();
//head
glTranslatef(0.0f, 4.0f, 0.0f);
glColor3f(1.0f, 1.0f, 0.0f);
gluSphere(pObj,4.0f, 20, 20);
//nose
glTranslatef(0.0f, 0.0f, 4.0f);
glColor3f(1.0f, 0.5f, 0.0f);
gluCylinder(pObj,1.0f, 0.0f, 2.0f, 5, 5);
//eyes
glPushMatrix();
glTranslatef(1.5f, 1.5f, -1.2f);
glColor3f(1.0f, 1.0f, 1.0f);
gluSphere(pObj,1.0f, 10, 10);
glTranslatef(0.0f, 0.2f, 0.5f);
glColor3f(0.0f, 0.5f, 1.0f);
gluSphere(pObj,0.7f, 10, 10);
glPopMatrix();
glPushMatrix();
glTranslatef(-1.5f, 1.5f, -1.2f);
glColor3f(1.0f, 1.0f, 1.0f);
gluSphere(pObj,1.0f, 10, 10);
glTranslatef(0.0f, 0.2f, 0.5f);
glColor3f(0.0f, 0.5f, 1.0f);
gluSphere(pObj,0.7f, 10, 10);
glPopMatrix();
//mouth
GLboolean cullingIsOn;
glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
glDisable(GL_CULL_FACE);
glTranslatef(0.0f, -1.8f, -1.2f);
glRotatef(-45, 1.0f, 0.0f, 0.0f);
glColor3f(1.0f, 0.0f, 0.0f);
gluDisk(pObj,0.0f, 1.5f, 10, 10);
if (cullingIsOn==GL_TRUE) glEnable(GL_CULL_FACE);
glPopMatrix();
glPopMatrix();
}
void MyMovingSmiley::tickTime(long int elapseTime) //elapsetime in milisec
{
float elapseTimeInSec = elapseTime / 1000.0f;
posx += elapseTimeInSec * velx;
posy += elapseTimeInSec * vely;
posz += elapseTimeInSec * velz;
if (posx > 15)
{
posx = 30 - posx; //posx = 15 - (posx - 15)
velx = -velx;
}
else if (posx<-15)
{
posx = -30 - posx; //posx = -15 - (posx -(-15))
velx = -velx;
}
if (posz > 15)
{
posz = 30 - posz;
velz = -velz;
}
else if (posz<-15)
{
posz = -30 - posz;
velz = -velz;
}
};
MyFan::MyFan()
{
//Setup Quadric Object
pObj = gluNewQuadric();
gluQuadricNormals(pObj, GLU_SMOOTH);
pitchangle=-20.0f;
swingangle= 45.0f;
wingsangle= 30.0f;
swingspeed = 90.0; //degree per sec
wingsspeed = 720.0; //degree per sec
}
MyFan::~MyFan()
{
gluDeleteQuadric(pObj);
}
void MyFan::draw()
{
glPushMatrix();
drawBase();
glTranslatef(0.0f, 10.0f, 0.0f);
glRotatef(-45, 0.0f, 1.0f, 0.0f);
glRotatef(swingangle, 0.0f, 1.0f, 0.0f);
glRotatef(pitchangle, 1.0f, 0.0f, 0.0f);
glTranslatef(0.0f, 0.0f, 2.0f);
drawMiddle();
glTranslatef(0.0f, 0.0f, 6.0f);
glRotatef(wingsangle, 0.0f, 0.0f, 1.0f);
drawWings();
glPopMatrix();
}
void MyFan::drawBase()
{
glPushMatrix();
glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
glColor3f(1.0f, 0.0f, 0.0f);
gluCylinder(pObj, 5.0f, 1.0f, 2.0f, 10, 10);
glColor3f(1.0f, 0.0f, 1.0f);
gluCylinder(pObj, 1.0f, 1.0f, 10.0f, 10, 10);
glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
gluDisk(pObj, 0.0f, 5.0f, 10, 10);
glPopMatrix();
}
void MyFan::drawMiddle()
{
glPushMatrix();
glScalef(0.3f, 0.3f, 1.0f);
glColor3f(1.0f, 1.0f, 0.0f);
gluSphere(pObj, 5.0f, 10, 10);
glTranslatef(0.0f, 0.0f, 5.0f);
gluCylinder(pObj, 0.5f, 0.5f, 1.5f, 10, 10);
glPopMatrix();
}
void MyFan::drawWings()
{
GLboolean cullingIsOn;
glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
glDisable(GL_CULL_FACE);
GLboolean normalizeIsOn;
glGetBooleanv(GL_NORMALIZE, &normalizeIsOn);
glEnable(GL_NORMALIZE);
glColor3f(0.5f, 1.0f, 0.5f);
glPushMatrix();
glBegin(GL_TRIANGLES);
glNormal3f( 1.0f, 0.0f, 2.0f);
glVertex3f( 0.0f, 0.0f, 0.0f);
glVertex3f( 0.0f, 4.0f, 0.0f);
glVertex3f(-2.0f, 4.0f, 1.0f);
glNormal3f( 0.0f, 1.0f, 2.0f);
glVertex3f( 0.0f, 0.0f, 0.0f);
glVertex3f(-4.0f, 0.0f, 0.0f);
glVertex3f(-4.0f,-2.0f, 1.0f);
glNormal3f(-1.0f, 0.0f, 2.0f);
glVertex3f( 0.0f, 0.0f, 0.0f);
glVertex3f( 0.0f,-4.0f, 0.0f);
glVertex3f( 2.0f,-4.0f, 1.0f);
glNormal3f( 0.0f,-1.0f, 2.0f);
glVertex3f( 0.0f, 0.0f, 0.0f);
glVertex3f( 4.0f, 0.0f, 0.0f);
glVertex3f( 4.0f, 2.0f, 1.0f);
glEnd();
glPopMatrix();
if (cullingIsOn==GL_TRUE) glEnable(GL_CULL_FACE);
if (normalizeIsOn==GL_TRUE) glEnable(GL_NORMALIZE);
}
void MyFan::tickTime(long int elapseTime) //elapsetime in milisec
{
//to be filled up by you
};
