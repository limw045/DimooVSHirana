/*
 TCG6223 Computer Graphics
 CGLab08.hpp
 Objective: Header File for Lab08 on Curves and Surfaces Modeling (Three Bounding Boxes)
*/
#ifndef YP_CGLAB08_HPP
#define YP_CGLAB08_HPP

#include "CGLabmain.hpp"

namespace CGLab08 {

class MyBezierLine
{
 public:
    MyBezierLine() : controlpoints(NULL), uorder(0) { }
    ~MyBezierLine() { }
    void setup(const GLfloat* controlPoints, GLint uOrder);
    void draw(GLenum draw_mode = GL_LINE, GLint ures = 100);
    void drawControlPoints();
 private:
    GLint uorder;
    const GLfloat* controlpoints;
};

class MyBezierSurface
{
 public:
    MyBezierSurface() : controlpoints(NULL), uorder(0), vorder(0) { }
    ~MyBezierSurface() { }
    void setup(const GLfloat* controlPoints, GLint uOrder, GLint vOrder, const bool autonormal = false);
    void draw(GLenum draw_mode = GL_FILL, GLint ures = 100, GLint vres = 100);
    void drawControlPoints();
 private:
    GLint uorder, vorder;
    const GLfloat* controlpoints;
};

class MySweepSurface
{
 public:
    MySweepSurface() : profilepoints(NULL), numofprofilepoints(0), surfacepoints(NULL), numofsurfacepoints(0), numofverticallines(0) { }
    ~MySweepSurface()
    {
        if (surfacepoints) delete[] surfacepoints;
    }
    void setup(const GLfloat* profilePoints, GLint numOfProfilePoints, GLfloat degreeStart, GLfloat degreeEnd, GLfloat degreeStep);
    void draw();
 private:
    const GLfloat* profilepoints;
    GLint numofprofilepoints;
    GLfloat *surfacepoints;
    GLint numofsurfacepoints;
    GLint numofverticallines;
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
    MyBezierLine    mybezierline;
    MyBezierSurface mybeziersurface;
    MySweepSurface  mysweepsurface;
    MyBoundingBox   boxLeft;
    MyBoundingBox   boxCenter;
    MyBoundingBox   boxRight;

    MyVirtualWorld() 
        : boxLeft(-40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxCenter(0.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f),
          boxRight(40.0f, 0.0f, -10.0f, 30.0f, 30.0f, 30.0f)
    {
    }

    void draw()
    {
        // Draw Bezier Line (left box) - translated and centered vertically
        glPushMatrix();
            glTranslatef(-40.0f, -7.5f, -10.0f);
            glColor3f(1.0f, 0.0f, 1.0f); // Magenta curve
            mybezierline.draw(GL_LINE);
            glColor3f(0.0f, 1.0f, 1.0f); // Cyan control points
            mybezierline.drawControlPoints();
        glPopMatrix();

        // Draw Bezier Surface (center box) - translated and centered vertically
        glPushMatrix();
            glTranslatef(0.0f, -2.5f, -10.0f);
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow surface
            mybeziersurface.draw(GL_FILL);
            glColor3f(1.0f, 1.0f, 1.0f); // White control points
            mybeziersurface.drawControlPoints();
        glPopMatrix();

        // Draw Sweep Surface (right box) - translated and centered vertically
        glPushMatrix();
            glTranslatef(40.0f, -5.0f, -10.0f);
            glColor3f(1.0f, 1.0f, 1.0f); // White sweep surface
            mysweepsurface.draw();
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
        // 1. Setup Bezier Line
        static GLfloat controlpoints1[] =
        {
            -15.0f,  0.0f,  10.0f,
            -10.0f, 10.0f,   0.0f,
             15.0f, 15.0f,  -5.0f,
              0.0f,  5.0f, -10.0f
        };
        mybezierline.setup(controlpoints1, 4);

        // 2. Setup Bezier Surface
        static GLfloat controlpoints2[] =
        {
            -5.0f,  0.0f,  10.0f,
            -8.0f, 10.0f,  10.0f,
             8.0f, 10.0f,  10.0f,
             5.0f,  0.0f,  10.0f,

            -15.0f, -10.0f, 5.0f,
            -10.0f,  15.0f, 5.0f,
             10.0f,  15.0f, 5.0f,
             15.0f, -10.0f, 5.0f,

            -15.0f, -10.0f, -5.0f,
            -10.0f,  15.0f, -5.0f,
             10.0f,  15.0f, -5.0f,
             15.0f, -10.0f, -5.0f,

            -5.0f,  0.0f, -10.0f,
            -8.0f,  5.0f, -10.0f,
             8.0f,  5.0f, -10.0f,
             5.0f,  0.0f, -10.0f
        };
        mybeziersurface.setup(controlpoints2, 4, 4);

        // 3. Setup Sweep Surface
        static GLfloat profilepoints[] =
        {
            5.0f,  0.0f, 0.0f,
            4.0f,  2.0f, 0.0f,
            1.0f,  2.0f, 0.0f,
            1.0f,  6.0f, 0.0f,
            4.0f,  7.0f, 0.0f,
            6.0f,  8.0f, 0.0f,
            7.0f,  9.0f, 0.0f,
            7.5f, 10.0f, 0.0f
        };
        mysweepsurface.setup(profilepoints, 8, 0, 360, 5);
    }
};

}; //end of namespace CGLab08

#endif //YP_CGLAB08_HPP
