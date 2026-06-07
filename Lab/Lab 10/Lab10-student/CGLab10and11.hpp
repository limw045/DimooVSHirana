
/*
TCG6223 Computer Graphics
CGLab10and11.hpp
Objective: Header File for:
Lab10 on Lighting &
Lab11 on Textures
Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
INSTRUCTIONS
============
Please refer to CGLab10and11main.cpp for instructions
CHANGE LOG
==========
*/
#ifndef YP_CGLAB10and11_HPP
#define YP_CGLAB10and11_HPP
#include <cmath>
#include "CGLab09.hpp"
#include "CGLabmain.hpp"
#include "CGLab10and11.hpp"
namespace CGLab10and11 {
//Make use of the MyMovingSmiley and MyFan from Lab09
using CGLab09::MyMovingSmiley;
using CGLab09::MyFan;
class MySpotLights
{
public:
MySpotLights();
~MySpotLights();
void setupLights();
void toggleLight(int lightno);
void draw();
void tickTime(long int elapseTime); //elapsetime in milisec
private:
GLUquadricObj *pObj;
GLfloat rotateangle, rotatespeed;
bool lighton[3]; //keep track if lights are on or off
};
class MySwingLights
{
public:
MySwingLights();
~MySwingLights();
void setupLights();
void toggleLight(int lightno);
void draw();
void tickTime(long int elapseTime); //elapsetime in milisec
private:
GLUquadricObj *pObj;
GLfloat length; //length of the pendulum
GLfloat horizDisp1, horizDisp2; //horizontal displacement
long int timestart;
bool lighton[2]; //keep track if lights are on or off
};
class MyVirtualWorld
{
public:
MyVirtualWorld();
~MyVirtualWorld();
void init();
void setupLights();
void toggleLight(int lightno);
void draw();
void drawSpotLights();
void tickTime()
{
timenew = glutGet(GLUT_ELAPSED_TIME);
elapseTime = timenew - timeold;
timeold = timenew;
mymovingsmiley.tickTime(elapseTime);
myfan.tickTime(elapseTime); myspotlights.tickTime(elapseTime);
myswinglights.tickTime(elapseTime);
}
private:
long int timeold, timenew, elapseTime;
MyMovingSmiley mymovingsmiley;
MyFan myfan;
MySpotLights myspotlights;
MySwingLights myswinglights;
GLfloat xmin, zmin, xmax, zmax, height,
xdim, ydim, zdim;
void drawRoof();
void drawFloor();
void drawWalls();
void drawFourTeapot();
bool lighton[7]; //keep track if lights are on or off
};
}; //end of namespace CGLab10and11
#endif //YP_CGLAB10and11_HPP






