/*
 TCS2221 Computer Graphics
 CGLab03.cpp
 Objective: Lab03 on Points & Lines (Three Bounding Boxes)
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "CGLab03.hpp"

using namespace CGLab03;

void MyStar::draw()
{
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(3, 0x3cff); // 3 is repeat factor, 0x3cff is stipple pattern
    glLineWidth(3.0); // Set line thickness

    glBegin(GL_LINES);
        // Line 1: Yellow
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-10.0f, 5.0f, 0.0f);
        glVertex3f( 10.0f, 5.0f, 0.0f);

        // Line 2: Red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 10.0f, 5.0f, 0.0f);
        glVertex3f(-10.0f, -10.0f, 0.0f);

        // Line 3: Blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-10.0f, -10.0f, 0.0f);
        glVertex3f( 0.0f, 10.0f, 0.0f);

        // Line 4: Green to Magenta gradient
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 0.0f, 10.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3f( 10.0f, -10.0f, 0.0f);

        // Line 5: Cyan to Red gradient
        glColor3f(0.0f, 1.0f, 1.0f);
        glVertex3f( 10.0f, -10.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-10.0f, 5.0f, 0.0f);
    glEnd();

    glDisable(GL_LINE_STIPPLE);
}

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

void MyPoints::draw()
{
    glPointSize(1.0f);
    // srand(1); // Commented out for dynamic particle "force field" effect
    glColor3f(1.0f, 1.0f, 1.0f); // White points

    glBegin(GL_POINTS);
        for (int i = 0; i < 1000; ++i) {
            // Generate points within size 30 bounding box centered at (0,0,0)
            float x = (rand() % 10000) / 10000.0f * 30.0f - 15.0f;
            float y = (rand() % 10000) / 10000.0f * 30.0f - 15.0f;
            float z = (rand() % 10000) / 10000.0f * 30.0f - 15.0f;
            glVertex3f(x, y, z);
        }
    glEnd();
}

void MyCurve::draw()
{
    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow sine graph
    glBegin(GL_POINTS);
        // Spans X from -15 to 15 to fit inside right box (size 30)
        for (float x = -15.0f; x <= 15.0f; x += 0.05f) {
            float y = -10.0f * sin(3.14159265f * x / 15.0f);
            glVertex3f(x, y, 0.0f);
        }
    glEnd();
}
