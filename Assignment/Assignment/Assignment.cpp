#include <GL/glut.h>
#include <string>
#include <fstream>
#include <cmath>
#include "Assignment.hpp"

using namespace Assignment;

void MyVirtualWorld::init()
{
    // Enable basic OpenGL features
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    timeold = glutGet(GLUT_ELAPSED_TIME);
}

void MyVirtualWorld::draw()
{
    // Draw a simple placeholder shape (like a colored teapot) to verify everything compiles and runs
    glPushMatrix();
        glColor3f(0.0f, 1.0f, 1.0f); // Cyan
        glFrontFace(GL_CW);
        glutSolidTeapot(5.0);
        glFrontFace(GL_CCW);
    glPopMatrix();
}

void MyVirtualWorld::tickTime()
{
    timenew    = glutGet(GLUT_ELAPSED_TIME);
    elapseTime = timenew - timeold;
    timeold    = timenew;
    
    // Add animation ticks here if needed
}
