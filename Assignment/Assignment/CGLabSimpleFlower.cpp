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
    // Draw 8 petals around the center
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
            // Spaced out by 45 degrees, starting at 22.5 degrees to keep the gaps aligned with axes
            glRotatef(22.5f + i * 45.0f, 0.0f, 0.0f, 1.0f);
            drawPetal();
        glPopMatrix();
    }
}

void SimpleFlower::drawPetal(){
    glDisable(GL_CULL_FACE);
    
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); // Red Petal
        
        // Symmetrical trapezoid with base touching neighboring petals
        float angleIn = 22.5f * 3.14159265f / 180.0f;  // Touch at the base
        float angleOut = 12.5f * 3.14159265f / 180.0f; // Taper at the tip
        
        float cosIn = cos(angleIn);
        float sinIn = sin(angleIn);
        float cosOut = cos(angleOut);
        float sinOut = sin(angleOut);
        
        float rIn = 2.0f;     // Starts where the green disk ends (radius 2.0)
        float rOut = 5.5f;    // Outer tip radius (length of petal)
        
        // Counter-clockwise vertex order
        glVertex3f(rIn * cosIn, -rIn * sinIn, 0.0f);
        glVertex3f(rOut * cosOut, -rOut * sinOut, 0.0f);
        glVertex3f(rOut * cosOut, rOut * sinOut, 0.0f);
        glVertex3f(rIn * cosIn, rIn * sinIn, 0.0f);
    glEnd();
    
    glEnable(GL_CULL_FACE);
}
