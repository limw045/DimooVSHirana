/*
 TCG6223 Computer Graphics
 CGLab07.hpp
 Objective: Header File for Lab07 on Hierarchical Modeling (Lego Bricks)
*/
#ifndef YP_CGLAB07_HPP
#define YP_CGLAB07_HPP

#include "CGLabmain.hpp"

namespace CGLab07 {

class MyLego
{
 public:
    MyLego();
    ~MyLego();
    void draw();

    // Reusable Lego components
    void draw2x2Lego();
    void draw2x4Lego();
    void draw4x4Lego();
    void draw2x8Lego();
    void draw8x8Lego();

    // Testing and assembly methods
    void draw1();
    void draw2();
    void drawOneTower();
    void drawTowers();

 private:
    void drawTopCylinders();
    void drawBottomCylinder();
    void drawCube();
    GLUquadricObj *pObj;
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
    MyLego        mylego;
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
        // Draw single 2x2 Lego brick with local axes in left box
        glPushMatrix();
            glTranslatef(-40.0f, -0.5f, -10.0f); // Center y slightly adjusted
            mylego.draw2();
        glPopMatrix();

        // Draw single Lego tower in center box
        glPushMatrix();
            glTranslatef(0.0f, -7.5f, -10.0f); // Ground it to the bottom of the box
            mylego.drawOneTower();
        glPopMatrix();

        // Draw complex Lego castle of towers in right box
        glPushMatrix();
            glTranslatef(40.0f, -7.5f, -10.0f); // Ground it to the bottom of the box
            mylego.drawTowers();
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

}; //end of namespace CGLab07

#endif //YP_CGLAB07_HPP
