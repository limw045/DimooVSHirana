/*
 TCG6223 Computer Graphics
 CGLab06.cpp
 Objective: Lab06 (Outer Space Scene)
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "CGLab06.hpp"

using namespace CGLab06;

void MyPlanets::drawStars()
{
    glDisable(GL_LIGHTING); // Stars shouldn't be affected by lighting
    glPointSize(1.0f);
    
    // Fixed random seed so stars stay in place each frame
    srand(1);
    
    glColor3f(1.0f, 1.0f, 1.0f); // White stars
    glBegin(GL_POINTS);
        for (int i = 0; i < 1200; ++i)
        {
            // Scattered widely around the workspace viewing volume
            float x = (rand() % 10000) / 10000.0f * 300.0f - 150.0f;
            float y = (rand() % 10000) / 10000.0f * 300.0f - 150.0f;
            float z = (rand() % 10000) / 10000.0f * 300.0f - 150.0f;
            glVertex3f(x, y, z);
        }
    glEnd();
    glEnable(GL_LIGHTING); // Re-enable lighting for planets/ship
}

void MyPlanets::drawPlanets()
{
    glDisable(GL_CULL_FACE);
    GLUquadricObj *pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // 1. Jupiter (Large Grey sphere)
    glPushMatrix();
        glColor3f(0.7f, 0.7f, 0.7f); // Grey planet body
        glTranslatef(-16.0f, 0.0f, -5.0f); 
        gluSphere(pObj, 8.5f, 48, 48); // High segment count for smoothness
    glPopMatrix();

    // 2. Mars (Small Red sphere in the distance)
    glPushMatrix();
        glColor3f(1.0f, 0.1f, 0.1f); // Red planet body
        glTranslatef(-3.0f, 10.0f, -22.0f); 
        gluSphere(pObj, 1.3f, 24, 24);
    glPopMatrix();

    // 3. Saturn (White/grey sphere with tilted ring)
    glPushMatrix();
        // Saturn body sphere
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(16.0f, 0.0f, -5.0f);
        gluSphere(pObj, 4.5f, 40, 40);
        
        // Tilt and render Saturn ring
        glRotatef(75.0f, 1.0f, 0.0f, 0.0f); // Pitch the ring
        glRotatef(15.0f, 0.0f, 1.0f, 0.0f); // Yaw the ring slightly
        glColor3f(0.55f, 0.38f, 0.38f); // Soft brown-red ring color
        gluDisk(pObj, 5.5f, 9.5f, 48, 4); // Saturn flat ring disk
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glEnable(GL_CULL_FACE);
}

void Spaceship::draw()
{
    glDisable(GL_CULL_FACE);
    GLUquadricObj *pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // Spaceship body (blue cylinder)
    glColor3f(0.3f, 0.4f, 0.8f);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -3.0f); // Center body along Z
        gluCylinder(pObj, 1.0f, 1.0f, 6.0f, 24, 24);
    glPopMatrix();

    // Front Cone (blue cone pointing to +z)
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 3.0f);
        gluCylinder(pObj, 1.0f, 0.0f, 2.5f, 24, 24);
    glPopMatrix();

    // Back Cone (blue cone pointing to -z)
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Flip direction to -z
        gluCylinder(pObj, 1.0f, 0.0f, 2.5f, 24, 24);
    glPopMatrix();

    // Wings: 4 yellow wings in an X-wing configuration
    glColor3f(0.9f, 0.9f, 0.0f); // Yellow wings
    glBegin(GL_TRIANGLES);
        // Top-Right Wing
        glVertex3f(0.0f, 0.0f, -2.5f);
        glVertex3f(0.0f, 0.0f, 2.5f);
        glVertex3f(3.5f, 1.8f, -2.0f);

        // Top-Left Wing
        glVertex3f(0.0f, 0.0f, -2.5f);
        glVertex3f(0.0f, 0.0f, 2.5f);
        glVertex3f(-3.5f, 1.8f, -2.0f);

        // Bottom-Left Wing
        glVertex3f(0.0f, 0.0f, -2.5f);
        glVertex3f(0.0f, 0.0f, 2.5f);
        glVertex3f(-3.5f, -1.8f, -2.0f);

        // Bottom-Right Wing
        glVertex3f(0.0f, 0.0f, -2.5f);
        glVertex3f(0.0f, 0.0f, 2.5f);
        glVertex3f(3.5f, -1.8f, -2.0f);
    glEnd();

    gluDeleteQuadric(pObj);
    glEnable(GL_CULL_FACE);
}
