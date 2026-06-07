/*
 TCS2221 Computer Graphics
 CGLab05.hpp
 Objective: Header File for Lab05 on Modeling Transformations and Quadrics
*/
#ifndef YP_CGLAB05_HPP
#define YP_CGLAB05_HPP

#include "CGLabmain.hpp"

namespace CGLab05 {

class MyArrows
{
 public:
    void drawArrow();
    void draw1();
    void draw2();
    void draw3();
    void draw4();
    void draw5();
    void draw6();
    void draw7(); // Spiral/Helix of arrows
};

class MyQuadricDemo
{
 public:
    void draw();
};

class MySnowman
{
 public:
    void draw();
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
    MyArrows      myarrows;
    MyQuadricDemo myquadricdemo;
    MySnowman     mysnowman;
    MyBoundingBox boxLeft;
    MyBoundingBox boxCenter;
    MyBoundingBox boxRight;

    MyVirtualWorld() 
        : boxLeft(-40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxCenter(0.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxRight(40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f)
    {
    }

    void draw()
    {
        // Draw spiral of arrows in left box
        glPushMatrix();
            glTranslatef(-40.0f, 0.0f, -10.0f);
            myarrows.draw7();
        glPopMatrix();

        // Draw quadrics demo in center box (scaled to fit)
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -10.0f);
            glScalef(0.3f, 0.3f, 0.3f);
            // The demo spans x from -40 to 42, which is centered at 1.0. 
            // So we translate by -1.0 to center it in X.
            glTranslatef(-1.0f, 0.0f, 0.0f);
            myquadricdemo.draw();
        glPopMatrix();

        // Draw snowman in right box (translated and centered in Y)
        glPushMatrix();
            glTranslatef(40.0f, 2.0f, -10.0f); // Center y slightly adjusted
            mysnowman.draw();
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
        // do nothing, reserved for doing animation
    }

    void init()
    {
        // for any one-time only initialization of the
        // virtual world before any rendering takes place
        // BUT after OpenGL has been initialized
    }
};

}; //end of namespace CGLab05

#endif //YP_CGLAB05_HPP
