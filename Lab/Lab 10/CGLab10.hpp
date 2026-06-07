/*
 TCG6223 Computer Graphics
 CGLab10.hpp
 Objective: Header File for Lab10 on Lighting
 Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
*/
#ifndef YP_CGLAB10_HPP
#define YP_CGLAB10_HPP

#include <cmath>
#include "CGLab09.hpp"
#include "CGLabmain.hpp"

namespace CGLab10 {

// Make use of MyMovingSmiley and MyFan from Lab09
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
    void tickTime(long int elapseTime); // elapsetime in milisec
 private:
    GLUquadricObj *pObj;
    GLfloat rotateangle, rotatespeed;
    bool lighton[3]; // keep track of red, green, blue spotlights (GL_LIGHT1, GL_LIGHT2, GL_LIGHT3)
};

class MySwingLights
{
 public:
    MySwingLights();
    ~MySwingLights();
    void setupLights();
    void toggleLight(int lightno);
    void draw();
    void tickTime(long int elapseTime); // elapsetime in milisec
 private:
    GLUquadricObj *pObj;
    GLfloat length; // length of the pendulum
    GLfloat horizDisp1, horizDisp2; // horizontal displacements
    long int timestart;
    bool lighton[2]; // keep track of two swinging point lights (GL_LIGHT4, GL_LIGHT5)
};

class MyFlyingLights
{
 public:
    MyFlyingLights();
    ~MyFlyingLights();
    void setupLights();
    void toggleLight();
    void draw();
    void tickTime(long int elapseTime); // elapsetime in milisec
 private:
    GLUquadricObj *pObj;
    GLfloat posx, posy, posz; // flying coordinates
    float timeAccumulator;
    bool lighton; // keep track of the flying light (GL_LIGHT6)
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
        myfan.tickTime(elapseTime);
        myspotlights.tickTime(elapseTime);
        myswinglights.tickTime(elapseTime);
        myflyinglights.tickTime(elapseTime);
    }
 private:
    long int timeold, timenew, elapseTime;
    MyMovingSmiley mymovingsmiley;
    MyFan myfan;
    MySpotLights myspotlights;
    MySwingLights myswinglights;
    MyFlyingLights myflyinglights;
    
    GLfloat xmin, zmin, xmax, zmax, height, xdim, ydim, zdim;
    
    void drawRoof();
    void drawFloor();
    void drawWalls();
    void drawFourTeapot();
    bool lighton[7]; // [0] directional, [1-3] spotlights, [4-5] swinglights, [6] firefly
};

}; //end of namespace CGLab10

#endif //YP_CGLAB10_HPP
