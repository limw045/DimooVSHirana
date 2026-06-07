
/*
TCG6223 Computer Graphics
CGLab09.hpp
Objective: Header File for Lab10 on Animation
Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
INSTRUCTIONS
============
Please refer to CGLabmain.cpp for instructions
*/
#ifndef YP_CGLAB09_HPP
#define YP_CGLAB09_HPP
#include <GL/glut.h>
#include "CGLabmain.hpp"
namespace CGLab09 {
class MyMovingSmiley
{
public:
MyMovingSmiley();
~MyMovingSmiley();
void draw();
void tickTime(long int elapseTime);
private:
GLUquadricObj *pObj;
GLfloat posx, posy, posz;
GLfloat roty;
GLfloat velx, vely, velz; //unit per sec
};
class MyFan
{
public:
MyFan();
~MyFan();
void tickTime(long int elapseTime);
void draw();
private:
void drawBase();
void drawMiddle();
void drawWings();
GLUquadricObj *pObj;
GLfloat pitchangle;
GLfloat swingangle, wingsangle; //in degree
GLfloat swingspeed; //in degree per sec
GLfloat wingsspeed; //in degree per sec
};
//------------------------------------
//the main program will call methods from this class
class MyVirtualWorld
{
public:
MyMovingSmiley mymovingsmiley;
MyFan myfan;
long int timeold, timenew, elapseTime;
void draw()
{
drawFloor();
mymovingsmiley.draw();
myfan.draw();
}
void drawFloor()
{
glDisable(GL_CULL_FACE);
glColor3f(0.0f, 0.0f, 1.0f);
glBegin(GL_QUADS);
glVertex3f( 19.0f, 0.0f, 19.0f);
glVertex3f( 19.0f, 0.0f,-19.0f);
glVertex3f(-19.0f, 0.0f,-19.0f);
glVertex3f(-19.0f, 0.0f, 19.0f);
glEnd();
glEnable(GL_CULL_FACE);
}
//NOTE: different compare with those in earlier lab
void tickTime()
{
timenew = glutGet(GLUT_ELAPSED_TIME);
elapseTime = timenew - timeold;
timeold = timenew;
mymovingsmiley.tickTime(elapseTime);
myfan.tickTime(elapseTime);
}
//for any one-time only initialization of the
// virtual world before any rendering takes place
// BUT after OpenGL has been initialized
void init()
{
timeold = glutGet(GLUT_ELAPSED_TIME);
}
};
}; //end of namespace CGLab10
#endif //YP_CGLAB10_HPP
