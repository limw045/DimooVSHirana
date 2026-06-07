/*
 TCG6223 Computer Graphics
 CGLab07.cpp
 Objective: Lab07 on Hierarchical Modeling (Lego Bricks)
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "CGLab07.hpp"

using namespace CGLab07;

MyLego::MyLego()
{
    // Setup Quadric Object
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
}

MyLego::~MyLego()
{
    gluDeleteQuadric(pObj);
}

void MyLego::draw()
{
    glDisable(GL_CULL_FACE);
    
    // Default call draws a 2x2 Lego brick
    glColor3f(1.0f, 1.0f, 1.0f);
    draw2x2Lego();

    glEnable(GL_CULL_FACE);
}

// Draw a lego piece with dimension 2.0 x 1.0 x 2.0,
// the bottom center of the lego piece is at 0,0,0 (as in Page 3 & 4)
void MyLego::drawCube()
{
    GLfloat dimx = 2.0f, dimy = 1.0f, dimz = 2.0f;
    GLfloat xmax = dimx/2.0f;
    GLfloat xmin = -xmax;
    GLfloat ymax = dimy;
    GLfloat ymin = 0.0f;
    GLfloat zmax = dimz/2.0f;
    GLfloat zmin = -zmax;

    glBegin(GL_QUADS);
        // Front face
        glVertex3f(xmin, ymax, zmax);
        glVertex3f(xmin, ymin, zmax);
        glVertex3f(xmax, ymin, zmax);
        glVertex3f(xmax, ymax, zmax);
        // Back face
        glVertex3f(xmin, ymax, zmin);
        glVertex3f(xmax, ymax, zmin);
        glVertex3f(xmax, ymin, zmin);
        glVertex3f(xmin, ymin, zmin);
        // Right face
        glVertex3f(xmax, ymax, zmax);
        glVertex3f(xmax, ymin, zmax);
        glVertex3f(xmax, ymin, zmin);
        glVertex3f(xmax, ymax, zmin);
        // Left face
        glVertex3f(xmin, ymax, zmax);
        glVertex3f(xmin, ymax, zmin);
        glVertex3f(xmin, ymin, zmin);
        glVertex3f(xmin, ymin, zmax);
        // Top face
        glVertex3f(xmin, ymax, zmax);
        glVertex3f(xmax, ymax, zmax);
        glVertex3f(xmax, ymax, zmin);
        glVertex3f(xmin, ymax, zmin);
        // Bottom face (hollowed out slightly in real Lego, represented at y = ymin + 0.2)
        glVertex3f(xmin, ymin + 0.2f, zmax);
        glVertex3f(xmax, ymin + 0.2f, zmax);
        glVertex3f(xmax, ymin + 0.2f, zmin);
        glVertex3f(xmin, ymin + 0.2f, zmin);
    glEnd();
}

// Underneath socket cylinder (Page 5)
void MyLego::drawBottomCylinder()
{
    gluCylinder(pObj, 0.5f, 0.5f, 0.2f, 26, 13);
}

// 4 stud cylinders on top face of the Lego brick (Page 6)
void MyLego::drawTopCylinders()
{
    glPushMatrix();
        glTranslatef(-0.5f, -0.5f, 0.0f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.2f, 26, 13);
        glTranslatef(1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.2f, 26, 13);
        glTranslatef(0.0f, 1.0f, 0.0f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.2f, 26, 13);
        glTranslatef(-1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.2f, 26, 13);
    glPopMatrix();
}

// Combine Cube, Bottom cylinder, and Top studs hierarchically (Page 10)
void MyLego::draw2x2Lego()
{
    glPushMatrix();
        drawCube();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate cylinder orientation to vertical (Y axis)
        drawBottomCylinder();
        glTranslatef(0.0f, 0.0f, 1.0f); // Move to the top face (y = 1.0)
        drawTopCylinders();
    glPopMatrix();
}

// Reusable larger blocks (Page 11 & 12)
void MyLego::draw2x4Lego()
{
    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 0.0f);
        draw2x2Lego();
        glTranslatef(2.0f, 0.0f, 0.0f);
        draw2x2Lego();
    glPopMatrix();
}

void MyLego::draw4x4Lego()
{
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.0f);
        draw2x4Lego();
        glTranslatef(0.0f, 0.0f, 2.0f);
        draw2x4Lego();
    glPopMatrix();
}

void MyLego::draw2x8Lego()
{
    glPushMatrix();
        glTranslatef(-2.0f, 0.0f, 0.0f);
        draw2x4Lego();
        glTranslatef(4.0f, 0.0f, 0.0f);
        draw2x4Lego();
    glPopMatrix();
}

void MyLego::draw8x8Lego()
{
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -3.0f);
        draw2x8Lego();
        glTranslatef(0.0f, 0.0f, 2.0f);
        draw2x8Lego();
        glTranslatef(0.0f, 0.0f, 2.0f);
        draw2x8Lego();
        glTranslatef(0.0f, 0.0f, 2.0f);
        draw2x8Lego();
    glPopMatrix();
}

void MyLego::draw1()
{
    MyAxis axis;
    axis.setLength(6.0f, 6.0f, 6.0f);
    axis.setLineWidth(3);
    axis.setLineStipple(1, 0xff60);
    axis.draw();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCube();
}

// Renders the single brick showing transformations (Page 10)
void MyLego::draw2()
{
    MyAxis axis1;
    axis1.setLength(6.0f, 6.0f, 6.0f);
    axis1.setLineWidth(3);
    axis1.setLineStipple(1, 0xff60);

    MyAxis axis2;
    axis2.setLength(5.0f, 5.0f, 5.0f);
    axis2.setLineWidth(5);
    axis2.setLineStipple(1, 0x6060);

    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCube();
        
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        axis1.draw();
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawBottomCylinder();
        
        glTranslatef(0.0f, 0.0f, 1.0f);
        axis2.draw();
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTopCylinders();
    glPopMatrix();
}

// Build a single multi-colored Lego tower (Page 13)
void MyLego::drawOneTower()
{
    // Layer 1: Grey/white 8x8 base
    glColor3f(0.75f, 0.75f, 0.75f);
    glPushMatrix();
        draw8x8Lego();
    glPopMatrix();

    // Layer 2: 4 Blue 2x2 corner bricks and 1 Red 4x4 center brick
    glColor3f(0.0f, 0.3f, 0.9f); // Blue corners
    glPushMatrix();
        glTranslatef(-3.0f, 1.0f, 3.0f);
        draw2x2Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 1.0f, 3.0f);
        draw2x2Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 1.0f, -3.0f);
        draw2x2Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 1.0f, -3.0f);
        draw2x2Lego();
    glPopMatrix();

    glColor3f(0.9f, 0.1f, 0.1f); // Red center
    glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f);
        draw4x4Lego();
    glPopMatrix();

    // Layer 3: Cyan 4x4 brick
    glColor3f(0.0f, 0.8f, 0.8f);
    glPushMatrix();
        glTranslatef(0.0f, 2.0f, 0.0f);
        draw4x4Lego();
    glPopMatrix();

    // Layer 4: Yellow 2x2 brick
    glColor3f(0.9f, 0.9f, 0.0f);
    glPushMatrix();
        glTranslatef(0.0f, 3.0f, 0.0f);
        draw2x2Lego();
    glPopMatrix();

    // Layer 5: Magenta 2x2 brick
    glColor3f(0.9f, 0.0f, 0.9f);
    glPushMatrix();
        glTranslatef(0.0f, 4.0f, 0.0f);
        draw2x2Lego();
    glPopMatrix();
}

// Build complex castle of towers with walkways (Page 13)
void MyLego::drawTowers()
{
    // 4 Corner Towers
    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, 10.0f);
        drawOneTower();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(10.0f, 0.0f, 10.0f);
        drawOneTower();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, -10.0f);
        drawOneTower();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(10.0f, 0.0f, -10.0f);
        drawOneTower();
    glPopMatrix();

    // Green connecting bridges (bridges sit at height y = 2.0 on top of Layer 2 center brick)
    glColor3f(0.0f, 0.7f, 0.2f); // Forest green walkways

    // Front Bridge (FL to FR)
    glPushMatrix();
        glTranslatef(-4.0f, 2.0f, 10.0f);
        draw2x8Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 2.0f, 10.0f);
        draw2x8Lego();
    glPopMatrix();

    // Back Bridge (BL to BR)
    glPushMatrix();
        glTranslatef(-4.0f, 2.0f, -10.0f);
        draw2x8Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 2.0f, -10.0f);
        draw2x8Lego();
    glPopMatrix();

    // Left Bridge (FL to BL) - rotated 90 degrees around Y axis
    glPushMatrix();
        glTranslatef(-10.0f, 2.0f, -4.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        draw2x8Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-10.0f, 2.0f, 4.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        draw2x8Lego();
    glPopMatrix();

    // Right Bridge (FR to BR) - rotated 90 degrees around Y axis
    glPushMatrix();
        glTranslatef(10.0f, 2.0f, -4.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        draw2x8Lego();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(10.0f, 2.0f, 4.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        draw2x8Lego();
    glPopMatrix();
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
