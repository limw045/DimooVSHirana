/*
TCG6223 Computer Graphics
Trimester March/April 2026 (Term 2610)
CGLabSimpleFlower.cpp
Objective: Implementation File for Simple Flower Lab Assignment
*/

#include <GL/glut.h>
#include <string>
#include <fstream>
#include <cmath>
#include "CGLabSimpleFlower.hpp"

using namespace CGLabSimpleFlower;

void SimpleFlower::drawCore(){
    glDisable(GL_CULL_FACE);
    
    //Quadric Object
    GLUquadricObj *pObj;
    //create and initialize quadric
    pObj = gluNewQuadric();
    
    glPushMatrix();
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        glTranslatef(0.0f, 0.0f, 0.0f);
        gluDisk(pObj, 0.0f, 2.0f, 26, 13);
    glPopMatrix();
    
    // Delete quadric object to prevent memory leak
    gluDeleteQuadric(pObj);
    
    glEnable(GL_CULL_FACE);
}

void SimpleFlower::drawPetals(){
    // Draw 10 petals around the center
    for (int i = 0; i < 10; ++i) {
        glPushMatrix();
            // Spaced out by 36 degrees, starting at 18 degrees to align top petal with Y axis and X axis with a gap
            glRotatef(18.0f + i * 36.0f, 0.0f, 0.0f, 1.0f);
            drawPetal();
        glPopMatrix();
    }
}

void SimpleFlower::drawPetal(){
    glDisable(GL_CULL_FACE);
    
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); // Red Petal
        
        // Exact Cartesian coordinates for a 36-degree sector wedge (wide at base, narrow at tip)
        // Base: x = 2.0 * cos(18 deg) = 1.90, y = +- 2.0 * sin(18 deg) = +- 0.62 (touches adjacent petals)
        // Tip:  x = 4.5, y = +- 0.35 (narrower flat tip)
        
        // Counter-clockwise vertex order
        glVertex3f(1.90f, -0.62f, 0.0f); // Bottom-left (base)
        glVertex3f(4.50f, -0.35f, 0.0f); // Bottom-right (tip)
        glVertex3f(4.50f,  0.35f, 0.0f); // Top-right (tip)
        glVertex3f(1.90f,  0.62f, 0.0f); // Top-left (base)
    glEnd();
    
    glEnable(GL_CULL_FACE);
}
