/*
 TCG6223 Computer Graphics
 CGLab08.cpp
 Objective: Lab08 on Drawing Curves and Surfaces
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "CGLab08.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace CGLab08;

// --- MyBezierLine Implementation (Page 2 & 3) ---

void MyBezierLine::setup(const GLfloat* controlPoints, GLint uOrder)
{
    controlpoints = controlPoints;
    uorder = uOrder;
    
    // Setup the 1D Bezier map
    glMap1f(GL_MAP1_VERTEX_3, // to be generated data
            0.0f,             // lower u range
            1.0f,             // higher u range
            3,                // u control point stride in array
            uorder,           // order of u (number of control points)
            controlpoints);   // control points array
            
    glEnable(GL_MAP1_VERTEX_3);
}

void MyBezierLine::draw(GLenum draw_mode, GLint ures)
{
    // Map to u range 0.0 - 1.0 with ures intervals
    glMapGrid1f(ures, 0.0f, 1.0f);
    // Evaluate the Bezier curve
    glEvalMesh1(draw_mode, 0, ures);
}

void MyBezierLine::drawControlPoints()
{
    int index = 0;
    glBegin(GL_LINE_STRIP);
        for(int i = 0; i < uorder; ++i)
        {
            glVertex3fv(controlpoints + index);
            index += 3;
        }
    glEnd();
}

// --- MyBezierSurface Implementation (Page 5 & 6) ---

void MyBezierSurface::setup(const GLfloat* controlPoints, GLint uOrder, GLint vOrder, const bool autonormal)
{
    controlpoints = controlPoints;
    uorder = uOrder;
    vorder = vOrder;

    // Setup the 2D Bezier map
    glMap2f(GL_MAP2_VERTEX_3, // to be generated data
            0.0f,             // lower u range
            1.0f,             // higher u range
            3,                // u control point stride in array
            uorder,           // order of u
            0.0f,             // lower v range
            1.0f,             // higher v range
            3 * uorder,       // v control point stride in array
            vorder,           // order of v
            controlpoints);   // control points array

    glEnable(GL_MAP2_VERTEX_3);
    
    if (autonormal)
        glEnable(GL_AUTO_NORMAL);
}

void MyBezierSurface::draw(GLenum draw_mode, GLint ures, GLint vres)
{
    glDisable(GL_CULL_FACE);
    // Set 2D grid map
    glMapGrid2f(ures, 0.0f, 1.0f, vres, 0.0f, 1.0f);
    // Evaluate Bezier surface
    glEvalMesh2(draw_mode, 0, ures, 0, vres);
    glEnable(GL_CULL_FACE);
}

void MyBezierSurface::drawControlPoints()
{
    int index = 0;
    
    // Draw control point lines along U direction
    for (int j = 0; j < vorder; ++j)
    {
        glBegin(GL_LINE_STRIP);
            for (int i = 0; i < uorder; ++i)
            {
                glVertex3fv(controlpoints + index);
                index += 3;
            }
        glEnd();
    }
    
    // Draw control point lines along V direction
    for (int i = 0; i < uorder; ++i)
    {
        index = 3 * i;
        glBegin(GL_LINE_STRIP);
            for (int j = 0; j < vorder; ++j)
            {
                glVertex3fv(controlpoints + index);
                index += uorder * 3;
            }
        glEnd();
    }
}

// --- MySweepSurface Implementation (Page 8) ---

void MySweepSurface::setup(const GLfloat* profilePoints, GLint numOfProfilePoints, GLfloat degreeStart, GLfloat degreeEnd, GLfloat degreeStep)
{
    profilepoints = profilePoints;
    numofprofilepoints = numOfProfilePoints;
    
    if (surfacepoints) 
        delete[] surfacepoints;

    numofverticallines = 1 + static_cast<int>(floor((degreeEnd - degreeStart) / degreeStep));
    numofsurfacepoints = numofprofilepoints * numofverticallines;
    surfacepoints = new GLfloat[3 * numofsurfacepoints];

    int surfptsindex = 0;
    GLfloat degree = degreeStart;
    
    for (int i = 0; i < numofverticallines; ++i, degree += degreeStep)
    {
        GLfloat radian = degree * M_PI / 180.0;
        GLfloat c = cos(radian);
        GLfloat s = sin(radian);
        int profileindex = 0;
        
        for (int j = 0; j < numofprofilepoints; ++j)
        {
            surfacepoints[surfptsindex]     = c * profilepoints[profileindex] + s * profilepoints[profileindex+2];
            surfacepoints[surfptsindex+1]   = profilepoints[profileindex+1];
            surfacepoints[surfptsindex+2]   = -s * profilepoints[profileindex] + c * profilepoints[profileindex+2];
            
            profileindex += 3;
            surfptsindex += 3;
        }
    }
}

void MySweepSurface::draw()
{
    glDisable(GL_CULL_FACE);
    int index = 0;
    
    for (int i = 0; i < numofverticallines - 1; ++i)
    {
        glBegin(GL_QUAD_STRIP);
            for (int j = 0; j < numofprofilepoints; ++j)
            {
                glVertex3fv(surfacepoints + index);
                glVertex3fv(surfacepoints + index + 3 * numofprofilepoints);
                index += 3;
            }
        glEnd();
    }
    glEnable(GL_CULL_FACE);
}

// --- MyBoundingBox Implementation ---

void MyBoundingBox::draw()
{
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow outline

        // Top loop
        glVertex3f(x[0], y[0], z[0]); glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[1], y[1], z[1]); glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[2], y[2], z[2]); glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[3], y[3], z[3]); glVertex3f(x[0], y[0], z[0]);

        // Bottom loop
        glVertex3f(x[4], y[4], z[4]); glVertex3f(x[5], y[5], z[5]);
        glVertex3f(x[5], y[5], z[5]); glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[6], y[6], z[6]); glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[7], y[7], z[7]); glVertex3f(x[4], y[4], z[4]);

        // Vertical edges
        glVertex3f(x[0], y[0], z[0]); glVertex3f(x[4], y[4], z[4]);
        glVertex3f(x[1], y[1], z[1]); glVertex3f(x[5], y[5], z[5]);
        glVertex3f(x[2], y[2], z[2]); glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[3], y[3], z[3]); glVertex3f(x[7], y[7], z[7]);
    glEnd();
}

void MyBoundingBox::drawGrid()
{
    glLineWidth(0.5f);
    glColor3f(0.3f, 0.3f, 0.4f); // Dim grey-blue
    glBegin(GL_LINES);
        GLfloat hx = dx / 2.0f;
        GLfloat hy = dy / 2.0f;
        GLfloat hz = dz / 2.0f;

        // Compute division coordinates inside the box (at 1/4 intervals)
        float div_x[] = { -hx / 2.0f, 0.0f, hx / 2.0f };
        float div_y[] = { -hy / 2.0f, 0.0f, hy / 2.0f };
        float div_z[] = { -hz / 2.0f, 0.0f, hz / 2.0f };

        // Front (z = cz + hz) & Back (z = cz - hz) faces
        for (int i = 0; i < 3; i++) {
            float vx = cx + div_x[i];
            float vy = cy + div_y[i];
            // Vertical lines
            glVertex3f(vx, cy - hy, cz + hz); glVertex3f(vx, cy + hy, cz + hz);
            glVertex3f(vx, cy - hy, cz - hz); glVertex3f(vx, cy + hy, cz - hz);
            // Horizontal lines
            glVertex3f(cx - hx, vy, cz + hz); glVertex3f(cx + hx, vy, cz + hz);
            glVertex3f(cx - hx, vy, cz - hz); glVertex3f(cx + hx, vy, cz - hz);
        }

        // Left (x = cx - hx) & Right (x = cx + hx) faces
        for (int i = 0; i < 3; i++) {
            float vy = cy + div_y[i];
            float vz = cz + div_z[i];
            // Vertical lines
            glVertex3f(cx - hx, cy - hy, vz); glVertex3f(cx - hx, cy + hy, vz);
            glVertex3f(cx + hx, cy - hy, vz); glVertex3f(cx + hx, cy + hy, vz);
            // Horizontal lines
            glVertex3f(cx - hx, vy, cz - hz); glVertex3f(cx - hx, vy, cz + hz);
            glVertex3f(cx + hx, vy, cz - hz); glVertex3f(cx + hx, vy, cz + hz);
        }

        // Top (y = cy + hy) & Bottom (y = cy - hy) faces
        for (int i = 0; i < 3; i++) {
            float vx = cx + div_x[i];
            float vz = cz + div_z[i];
            // Parallel to Z
            glVertex3f(vx, cy + hy, cz - hz); glVertex3f(vx, cy + hy, cz + hz);
            glVertex3f(vx, cy - hy, cz - hz); glVertex3f(vx, cy - hy, cz + hz);
            // Parallel to X
            glVertex3f(cx - hx, cy + hy, vz); glVertex3f(cx + hx, cy + hy, vz);
            glVertex3f(cx - hx, cy - hy, vz); glVertex3f(cx + hx, cy - hy, vz);
        }
    glEnd();
}
