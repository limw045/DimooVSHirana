/*
 TCG6223 Computer Graphics
 CGLab09.hpp
 Objective: Header File for Lab09 on Animation (Three Bounding Boxes)
*/
#ifndef YP_CGLAB09_HPP
#define YP_CGLAB09_HPP

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
    GLfloat velx, vely, velz;
    GLfloat rotyspeed;
    GLfloat gravity;
};

class MyFan
{
 public:
    MyFan();
    ~MyFan();
    void tickTime(long int elapseTime);
    void draw();
    void toggleRotation();
    void toggleSwing();
 private:
    void drawBase();
    void drawMiddle();
    void drawWings();
    GLUquadricObj *pObj;
    GLfloat pitchangle;
    GLfloat swingangle, wingsangle; // in degree
    GLfloat swingspeed; // in degree per sec
    GLfloat wingsspeed; // in degree per sec
    bool wingsOn;
    bool swingOn;
};

class MyDancingSnowman
{
 public:
    MyDancingSnowman();
    ~MyDancingSnowman();
    void draw();
    void tickTime(long int elapseTime);
 private:
    GLUquadricObj *pObj;
    GLfloat bounceY;
    GLfloat rotY;
    GLfloat tiltZ;
    GLfloat bounceSpeed;
    GLfloat rotSpeed;
    GLfloat tiltSpeed;
    float timeAccumulator;
};

class MyBoundingBox
{
 public:
    MyBoundingBox(GLfloat xcenter, GLfloat ycenter, GLfloat zcenter,
                  GLfloat xdim, GLfloat ydim, GLfloat zdim)
        : cx(xcenter), cy(ycenter), cz(zcenter), dx(xdim), dy(ydim), dz(zdim)
    {
        GLfloat xdimhalf = xdim/2.0f;
        GLfloat ydimhalf = ydim/2.0f;
        GLfloat zdimhalf = zdim/2.0f;

        y[0] = ycenter + ydimhalf;
        y[1] = y[2] = y[3] = y[0]; //top
        y[4] = ycenter - ydimhalf;
        y[7] = y[6] = y[5] = y[4]; //bottom

        x[1] = xcenter + xdimhalf;
        x[6] = x[5] = x[2] = x[1]; //right
        x[0] = xcenter - xdimhalf;
        x[7] = x[4] = x[3] = x[0]; //left

        z[0] = zcenter + zdimhalf;
        z[5] = z[4] = z[1] = z[0]; //front
        z[2] = zcenter - zdimhalf;
        z[7] = z[6] = z[3] = z[2]; //back
    }
    void draw();
    void drawGrid();
 private:
    GLfloat cx, cy, cz; // center coordinates
    GLfloat dx, dy, dz; // dimensions
    GLfloat x[8], y[8], z[8]; //(x,y,z) of 8 corner points
};

class MyVirtualWorld
{
 public:
    MyMovingSmiley mymovingsmiley;
    MyFan myfan;
    MyDancingSnowman mydancingsnowman;
    MyBoundingBox   boxLeft;
    MyBoundingBox   boxCenter;
    MyBoundingBox   boxRight;

    long int timeold, timenew, elapseTime;

    MyVirtualWorld() 
        : boxLeft(-40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxCenter(0.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxRight(40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          timeold(0), timenew(0), elapseTime(0)
    {
    }

    void draw()
    {
        // Draw Smiley (left box) - translated to left box center
        glPushMatrix();
            glTranslatef(-40.0f, 0.0f, -10.0f);
            mymovingsmiley.draw();
        glPopMatrix();

        // Draw Fan (center box) - translated to center box center
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -10.0f);
            myfan.draw();
        glPopMatrix();

        // Draw Snowman (right box) - translated to right box center
        glPushMatrix();
            glTranslatef(40.0f, 0.0f, -10.0f);
            mydancingsnowman.draw();
        glPopMatrix();

        // Draw bounding boxes and their grids
        boxLeft.draw();
        boxLeft.drawGrid();

        boxCenter.draw();
        boxCenter.drawGrid();

        boxRight.draw();
        boxRight.drawGrid();
    }

    void tickTime()
    {
        timenew = glutGet(GLUT_ELAPSED_TIME);
        elapseTime = timenew - timeold;
        timeold = timenew;

        mymovingsmiley.tickTime(elapseTime);
        myfan.tickTime(elapseTime);
        mydancingsnowman.tickTime(elapseTime);
    }

    void init()
    {
        timeold = glutGet(GLUT_ELAPSED_TIME);
    }

    void keyboard(unsigned char key, int x, int y);
};

}; //end of namespace CGLab09

#endif //YP_CGLAB09_HPP
