/*
 TCS2221 Computer Graphics
 CGLab05.cpp
 Objective: Lab05 on Modeling Transformations and Quadrics
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "CGLab05.hpp"

using namespace CGLab05;

void MyArrows::drawArrow()
{
    glBegin(GL_TRIANGLES);
        glVertex3f( 0.0f, 15.0f, 0.0f);
        glVertex3f(-2.0f, 13.0f, 0.0f);
        glVertex3f( 2.0f, 13.0f, 0.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, 13.0f, 0.0f);
        glVertex3f(-1.0f,  0.0f, 0.0f);
        glVertex3f( 1.0f,  0.0f, 0.0f);
        glVertex3f( 1.0f, 13.0f, 0.0f);
    glEnd();
}

void MyArrows::draw1()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glTranslatef(-10.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawArrow();

    glTranslatef(0.0f, 0.0f, -10.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    drawArrow();

    glTranslatef(10.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawArrow();
    glEnable(GL_CULL_FACE);
}

void MyArrows::draw2()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -10.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(10.0f, 0.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void MyArrows::draw3()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawArrow();

    glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    drawArrow();

    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawArrow();
    glEnable(GL_CULL_FACE);
}

void MyArrows::draw4()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glPushMatrix();
        glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void MyArrows::draw5()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, 0.0f);
        glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -10.0f);
        glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(10.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void MyArrows::draw6()
{
    glDisable(GL_CULL_FACE);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawArrow();

    glPushMatrix();
        glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-10.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, 0.0f, -10.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawArrow();
    glPopMatrix();

    glPushMatrix();
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(10.0f, 0.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawArrow();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

// 3D Helix spiral of arrows (Task 1E)
void MyArrows::draw7()
{
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < 36; ++i)
    {
        float t = (float)i / 35.0f; // Interpolate from 0.0 to 1.0
        glPushMatrix();
            glRotatef(i * 10.0f, 0.0f, 0.0f, 1.0f); // Rotate gradually around Z axis
            glTranslatef(10.0f, 0.0f, i * 0.5f - 9.0f); // Spaced radius in X, spiral height in Z
            glColor3f(0.0f, t, 1.0f - t); // Transition from Blue (i=0) to Green (i=35)
            // Scale the arrow down a bit to fit neatly inside the box
            glScalef(0.6f, 0.6f, 0.6f);
            drawArrow();
        glPopMatrix();
    }
    glEnable(GL_CULL_FACE);
}

void MyQuadricDemo::draw()
{
    glDisable(GL_CULL_FACE);
    GLUquadricObj *pObj;
    pObj = gluNewQuadric();

    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);

    glPushMatrix();
        // White Cylinder
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(-40.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 2.0f, 12.0f, 20.0f, 24, 72);

        // Yellow Cylinder (coarse)
        glColor3f(1.0f, 1.0f, 0.0f);
        glTranslatef(20.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 2.0f, 12.0f, 20.0f, 72, 4);

        // Green Sphere (large)
        glColor3f(0.0f, 1.0f, 0.0f);
        glTranslatef(14.0f, 0.0f, 0.0f);
        gluSphere(pObj, 8.0f, 24, 12);

        // Blue Sphere (small)
        glColor3f(0.0f, 0.0f, 1.0f);
        glTranslatef(14.0f, 0.0f, 0.0f);
        gluSphere(pObj, 4.0f, 24, 12);

        // Red Disk
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(14.0f, 0.0f, 0.0f);
        gluDisk(pObj, 4.0f, 10.0f, 26, 13);

        // Cyan Disk (no inner hole)
        glColor3f(0.0f, 1.0f, 1.0f);
        glTranslatef(20.0f, 0.0f, 0.0f);
        gluDisk(pObj, 0.0f, 10.0f, 26, 13);
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glEnable(GL_CULL_FACE);
}

// 3D Snowman using Quadrics (Task 2B)
void MySnowman::draw()
{
    glDisable(GL_CULL_FACE);
    GLUquadricObj *pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // Bottom sphere (large, white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
        glTranslatef(0.0f, -6.0f, 0.0f);
        gluSphere(pObj, 5.0f, 32, 32);
    glPopMatrix();

    // Middle sphere (medium, white)
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        gluSphere(pObj, 3.5f, 32, 32);
    glPopMatrix();

    // Head sphere (small, white)
    glPushMatrix();
        glTranslatef(0.0f, 4.5f, 0.0f);
        gluSphere(pObj, 2.5f, 32, 32);
    glPopMatrix();

    // Eyes (black dots)
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
        glTranslatef(0.8f, 5.2f, 2.1f);
        gluSphere(pObj, 0.3f, 16, 16);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-0.8f, 5.2f, 2.1f);
        gluSphere(pObj, 0.3f, 16, 16);
    glPopMatrix();

    // Nose (orange cone)
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
        glTranslatef(0.0f, 4.5f, 2.0f); // translated to head front
        gluCylinder(pObj, 0.5f, 0.0f, 2.5f, 16, 16); // cone points along +z
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glEnable(GL_CULL_FACE);
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
